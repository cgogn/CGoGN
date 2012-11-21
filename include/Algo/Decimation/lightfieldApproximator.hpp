/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2012, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

namespace CGoGN
{

namespace Algo
{

namespace Decimation
{

/************************************************************************************
 *                         FRAME INTERPOLATION APPROXIMATOR                         *
 ************************************************************************************/

template <typename PFP>
void Approximator_FrameInterpolation<PFP>::approximate(Dart d)
{
	const Dart dd = this->m_map.phi2(d) ;

	// if QEM placed new vertex at one of the previous : place same frame
//	if (this->m_approxposition[d] == this->m_position[d]) // new Position is position of vertex d
//	{
//		for (unsigned int i = 0 ; i < 3 ; ++i)
//			this->m_approx[i][d] = this->m_attrV[i]->operator[](d) ;
//		//std::cout << "fallback to p1" << std::endl ;
//	}
//	else if (this->m_approxposition[d] == this->m_position[dd]) // new Position is position of vertex dd
//	{
//		for (unsigned int i = 0 ; i < 3 ; ++i)
//			this->m_approx[i][d] = this->m_attrV[i]->operator[](dd) ;
//		// std::cout << "fallback to p2" << std::endl ;
//	}
//	else
	{
		// Create two segments : v0-v1 and v0-v
		VEC3 v0v1 = this->m_position[dd] ;
		v0v1 -= this->m_position[d] ;

		VEC3 v0v = this->m_approxposition[d] ;
		v0v -= this->m_position[d] ;

		// Orthogonal projection of v0-v onto v0-v1 : get coefficient t
		REAL t = this->m_position[d] == this->m_position[dd] ? 1. : (v0v1 * v0v) / v0v1.norm() ;
		t = std::max (std::min (t , REAL(1)) , REAL(0) ) ; // clamp it to [0,1]

		VEC3& normal1 = this->m_attrV[2]->operator[](d) ;
		VEC3& normal2 = this->m_attrV[2]->operator[](dd) ;

		//Geom::Vector<3,double> n1 = normal1 ;
		//Geom::Vector<3,double> n2 = normal2 ;

		VEC3 newN = slerp(normal1,normal2,t) ; // spherical interpolation
		newN.normalize() ;
		// assert (!isnan(newN[0])) ;

		VEC3 newT ;
		if (normal2 != normal1)
			newT = normal2 ^ normal1 ; // i is perpendicular to newNormal
		else
			newT = normal1 ^ VEC3(1,2,3) ; // second random vector
		newT.normalize() ;

		VEC3 newB = newN ^ newT ; // (i,j,n) is a direct frame
		newB.normalize() ;

		// save into m_approx
		this->m_approx[0][d] = newT ;
		this->m_approx[1][d] = newB ;
		this->m_approx[2][d] = newN ;
	}
}

/************************************************************************************
 *                      HEMIFUNCTION COEFS APPROXIMATOR                             *
 ************************************************************************************/

template <typename PFP>
bool Approximator_HemiFuncCoefs<PFP>::init()
{
	// get frames
	m_frameT = this->m_map.template getAttribute<VEC3, VERTEX>("frameT") ;
	m_frameB = this->m_map.template getAttribute<VEC3, VERTEX>("frameB") ;
	m_frameN = this->m_map.template getAttribute<VEC3, VERTEX>("frameN") ;

	assert((m_frameT.isValid() && m_frameB.isValid() && m_frameN.isValid()) || !"Frame embeddings are not set") ;

	m_newFrameT = this->m_map.template getAttribute<VEC3, EDGE>("approx_frameT") ;
	m_newFrameB = this->m_map.template getAttribute<VEC3, EDGE>("approx_frameB") ;
	m_newFrameN = this->m_map.template getAttribute<VEC3, EDGE>("approx_frameN") ;

	assert((m_newFrameT.isValid() && m_newFrameB.isValid() && m_frameN.isValid())
			|| !"New frame embeddings are not set") ;

	if(this->m_predictor)
	{
		return false ;
	}

	return m_frameT.isValid() && m_frameB.isValid() && m_frameN.isValid() && m_newFrameT.isValid() && m_newFrameB.isValid() && m_newFrameN.isValid() && (m_nbCoefs > 0) ;
}

template <typename PFP>
void Approximator_HemiFuncCoefs<PFP>::approximate(Dart d)
{
	const Dart& dd = this->m_map.phi1(d) ;

	// get coefs of v1 and v2
	std::vector<VEC3> coefs1, coefs2 ;
	coefs1.resize(m_nbCoefs) ; coefs2.resize(m_nbCoefs) ;
	for (unsigned int i = 0 ; i < m_nbCoefs ; ++i)
	{
		coefs1[i] = m_coefs[i]->operator[](d) ;
		coefs2[i] = m_coefs[i]->operator[](dd) ;
	}

	const VEC3& T = m_newFrameT[d] ;
	const VEC3& T1 = m_frameT[d] ;
	const VEC3& T2 = m_frameT[dd] ;
	const VEC3& B1 = m_frameB[d] ;
	const VEC3& B2 = m_frameB[dd] ;
	const VEC3& N = m_newFrameN[d] ;
	const VEC3& N1 = m_frameN[d] ;
	const VEC3& N2 = m_frameN[dd] ;

	//assert(abs(T * N1) < 1e-6 || !"Approximator_FrameInterpolation<PFP>::approximate: T is not located in the first tangent plane") ;
	//assert(abs(T * N2) < 1e-6 || !"Approximator_FrameInterpolation<PFP>::approximate: T is not located in the second tangent plane") ;

	// Compute D1' and D2'
	VEC3 B1prime = N1 ^ T ;
	B1prime.normalize() ;
	VEC3 B2prime = N2 ^ T ;
	B2prime.normalize() ;

	// Rotation dans sens trigo dans le plan tangent autour de N (T1 --> T)
	const REAL gamma1 = ((B1 * T) > 0 ? 1 : -1) * acos( std::max(std::min(1.0f, T1 * T ), -1.0f)) ; // angle positif ssi
	const REAL gamma2 = ((B2 * T) > 0 ? 1 : -1) * acos( std::max(std::min(1.0f, T2 * T ), -1.0f)) ; // -PI/2 < angle(i,j1) < PI/2  ssi i*j1 > 0
	// Rotation dans le sens trigo autour de l'axe T (N1 --> N)
	const REAL alpha1 = ((N * B1prime) > 0 ? -1 : 1) * acos( std::max(std::min(1.0f, N * N1), -1.0f) ) ; // angle positif ssi
	const REAL alpha2 = ((N * B2prime) > 0 ? -1 : 1) * acos( std::max(std::min(1.0f, N * N2), -1.0f) ) ; // PI/2 < angle(j1',n) < -PI/2 ssi j1'*n < 0

	assert (m_quadricHF.isValid() || !"LightfieldApproximator:approximate quadricHF is not valid") ;

//	if (T == T1)
//	{
//		// std::cout << "Fallback to p1" << std::endl ;
//
//		m_quadricHF[d] = Utils::QuadricHF<REAL>(coefs1, gamma1, 0) ;
//		m_quadricHF[d] += Utils::QuadricHF<REAL>(coefs2, gamma2, alpha2 - alpha1) ;
//
//		for (unsigned int i = 0 ; i < m_nbCoefs ; ++i)
//			this->m_approx[i][d] = m_coefs[i]->operator[](d) ;
//	}
//	else if (T == T2)
//	{
//		// std::cout << "Fallback to p2" << std::endl ;
//
//		m_quadricHF[d] = Utils::QuadricHF<REAL>(coefs1, gamma1, alpha1 - alpha2) ;
//		m_quadricHF[d] += Utils::QuadricHF<REAL>(coefs2, gamma2, 0) ;
//
//		for (unsigned int i = 0 ; i < m_nbCoefs ; ++i)
//			this->m_approx[i][d] = m_coefs[i]->operator[](dd) ;
//	}
//	else
	{
//		std::cout << "N1: " << N1 << std::endl ;
//		std::cout << "N2: " << N2 << std::endl ;
//		std::cout << "N: " << N << std::endl ;
//		std::cout << "T: " << T << std::endl ;
//		std::cout << "T1: " << T1 << std::endl ;
//		std::cout << "T2: " << T2 << std::endl ;
//		std::cout << "gamma1 " << gamma1 << std::endl ;
//		std::cout << "gamma2 " << gamma2 << std::endl ;
//		std::cout << "alpha1 " << alpha1 << std::endl ;
//		std::cout << "alpha2 " << alpha2 << std::endl ;
		// Create and sum quadrics
		m_quadricHF[d] = Utils::QuadricHF<REAL>(coefs1, gamma1, alpha1) ;
		m_quadricHF[d] += Utils::QuadricHF<REAL>(coefs2, gamma2, alpha2) ;

		std::vector<VEC3> coefs ;
		// Compute new function
		bool opt = m_quadricHF[d].findOptimizedCoefs(coefs) ;
		// copy result
		for (unsigned int i = 0 ; i < m_nbCoefs ; ++i)
			this->m_approx[i][d] = coefs[i] ;

		if (!opt)
			std::cerr << "LightfieldApproximator::Inversion failed: coefs are averaged" << std::endl ;
	}
}


/************************************************************************************
 *           FRAME INTERPOLATION APPROXIMATOR FOR HALF EDGE CONTRACTION             *
 ************************************************************************************/

template <typename PFP>
void Approximator_FrameInterpolationHalfEdge<PFP>::approximate(Dart d)
{
	const Dart dd = this->m_map.phi2(d) ;

	// if QEM placed new vertex at one of the previous : place same frame
//	if (this->m_approxposition[d] == this->m_position[d]) // new Position is position of vertex d
//	{
//		for (unsigned int i = 0 ; i < 3 ; ++i)
//			this->m_approx[i][d] = this->m_attrV[i]->operator[](d) ;
//		//std::cout << "fallback to p1" << std::endl ;
//	}
//	else if (this->m_approxposition[d] == this->m_position[dd]) // new Position is position of vertex dd
//	{
//		for (unsigned int i = 0 ; i < 3 ; ++i)
//			this->m_approx[i][d] = this->m_attrV[i]->operator[](dd) ;
//		// std::cout << "fallback to p2" << std::endl ;
//	}
//	else
	{
		// Create two segments : v0-v1 and v0-v
		VEC3 v0v1 = this->m_position[dd] ;
		v0v1 -= this->m_position[d] ;

		VEC3 v0v = this->m_approxposition[d] ;
		v0v -= this->m_position[d] ;

		// Orthogonal projection of v0-v onto v0-v1 : get coefficient t
		REAL t = this->m_position[d] == this->m_position[dd] ? 1. : (v0v1 * v0v) / v0v1.norm() ;
		t = std::max (std::min (t , REAL(1)) , REAL(0) ) ; // clamp it to [0,1]

		VEC3& normal1 = this->m_attrV[2]->operator[](d) ;
		VEC3& normal2 = this->m_attrV[2]->operator[](dd) ;

		// Keep normal
		VEC3 newN = normal1 ;
		// assert (!isnan(newN[0])) ;

		// but do the rotation in the tangent plane
		VEC3 newT ;
		if (normal2 != normal1)
			newT = normal2 ^ normal1 ; // i is perpendicular to newNormal
		else
			newT = normal1 ^ VEC3(1,2,3) ; // second random vector
		newT.normalize() ;

		VEC3 newB = newN ^ newT ; // (i,j,n) is a direct frame
		newB.normalize() ;

		// save into m_approx
		this->m_approx[0][d] = newT ;
		this->m_approx[1][d] = newB ;
		this->m_approx[2][d] = newN ;
	}
}

/************************************************************************************
 *            HEMIFUNCTION COEFS APPROXIMATOR FOR HALF EDGE CONTRACTION             *
 ************************************************************************************/

template <typename PFP>
bool Approximator_HemiFuncCoefsHalfEdge<PFP>::init()
{
	// get frames
	m_frameT = this->m_map.template getAttribute<VEC3, VERTEX>("frameT") ;
	m_frameB = this->m_map.template getAttribute<VEC3, VERTEX>("frameB") ;
	m_frameN = this->m_map.template getAttribute<VEC3, VERTEX>("frameN") ;

	assert((m_frameT.isValid() && m_frameB.isValid() && m_frameN.isValid()) || !"Frame embeddings are not set") ;

	m_newFrameT = this->m_map.template getAttribute<VEC3, DART>("approx_frameT") ;
	m_newFrameB = this->m_map.template getAttribute<VEC3, DART>("approx_frameB") ;
	m_newFrameN = this->m_map.template getAttribute<VEC3, DART>("approx_frameN") ;

	assert((m_newFrameT.isValid() && m_newFrameB.isValid() && m_frameN.isValid())
			|| !"New frame embeddings are not set") ;

	if(this->m_predictor)
	{
		return false ;
	}

	return m_frameT.isValid() && m_frameB.isValid() && m_frameN.isValid() && m_newFrameT.isValid() && m_newFrameB.isValid() && m_newFrameN.isValid() && (m_nbCoefs > 0) ;
}

template <typename PFP>
void Approximator_HemiFuncCoefsHalfEdge<PFP>::approximate(Dart d)
{
	const Dart& dd = this->m_map.phi1(d) ;

	// get coefs of v1 and v2
	std::vector<VEC3> coefs1, coefs2 ;
	coefs1.resize(m_nbCoefs) ; coefs2.resize(m_nbCoefs) ;
	for (unsigned int i = 0 ; i < m_nbCoefs ; ++i)
	{
		coefs1[i] = m_coefs[i]->operator[](d) ;
		coefs2[i] = m_coefs[i]->operator[](dd) ;
	}

	const VEC3& T = m_newFrameT[d] ;
	const VEC3& T1 = m_frameT[d] ;
	const VEC3& T2 = m_frameT[dd] ;
	const VEC3& B1 = m_frameB[d] ;
	const VEC3& B2 = m_frameB[dd] ;
	const VEC3& N = m_newFrameN[d] ;
	const VEC3& N1 = m_frameN[d] ;
	const VEC3& N2 = m_frameN[dd] ;

	//assert(abs(T * N1) < 1e-6 || !"Approximator_FrameInterpolation<PFP>::approximate: T is not located in the first tangent plane") ;
	//assert(abs(T * N2) < 1e-6 || !"Approximator_FrameInterpolation<PFP>::approximate: T is not located in the second tangent plane") ;

	// Compute D1' and D2'
	VEC3 B1prime = N1 ^ T ;
	B1prime.normalize() ;
	VEC3 B2prime = N2 ^ T ;
	B2prime.normalize() ;

	// Rotation dans sens trigo dans le plan tangent autour de N (T1 --> T)
	const REAL gamma1 = ((B1 * T) > 0 ? 1 : -1) * acos( std::max(std::min(1.0f, T1 * T ), -1.0f)) ; // angle positif ssi
	const REAL gamma2 = ((B2 * T) > 0 ? 1 : -1) * acos( std::max(std::min(1.0f, T2 * T ), -1.0f)) ; // -PI/2 < angle(i,j1) < PI/2  ssi i*j1 > 0
	// Rotation dans le sens trigo autour de l'axe T (N1 --> N)
	const REAL alpha1 = (N == N1) ? 0 : ((N * B1prime) > 0 ? -1 : 1) * acos( std::max(std::min(1.0f, N * N1), -1.0f) ) ; // angle positif ssi
	const REAL alpha2 = (N == N2) ? 0 : ((N * B2prime) > 0 ? -1 : 1) * acos( std::max(std::min(1.0f, N * N2), -1.0f) ) ; // PI/2 < angle(j1',n) < -PI/2 ssi j1'*n < 0

	assert (m_quadricHF.isValid() || !"LightfieldApproximator:approximate quadricHF is not valid") ;

//	if (T == T1)
//	{
//		// std::cout << "Fallback to p1" << std::endl ;
//
//		m_quadricHF[d] = Utils::QuadricHF<REAL>(coefs1, gamma1, 0) ;
//		m_quadricHF[d] += Utils::QuadricHF<REAL>(coefs2, gamma2, alpha2 - alpha1) ;
//
//		for (unsigned int i = 0 ; i < m_nbCoefs ; ++i)
//			this->m_approx[i][d] = m_coefs[i]->operator[](d) ;
//	}
//	else if (T == T2)
//	{
//		// std::cout << "Fallback to p2" << std::endl ;
//
//		m_quadricHF[d] = Utils::QuadricHF<REAL>(coefs1, gamma1, alpha1 - alpha2) ;
//		m_quadricHF[d] += Utils::QuadricHF<REAL>(coefs2, gamma2, 0) ;
//
//		for (unsigned int i = 0 ; i < m_nbCoefs ; ++i)
//			this->m_approx[i][d] = m_coefs[i]->operator[](dd) ;
//	}
//	else
	{
//		std::cout << "N1: " << N1 << std::endl ;
//		std::cout << "N2: " << N2 << std::endl ;
//		std::cout << "N: " << N << std::endl ;
//		std::cout << "T: " << T << std::endl ;
//		std::cout << "T1: " << T1 << std::endl ;
//		std::cout << "T2: " << T2 << std::endl ;
//		std::cout << "gamma1 " << gamma1 << std::endl ;
//		std::cout << "gamma2 " << gamma2 << std::endl ;
//		std::cout << "alpha1 " << alpha1 << std::endl ;
//		std::cout << "alpha2 " << alpha2 << std::endl ;

		// Add quadric for first one only (for half-edge contraction)
		m_quadricHF[d] = Utils::QuadricHF<REAL>(coefs1, gamma1, alpha1) ;

		std::vector<VEC3> coefs ;
		// Compute new function
		bool opt = m_quadricHF[d].findOptimizedCoefs(coefs) ;
		// copy result
		for (unsigned int i = 0 ; i < m_nbCoefs ; ++i)
			this->m_approx[i][d] = coefs[i] ;

		if (!opt)
		{
			std::cerr << "LightfieldApproximatorHalfCollapse::Optimization failed (should never happen since no optim is done)" << std::endl ;
			std::cout << alpha1 << std::endl ;
		}

		// Add second one for error evaluation
		m_quadricHF[d] += Utils::QuadricHF<REAL>(coefs2, gamma2, alpha2) ;
	}
}

} //namespace Decimation

} //namespace Algo

} //namespace CGoGN
