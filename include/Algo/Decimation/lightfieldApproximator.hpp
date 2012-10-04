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
 *                      LIGHTFIELD QUADRIC METRIC : frame                           *
 ************************************************************************************/

template <typename PFP>
void Approximator_FrameHalf<PFP>::approximate(Dart d)
{
	this->m_approx[d] = this->m_attrV[d] ;
}

/************************************************************************************
 *                      LIGHTFIELD QUADRIC METRIC : functions                       *
 ************************************************************************************/

template <typename PFP>
bool Approximator_RGBfunctionsHalf<PFP>::init()
{
	// get actual frames and hypothetical approximated frames
	m_frame = this->m_map.template getAttribute<MATRIX33, VERTEX>("frame") ;
	m_approxFrame = this->m_map.template getAttribute<MATRIX33, EDGE>("approx_frame") ;
	m_quadricRGBfunctions = this->m_map.template getAttribute<QuadricRGBfunctions<REAL>, EDGE>("quadricRGBfunctions") ;

	MapBrowserLinked<typename PFP::MAP> mb(this->m_map) ;
	this->m_map.template foreach_orbit<EDGE>(mb) ;

	// create quadric embedding for computing and set them to 0
	for (Dart d = mb.begin() ; d != mb.end() ; mb.next(d))
		m_quadricRGBfunctions[d].zero() ;

	// Check on embeddings
	if (!m_frame.isValid() || !m_approxFrame.isValid() || !m_quadricRGBfunctions.isValid())
	{
		CGoGNerr << "Approximator_RGBfunctions::init() --> No approxPosition or no quadricRGBfunctions specified" << CGoGNendl ;
		return false ;
	}

	return true ;
}

template <typename PFP>
void Approximator_RGBfunctionsHalf<PFP>::approximate(Dart d)
{
	MAP& m = this->m_map ;
	Dart dd = m.phi2(d) ;	// get the two vertices

	// Approximation
	this->m_approx[d] = this->m_attrV[d] ;

	// Compute quadrics for error evaluation
	// get hypothetical local frames
	VEC3 i,n ;

	m_approxFrame[d].getSubVectorH(0,0,i) ;
	m_approxFrame[d].getSubVectorH(2,0,n) ;

	// Get previous local frames
	VEC3 n1,n2,i1,i2,j1,j2 ;

	m_frame[d].getSubVectorH(0,0,i1) ;
	m_frame[dd].getSubVectorH(0,0,i2) ;

	m_frame[d].getSubVectorH(1,0,j1) ;
	m_frame[dd].getSubVectorH(1,0,j2) ;

	m_frame[d].getSubVectorH(2,0,n1) ;
	m_frame[dd].getSubVectorH(2,0,n2) ;

	// Compute j1' and j2'
	VEC3 j1pr = n1 ^ i ;
	j1pr.normalize() ;
	VEC3 j2pr = n2 ^ i ;
	j2pr.normalize() ;

	// Rotation dans sens trigo dans le plan tangent autour de n (i1->i)
//	REAL gamma1 = ((j1 * i) > 0 ? 1 : -1) * acos( std::max(std::min(1.0f, i1 * i ), -1.0f)) ; // angle positif ssi
	REAL gamma2 = ((j2 * i) > 0 ? 1 : -1) * acos( std::max(std::min(1.0f, i2 * i ), -1.0f)) ; // -PI/2 < angle(i,j1) < PI/2  ssi i*j1 > 0
	// Rotation dans le sens trigo autour de l'axe i (n1->n)
//	REAL phi2_1 = ((n * j1pr) > 0 ? -1 : 1) * acos( std::max(std::min(1.0f, n * n1), -1.0f) ) ; // angle positif ssi
	REAL alpha2 = ((n * j2pr) > 0 ? -1 : 1) * acos( std::max(std::min(1.0f, n * n2), -1.0f) ) ; // PI/2 < angle(j1',n) < -PI/2 ssi j1'*n < 0

//	assert (-0.01 < gamma1 && gamma1 < 0.01) ;
//	assert (-0.01 < phi2_1 && phi2_1 < 0.01) ;
	REAL gamma1 = REAL(0) ;
	REAL phi2_1 = REAL(0) ;

	// Create and sum quadrics
	m_quadricRGBfunctions[d] += QuadricRGBfunctions<REAL>(this->m_attrV[d], gamma1, phi2_1) ;
	m_quadricRGBfunctions[d] += QuadricRGBfunctions<REAL>(this->m_attrV[dd], gamma2, alpha2) ;
}

/************************************************************************************
 *                      LIGHTFIELD QUADRIC METRIC : frame                           *
 ************************************************************************************/

template <typename PFP>
bool Approximator_Frame<PFP>::init()
{
	m_position = this->m_map.template getAttribute<VEC3, VERTEX>("position") ;
	m_approxPosition = this->m_map.template getAttribute<VEC3, EDGE>("approx_position") ;

	if (!m_position.isValid() || !m_approxPosition.isValid())
	{
		CGoGNerr << "Approximator_Frame::init() --> No approxPosition or no quadricRGBfunctions specified" << CGoGNendl ;
		return false ;
	}
	return true ;
}

template <typename PFP>
void Approximator_Frame<PFP>::approximate(Dart d)
{
	MAP& m = this->m_map ;
	Dart dd = m.phi2(d) ;	// get 2 vertices

	// if QEM placed new vertex at one of the previous : place same frame
	if (this->m_approxPosition[d] == this->m_position[d]) // new Position is position of vertex d
		this->m_approx[d] = this->m_attrV[d] ;
	else if (this->m_approxPosition[d] == this->m_position[dd]) // new Position is position of vertex dd
		this->m_approx[d] = this->m_attrV[dd] ;
	else {
		// Create two segments : v0-v1 and v0-v
		VEC3 segment = this->m_position[dd] ;
		segment -= this->m_position[d] ;

		VEC3 segmentNew = m_approxPosition[d] ;
		segmentNew -= this->m_position[d] ;

		// Orthogonal projection of v0-v onto v0-v1 : get coefficient t
		REAL t = (segment * segmentNew) / segment.norm() ;
		t = std::max (std::min (t , REAL(1)) , REAL(0) ) ; // clamp it to [0,1]

		VEC3 n1, n2 ;
		this->m_attrV[d].getSubVectorH(2, 0, n1) ; // get normal n1
		this->m_attrV[dd].getSubVectorH(2, 0, n2) ; // get normal n2

		VEC3 newN = slerp(n1,n2,t) ; // spherical interpolation
		newN.normalize() ;

		VEC3 newI = n2 ^ n1 ; // i is perpendicular to newNormal
		newI.normalize() ;

		VEC3 newJ = newN ^ newI ; // (i,j,n) is a direct frame
		newJ.normalize() ;

		// save into m_approx
		if (!this->m_approx[d].setSubVectorH(0,0,newI) ||
			!this->m_approx[d].setSubVectorH(1,0,newJ) ||
			!this->m_approx[d].setSubVectorH(2,0,newN) )
			assert(!"Approximator_Frame::approximate") ;
	}

}

/************************************************************************************
 *                      LIGHTFIELD QUADRIC METRIC : functions                       *
 ************************************************************************************/

template <typename PFP>
bool Approximator_RGBfunctions<PFP>::init()
{
	// get actual frames and hypothetical approximated frames
	m_frame = this->m_map.template getAttribute<MATRIX33, VERTEX>("frame") ;
	m_approxFrame = this->m_map.template getAttribute<MATRIX33, EDGE>("approx_frame") ;
	m_quadricRGBfunctions = this->m_map.template getAttribute<QuadricRGBfunctions<REAL>, EDGE>("quadricRGBfunctions") ;

	MapBrowserLinked<typename PFP::MAP> mb(this->m_map) ;
	this->m_map.template foreach_orbit<EDGE>(mb) ;

	// create quadric embedding for computing and set them to 0
	for (Dart d = mb.begin() ; d != mb.end() ; mb.next(d))
		m_quadricRGBfunctions[d].zero() ;

	// Check on embeddings
	if (!m_frame.isValid() || !m_approxFrame.isValid() || !m_quadricRGBfunctions.isValid())
	{
		CGoGNerr << "Approximator_RGBfunctions::init() --> No approxPosition or no quadricRGBfunctions specified" << CGoGNendl ;
		return false ;
	}
	return true ;
}

template <typename PFP>
void Approximator_RGBfunctions<PFP>::approximate(Dart d)
{
	MAP& m = this->m_map ;
	Dart dd = m.phi1(d) ;	// get the two vertices

	// get hypothetical local frames
	VEC3 i,n ;

	m_approxFrame[d].getSubVectorH(0,0,i) ;
	m_approxFrame[d].getSubVectorH(2,0,n) ;

	// Get previous local frames
	VEC3 n1,n2,i1,i2,j1,j2 ;

	m_frame[d].getSubVectorH(0,0,i1) ;
	m_frame[dd].getSubVectorH(0,0,i2) ;

	m_frame[d].getSubVectorH(1,0,j1) ;
	m_frame[dd].getSubVectorH(1,0,j2) ;

	m_frame[d].getSubVectorH(2,0,n1) ;
	m_frame[dd].getSubVectorH(2,0,n2) ;

	// Compute j1' and j2'
	VEC3 j1pr = n1 ^ i ;
	j1pr.normalize() ;
	VEC3 j2pr = n2 ^ i ;
	j2pr.normalize() ;

	// Rotation dans sens trigo dans le plan tangent autour de n (i1->i)
	REAL gamma1 = ((j1 * i) > 0 ? 1 : -1) * acos( std::max(std::min(1.0f, i1 * i ), -1.0f)) ; // angle positif ssi
	REAL gamma2 = ((j2 * i) > 0 ? 1 : -1) * acos( std::max(std::min(1.0f, i2 * i ), -1.0f)) ; // -PI/2 < angle(i,j1) < PI/2  ssi i*j1 > 0
	// Rotation dans le sens trigo autour de l'axe i (n1->n)
	REAL phi2_1 = ((n * j1pr) > 0 ? -1 : 1) * acos( std::max(std::min(1.0f, n * n1), -1.0f) ) ; // angle positif ssi
	REAL alpha2 = ((n * j2pr) > 0 ? -1 : 1) * acos( std::max(std::min(1.0f, n * n2), -1.0f) ) ; // PI/2 < angle(j1',n) < -PI/2 ssi j1'*n < 0

//	assert (-3.15 < gamma1 && gamma1 <= 3.15) ;
//	assert (-3.15 < gamma2 && gamma2 <= 3.15) ;
//	assert (-3.15 < phi2_1 && phi2_1 <= 3.15) ;
//	assert (-3.15 < alpha2 && alpha2 <= 3.15) ;

//	MATRIX36 &f1 = this->m_attrV[d] ;
//	MATRIX36 &f2 = this->m_attrV[dd] ;

	// Create and sum quadrics
	m_quadricRGBfunctions[d].zero() ;
	m_quadricRGBfunctions[d] += QuadricRGBfunctions<REAL>(this->m_attrV[d], gamma1, phi2_1) ;
	m_quadricRGBfunctions[d] += QuadricRGBfunctions<REAL>(this->m_attrV[dd], gamma2, alpha2) ;

	// Compute new function
	if (! m_quadricRGBfunctions[d].findOptimizedRGBfunctions(this->m_approx[d])) {
		this->m_approx[d] = this->m_attrV[d]; // if fail take first one
	}
//	MATRIX36 &newF = this->m_approx[d] ;
//	CGoGNout << "Approx of : " <<CGoGNendl ;
//	CGoGNout << "Frame1 : " << m_frame[d] << CGoGNendl ;
//	CGoGNout << "Function1 : "<< this->m_attrV[d] << CGoGNendl ;
//	CGoGNout << "gamma1 : " << gamma1 << CGoGNendl ;
//	CGoGNout << "Frame2 : " << m_frame[dd] << CGoGNendl ;
//	CGoGNout << "Function2 : "<< this->m_attrV[dd] << CGoGNendl ;
//	CGoGNout << "gamma2 : " << gamma2 << CGoGNendl ;
//	CGoGNout << "is " << CGoGNendl ;
//	CGoGNout << "Frame :" << m_approxFrame[d] << CGoGNendl ;
//	CGoGNout << "Function : " << this->m_approx[d] << CGoGNendl ;
//	CGoGNout << CGoGNendl ;
}

} //namespace Decimation

} //namespace Algo

} //namespace CGoGN
