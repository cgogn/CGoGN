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
	if (this->m_approxposition[d] == this->m_position[d]) // new Position is position of vertex d
	{
		for (unsigned int i = 0 ; i < 3 ; ++i)
			this->m_approx[i][d] = this->m_attrV[i]->operator[](d) ;
	}
	else if (this->m_approxposition[d] == this->m_position[dd]) // new Position is position of vertex dd
	{
		for (unsigned int i = 0 ; i < 3 ; ++i)
			this->m_approx[i][d] = this->m_attrV[i]->operator[](dd) ;
	}
	else
	{
		// Create two segments : v0-v1 and v0-v
		VEC3 v0v1 = this->m_position[dd] ;
		v0v1 -= this->m_position[d] ;

		VEC3 v0v = this->m_approxposition[d] ;
		v0v -= this->m_position[d] ;

		// Orthogonal projection of v0-v onto v0-v1 : get coefficient t
		REAL t = (v0v1 * v0v) / v0v1.norm() ;
		t = std::max (std::min (t , REAL(1)) , REAL(0) ) ; // clamp it to [0,1]

		const VEC3& normal1 = this->m_attrV[2]->operator[](d) ;
		const VEC3& normal2 = this->m_attrV[2]->operator[](dd) ;

		VEC3 newN = slerp(normal1,normal2,t) ; // spherical interpolation
		newN.normalize() ;

		VEC3 newT = normal2 ^ normal1 ; // i is perpendicular to newNormal
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

	// get quadric
	m_quadricHF = this->m_map.template getAttribute<QuadricHF<REAL>, VERTEX>("HFquadric") ;
	// Does not require to be valid (if it is not, altenatives will be used).

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

	QuadricHF<REAL> q ;
	if (m_quadricHF.isValid()) // if the selector does provide a quadricHF
	{
		q = m_quadricHF[d] ;
		q += m_quadricHF[dd] ;
	}
	else // the selector does not provide a quadricHF
	{
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
		const VEC3& B1 = m_newFrameB[d] ;
		const VEC3& B2 = m_newFrameB[d] ;
		const VEC3& N = m_frameN[d] ;
		const VEC3& N1 = m_newFrameN[d] ;
		const VEC3& N2 = m_newFrameN[dd] ;

		assert(T * (N1 ^ N2) > 0.99 || !"T is not an intersection of the two tangent planes") ;

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

		// Create and sum quadrics
		q = QuadricHF<REAL>(coefs1, gamma1, alpha1) ;
		q += QuadricHF<REAL>(coefs2, gamma2, alpha2) ;
	}

	std::vector<VEC3> coefs ;
	// Compute new function
	bool opt = q.findOptimizedCoefs(coefs) ;
	// copy result
	for (unsigned int i = 0 ; i < m_nbCoefs ; ++i)
		this->m_approx[i][d] = opt ? (m_coefs[i]->operator[](d) + m_coefs[i]->operator[](dd))/2 : m_coefs[i]->operator[](d) ; // if fail take first one
	assert(false || !"TODO: what todo when fail") ;
}


} //namespace Decimation

} //namespace Algo

} //namespace CGoGN
