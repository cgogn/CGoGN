/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2011, IGG Team, LSIIT, University of Strasbourg           *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Web site: http://cgogn.u-strasbg.fr/                                         *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

namespace CGoGN {

namespace Geom {

template<typename PFP>
Frame<PFP>::Frame(const VEC3& X, const VEC3& Y, const VEC3& Z)
{
	m_X = X ;
	m_Y = Y ;
	m_Z = Z ;
}

template<typename PFP>
Frame<PFP>::Frame(const VEC3& EulerAngles)
{
	const VEC3 X(Xx,Xy,Xz) ;
	const VEC3 Z(Zx,Zy,Zz) ;

	// get known data
	const REAL& alpha = EulerAngles[0] ;
	const REAL& beta = EulerAngles[1] ;
	const REAL& gamma = EulerAngles[2] ;

	const VEC3 lineOfNodes = rotate<REAL>(Z,alpha,X) ; // rotation around reference normal of vector T
	m_Z = rotate<REAL>(lineOfNodes,gamma,Z) ; // rotation around line of nodes of vector N
	m_X = rotate<REAL>(m_Z,beta,lineOfNodes) ; // rotation around new normal of vector represented by line of nodes
	m_Y = m_Z ^ m_X ;
}

template<typename PFP>
typename Geom::Vector<3,typename PFP::REAL> Frame<PFP>::getCompressed() const
{
	VEC3 EulerAngles ;

	const VEC3 X(Xx,Xy,Xz) ;
	const VEC3 Y(Yx,Yy,Yz) ;
	const VEC3 Z(Zx,Zy,Zz) ;

	REAL& alpha = EulerAngles[0] ;
	REAL& beta = EulerAngles[1] ;
	REAL& gamma = EulerAngles[2] ;

	VEC3 lineOfNodes = Z ^ m_Z ;
	if (lineOfNodes.norm2() < 1e-5) // if Z ~= m_Z
	{
		lineOfNodes = X ; // = reference T
		alpha = 0 ;
		gamma = 0 ;
	}
	else
	{
		lineOfNodes.normalize() ;

		// angle between reference T and line of nodes
		alpha = (Y*lineOfNodes > 0 ? 1 : -1) * std::acos(std::max(std::min(REAL(1.0), X*lineOfNodes ),REAL(-1.0))) ;
		// angle between reference normal and normal
		gamma = std::acos(std::max(std::min(REAL(1.0), Z*m_Z ),REAL(-1.0))) ; // gamma is always positive because the direction of vector lineOfNodes=(reference normal)^(normal) (around which a rotation of angle beta is done later on) changes depending on the side on which they lay w.r.t eachother.
	}
	// angle between line of nodes and T
	beta = (m_Y*lineOfNodes > 0 ? -1 : 1) * std::acos(std::max(std::min(REAL(1.0), m_X*lineOfNodes ),REAL(-1.0))) ;

	return EulerAngles ;
}

template<typename PFP>
bool Frame<PFP>::equals(const Geom::Frame<PFP>& lf, REAL epsilon) const
{
	VEC3 dX = m_X - lf.getX() ;
	VEC3 dY = m_Y - lf.getY() ;
	VEC3 dZ = m_Z - lf.getZ() ;

	return dX.norm2() < epsilon && dY.norm2() < epsilon && dZ.norm2() < epsilon ;
}

template<typename PFP>
bool Frame<PFP>::operator==(const Frame<PFP>& lf) const
{
	return this->equals(lf) ;
}

template<typename PFP>
bool Frame<PFP>::operator!=(const Frame<PFP>& lf) const
{
	return !(this->equals(lf)) ;
}

template<typename PFP>
bool Frame<PFP>::isDirect(REAL epsilon) const
{
	VEC3 new_Y = m_Z ^ m_X ;		// direct
	VEC3 diffs = new_Y - m_Y ;		// differences with existing B
	REAL diffNorm = diffs.norm2() ;	// Norm of this differences vector

	return (diffNorm < epsilon) ;		// Verify that this difference is very small
}

template<typename PFP>
bool Frame<PFP>::isOrthogonal(REAL epsilon) const
{
	return (fabs(m_X * m_Y) < epsilon) && (fabs(m_Z * m_Y) < epsilon) && (fabs(m_X * m_Z) < epsilon) ;
}

template<typename PFP>
bool Frame<PFP>::isNormalized(REAL epsilon) const
{
	return 		(1-epsilon < m_Z.norm2() && m_Z.norm2() < 1+epsilon)
			&& 	(1-epsilon < m_X.norm2() && m_X.norm2() < 1+epsilon)
			&&	(1-epsilon < m_Y.norm2() && m_Y.norm2() < 1+epsilon) ;
}

template<typename PFP>
bool Frame<PFP>::isOrthoNormalDirect(REAL epsilon) const
{
	return isOrthogonal(epsilon) && isNormalized(epsilon) && isDirect(epsilon) ;
}


template<typename REAL>
Geom::Vector<3,REAL> carthToSpherical (const Geom::Vector<3,REAL>& carth)
{
	Geom::Vector<3,REAL> res ;

	const REAL& x = carth[0] ;
	const REAL& y = carth[1] ;
	const REAL& z = carth[2] ;

	REAL& rho = res[0] ;
	REAL& theta = res[1] ;
	REAL& phi = res[2] ;

	rho = carth.norm() ;
	theta = ((y < 0) ? -1 : 1) * std::acos(x / REAL(sqrt(x*x + y*y)) )  ;
	if (isnan(theta))
		theta = 0.0 ;
	phi = std::asin(z) ;

	return res ;
}

template<typename REAL>
Geom::Vector<3,REAL> sphericalToCarth (const Geom::Vector<3,REAL>& sph)
{
	Geom::Vector<3,REAL> res ;

	const REAL& rho = sph[0] ;
	const REAL& theta = sph[1] ;
	const REAL& phi = sph[2] ;

	REAL& x = res[0] ;
	REAL& y = res[1] ;
	REAL& z = res[2] ;

	x = rho*cos(theta)*cos(phi) ;
	y = rho*sin(theta)*cos(phi) ;
	z = rho*sin(phi) ;

	assert(-1.000001 < x && x < 1.000001) ;
	assert(-1.000001 < y && y < 1.000001) ;
	assert(-1.000001 < z && z < 1.000001) ;

	return res ;
}

template <typename REAL>
Geom::Vector<3,REAL> rotate (Geom::Vector<3,REAL> axis, REAL angle, Geom::Vector<3,REAL> vector)
{
	axis.normalize() ;

	const REAL& u = axis[0] ;
	const REAL& v = axis[1] ;
	const REAL& w = axis[2] ;

	const REAL& x = vector[0] ;
	const REAL& y = vector[1] ;
	const REAL& z = vector[2] ;

	Geom::Vector<3,REAL> res ;
	REAL& xp = res[0] ;
	REAL& yp = res[1] ;
	REAL& zp = res[2] ;

	const REAL tmp1 = u*x+v*y+w*z ;
	const REAL cos = std::cos(angle) ;
	const REAL sin = std::sin(angle) ;

	xp = u*tmp1*(1-cos) + x*cos+(v*z-w*y)*sin ;
	yp = v*tmp1*(1-cos) + y*cos-(u*z-w*x)*sin ;
	zp = w*tmp1*(1-cos) + z*cos+(u*y-v*x)*sin ;

	return res ;
}

} // Geom

} // CGoGN
