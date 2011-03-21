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
 *                            QUADRIC ERROR METRIC (Memoryless version)             *
 ************************************************************************************/

template <typename PFP>
bool HalfEdgeSelector_QEMml<PFP>::init()
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

	CellMarker vMark(m, VERTEX_CELL) ;
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
			mark.markOrbit(FACE_ORBIT, d) ;
		}
	}

	// Init multimap for each Half-edge
	halfEdges.clear() ;

	for(Dart d = m.begin(); d != m.end(); m.next(d))
	{
		initHalfEdgeInfo(d) ;	// init the edges with their optimal info
	}							// and insert them in the multimap according to their error

	cur = halfEdges.begin() ; 	// init the current edge to the first one

	return true ;
}

template <typename PFP>
bool HalfEdgeSelector_QEMml<PFP>::nextEdge(Dart& d)
{
	if(cur == halfEdges.end() || halfEdges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void HalfEdgeSelector_QEMml<PFP>::updateBeforeCollapse(Dart d)
{
	MAP& m = this->m_map ;

	HalfEdgeInfo& edgeE = halfEdgeInfo[d] ;
	if(edgeE.valid)
		halfEdges.erase(edgeE.it) ;

	edgeE = halfEdgeInfo[m.phi1(d)] ;
	if(edgeE.valid)						// remove all
		halfEdges.erase(edgeE.it) ;

	edgeE = halfEdgeInfo[m.phi_1(d)] ;	// the halfedges that will disappear
	if(edgeE.valid)
		halfEdges.erase(edgeE.it) ;
										// from the multimap
	Dart dd = m.phi2(d) ;
	if(dd != d)
	{
		edgeE = halfEdgeInfo[dd] ;
		if(edgeE.valid)
			halfEdges.erase(edgeE.it) ;

		edgeE = halfEdgeInfo[m.phi1(dd)] ;
		if(edgeE.valid)
			halfEdges.erase(edgeE.it) ;

		edgeE = halfEdgeInfo[m.phi_1(dd)] ;
		if(edgeE.valid)
			halfEdges.erase(edgeE.it) ;
	}
}

/**
 * Update quadric of a vertex
 * Discards quadrics of d and assigns freshly calculated
 * quadrics depending on the actual planes surrounding d
 * @param dart d
 */
template <typename PFP>
void HalfEdgeSelector_QEMml<PFP>::recomputeQuadric(const Dart d, const bool recomputeNeighbors) {
	Dart dFront,dBack ;
	Dart dInit = d ;

	// Init Front
	dFront = dInit ;

	quadric[d].zero() ;

   	do {
   		// Make step
   		dBack = this->m_map.phi1(dFront) ;
       	dFront = this->m_map.alpha1(dFront) ;

       	if (dBack != dFront) { // if dFront is no border
           	quadric[d] += Quadric<REAL>(this->m_position[d],this->m_position[this->m_map.phi1(dFront)],this->m_position[dBack]) ;
       	}
       	if (recomputeNeighbors)
       		recomputeQuadric(dBack, false) ;

    } while(dFront != dInit) ;
}

template <typename PFP>
void HalfEdgeSelector_QEMml<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	MAP& m = this->m_map ;

	recomputeQuadric(d2, true) ;

	Dart vit = d2 ;
	do
	{
		updateHalfEdgeInfo(vit, true) ;
		updateHalfEdgeInfo(m.phi2(vit), true) ;
		updateHalfEdgeInfo(m.phi1(vit), true) ;
		updateHalfEdgeInfo(m.phi2(m.phi1(vit)), true) ;

		Dart stop = m.phi2(vit) ;
		Dart vit2 = m.alpha_1(m.phi1(vit)) ;
		do {
			updateHalfEdgeInfo(vit2, true) ;
			updateHalfEdgeInfo(m.phi2(vit2), true) ;
			updateHalfEdgeInfo(m.phi1(vit2), false) ;
			updateHalfEdgeInfo(m.phi2(m.phi1(vit2)), false) ;
			vit2 = m.alpha_1(vit2) ;

		} while (stop != vit2) ;
		vit = m.alpha1(vit) ;
	} while(vit != d2) ;

	cur = halfEdges.begin() ; // set the current edge to the first one
}

template <typename PFP>
void HalfEdgeSelector_QEMml<PFP>::initHalfEdgeInfo(Dart d)
{
	MAP& m = this->m_map ;
	HalfEdgeInfo heinfo ;
	if(m.edgeCanCollapse(d))
		computeHalfEdgeInfo(d, heinfo) ;
	else
		heinfo.valid = false ;

	halfEdgeInfo[d] = heinfo ;
}

template <typename PFP>
void HalfEdgeSelector_QEMml<PFP>::updateHalfEdgeInfo(Dart d, bool recompute)
{
	MAP& m = this->m_map ;
	HalfEdgeInfo& heinfo = halfEdgeInfo[d] ;
	if(recompute)
	{
		if(heinfo.valid)
			halfEdges.erase(heinfo.it) ;			// remove the edge from the multimap
		if(m.edgeCanCollapse(d))
			computeHalfEdgeInfo(d, heinfo) ;
		else
			heinfo.valid = false ;
	}
	else
	{
		if(m.edgeCanCollapse(d))
		{								 	// if the edge can be collapsed now
			if(!heinfo.valid)				 // but it was not before
				computeHalfEdgeInfo(d, heinfo) ;
		}
		else
		{								 // if the edge cannot be collapsed now
			if(heinfo.valid)				 // and it was before
			{
				halfEdges.erase(heinfo.it) ;
				heinfo.valid = false ;
			}
		}
	}
}

template <typename PFP>
void HalfEdgeSelector_QEMml<PFP>::computeHalfEdgeInfo(Dart d, HalfEdgeInfo& heinfo)
{
	MAP& m = this->m_map ;
	Dart dd = m.phi2(d) ;

	Quadric<REAL> quad ;
	quad += quadric[d] ;	// compute the sum of the
	quad += quadric[dd] ;	// two vertices quadrics

	m_positionApproximator->approximate(d) ;

	REAL err = quad(m_positionApproximator->getApprox(d)) ;
	heinfo.it = halfEdges.insert(std::make_pair(err, d)) ;
	heinfo.valid = true ;
}

/************************************************************************************
 *                         HALFEDGESELECTOR LIGHTFIELD                              *
 ************************************************************************************/

template <typename PFP>
bool HalfEdgeSelector_Lightfield<PFP>::init()
{
	MAP& m = this->m_map ;

	// Verify availability of required approximators
	char ok = 0 ;
	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = this->m_approximators.begin();
		it != this->m_approximators.end();
		++it)
	{
		// constraint : 3 approximators in specific order
		if(ok == 0 && (*it)->getApproximatedAttributeName() == "position")
		{
			m_positionApproximator = reinterpret_cast<Approximator<PFP, VEC3>* >(*it) ; // 1) position
			assert(m_positionApproximator->getType() != A_QEM) ; // A_QEM is not compatible for half-edge crit
			++ok ;
		}
		else if( ok == 1 && (*it)->getApproximatedAttributeName() == "frame")
		{
			m_frameApproximator = reinterpret_cast<Approximator<PFP, FRAME>* >(*it) ; // 2) frame (needs position)
			++ok ;
		}
		else if(ok == 2 && (*it)->getApproximatedAttributeName() == "colorPTM")
		{
			m_RGBfunctionsApproximator = reinterpret_cast<Approximator<PFP, RGBFUNCTIONS>* >(*it) ; // 3) functions (needs frame)
			++ok ;
		}
	}

	if(ok != 3)
		return false ;

	// Set quadric per vertex
	CellMarker vMark(m, VERTEX_CELL) ;
	for(Dart d = m.begin(); d != m.end(); m.next(d))
	{
		if(!vMark.isMarked(d))
		{
			Quadric<REAL> q ;		// create one quadric
			quadric[d] = q ;		// per vertex
			vMark.mark(d) ;
		}
	}

	// Compute quadric per vertex
	DartMarker mark(m) ;

	for(Dart d = m.begin(); d != m.end(); m.next(d)) // init QEM quadrics
	{
		if(!mark.isMarked(d))
		{
			Dart d1 = m.phi1(d) ;					// for each triangle,
			Dart d_1 = m.phi_1(d) ;					// initialize the quadric of the triangle
			Quadric<REAL> q(this->m_position[d], this->m_position[d1], this->m_position[d_1]) ;
			quadric[d] += q ;						// and add the contribution of
			quadric[d1] += q ;						// this quadric to the ones
			quadric[d_1] += q ;						// of the 3 incident vertices
			mark.markOrbit(FACE_ORBIT, d) ;
		}
	}

	// Init multimap for each Half-edge
	halfEdges.clear() ;

	for(Dart d = m.begin(); d != m.end(); m.next(d))
	{
		initHalfEdgeInfo(d) ;	// init the edges with their optimal info
	}							// and insert them in the multimap according to their error

	cur = halfEdges.begin() ; 	// init the current edge to the first one

	return true ;
}

template <typename PFP>
bool HalfEdgeSelector_Lightfield<PFP>::nextEdge(Dart& d)
{
	if(cur == halfEdges.end() || halfEdges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void HalfEdgeSelector_Lightfield<PFP>::updateBeforeCollapse(Dart d)
{
	MAP& m = this->m_map ;

	HalfEdgeInfo& edgeE = halfEdgeInfo[d] ;
	if(edgeE.valid)
		halfEdges.erase(edgeE.it) ;

	edgeE = halfEdgeInfo[m.phi1(d)] ;
	if(edgeE.valid)						// remove all
		halfEdges.erase(edgeE.it) ;

	edgeE = halfEdgeInfo[m.phi_1(d)] ;	// the halfedges that will disappear
	if(edgeE.valid)
		halfEdges.erase(edgeE.it) ;
										// from the multimap
	Dart dd = m.phi2(d) ;
	if(dd != d)
	{
		edgeE = halfEdgeInfo[dd] ;
		if(edgeE.valid)
			halfEdges.erase(edgeE.it) ;

		edgeE = halfEdgeInfo[m.phi1(dd)] ;
		if(edgeE.valid)
			halfEdges.erase(edgeE.it) ;

		edgeE = halfEdgeInfo[m.phi_1(dd)] ;
		if(edgeE.valid)
			halfEdges.erase(edgeE.it) ;
	}
}

/**
 * Update quadric of a vertex
 * Discards quadrics of d and assigns freshly calculated
 * quadrics depending on the actual planes surrounding d
 * @param dart d
 */
template <typename PFP>
void HalfEdgeSelector_Lightfield<PFP>::recomputeQuadric(const Dart d, const bool recomputeNeighbors) {
	Dart dFront,dBack ;
	Dart dInit = d ;

	// Init Front
	dFront = dInit ;

	quadric[d].zero() ;

   	do {
   		// Make step
   		dBack = this->m_map.phi1(dFront) ;
       	dFront = this->m_map.alpha1(dFront) ;

       	if (dBack != dFront) { // if dFront is no border
           	quadric[d] += Quadric<REAL>(this->m_position[d],this->m_position[this->m_map.phi1(dFront)],this->m_position[dBack]) ;
           	if (isnan(this->m_position[d][0]))
           		std::cout << "NaaaaN" << std::endl ;
       	}
       	if (recomputeNeighbors)
       		recomputeQuadric(dBack, false) ;

    } while(dFront != dInit) ;
}

template <typename PFP>
void HalfEdgeSelector_Lightfield<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	MAP& m = this->m_map ;

	recomputeQuadric(d2, true) ;

	Dart vit = d2 ;
	do
	{
		updateHalfEdgeInfo(vit, true) ;
		updateHalfEdgeInfo(m.phi2(vit), true) ;
		updateHalfEdgeInfo(m.phi1(vit), true) ;
		updateHalfEdgeInfo(m.phi2(m.phi1(vit)), true) ;

		Dart stop = m.phi2(vit) ;
		Dart vit2 = m.alpha_1(m.phi1(vit)) ;
		do {
			updateHalfEdgeInfo(vit2, true) ;
			updateHalfEdgeInfo(m.phi2(vit2), true) ;
			updateHalfEdgeInfo(m.phi1(vit2), false) ;
			updateHalfEdgeInfo(m.phi2(m.phi1(vit2)), false) ;
			vit2 = m.alpha_1(vit2) ;

		} while (stop != vit2) ;
		vit = m.alpha1(vit) ;
	} while(vit != d2) ;

	cur = halfEdges.begin() ; // set the current edge to the first one
}

template <typename PFP>
void HalfEdgeSelector_Lightfield<PFP>::initHalfEdgeInfo(Dart d)
{
	MAP& m = this->m_map ;
	HalfEdgeInfo heinfo ;
	if(m.edgeCanCollapse(d))
		computeHalfEdgeInfo(d, heinfo) ;
	else
		heinfo.valid = false ;

	halfEdgeInfo[d] = heinfo ;
}

template <typename PFP>
void HalfEdgeSelector_Lightfield<PFP>::updateHalfEdgeInfo(Dart d, bool recompute)
{
	MAP& m = this->m_map ;
	HalfEdgeInfo& heinfo = halfEdgeInfo[d] ;
	if(recompute)
	{
		if(heinfo.valid)
			halfEdges.erase(heinfo.it) ;			// remove the edge from the multimap
		if(m.edgeCanCollapse(d))
			computeHalfEdgeInfo(d, heinfo) ;
		else
			heinfo.valid = false ;
	}
	else
	{
		if(m.edgeCanCollapse(d))
		{								 	// if the edge can be collapsed now
			if(!heinfo.valid)				 // but it was not before
				computeHalfEdgeInfo(d, heinfo) ;
		}
		else
		{								 // if the edge cannot be collapsed now
			if(heinfo.valid)				 // and it was before
			{
				halfEdges.erase(heinfo.it) ;
				heinfo.valid = false ;
			}
		}
	}
}

template <typename PFP>
void HalfEdgeSelector_Lightfield<PFP>::computeHalfEdgeInfo(Dart d, HalfEdgeInfo& heinfo)
{
	MAP& m = this->m_map ;
	Dart dd = m.phi2(d) ;

	// New position
	Quadric<REAL> quad ;
	quad += quadric[d] ;	// compute the sum of the
	quad += quadric[dd] ;	// two vertices quadrics

	this->m_positionApproximator->approximate(d) ; 		// sets newPos
	VEC3 newPos = this->m_positionApproximator->getApprox(d) ; // get newPos

	// New Frame
	this->m_frameApproximator->approximate(d) ; 		// sets newF
	MATRIX33 newFrame = this->m_frameApproximator->getApprox(d) ; // get newF

	VEC3 n1,n2 ;
	if (! m_frame[d].getSubVectorH(2,0,n1))	{ // get the normals
		std::cout << "EdgeSelector_LightField::computeEdgeInfo --> getSubVectorH 1 failed " << std::endl;
		exit(2) ;
	}
	if (!m_frame[dd].getSubVectorH(2,0,n2))	{ // of the two vertices
		std::cout << "EdgeSelector_LightField::computeEdgeInfo --> getSubVectorH 2 failed " << std::endl;
		exit(3) ;
	}

	// New function
	this->m_RGBfunctionsApproximator->approximate(d) ; 	// sets quadricRGBf and newRGBf
	MATRIX36 newRGBf = this->m_RGBfunctionsApproximator->getApprox(d) ; // get newRGBf

	// Compute error
	REAL err = quad(newPos) + (2 * acos (n1 * n2)) + quadricRGBfunctions[d](newRGBf) ;

	heinfo.it = halfEdges.insert(std::make_pair(err, d)) ;
	heinfo.valid = true ;
}

/************************************************************************************
 *                             EDGESELECTOR LIGHTFIELD                              *
 ************************************************************************************

template <typename PFP>
bool EdgeSelector_Lightfield<PFP>::init()
{
	MAP& m = this->m_map ;

	// Verify availability of required approximators
	char ok = 0 ;
	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = this->m_approximators.begin();
		it != this->m_approximators.end();
		++it)
	{
		// constraint : 3 approximators in specific order
		if(ok == 0 && (*it)->getApproximatedAttributeName() == "position")
		{
			m_positionApproximator = reinterpret_cast<Approximator<PFP, VEC3>* >(*it) ; // 1) position
			++ok ;
		}
		else if( ok == 1 && (*it)->getApproximatedAttributeName() == "frame")
		{
			m_frameApproximator = reinterpret_cast<Approximator<PFP, FRAME>* >(*it) ; // 2) frame (needs position)
			++ok ;
		}
		else if(ok == 2 && (*it)->getApproximatedAttributeName() == "colorPTM")
		{
			m_RGBfunctionsApproximator = reinterpret_cast<Approximator<PFP, RGBFUNCTIONS>* >(*it) ; // 3) functions (needs frame)
			++ok ;
		}
	}

	if(ok != 3)
		return false ;

	edges.clear() ;

	CellMarker vMark(m, VERTEX_CELL) ;
	for(Dart d = m.begin(); d != m.end(); m.next(d))
	{
		if(!vMark.isMarked(d))
		{
			Quadric<REAL> q ;		// create one quadric
			quadric[d] = q ;		// per vertex
			vMark.mark(d) ;
		}
	}

	DartMarker mark(m) ;

	for(Dart d = m.begin(); d != m.end(); m.next(d)) // init QEM quadrics
	{
		if(!mark.isMarked(d))
		{
			Dart d1 = m.phi1(d) ;					// for each triangle,
			Dart d_1 = m.phi_1(d) ;					// initialize the quadric of the triangle
			Quadric<REAL> q(this->m_position[d], this->m_position[d1], this->m_position[d_1]) ;
			quadric[d] += q ;						// and add the contribution of
			quadric[d1] += q ;						// this quadric to the ones
			quadric[d_1] += q ;						// of the 3 incident vertices
			mark.markOrbit(FACE_ORBIT, d) ;
		}
	}

	CellMarker eMark(m, EDGE_CELL) ;
	for(Dart d = m.begin(); d != m.end(); m.next(d))
	{
		if(!eMark.isMarked(d))
		{
			initEdgeInfo(d) ;	// init the edges with their optimal info
			eMark.mark(d) ;		// and insert them in the multimap according to their error
		}
	}

	cur = edges.begin() ; // init the current edge to the first one

	return true ;
}

template <typename PFP>
bool EdgeSelector_Lightfield<PFP>::nextEdge(Dart& d)
{
	if(cur == edges.end() || edges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void EdgeSelector_Lightfield<PFP>::updateBeforeCollapse(Dart d)
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
void EdgeSelector_Lightfield<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
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
			Dart vit2 = m.alpha_1(m.phi1(vit)) ;		// - edges for which the criteria must be recomputed
			Dart stop = m.phi2(vit) ;					// - edges that must be re-embedded
			do											// - edges for which only the collapsibility must be re-tested
			{
				updateEdgeInfo(vit2, false) ;
				updateEdgeInfo(m.phi1(vit2), false) ;
				vit2 = m.alpha_1(vit2) ;
			} while(vit2 != stop) ;
		}
		else
			updateEdgeInfo(vit, true) ;

		vit = m.alpha1(vit) ;
	} while(vit != d2) ;

	cur = edges.begin() ; // set the current edge to the first one
}

template <typename PFP>
void EdgeSelector_Lightfield<PFP>::initEdgeInfo(Dart d)
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
void EdgeSelector_Lightfield<PFP>::updateEdgeInfo(Dart d, bool recompute)
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
		{								 	// if the edge can be collapsed now
			if(!einfo.valid)				 // but it was not before
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
void EdgeSelector_Lightfield<PFP>::computeEdgeInfo(Dart d, EdgeInfo& einfo)
{
	MAP& m = this->m_map ;
	Dart dd = m.phi2(d) ;

	// New position
	Quadric<REAL> quad ;
	quad += quadric[d] ;	// compute the sum of the
	quad += quadric[dd] ;	// two vertices quadrics

	this->m_positionApproximator->approximate(d) ; 		// sets newPos
	VEC3 newPos = this->m_positionApproximator->getApprox(d) ; // get newPos

	// New Frame
	this->m_frameApproximator->approximate(d) ; 		// sets newF
	MATRIX33 newFrame = this->m_frameApproximator->getApprox(d) ; // get newF

	VEC3 n1,n2 ;
	if (! m_frame[d].getSubVectorH(2,0,n1))	{ // get the normals
		std::cout << "EdgeSelector_LightField::computeEdgeInfo --> getSubVectorH 1 failed " << std::endl;
		exit(2) ;
	}
	if (!m_frame[dd].getSubVectorH(2,0,n2))	{ // of the two vertices
		std::cout << "EdgeSelector_LightField::computeEdgeInfo --> getSubVectorH 2 failed " << std::endl;
		exit(3) ;
	}

	// New function
	this->m_RGBfunctionsApproximator->approximate(d) ; 	// sets quadricRGBf and newRGBf
	MATRIX36 newRGBf = this->m_RGBfunctionsApproximator->getApprox(d) ; // get newRGBf

	QuadricRGBfunctions<typename PFP::REAL> quadRGBf = quadricRGBfunctions[d]; // get quadricRGBf

	// Compute error
	REAL err = quad(newPos) + (2 * acos (n1 * n2)) + quadRGBf(newRGBf) ;
//	REAL t1 = quad(newPos) ;
//	REAL t2 = (2 * acos (n1 * n2)) ;
//	REAL t3 = quadRGBf(newRGBf) ;

	einfo.it = edges.insert(std::make_pair(err, d)) ;
	einfo.valid = true ;
}*/

} // namespace Decimation

} // namespace Algo

} // namespace CGoGN
