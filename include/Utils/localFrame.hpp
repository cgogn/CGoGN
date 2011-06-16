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
	// get known data
	const REAL& thetaN = compressedFrame[0] ;
	const REAL& phiN = compressedFrame[1] ;
	const REAL& thetaT = compressedFrame[2] ;

	// compute phiT
	const REAL quot = std::tan(phiN)*(std::cos(thetaN)*std::cos(thetaT) + std::sin(thetaN)*std::sin(thetaT)) ; // Based on orthogonality
	REAL phiT = -std::atan(1/quot) ; // if quot==0, atan returns Pi/2
	if (phiT < 0.0) {
		phiT += M_PI ; // = Pi - |phiT|
//		std::cout << compressedFrame << phiT << std::endl ;
//		std::cout << "New phiT = " << -std::atan(1/Den) << std::endl ;
	}

	VEC2 Nspher(thetaN,phiN) ;
	VEC2 Tspher(thetaT,phiT) ;

	// convert to carthesian
	m_N = sphericalToCarth(Nspher) ;
	m_T = sphericalToCarth(Tspher) ;

	// compute B
	m_B = m_N ^ m_T ;
}

template<typename PFP>
typename PFP::VEC3 LocalFrame<PFP>::getCompressed() const
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
	std::cout << "Original phiT: " << Tspher[1] << std::endl ;

	return res ;
}

template<typename PFP>
bool LocalFrame<PFP>::equals(const Utils::LocalFrame<PFP>& lf, REAL epsilon) const
{
	VEC3 dT = m_T - lf.getT() ;
	VEC3 dB = m_B - lf.getB() ;
	VEC3 dN = m_N - lf.getN() ;

	if (dT.norm2() > epsilon)
		std::cout << dT.norm2() << std::endl ;
	if (dB.norm2() > epsilon)
		std::cout << dB.norm2() << std::endl ;

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
	return 		(1-1e-5 < m_N.norm2() && m_N.norm2() < epsilon)
			&& 	(1-1e-5 < m_T.norm2() && m_T.norm2() < epsilon)
			&&	(1-1e-5 < m_B.norm2() && m_B.norm2() < epsilon) ;
}

template<typename PFP>
bool LocalFrame<PFP>::isOrthoNormalDirect(REAL epsilon) const
{
	return isOrthogonal(epsilon) && isNormalized(epsilon) && isDirect(epsilon) ;
}

template<typename PFP>
typename Geom::Vector<2,typename PFP::REAL> LocalFrame<PFP>::carthToSpherical (const VEC3& carth) const
{
	VEC2 res ;

	const REAL& x = carth[0] ;
	const REAL& y = carth[1] ;
	const REAL& z = carth[2] ;

	REAL theta = ((y < 0) ? -1 : 1) * std::acos(x / REAL(sqrt(x*x + y*y)) )  ;
	if (isnan(theta))
		theta = 0.0 ;

	REAL phi = std::acos(z) ;

	res[0] = theta ;
	res[1] = phi ;

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
