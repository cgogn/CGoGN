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
 *							         MID EDGE                                       *
 ************************************************************************************/

template <typename PFP>
bool Approximator_MidEdge<PFP>::init()
{
	return true ;
}

template <typename PFP>
void Approximator_MidEdge<PFP>::approximate(Dart d)
{
	MAP& m = this->m_map ;

	// get some darts
	Dart dd = m.phi2(d) ;

	// get the contracted edge vertices positions
	VEC3 v1 = this->m_attrV[d] ;
	VEC3 v2 = this->m_attrV[dd] ;

	// Compute the approximated position
	this->m_approx[d] = (v1 + v2) / REAL(2) ;

//	if(this->m_predictor)
//	{
//
//	}
}

/************************************************************************************
 *							       HALF COLLAPSE                                    *
 ************************************************************************************/

template <typename PFP>
bool Approximator_HalfCollapse<PFP>::init()
{
//	if(this->m_predictor)
//	{
//		if(! ( this->m_predictor->getType() == P_HalfCollapse ) )
//		{
//			return false ;
//		}
//	}
	return true ;
}

template <typename PFP>
void Approximator_HalfCollapse<PFP>::approximate(Dart d)
{
	MAP& m = this->m_map ;

	this->m_approx[d] = this->m_attrV[d];

//	if(this->m_predictor)
//	{
//		Dart dd = m.phi2(d) ;
//		Dart d2 = m.phi2(m.phi_1(d)) ;
//		Dart dd2 = m.phi2(m.phi_1(dd)) ;
//
//		VEC3 v2 = this->m_attrV[0]->operator[](dd) ;
//
//		// temporary edge collapse
//		m.extractTrianglePair(d) ;
//		unsigned int newV = m.template embedNewCell<VERTEX>(d2) ;
//		for (unsigned int i = 0 ; i < this->m_attrV.size() ; ++i)
//		{
//			this->m_attrV[i]->operator[](newV) = this->m_approx[i][d] ;
//		}
//
//		// compute the detail vector
//		this->m_predictor->predict(d2, dd2) ;
//		for (unsigned int i = 0 ; i < this->m_attrV.size() ; ++i)
//		{
//			this->m_detail[i][d] = v2 - this->m_predictor->getPredict(1) ;
//		}
//
//		// vertex split to reset the initial connectivity and embeddings
//		m.insertTrianglePair(d, d2, dd2) ;
//		m.template embedOrbit<VERTEX>(d, m.template getEmbedding<VERTEX>(d)) ;
//		m.template embedOrbit<VERTEX>(dd, m.template getEmbedding<VERTEX>(dd)) ;
//	}
}

} //end namespace DecimationVolumes

} //end namespace Algo

} //end namespace CGoGN
