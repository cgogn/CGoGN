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

#include <time.h>
#include "Algo/Geometry/basic.h"
#include "Algo/Decimation/geometryApproximator.h"

namespace CGoGN
{

namespace Algo
{

namespace Decimation
{

/************************************************************************************
 *                                  MAP ORDER                                       *
 ************************************************************************************/

template <typename PFP>
bool EdgeSelector_MapOrder<PFP>::init()
{
	cur = this->m_map.begin() ;
	return true ;
}

template <typename PFP>
bool EdgeSelector_MapOrder<PFP>::nextEdge(Dart& d)
{
	MAP& m = this->m_map ;
	if(cur == m.end())
		return false ;
	d = cur ;
	return true ;
}

template <typename PFP>
void EdgeSelector_MapOrder<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	MAP& m = this->m_map ;
	cur = m.begin() ;
	while(!this->m_select(cur) || !m.edgeCanCollapse(cur))
	{
		m.next(cur) ;
		if(cur == m.end())
			break ;
	}
}

/************************************************************************************
 *                                    RANDOM                                        *
 ************************************************************************************/

template <typename PFP>
bool EdgeSelector_Random<PFP>::init()
{
	MAP& m = this->m_map ;

	darts.reserve(m.getNbDarts()) ;
	darts.clear() ;

	for(Dart d = m.begin(); d != m.end(); m.next(d))
		darts.push_back(d) ;

	srand(time(NULL)) ;
	int remains = darts.size() ;
	for(unsigned int i = 0; i < darts.size()-1; ++i) // generate the random permutation
	{
		int r = (rand() % remains) + i ;
		// swap ith and rth elements
		Dart tmp = darts[i] ;
		darts[i] = darts[r] ;
		darts[r] = tmp ;
		--remains ;
	}
	cur = 0 ;
	allSkipped = true ;

	return true ;
}

template <typename PFP>
bool EdgeSelector_Random<PFP>::nextEdge(Dart& d)
{
	if(cur == darts.size() && allSkipped)
		return false ;
	d = darts[cur] ;
	return true ;
}

template <typename PFP>
void EdgeSelector_Random<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	MAP& m = this->m_map ;
	allSkipped = false ;
	do
	{
		++cur ;
		if(cur == darts.size())
		{
			cur = 0 ;
			allSkipped = true ;
		}
	} while(!this->m_select(cur) || !m.edgeCanCollapse(darts[cur])) ;
}

/************************************************************************************
 *                                 EDGE LENGTH                                      *
 ************************************************************************************/

template <typename PFP>
bool EdgeSelector_Length<PFP>::init()
{
	MAP& m = this->m_map ;

	edges.clear() ;

	CellMarker<EDGE> eMark(m) ;
	for(Dart d = m.begin(); d != m.end(); m.next(d))
	{
		if(!eMark.isMarked(d))
		{
			initEdgeInfo(d) ;
			eMark.mark(d) ;
		}
	}

	cur = edges.begin() ; // init the current edge to the first one

	return true ;
}

template <typename PFP>
bool EdgeSelector_Length<PFP>::nextEdge(Dart& d)
{
	if(cur == edges.end() || edges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void EdgeSelector_Length<PFP>::updateBeforeCollapse(Dart d)
{
	MAP& m = this->m_map ;

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
}

template <typename PFP>
void EdgeSelector_Length<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	MAP& m = this->m_map ;

	Dart vit = d2 ;
	do
	{
		updateEdgeInfo(m.phi1(vit), false) ;			// must recompute some edge infos in the
		if(vit == d2 || vit == dd2)						// neighborhood of the collapsed edge
		{
			initEdgeInfo(vit) ;							// various optimizations are applied here by
														// treating differently :
			Dart vit2 = m.phi12(m.phi1(vit)) ;		// - edges for which the criteria must be recomputed
			Dart stop = m.phi2(vit) ;					// - edges that must be re-embedded
			do											// - edges for which only the collapsibility must be re-tested
			{
				updateEdgeInfo(vit2, false) ;
				updateEdgeInfo(m.phi1(vit2), false) ;
				vit2 = m.phi12(vit2) ;
			} while(vit2 != stop) ;
		}
		else
			updateEdgeInfo(vit, true) ;

		vit = m.phi2_1(vit) ;
	} while(vit != d2) ;

	cur = edges.begin() ; // set the current edge to the first one
}

template <typename PFP>
void EdgeSelector_Length<PFP>::initEdgeInfo(Dart d)
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
void EdgeSelector_Length<PFP>::updateEdgeInfo(Dart d, bool recompute)
{
	MAP& m = this->m_map ;
	EdgeInfo& einfo = edgeInfo[d] ;
	if(recompute)
	{
		if(einfo.valid)
			edges.erase(einfo.it) ;			// remove the edge from the multimap
		if(m.edgeCanCollapse(d))
			computeEdgeInfo(d, einfo) ;
		else
			einfo.valid = false ;
	}
	else
	{
		if(m.edgeCanCollapse(d))
		{									// if the edge can be collapsed now
			if(!einfo.valid)				// but it was not before
				computeEdgeInfo(d, einfo) ;
		}
		else
		{									// if the edge cannot be collapsed now
			if(einfo.valid)					// and it was before
			{
				edges.erase(einfo.it) ;
				einfo.valid = false ;
			}
		}
	}
}

template <typename PFP>
void EdgeSelector_Length<PFP>::computeEdgeInfo(Dart d, EdgeInfo& einfo)
{
	VEC3 vec = Algo::Geometry::vectorOutOfDart<PFP>(this->m_map, d, this->m_position) ;
	einfo.it = edges.insert(std::make_pair(vec.norm2(), d)) ;
	einfo.valid = true ;
}

/************************************************************************************
 *                            QUADRIC ERROR METRIC                                  *
 ************************************************************************************/

template <typename PFP>
bool EdgeSelector_QEM<PFP>::init()
{
	MAP& m = this->m_map ;

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

	edges.clear() ;

	CellMarker<VERTEX> vMark(m) ;
	for(Dart d = m.begin(); d != m.end(); m.next(d))
	{
		if(!vMark.isMarked(d))
		{
			Quadric<REAL> q ;	// create one quadric
			quadric[d] = q ;	// per vertex
			vMark.mark(d) ;
		}
	}

	DartMarker mark(m) ;
	for(Dart d = m.begin(); d != m.end(); m.next(d))
	{
		if(!mark.isMarked(d))
		{
			Dart d1 = m.phi1(d) ;				// for each triangle,
			Dart d_1 = m.phi_1(d) ;				// initialize the quadric of the triangle
			Quadric<REAL> q(this->m_position[d], this->m_position[d1], this->m_position[d_1]) ;
			quadric[d] += q ;					// and add the contribution of
			quadric[d1] += q ;					// this quadric to the ones
			quadric[d_1] += q ;					// of the 3 incident vertices
			mark.markOrbit<FACE>(d) ;
		}
	}

	CellMarker<EDGE> eMark(m) ;
	for(Dart d = m.begin(); d != m.end(); m.next(d))
	{
		if(!eMark.isMarked(d))
		{
			initEdgeInfo(d) ;	// init the edges with their optimal position
			eMark.mark(d) ;		// and insert them in the multimap according to their error
		}
	}

	cur = edges.begin() ; // init the current edge to the first one

	return true ;
}

template <typename PFP>
bool EdgeSelector_QEM<PFP>::nextEdge(Dart& d)
{
	if(cur == edges.end() || edges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void EdgeSelector_QEM<PFP>::updateBeforeCollapse(Dart d)
{
	MAP& m = this->m_map ;

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
}

template <typename PFP>
void EdgeSelector_QEM<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	MAP& m = this->m_map ;

	quadric[d2] = tmpQ ;

	Dart vit = d2 ;
	do
	{
		updateEdgeInfo(m.phi1(vit), false) ;			// must recompute some edge infos in the
		if(vit == d2 || vit == dd2)						// neighborhood of the collapsed edge
		{
			initEdgeInfo(vit) ;							// various optimizations are applied here by
														// treating differently :
			Dart vit2 = m.phi12(m.phi1(vit)) ;		// - edges for which the criteria must be recomputed
			Dart stop = m.phi2(vit) ;					// - edges that must be re-embedded
			do											// - edges for which only the collapsibility must be re-tested
			{
				updateEdgeInfo(vit2, false) ;
				updateEdgeInfo(m.phi1(vit2), false) ;
				vit2 = m.phi12(vit2) ;
			} while(vit2 != stop) ;
		}
		else
			updateEdgeInfo(vit, true) ;

		vit = m.phi2_1(vit) ;
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

	std::cout << quad(m_positionApproximator->getApprox(d)) << std::endl ;

	einfo.it = edges.insert(std::make_pair(err, d)) ;
	einfo.valid = true ;
}

/************************************************************************************
 *                            QUADRIC ERROR METRIC (Memoryless version)             *
 ************************************************************************************/

template <typename PFP>
bool EdgeSelector_QEMml<PFP>::init()
{
	MAP& m = this->m_map ;

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

	edges.clear() ;

	CellMarker<VERTEX> vMark(m) ;
	for(Dart d = m.begin(); d != m.end(); m.next(d))
	{
		if(!vMark.isMarked(d))
		{
			Quadric<REAL> q ;	// create one quadric
			quadric[d] = q ;	// per vertex
			vMark.mark(d) ;
		}
	}

	DartMarker mark(m) ;
	for(Dart d = m.begin(); d != m.end(); m.next(d))
	{
		if(!mark.isMarked(d))
		{
			Dart d1 = m.phi1(d) ;				// for each triangle,
			Dart d_1 = m.phi_1(d) ;				// initialize the quadric of the triangle
			Quadric<REAL> q(this->m_position[d], this->m_position[d1], this->m_position[d_1]) ;
			quadric[d] += q ;					// and add the contribution of
			quadric[d1] += q ;					// this quadric to the ones
			quadric[d_1] += q ;					// of the 3 incident vertices
			mark.markOrbit<FACE>(d) ;
		}
	}

	CellMarker<EDGE> eMark(m) ;
	for(Dart d = m.begin(); d != m.end(); m.next(d))
	{
		if(!eMark.isMarked(d))
		{
			initEdgeInfo(d) ;	// init the edges with their optimal position
			eMark.mark(d) ;		// and insert them in the multimap according to their error
		}
	}

	cur = edges.begin() ; // init the current edge to the first one

	return true ;
}

template <typename PFP>
bool EdgeSelector_QEMml<PFP>::nextEdge(Dart& d)
{
	if(cur == edges.end() || edges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void EdgeSelector_QEMml<PFP>::updateBeforeCollapse(Dart d)
{
	MAP& m = this->m_map ;

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
}

/**
 * Update quadric of a vertex
 * Discards quadrics of d and assigns freshly calculated
 * quadrics depending on the actual planes surrounding d
 * @param dart d
 */
template <typename PFP>
void EdgeSelector_QEMml<PFP>::recomputeQuadric(const Dart d, const bool recomputeNeighbors) {
	Dart dFront,dBack ;
	Dart dInit = d ;

	// Init Front
	dFront = dInit ;

	quadric[d].zero() ;

   	do {
   		// Make step
   		dBack = this->m_map.phi2(dFront) ;
       	dFront = this->m_map.phi2_1(dFront) ;

       	if (dBack != dFront) { // if dFront is no border
           	quadric[d] += Quadric<REAL>(this->m_position[d],this->m_position[this->m_map.phi2(dFront)],this->m_position[dBack]) ;
       	}
       	if (recomputeNeighbors)
       		recomputeQuadric(dBack, false) ;

    } while(dFront != dInit) ;
}


template <typename PFP>
void EdgeSelector_QEMml<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	MAP& m = this->m_map ;

	// for local vertex and neighbors
	recomputeQuadric(d2, true) ;

	Dart vit = d2 ;
	do
	{
		updateEdgeInfo(m.phi1(vit), true) ;			// must recompute some edge infos in the
		if(vit == d2 || vit == dd2)					// neighborhood of the collapsed edge
			initEdgeInfo(vit) ;						// various optimizations are applied here by
		else										// treating differently :
			updateEdgeInfo(vit, true) ;

		Dart vit2 = m.phi12(m.phi1(vit)) ;		// - edges for which the criteria must be recomputed
		Dart stop = m.phi2(vit) ;					// - edges that must be re-embedded
		do											// - edges for which only the collapsibility must be re-tested
		{
			updateEdgeInfo(vit2, true) ;
			updateEdgeInfo(m.phi1(vit2), false) ;
			vit2 = m.phi12(vit2) ;
		} while(vit2 != stop) ;

		vit = m.phi2_1(vit) ;
	} while(vit != d2) ;

	cur = edges.begin() ; // set the current edge to the first one
}

template <typename PFP>
void EdgeSelector_QEMml<PFP>::initEdgeInfo(Dart d)
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
void EdgeSelector_QEMml<PFP>::updateEdgeInfo(Dart d, bool recompute)
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
void EdgeSelector_QEMml<PFP>::computeEdgeInfo(Dart d, EdgeInfo& einfo)
{
	MAP& m = this->m_map ;
	Dart dd = m.phi2(d) ;

	Quadric<REAL> quad ;
	quad += quadric[d] ;	// compute the sum of the
	quad += quadric[dd] ;	// two vertices quadrics

	m_positionApproximator->approximate(d) ;

	REAL err = quad(m_positionApproximator->getApprox(d)) ;
	einfo.it = edges.insert(std::make_pair(err, d)) ;
	einfo.valid = true ;
}

/************************************************************************************
 *                                   CURVATURE                                      *
 ************************************************************************************/

template <typename PFP>
bool EdgeSelector_Curvature<PFP>::init()
{
	MAP& m = this->m_map ;

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

	edges.clear() ;

	CellMarker<EDGE> eMark(m) ;
	for(Dart d = m.begin(); d != m.end(); m.next(d))
	{
		if(!eMark.isMarked(d))
		{
			initEdgeInfo(d) ;	// init the edges with their optimal position
			eMark.mark(d) ;		// and insert them in the multimap according to their error
		}
	}

	cur = edges.begin() ; // init the current edge to the first one

	return true ;
}

template <typename PFP>
bool EdgeSelector_Curvature<PFP>::nextEdge(Dart& d)
{
	if(cur == edges.end() || edges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void EdgeSelector_Curvature<PFP>::updateBeforeCollapse(Dart d)
{
	MAP& m = this->m_map ;

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
}

template <typename PFP>
void EdgeSelector_Curvature<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	MAP& m = this->m_map ;

	normal[d2] = Algo::Geometry::vertexNormal<PFP>(m, d2, this->m_position) ;
	Algo::Geometry::computeCurvatureVertex_NormalCycles<PFP>(m, d2, radius, this->m_position, normal, edgeangle, kmax, kmin, Kmax, Kmin, Knormal) ;

	Dart vit = d2 ;
	do
	{
		Dart nVert = m.phi1(vit) ;
		normal[nVert] = Algo::Geometry::vertexNormal<PFP>(m, nVert, this->m_position) ;
		Algo::Geometry::computeCurvatureVertex_NormalCycles<PFP>(m, nVert, radius, this->m_position, normal, edgeangle, kmax, kmin, Kmax, Kmin, Knormal) ;

		updateEdgeInfo(m.phi1(vit), false) ;			// must recompute some edge infos in the
		if(vit == d2 || vit == dd2)						// neighborhood of the collapsed edge
		{
			initEdgeInfo(vit) ;							// various optimizations are applied here by
														// treating differently :
			Dart vit2 = m.phi12(m.phi1(vit)) ;		// - edges for which the criteria must be recomputed
			Dart stop = m.phi2(vit) ;					// - edges that must be re-embedded
			do											// - edges for which only the collapsibility must be re-tested
			{
				updateEdgeInfo(vit2, false) ;
				updateEdgeInfo(m.phi1(vit2), false) ;
				vit2 = m.phi12(vit2) ;
			} while(vit2 != stop) ;
		}
		else
			updateEdgeInfo(vit, true) ;

		vit = m.phi2_1(vit) ;
	} while(vit != d2) ;

	cur = edges.begin() ; // set the current edge to the first one
}

template <typename PFP>
void EdgeSelector_Curvature<PFP>::initEdgeInfo(Dart d)
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
void EdgeSelector_Curvature<PFP>::updateEdgeInfo(Dart d, bool recompute)
{
	MAP& m = this->m_map ;
	EdgeInfo& einfo = edgeInfo[d] ;
	if(recompute)
	{
		if(einfo.valid)
			edges.erase(einfo.it) ;			// remove the edge from the multimap
		if(m.edgeCanCollapse(d))
			computeEdgeInfo(d, einfo) ;
		else
			einfo.valid = false ;
	}
	else
	{
		if(m.edgeCanCollapse(d))
		{									// if the edge can be collapsed now
			if(!einfo.valid)				// but it was not before
				computeEdgeInfo(d, einfo) ;
		}
		else
		{									// if the edge cannot be collapsed now
			if(einfo.valid)					// and it was before
			{
				edges.erase(einfo.it) ;
				einfo.valid = false ;
			}
		}
	}
}

template <typename PFP>
void EdgeSelector_Curvature<PFP>::computeEdgeInfo(Dart d, EdgeInfo& einfo)
{
	MAP& m = this->m_map ;
	Dart dd = m.phi2(d) ;

	unsigned int v1 = m.template getEmbedding<VERTEX>(d) ;
	unsigned int v2 = m.template getEmbedding<VERTEX>(dd) ;

	m_positionApproximator->approximate(d) ;

	// temporary edge collapse
	Dart d2 = m.phi2(m.phi_1(d)) ;
	Dart dd2 = m.phi2(m.phi_1(dd)) ;
	m.extractTrianglePair(d) ;
	unsigned int newV = m.template embedNewCell<VERTEX>(d2) ;
	this->m_position[newV] = m_positionApproximator->getApprox(d) ;

	// compute things on the coarse version of the mesh
	normal[newV] = Algo::Geometry::vertexNormal<PFP>(m, d2, this->m_position) ;
	Algo::Geometry::computeCurvatureVertex_NormalCycles<PFP>(m, d2, radius, this->m_position, normal, edgeangle, kmax, kmin, Kmax, Kmin, Knormal) ;

//	VEC3 norm = normal[newV] ;
	REAL mCurv = (kmax[newV] + kmin[newV]) / REAL(2) ;
//	VEC3 cDir1 = Kmax[newV] ;

	// vertex split to reset the initial connectivity and embeddings
	m.insertTrianglePair(d, d2, dd2) ;
	m.template embedOrbit<VERTEX>(d, v1) ;
	m.template embedOrbit<VERTEX>(dd, v2) ;

	REAL err = 0 ;

//	REAL norm_deviation_1 = REAL(1) / abs(norm * normal[v1]) ;
//	REAL norm_deviation_2 = REAL(1) / abs(norm * normal[v2]) ;
//	err += norm_deviation_1 + norm_deviation_2 ;

	REAL mCurv_deviation_1 = abs(mCurv - (kmax[v1] + kmin[v1] / REAL(2))) ;
	REAL mCurv_deviation_2 = abs(mCurv - (kmax[v2] + kmin[v2] / REAL(2))) ;
	err += mCurv_deviation_1 + mCurv_deviation_2 ;

//	REAL cDir1_deviation_1 = REAL(1) / abs(cDir1 * Kmax[v1]) ;
//	REAL cDir1_deviation_2 = REAL(1) / abs(cDir1 * Kmax[v2]) ;
//	err += cDir1_deviation_1 + cDir1_deviation_2 ;

	einfo.it = edges.insert(std::make_pair(err, d)) ;
	einfo.valid = true ;
}

/************************************************************************************
 *                                  MIN DETAIL                                      *
 ************************************************************************************/

template <typename PFP>
bool EdgeSelector_MinDetail<PFP>::init()
{
	MAP& m = this->m_map ;

	bool ok = false ;
	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = this->m_approximators.begin();
		it != this->m_approximators.end() && !ok;
		++it)
	{
		if((*it)->getApproximatedAttributeName() == "position" && (*it)->getPredictor())
		{
			m_positionApproximator = reinterpret_cast<Approximator<PFP, VEC3>* >(*it) ;
			ok = true ;
		}
	}

	if(!ok)
		return false ;

	edges.clear() ;

	TraversorE<MAP> travE(m);
	for(Dart dit = travE.begin() ; dit != travE.end() ; dit = travE.next())
	{
		initEdgeInfo(dit);
	}

	cur = edges.begin() ; // init the current edge to the first one

	return true ;
}

template <typename PFP>
bool EdgeSelector_MinDetail<PFP>::nextEdge(Dart& d)
{
	if(cur == edges.end() || edges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void EdgeSelector_MinDetail<PFP>::updateBeforeCollapse(Dart d)
{
	MAP& m = this->m_map ;

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
}

template <typename PFP>
void EdgeSelector_MinDetail<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	MAP& m = this->m_map ;

	Dart vit = d2 ;
	do
	{
		updateEdgeInfo(m.phi1(vit), false) ;			// must recompute some edge infos in the
		if(vit == d2 || vit == dd2)						// neighborhood of the collapsed edge
		{
			initEdgeInfo(vit) ;							// various optimizations are applied here by
														// treating differently :
			Dart vit2 = m.phi12(m.phi1(vit)) ;		// - edges for which the criteria must be recomputed
			Dart stop = m.phi2(vit) ;					// - edges that must be re-embedded
			do											// - edges for which only the collapsibility must be re-tested
			{
				updateEdgeInfo(vit2, false) ;
				updateEdgeInfo(m.phi1(vit2), false) ;
				vit2 = m.phi12(vit2) ;
			} while(vit2 != stop) ;
		}
		else
			updateEdgeInfo(vit, true) ;

		vit = m.phi2_1(vit) ;
	} while(vit != d2) ;

	cur = edges.begin() ; // set the current edge to the first one
}

template <typename PFP>
void EdgeSelector_MinDetail<PFP>::initEdgeInfo(Dart d)
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
void EdgeSelector_MinDetail<PFP>::updateEdgeInfo(Dart d, bool recompute)
{
	MAP& m = this->m_map ;
	EdgeInfo& einfo = edgeInfo[d] ;
	if(recompute)
	{
		if(einfo.valid)
			edges.erase(einfo.it) ;			// remove the edge from the multimap
		if(m.edgeCanCollapse(d))
			computeEdgeInfo(d, einfo) ;
		else
			einfo.valid = false ;
	}
	else
	{
		if(m.edgeCanCollapse(d))
		{									// if the edge can be collapsed now
			if(!einfo.valid)				// but it was not before
				computeEdgeInfo(d, einfo) ;
		}
		else
		{									// if the edge cannot be collapsed now
			if(einfo.valid)					// and it was before
			{
				edges.erase(einfo.it) ;
				einfo.valid = false ;
			}
		}
	}
}

template <typename PFP>
void EdgeSelector_MinDetail<PFP>::computeEdgeInfo(Dart d, EdgeInfo& einfo)
{
	// Dart dd = this->m_map.phi2(d) ;
	REAL err = REAL(0) ;

//	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = this->m_approximators.begin();
//		it != this->m_approximators.end();
//		++it)
//	{
//		if((*it)->getPredictor())
//		{
//			(*it)->approximate(d) ;
//			err += (*it)->detailMagnitude(d) ;
//		}
//	}

	m_positionApproximator->approximate(d) ;
	err = m_positionApproximator->getDetail(d).norm2() ;

	einfo.it = edges.insert(std::make_pair(err, d)) ;
	einfo.valid = true ;
}

} // namespace Decimation

} // namespace Algo

} // namespace CGoGN
