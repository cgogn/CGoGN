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

#include "Algo/DecimationVolumes/geometryApproximator.h"
#include <time.h>

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
bool EdgeSelector_QEM<PFP>::init()
{
	MAP& m = this->m_map ;

	//searching the Geometry Approximator
	bool ok = false ;
	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = this->m_approximators.begin();
			it != this->m_approximators.end() && !ok;
			++it)
	{
		if((*it)->getApproximatedAttributeName() == "position")
		{
			m_positionApproximator = reinterpret_cast<Approximator<PFP, VEC3>* >(*it) ;
			ok = true ;
		}
	}

	if(!ok)
		return false ;

	// init the Quadric error metric for cells
	edges.clear();
	TraversorV<MAP> travV(m);
	for(Dart dit = travV.begin() ; dit != travV.end() ; dit = travV.next())
	{
		//create one quadric per vertex
		Quadric<REAL> q;
		quadric[dit] = q;
	}

	// init the quadric for each boundary triangle
	TraversorF<MAP> travF(m);
	for(Dart dit = travF.begin() ; dit != travF.end() ; dit = travF.next())
	{
		Dart d1 = m.phi1(dit) ;				// for each triangle,
		Dart d_1 = m.phi_1(dit) ;				// initialize the quadric of the triangle
		Quadric<REAL> q(this->m_position[dit], this->m_position[d1], this->m_position[d_1]) ;
		quadric[dit] += q ;					// and add the contribution of
		quadric[d1] += q ;					// this quadric to the ones
		quadric[d_1] += q ;					// of the 3 incident vertices

	}

	// init the edges with their optimal position
	// and insert them in the multimap according to their error
	TraversorE<MAP> travE(m);
	for(Dart dit = travE.begin() ; dit != travE.end() ; dit = travE.next())
	{
		initEdgeInfo(dit);
	}

	// init the current edge to the first one
	cur = edges.begin();

	return true;
}

template <typename PFP>
Operator<PFP>* EdgeSelector_QEM<PFP>::nextOperator()
{
	MAP& m = this->m_map ;
	Operator<PFP>* op = NULL;

	if(cur != edges.end() && !edges.empty())
	{
		op = new CollapseEdgeOperator<PFP>((*cur).second);

		op->setFirstIncidentEdge(m.phi2(m.phi_1((*cur).second)));
		op->setSecondIncidentEdge(m.phi2(m.phi_1(m.phi2((*cur).second))));
	}

	return op ;
}

template <typename PFP>
bool EdgeSelector_QEM<PFP>::updateBeforeOperation(Operator<PFP>* op)
{
	MAP& m = this->m_map ;
	Dart d = op->getEdge();

	EdgeInfo& edgeE = edgeInfo[d] ;
	if(edgeE.valid)
		edges.erase(edgeE.it) ;

	edgeE = edgeInfo[m.phi1(d)] ;
	if(edgeE.valid)					// remove all
		edges.erase(edgeE.it) ;

	edgeE = edgeInfo[m.phi_1(d)] ;	// the concerned edges
	if(edgeE.valid)
		edges.erase(edgeE.it) ;
									// from the multimap
	Dart dd = m.phi2(d) ;
	if(dd != d)
	{
		edgeE = edgeInfo[m.phi1(dd)] ;
		if(edgeE.valid)
			edges.erase(edgeE.it) ;

		edgeE = edgeInfo[m.phi_1(dd)] ;
		if(edgeE.valid)
			edges.erase(edgeE.it) ;
	}

	tmpQ.zero() ;			// compute quadric for the new
	tmpQ += quadric[d] ;	// vertex as the sum of those
	tmpQ += quadric[dd] ;	// of the contracted vertices

	return true;
}

template <typename PFP>
void EdgeSelector_QEM<PFP>::updateAfterOperation(Operator<PFP>* op)
{
	MAP& m = this->m_map ;
	Dart d2 = op->getFirstIncidentEdge();
	Dart dd2 = op->getSecondIncidentEdge();

	quadric[d2] = tmpQ ;

	Dart vit = d2 ;
	do
	{
		updateEdgeInfo(m.phi1(vit), false) ;			// must recompute some edge infos in the
		if(vit == d2 || vit == dd2)						// neighborhood of the collapsed edge
		{
			initEdgeInfo(vit) ;							// various optimizations are applied here by
														// treating differently :
			Dart vit2 = m.phi1(m.phi2(m.phi1(vit))) ;		// - edges for which the criteria must be recomputed
			Dart stop = m.phi2(vit) ;					// - edges that must be re-embedded
			do											// - edges for which only the collapsibility must be re-tested
			{
				updateEdgeInfo(vit2, false) ;
				updateEdgeInfo(m.phi1(vit2), false) ;
				vit2 = m.phi1(m.phi2(vit2)) ;
			} while(vit2 != stop) ;
		}
		else
			updateEdgeInfo(vit, true) ;

		vit = m.phi2(m.phi_1(vit)) ;
	} while(vit != d2) ;

	cur = edges.begin() ; // set the current edge to the first one

}

template <typename PFP>
void EdgeSelector_QEM<PFP>::initEdgeInfo(Dart d)
{
	MAP& m = this->m_map ;
	EdgeInfo einfo ;
	if(m.edgeCanCollapse(d))
		computeEdgeInfo(d, einfo) ;
	else
		einfo.valid = false ;
	edgeInfo[d] = einfo ;
}

template <typename PFP>
void EdgeSelector_QEM<PFP>::updateEdgeInfo(Dart d, bool recompute)
{
	MAP& m = this->m_map ;
	EdgeInfo& einfo = edgeInfo[d] ;

	if(recompute)
	{
		if(einfo.valid)
			edges.erase(einfo.it) ;		// remove the edge from the multimap
		if(m.edgeCanCollapse(d))
			computeEdgeInfo(d, einfo) ;
		else
			einfo.valid = false ;
	}
	else
	{
		if(m.edgeCanCollapse(d))
		{								 	// if the edge can be collapsed now
			if(!einfo.valid)				// but it was not before
				computeEdgeInfo(d, einfo) ;
		}
		else
		{								 // if the edge cannot be collapsed now
			if(einfo.valid)				 // and it was before
			{
				edges.erase(einfo.it) ;
				einfo.valid = false ;
			}
		}
	}
}

template <typename PFP>
void EdgeSelector_QEM<PFP>::computeEdgeInfo(Dart d, EdgeInfo& einfo)
{
	MAP& m = this->m_map ;
	Dart dd = m.phi2(d) ;

	Quadric<REAL> quad ;
	quad += quadric[d] ;	// compute the sum of the
	quad += quadric[dd] ;	// two vertices quadrics

	m_positionApproximator->approximate(d) ;

	REAL err = std::max(REAL(0),REAL(quad(m_positionApproximator->getApprox(d)))) ;

	einfo.it = edges.insert(std::make_pair(err, d)) ;
	einfo.valid = true ;
}




} //end namespace DecimationVolumique
} //end namespace Algo
} //end namespace CGoGN
