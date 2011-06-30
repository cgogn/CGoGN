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


namespace Utils {

template<typename PFP>
LocalFrame<PFP>::LocalFrame(const VEC3& T, const VEC3& B, const VEC3& N)
{
	m_T = T ;
	m_B = B ;
	m_N = N ;
}

template<typename PFP>
LocalFrame<PFP>::LocalFrame(const VEC3& EulerAngles)
{
	const VEC3 T(Tx,Ty,Tz) ;
	const VEC3 N(Nx,Ny,Nz) ;

	// get known data
	const REAL& alpha = EulerAngles[0] ;
	const REAL& gamma = EulerAngles[1] ;
	const REAL& beta = EulerAngles[2] ;

	const VEC3 lineOfNodes = rotate<REAL>(N,alpha,T) ; // rotation around reference normal of vector T
	m_N = rotate<REAL>(lineOfNodes,gamma,N) ; // rotation around line of nodes of vector N
	m_T = rotate<REAL>(m_N,beta,lineOfNodes) ; // rotation around new normal of vector represented by line of nodes
	m_B = m_N ^ m_T ;
}

template<typename PFP>
typename Geom::Vector<3,typename PFP::REAL> LocalFrame<PFP>::getCompressed() const
{
	VEC3 EulerAngles ;

	const VEC3 T(Tx,Ty,Tz) ;
	const VEC3 B(Bx,By,Bz) ;
	const VEC3 N(Nx,Ny,Nz) ;

	REAL& alpha = EulerAngles[0] ;
	REAL& gamma = EulerAngles[1] ;
	REAL& beta = EulerAngles[2] ;

	VEC3 lineOfNodes = N ^ m_N ;
	lineOfNodes.normalize() ;

	// angle between reference T and line of nodes
	alpha = (B*lineOfNodes > 0 ? 1 : -1) * std::acos(std::max(std::min(REAL(1.0), T*lineOfNodes ),REAL(-1.0))) ;
	// angle between line of nodes and T
	gamma = std::acos(std::max(std::min(REAL(1.0), N*m_N ),REAL(-1.0))) ; // beta is always positive because the direction of vector lineOfNodes=(reference normal)^(normal) (around which a rotation of angle beta is done later on) changes depending on the side on which they lay w.r.t eachother.
	// angle between reference normal and normal
	beta = (m_B*lineOfNodes > 0 ? -1 : 1) * std::acos(std::max(std::min(REAL(1.0), m_T*lineOfNodes ),REAL(-1.0))) ;

	return EulerAngles ;
}

template<typename PFP>
bool LocalFrame<PFP>::equals(const Utils::LocalFrame<PFP>& lf, REAL epsilon) const
{
	VEC3 dT = m_T - lf.getT() ;
	VEC3 dB = m_B - lf.getB() ;
	VEC3 dN = m_N - lf.getN() ;

	return dT.norm2() < epsilon && dB.norm2() < epsilon && dN.norm2() < epsilon ;
}

template<typename PFP>
bool LocalFrame<PFP>::operator==(const LocalFrame<PFP>& lf) const
{
	return this->equals(lf) ;
}

template<typename PFP>
bool LocalFrame<PFP>::operator!=(const LocalFrame<PFP>& lf) const
{
	return !(this->equals(lf)) ;
}

template<typename PFP>
bool LocalFrame<PFP>::isDirect(REAL epsilon) const
{
	VEC3 new_B = m_N ^ m_T ;		// direct
	VEC3 diffs = new_B - m_B ;		// differences with existing B
	REAL diffNorm = diffs.norm2() ;	// Norm of this differences vector

	return (diffNorm < epsilon) ;		// Verify that this difference is very small
}

template<typename PFP>
bool LocalFrame<PFP>::isOrthogonal(REAL epsilon) const
{
	return (fabs(m_T * m_B) < epsilon) && (fabs(m_N * m_B) < epsilon) && (fabs(m_T * m_N) < epsilon) ;
}

template<typename PFP>
bool LocalFrame<PFP>::isNormalized(REAL epsilon) const
{
	return 		(1-epsilon < m_N.norm2() && m_N.norm2() < 1+epsilon)
			&& 	(1-epsilon < m_T.norm2() && m_T.norm2() < 1+epsilon)
			&&	(1-epsilon < m_B.norm2() && m_B.norm2() < 1+epsilon) ;
}

template<typename PFP>
bool LocalFrame<PFP>::isOrthoNormalDirect(REAL epsilon) const
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

} // Utils

} // CGoGN
