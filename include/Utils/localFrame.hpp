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
LocalFrame<PFP>::LocalFrame(const VEC3& compressedFrame)
{
	VEC2 Nspher ;
	VEC2 Tspher ;

	REAL& thetaN = Nspher[0] ; // known
	REAL& phiN = Nspher[1] ; // known
	REAL& thetaT = Tspher[0] ; // known
	REAL& phiT = Tspher[1] ; // to be decoded

	// compute phiT
	REAL Den,Nom ;
	Den = sin(phiN)*(cos(thetaN)*cos(thetaT) + sin(thetaN)*sin(thetaT)) ; // Based on orthogonality
	Nom = cos(phiN) ;
	phiT = -atan(Nom/Den) ; // if A==0, atan returns Pi/2
	if (phiT < 0.0)
		phiT = M_PI + phiT ; // = Pi - |phiT|

	// convert to carthesian
	m_N = sphericalToCarth(Nspher) ;
	m_T = sphericalToCarth(Tspher) ;

	// compute B
	m_B = m_N ^ m_T ;
}

template<typename PFP>
typename PFP::VEC3 LocalFrame<PFP>::getCompressed()
{
	VEC3 res ;

	REAL& thetaN = res[0] ;
	REAL& phiN = res[1] ;
	REAL& thetaT = res[2] ;

	// convert to spherical coordinates
	VEC2 Nspher = carthToSpherical(m_N) ;
	VEC2 Tspher = carthToSpherical(m_T) ;

	// extract the three scalars
	thetaN = Nspher[0] ;
	phiN = Nspher[1] ;
	thetaT = Tspher[0] ;

	return res ;
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
bool LocalFrame<PFP>::isDirect() const
{
	VEC3 new_B = m_N ^ m_T ;		// direct
	VEC3 diffs = new_B - m_B ;		// differences with existing B
	REAL diffNorm = diffs.norm2() ;	// Norm of this differences vector

	return (diffNorm < 1e-10) ;		// Verify that this difference is very small
}

template<typename PFP>
bool LocalFrame<PFP>::isOrthogonal() const
{
	return (fabs(m_T * m_B) < 1e-5) && (fabs(m_N * m_B) < 1e-5) && (fabs(m_T * m_N) < 1e-5) ;
}

template<typename PFP>
bool LocalFrame<PFP>::isNormalized() const
{
	return 		(1-1e-5 < m_N.norm2() && m_N.norm2() < 1+1e-5)
			&& 	(1-1e-5 < m_T.norm2() && m_T.norm2() < 1+1e-5)
			&&	(1-1e-5 < m_B.norm2() && m_B.norm2() < 1+1e-5) ;
}

template<typename PFP>
bool LocalFrame<PFP>::isOrthoNormalDirect() const
{
	return isOrthogonal() && isNormalized() && isDirect() ;
}

template<typename PFP>
typename Geom::Vector<2,typename PFP::REAL> LocalFrame<PFP>::carthToSpherical (const VEC3& carth) const
{
	VEC2 res ;

	const REAL& x = carth[0] ;
	const REAL& y = carth[1] ;
	const REAL& z = carth[2] ;

	REAL& theta = res[0] ;
	REAL& phi = res[1] ;

	phi = acos(z) ;
	const REAL sinphi = sin(phi) ;
	if (sinphi == 0.0)
		theta = 0.0 ;
	else
		theta = ((y > 0) ? 1 : -1) * acos(std::min(REAL(1.0),std::max(REAL(-1.0),x / sinphi))) ;

	assert (-(M_PI+0.000001) <= theta && theta <= M_PI+0.000001) ;
	assert (-0.000001 < phi && phi <= M_PI+0.000001) ;
	assert (!isnan(theta) || !"carthToSpherical : Theta is NaN !") ;
	assert (!isnan(phi) || !"carthToSpherical : Phi is NaN !") ;

	return res ;
}

template<typename PFP>
typename PFP::VEC3 LocalFrame<PFP>::sphericalToCarth (const VEC2& sph) const
{
	VEC3 res ;

	const REAL& theta = sph[0] ;
	const REAL& phi = sph[1] ;

	REAL& x = res[0] ;
	REAL& y = res[1] ;
	REAL& z = res[2] ;

	x = cos(theta)*sin(phi) ;
	y = sin(theta)*sin(phi) ;
	z = cos(phi) ;

	assert(-1.000001 < x && x < 1.000001) ;
	assert(-1.000001 < y && y < 1.000001) ;
	assert(-1.000001 < z && z < 1.000001) ;

	return res ;
}

} // Utils

} // CGoGN
