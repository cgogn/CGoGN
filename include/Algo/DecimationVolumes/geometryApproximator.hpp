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

#include "Algo/DecimationVolumes/selector.h"
#include "Algo/Geometry/centroid.h"

namespace CGoGN
{

namespace Algo
{

namespace DecimationVolumes
{

/************************************************************************************
 *                            QUADRIC ERROR METRIC                                  *
 ************************************************************************************/

template <typename PFP>
bool Approximator_QEM<PFP>::init()
{
	m_quadric = this->m_map.template getAttribute<Quadric<REAL>, VERTEX>("QEMquadric") ;

	if(this->m_predictor)
	{
		return false ;
	}
	return true ;
}

template <typename PFP>
void Approximator_QEM<PFP>::approximate(Operator<PFP> *op)
{
	Dart d = op->getEdge();
	approximate(d);
}

template <typename PFP>
void Approximator_QEM<PFP>::approximate(Dart d)
{
	MAP& m = this->m_map ;

	// get some darts
	Dart dd = m.phi2(d) ;

	Quadric<REAL> q1, q2 ;
	if(!m_quadric.isValid()) // if the selector is not QEM, compute local error quadrics
	{
		// compute the error quadric associated to v1
		Dart it = d ;
		do
		{
			Quadric<REAL> q(this->m_attrV[it], this->m_attrV[m.phi1(it)], this->m_attrV[m.phi_1(it)]) ;
			q1 += q ;
			it = m.phi2_1(it) ;
		} while(it != d) ;

		// compute the error quadric associated to v2
		it = dd ;
		do
		{
			Quadric<REAL> q(this->m_attrV[it], this->m_attrV[m.phi1(it)], this->m_attrV[m.phi_1(it)]) ;
			q2 += q ;
			it = m.phi2_1(it) ;
		} while(it != dd) ;
	}
	else // if the selector is QEM, use the error quadrics computed by the selector
	{
		q1 = m_quadric[d] ;
		q2 = m_quadric[dd] ;
	}

	Quadric<REAL> quad ;
	quad += q1 ;	// compute the sum of the
	quad += q2 ;	// two vertices quadrics

	VEC3 res ;
	bool opt = quad.findOptimizedPos(res) ;	// try to compute an optimized position for the contraction of this edge
	if(!opt)
	{
		VEC3 p1 = this->m_attrV[d] ;	// let the new vertex lie
		VEC3 p2 = this->m_attrV[dd] ;	// on either one of the two endpoints
		VEC3 p12 = (p1 + p2) / 2.0f ;	// or the middle of the edge
		REAL e1 = quad(p1) ;
		REAL e2 = quad(p2) ;
		REAL e12 = quad(p12) ;
		REAL minerr = std::min(std::min(e1, e2), e12) ;	// consider only the one for
		if(minerr == e12) this->m_approx[d] = p12 ;		// which the error is minimal
		else if(minerr == e1) this->m_approx[d] = p1 ;
		else this->m_approx[d] = p2 ;
	}
	else
		this->m_approx[d] = res ;
}

} //end namespace DecimationVolumes

} //end namespace Algo

} //end namespace CGoGN
