/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009, IGG Team, LSIIT, University of Strasbourg                *
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
* Web site: https://iggservis.u-strasbg.fr/CGoGN/                              *
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
bool Approximator_Frame<PFP>::init()
{
	m_position = this->m_map.template getAttribute<VEC3>(VERTEX_ORBIT, "position") ;
	m_approxPosition = this->m_map.template getAttribute<VEC3>(EDGE_ORBIT, "approx_position") ;

	if (!m_position.isValid() || !m_approxPosition.isValid())
	{
		std::cerr << "Approximator_Frame::init() --> No approxPosition or no quadricRGBfunctions specified" << std::endl ;
		return false ;
	}
	return true ;
}

template <typename PFP>
void Approximator_Frame<PFP>::approximate(Dart d)
{
	MAP& m = this->m_map ;
	Dart dd = m.phi2(d) ;	// get some darts

	if (this->m_approxPosition[d] == this->m_position[d]) // new Position is position of vertex d
		this->m_approx[d] = this->m_attrV[d] ;
	else if (this->m_approxPosition[d] == this->m_position[dd]) // new Position is position of vertex dd
		this->m_approx[d] = this->m_attrV[dd] ;
	else {
		VEC3 segment = this->m_position[dd] ;
		segment -= this->m_position[d] ;

		VEC3 segmentNew = m_approxPosition[d] ;
		segment -= this->m_position[d] ;

		REAL t = std::max (std::min (segment * segmentNew , REAL(1)) , REAL(0) ) ; // Orthogonal projection on segment v1-v2 of new vertex


		VEC3 n1, n2 ;
		this->m_attrV[d].getSubVectorH(3, 1, n1) ;
		this->m_attrV[dd].getSubVectorH(3, 1, n2) ;

		VEC3 newN = slerp(n1,n2,t) ; // spherical interpolation
		newN.normalize() ;

		VEC3 newI = n1 ^ n2 ; // i is perpendicular to newNormal
		newI.normalize() ;

		VEC3 newJ = newN ^ newI ;
		newJ.normalize() ;

		this->m_approx[d].setSubVectorH(1,1,newI) ;
		this->m_approx[d].setSubVectorH(2,1,newJ) ;
		this->m_approx[d].setSubVectorH(3,1,newN) ;
	}
}

/************************************************************************************
 *                      LIGHTFIELD QUADRIC METRIC : functions                       *
 ************************************************************************************/

template <typename PFP>
bool Approximator_RGBfunctions<PFP>::init()
{
	m_frame = this->m_map.template getAttribute<MATRIX33>(VERTEX_ORBIT, "frame") ;
	m_approxFrame = this->m_map.template getAttribute<MATRIX33>(EDGE_ORBIT, "approx_frame") ;
	m_quadricRGBfunctions = this->m_map.template getAttribute<QuadricRGBfunctions<REAL> >(EDGE_ORBIT, "QuadricRGBfunctions") ;

	if (!m_frame.isValid() || !m_approxFrame.isValid() || !m_quadricRGBfunctions.isValid())
	{
		std::cerr << "Approximator_RGBfunctions::init() --> No approxPosition or no quadricRGBfunctions specified" << std::endl ;
		return false ;
	}
	return true ;
}

template <typename PFP>
void Approximator_RGBfunctions<PFP>::approximate(Dart d)
{
	MAP& m = this->m_map ;
	Dart dd = m.phi2(d) ;	// get some darts

	VEC3 i,n ;
	VEC3 n1,n2,i1,i2,j1,j2 ;
	m_approxFrame[d].getSubVectorH(1,1,i) ;
	m_approxFrame[d].getSubVectorH(3,1,n) ;

	m_frame[d].getSubVectorH(1,1,i1) ;
	m_frame[dd].getSubVectorH(1,1,i2) ;

	m_frame[d].getSubVectorH(2,1,j1) ;
	m_frame[dd].getSubVectorH(2,1,j2) ;

	m_frame[d].getSubVectorH(3,1,n1) ;
	m_frame[dd].getSubVectorH(3,1,n2) ;

	VEC3 j1pr = n1 ^ i ;
	VEC3 j2pr = n2 ^ i ;

	REAL alpha1 = ((n * j1pr) > 0 ? 1 : -1) * acos(n * n1) ;
	REAL alpha2 = ((n * j2pr) > 0 ? 1 : -1) * acos(n * n2) ;
	REAL gamma1 = ((j1 * i) > 0 ? 1 : -1) * acos( i1 * i ) ;
	REAL gamma2 = ((j2 * i) > 0 ? 1 : -1) * acos( i2 * i ) ;

	m_quadricRGBfunctions[d] += QuadricRGBfunctions<REAL>(this->m_attrV[d],alpha1,gamma1) ;
	m_quadricRGBfunctions[d] += QuadricRGBfunctions<REAL>(this->m_attrV[dd],alpha2,gamma2) ;

	// New RGBf
	if (! m_quadricRGBfunctions[d].findOptimizedRGBfunctions(this->m_approx[d]))
		this->m_approx[d] = this->m_attrV[d];
}

} //namespace Decimation

} //namespace Algo

} //namespace CGoGN
