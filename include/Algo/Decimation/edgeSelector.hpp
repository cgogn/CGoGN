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

//#include <time.h>
//#include "Algo/Geometry/basic.h"

namespace CGoGN
{

namespace Algo
{

namespace Surface
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
	typename PFP::MAP& m = this->m_map ;
	if(cur == m.end())
		return false ;
	d = cur ;
	return true ;
}

template <typename PFP>
void EdgeSelector_MapOrder<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	typename PFP::MAP& m = this->m_map ;
	cur = m.begin() ;
	while( !m.edgeCanCollapse(cur))
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
	typename PFP::MAP& m = this->m_map ;

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
	typename PFP::MAP& m = this->m_map ;
	allSkipped = false ;
	do
	{
		++cur ;
		if(cur == darts.size())
		{
			cur = 0 ;
			allSkipped = true ;
		}
	} while( !m.edgeCanCollapse(darts[cur])) ;
}

template <typename PFP>
void EdgeSelector_Random<PFP>::updateWithoutCollapse()
{
	typename PFP::MAP& m = this->m_map ;
	allSkipped = false ;
	do
	{
		++cur ;
		if(cur == darts.size())
		{
			cur = 0 ;
			allSkipped = true ;
		}
	} while( !m.edgeCanCollapse(darts[cur])) ;
}

/************************************************************************************
 *                                 EDGE LENGTH                                      *
 ************************************************************************************/

template <typename PFP>
bool EdgeSelector_Length<PFP>::init()
{
	typename PFP::MAP& m = this->m_map ;

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
	typename PFP::MAP& m = this->m_map ;

	EdgeInfo *edgeE = &(edgeInfo[d]) ;
	if(edgeE->valid)
		edges.erase(edgeE->it) ;

	edgeE = &(edgeInfo[m.phi1(d)]) ;
	if(edgeE->valid)					// remove all
		edges.erase(edgeE->it) ;

	edgeE = &(edgeInfo[m.phi_1(d)]) ;	// the concerned edges
	if(edgeE->valid)
		edges.erase(edgeE->it) ;
									// from the multimap
	Dart dd = m.phi2(d) ;
	if(dd != d)
	{
		edgeE = &(edgeInfo[m.phi1(dd)]) ;
		if(edgeE->valid)
			edges.erase(edgeE->it) ;

		edgeE = &(edgeInfo[m.phi_1(dd)]) ;
		if(edgeE->valid)
			edges.erase(edgeE->it) ;
	}
}

template <typename PFP>
void EdgeSelector_Length<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	typename PFP::MAP& m = this->m_map ;

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
void EdgeSelector_Length<PFP>::updateWithoutCollapse()
{
	EdgeInfo& einfo = edgeInfo[(*cur).second] ;
	einfo.valid = false ;
	edges.erase(einfo.it) ;

	//edges.erase(cur) ;
	cur = edges.begin();
}

template <typename PFP>
void EdgeSelector_Length<PFP>::initEdgeInfo(Dart d)
{
	typename PFP::MAP& m = this->m_map ;
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
	typename PFP::MAP& m = this->m_map ;
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
	VEC3 vec = Algo::Surface::Geometry::vectorOutOfDart<PFP>(this->m_map, d, this->m_position) ;
	einfo.it = edges.insert(std::make_pair(vec.norm2(), d)) ;
	einfo.valid = true ;
}

/************************************************************************************
 *                            QUADRIC ERROR METRIC                                  *
 ************************************************************************************/

template <typename PFP>
bool EdgeSelector_QEM<PFP>::init()
{
	typename PFP::MAP& m = this->m_map ;

	bool ok = false ;
	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = this->m_approximators.begin();
		it != this->m_approximators.end() && !ok;
		++it)
	{
		if((*it)->getApproximatedAttributeName() == "position")
		{
			assert((*it)->getType() != A_hQEM || !"Approximator(hQEM) and selector (EdgeSelector_QEM) are not compatible") ;
			assert((*it)->getType() != A_hHalfCollapse || !"Approximator(hHalfCollapse) and selector (EdgeSelector_QEM) are not compatible") ;
			assert((*it)->getType() != A_Lightfield || !"Approximator(hLightfield) and selector (EdgeSelector_QEM) are not compatible") ;
			m_positionApproximator = reinterpret_cast<Approximator<PFP, VEC3,EDGE>* >(*it) ;
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
			Utils::Quadric<REAL> q ;	// create one quadric
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
			Utils::Quadric<REAL> q(this->m_position[d], this->m_position[d1], this->m_position[d_1]) ;
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
	typename PFP::MAP& m = this->m_map ;

	EdgeInfo *edgeE = &(edgeInfo[d]) ;
	if(edgeE->valid)
		edges.erase(edgeE->it) ;

	edgeE = &(edgeInfo[m.phi1(d)]) ;
	if(edgeE->valid)					// remove all
		edges.erase(edgeE->it) ;

	edgeE = &(edgeInfo[m.phi_1(d)]) ;	// the concerned edges
	if(edgeE->valid)
		edges.erase(edgeE->it) ;
									// from the multimap
	Dart dd = m.phi2(d) ;
	if(dd != d)
	{
		edgeE = &(edgeInfo[m.phi1(dd)]) ;
		if(edgeE->valid)
			edges.erase(edgeE->it) ;

		edgeE = &(edgeInfo[m.phi_1(dd)]) ;
		if(edgeE->valid)
			edges.erase(edgeE->it) ;
	}

	tmpQ.zero() ;			// compute quadric for the new
	tmpQ += quadric[d] ;	// vertex as the sum of those
	tmpQ += quadric[dd] ;	// of the contracted vertices
}

template <typename PFP>
void EdgeSelector_QEM<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	typename PFP::MAP& m = this->m_map ;

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
void EdgeSelector_QEM<PFP>::updateWithoutCollapse()
{
	EdgeInfo& einfo = edgeInfo[(*cur).second] ;
	einfo.valid = false ;
	edges.erase(einfo.it) ;

	//edges.erase(cur) ;
	cur = edges.begin();
}

template <typename PFP>
void EdgeSelector_QEM<PFP>::initEdgeInfo(Dart d)
{
	typename PFP::MAP& m = this->m_map ;
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
	typename PFP::MAP& m = this->m_map ;
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
	typename PFP::MAP& m = this->m_map ;
	Dart dd = m.phi2(d) ;

	Utils::Quadric<REAL> quad ;
	quad += quadric[d] ;	// compute the sum of the
	quad += quadric[dd] ;	// two vertices quadrics

	m_positionApproximator->approximate(d) ;

	REAL err = quad(m_positionApproximator->getApprox(d)) ;

	einfo.it = edges.insert(std::make_pair(err, d)) ;
	einfo.valid = true ;
}

/************************************************************************************
 *                            QUADRIC ERROR METRIC (Memoryless version)             *
 ************************************************************************************/

template <typename PFP>
bool EdgeSelector_QEMml<PFP>::init()
{
	typename PFP::MAP& m = this->m_map ;

	bool ok = false ;
	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = this->m_approximators.begin();
		it != this->m_approximators.end() && !ok;
		++it)
	{
		if((*it)->getApproximatedAttributeName() == "position")
		{
			assert((*it)->getType() != A_hQEM || !"Approximator(hQEM) and selector (EdgeSelector_QEMml) are not compatible") ;
			assert((*it)->getType() != A_hHalfCollapse || !"Approximator(hHalfCollapse) and selector (EdgeSelector_QEMml) are not compatible") ;
			assert((*it)->getType() != A_Lightfield || !"Approximator(hLightfield) and selector (EdgeSelector_QEMml) are not compatible") ;
			m_positionApproximator = reinterpret_cast<Approximator<PFP, VEC3,EDGE>* >(*it) ;
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
			Utils::Quadric<REAL> q ;	// create one quadric
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
			Utils::Quadric<REAL> q(this->m_position[d], this->m_position[d1], this->m_position[d_1]) ;
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
	typename PFP::MAP& m = this->m_map ;

	EdgeInfo *edgeE = &(edgeInfo[d]) ;
	if(edgeE->valid)
		edges.erase(edgeE->it) ;

	edgeE = &(edgeInfo[m.phi1(d)]) ;
	if(edgeE->valid)					// remove all
		edges.erase(edgeE->it) ;

	edgeE = &(edgeInfo[m.phi_1(d)]) ;	// the concerned edges
	if(edgeE->valid)
		edges.erase(edgeE->it) ;
									// from the multimap
	Dart dd = m.phi2(d) ;
	if(dd != d)
	{
		edgeE = &(edgeInfo[m.phi1(dd)]) ;
		if(edgeE->valid)
			edges.erase(edgeE->it) ;

		edgeE = &(edgeInfo[m.phi_1(dd)]) ;
		if(edgeE->valid)
			edges.erase(edgeE->it) ;
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
           	quadric[d] += Utils::Quadric<REAL>(this->m_position[d],this->m_position[this->m_map.phi2(dFront)],this->m_position[dBack]) ;
       	}
       	if (recomputeNeighbors)
       		recomputeQuadric(dBack, false) ;

    } while(dFront != dInit) ;
}

template <typename PFP>
void EdgeSelector_QEMml<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	typename PFP::MAP& m = this->m_map ;

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
void EdgeSelector_QEMml<PFP>::updateWithoutCollapse()
{
	EdgeInfo& einfo = edgeInfo[(*cur).second] ;
	einfo.valid = false ;
	edges.erase(einfo.it) ;

	//edges.erase(cur) ;
	cur = edges.begin();
}

template <typename PFP>
void EdgeSelector_QEMml<PFP>::initEdgeInfo(Dart d)
{
	typename PFP::MAP& m = this->m_map ;
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
	typename PFP::MAP& m = this->m_map ;
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
	typename PFP::MAP& m = this->m_map ;
	Dart dd = m.phi2(d) ;

	Utils::Quadric<REAL> quad ;
	quad += quadric[d] ;	// compute the sum of the
	quad += quadric[dd] ;	// two vertices quadrics

	m_positionApproximator->approximate(d) ;

	REAL err = quad(m_positionApproximator->getApprox(d)) ;
	einfo.it = edges.insert(std::make_pair(err, d)) ;
	einfo.valid = true ;
}

/************************************************************************************
 *                            Metric based on Face Normal and Area deviation        *
 ************************************************************************************/

template <typename PFP>
bool EdgeSelector_NormalArea<PFP>::init()
{
	typename PFP::MAP& m = this->m_map ;

	bool ok = false ;
	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = this->m_approximators.begin();
		it != this->m_approximators.end() && !ok;
		++it)
	{
		if((*it)->getApproximatedAttributeName() == "position")
		{
//			assert((*it)->getType() == A_MidEdge || (*it)->getType() == A_NormalArea || !"Only MidEdge and NormalArea Approximator are valid") ;
			m_positionApproximator = reinterpret_cast<Approximator<PFP, VEC3,EDGE>* >(*it) ;
			ok = true ;
		}
	}

	if(!ok)
		return false ;

	edges.clear() ;

	TraversorE<MAP> travE(m);
	for(Dart dit = travE.begin() ; dit != travE.end() ; dit = travE.next())
	{
		computeEdgeMatrix(dit);
//		const VEC3 e = Algo::Geometry::vectorOutOfDart<PFP>(m, dit, this->m_position) ;
//		edgeMatrix[dit].identity();
//		edgeMatrix[dit] *= e.norm2();
//		edgeMatrix[dit] -= Geom::transposed_vectors_mult(e,e) ;
	}

	for(Dart dit = travE.begin() ; dit != travE.end() ; dit = travE.next())
	{
		initEdgeInfo(dit) ;	// init "edgeInfo" and "edges"
	}

	cur = edges.begin() ; // init the current edge to the first one

	return true ;
}

template <typename PFP>
bool EdgeSelector_NormalArea<PFP>::nextEdge(Dart& d)
{
	if(cur == edges.end() || edges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void EdgeSelector_NormalArea<PFP>::updateBeforeCollapse(Dart d)
{
	typename PFP::MAP& m = this->m_map ;
	assert(!m.isBoundaryEdge(d));

	EdgeInfo* edgeE = &(edgeInfo[d]) ;
	if(edgeE->valid)
	{
		edges.erase(edgeE->it) ;
		edgeE->valid = false;
	}

	edgeE = &(edgeInfo[m.phi1(d)]) ;
	if(edgeE->valid)					// remove all
	{
		edges.erase(edgeE->it) ;
		edgeE->valid = false;
	}

	edgeE = &(edgeInfo[m.phi_1(d)]) ;	// the concerned edges
	if(edgeE->valid)
	{
		edges.erase(edgeE->it) ;
		edgeE->valid = false;
	}

									// from the multimap
	Dart dd = m.phi2(d) ;
	edgeE = &(edgeInfo[m.phi1(dd)]) ;
	if(edgeE->valid)
	{
		edges.erase(edgeE->it) ;
		edgeE->valid = false;
	}

	edgeE = &(edgeInfo[m.phi_1(dd)]) ;
	if(edgeE->valid)
	{
		edges.erase(edgeE->it) ;
		edgeE->valid = false;
	}
}

template <typename PFP>
void EdgeSelector_NormalArea<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	typename PFP::MAP& m = this->m_map ;

	// update the edge matrices
	Traversor2VE<MAP> te (m,d2);
	for(Dart dit = te.begin() ; dit != te.end() ; dit = te.next())
	{
		computeEdgeMatrix(dit);
	}

	// update the multimap

	Traversor2VVaE<MAP> tv (m,d2);
	CellMarker<EDGE> eMark (m);

	for(Dart dit = tv.begin() ; dit != tv.end() ; dit = tv.next())
	{
		Traversor2VE<MAP> te2 (m,dit);
		for(Dart dit2 = te2.begin() ; dit2 != te2.end() ; dit2 = te2.next())
		{
			if (!eMark.isMarked(dit2))
			{
				updateEdgeInfo(dit2) ;
				eMark.mark(dit2);
			}
		}
	}

	cur = edges.begin() ; // set the current edge to the first one
}

template <typename PFP>
void EdgeSelector_NormalArea<PFP>::initEdgeInfo(Dart d)
{
	typename PFP::MAP& m = this->m_map ;
	EdgeInfo einfo ;
	if(m.edgeCanCollapse(d))
		computeEdgeInfo(d, einfo) ;
	else
		einfo.valid = false ;
	edgeInfo[d] = einfo ;
}

template <typename PFP>
void EdgeSelector_NormalArea<PFP>::updateEdgeInfo(Dart d)
{
	typename PFP::MAP& m = this->m_map ;
	EdgeInfo& einfo = edgeInfo[d] ;

	if(einfo.valid)
		edges.erase(einfo.it) ;		// remove the edge from the multimap

	if(m.edgeCanCollapse(d))
		computeEdgeInfo(d, einfo) ;
	else
		einfo.valid = false ;
}

template <typename PFP>
void EdgeSelector_NormalArea<PFP>::computeEdgeInfo(Dart d, EdgeInfo& einfo)
{
	typename PFP::MAP& m = this->m_map ;
	Dart dd = m.phi2(d);
	Geom::Matrix33f M1; // init zero included
	Geom::Matrix33f M2; // init zero included

	assert(! m.isBoundaryEdge(d));

	Traversor2VF<MAP> td (m,d);
	Dart it = td.begin();
	it = td.next();
	Dart it2 = td.next();
	while( it2 != td.end())
	{
		M1 += edgeMatrix[m.phi1(it)];
		it = it2;
		it2 = td.next();
	}

	Traversor2VF<MAP> tdd (m,dd);
	it = tdd.begin();
	it = tdd.next();
	it2 = tdd.next();
	while( it2 != tdd.end())
	{
		M2 += edgeMatrix[m.phi1(it)];
		it = it2;
		it2 = tdd.next();
	}

	m_positionApproximator->approximate(d) ;
	const VEC3& a = m_positionApproximator->getApprox(d) ;

	const VEC3 av1 = a - this->m_position[d] ;
	const VEC3 av2 = a - this->m_position[dd] ;

	REAL err = av1 * (M1 * av1) + av2 * (M2 * av2);

/*
	// TODO : test to normalize by area
	// TODO : not border-safe
 	REAL area = 0.0;
	Traversor2EEaV<MAP> ta (m,d);
	for (Dart dita = ta.begin(); dita != ta.end(); dita=ta.next())
	{
		area += Algo::Geometry::triangleArea<PFP>(m,dita,this->m_position);
	}
	err /= area ; // résultats sensiblment identiques à ceux sans pris en compte de l'aire.
//	err /= area*area ; // ca favorise la contraction des gros triangles : maillages très in-homogènes et qualité géométrique mauvaise
*/
	einfo.it = edges.insert(std::make_pair(err, d)) ;
	einfo.valid = true ;
}

template <typename PFP>
void EdgeSelector_NormalArea<PFP>::computeEdgeMatrix(Dart d)
{
	const typename PFP::VEC3 e = Algo::Surface::Geometry::vectorOutOfDart<PFP>(this->m_map, d, this->m_position) ;
	edgeMatrix[d].identity();
	edgeMatrix[d] *= e.norm2();
	edgeMatrix[d] -= Geom::transposed_vectors_mult(e,e) ;
	// TODO : test : try to normalize by area
//	edgeMatrix[d] /= e.norm2(); // pas d'amélioration significative par rapport à la version sans normalisation
//	edgeMatrix[d] /= e.norm2() * e.norm2(); // étonnament bon : sur certains maillages équivalant à la QEMml
}

/************************************************************************************
 *                                   CURVATURE                                      *
 ************************************************************************************/

template <typename PFP>
bool EdgeSelector_Curvature<PFP>::init()
{
	typename PFP::MAP& m = this->m_map ;

	bool ok = false ;
	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = this->m_approximators.begin();
		it != this->m_approximators.end() && !ok;
		++it)
	{
		if((*it)->getApproximatedAttributeName() == "position")
		{
			assert((*it)->getType() != A_hQEM || !"Approximator(hQEM) and selector (EdgeSelector_Curvature) are not compatible") ;
			assert((*it)->getType() != A_hHalfCollapse || !"Approximator(hHalfCollapse) and selector (EdgeSelector_Curvature) are not compatible") ;
			assert((*it)->getType() != A_Lightfield || !"Approximator(hLightfield) and selector (EdgeSelector_Curvature) are not compatible") ;
			m_positionApproximator = reinterpret_cast<Approximator<PFP, VEC3,EDGE>* >(*it) ;
			ok = true ;
		}
	}

	if(!ok)
		return false ;

	edges.clear() ;

	CellMarker<VERTEX> eMark(m) ;
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
	typename PFP::MAP& m = this->m_map ;

	EdgeInfo *edgeE = &(edgeInfo[d]) ;
	if(edgeE->valid)
		edges.erase(edgeE->it) ;

	edgeE = &(edgeInfo[m.phi1(d)]) ;
	if(edgeE->valid)					// remove all
		edges.erase(edgeE->it) ;

	edgeE = &(edgeInfo[m.phi_1(d)]) ;	// the concerned edges
	if(edgeE->valid)
		edges.erase(edgeE->it) ;
									// from the multimap
	Dart dd = m.phi2(d) ;
	if(dd != d)
	{
		edgeE = &(edgeInfo[m.phi1(dd)]) ;
		if(edgeE->valid)
			edges.erase(edgeE->it) ;

		edgeE = &(edgeInfo[m.phi_1(dd)]) ;
		if(edgeE->valid)
			edges.erase(edgeE->it) ;
	}
}

template <typename PFP>
void EdgeSelector_Curvature<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	typename PFP::MAP& m = this->m_map ;

	normal[d2] = Algo::Surface::Geometry::vertexNormal<PFP>(m, d2, this->m_position) ;
	Algo::Surface::Geometry::computeCurvatureVertex_NormalCycles<PFP>(m, d2, radius, this->m_position, normal, edgeangle, kmax, kmin, Kmax, Kmin, Knormal) ;

	Dart vit = d2 ;
	do
	{
		Dart nVert = m.phi1(vit) ;
		normal[nVert] = Algo::Surface::Geometry::vertexNormal<PFP>(m, nVert, this->m_position) ;
		Algo::Surface::Geometry::computeCurvatureVertex_NormalCycles<PFP>(m, nVert, radius, this->m_position, normal, edgeangle, kmax, kmin, Kmax, Kmin, Knormal) ;

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
void EdgeSelector_Curvature<PFP>::updateWithoutCollapse()
{
	EdgeInfo& einfo = edgeInfo[(*cur).second] ;
	einfo.valid = false ;
	edges.erase(einfo.it) ;

	//edges.erase(cur) ;
	cur = edges.begin();
}

template <typename PFP>
void EdgeSelector_Curvature<PFP>::initEdgeInfo(Dart d)
{
	typename PFP::MAP& m = this->m_map ;
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
	typename PFP::MAP& m = this->m_map ;
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
	typename PFP::MAP& m = this->m_map ;
	Dart dd = m.phi2(d) ;

	unsigned int v1 = m.template getEmbedding<VERTEX>(d) ;
	unsigned int v2 = m.template getEmbedding<VERTEX>(dd) ;

	m_positionApproximator->approximate(d) ;

	// temporary edge collapse
	Dart d2 = m.phi2(m.phi_1(d)) ;
	Dart dd2 = m.phi2(m.phi_1(dd)) ;
	m.extractTrianglePair(d) ;
	unsigned int newV = m.template setOrbitEmbeddingOnNewCell<VERTEX>(d2) ;
	this->m_position[newV] = m_positionApproximator->getApprox(d) ;

	// compute things on the coarse version of the mesh
	normal[newV] = Algo::Surface::Geometry::vertexNormal<PFP>(m, d2, this->m_position) ;
	Algo::Surface::Geometry::computeCurvatureVertex_NormalCycles<PFP>(m, d2, radius, this->m_position, normal, edgeangle, kmax, kmin, Kmax, Kmin, Knormal) ;

//	VEC3 norm = normal[newV] ;
	REAL mCurv = (kmax[newV] + kmin[newV]) / REAL(2) ;
//	VEC3 cDir1 = Kmax[newV] ;

	// vertex split to reset the initial connectivity and embeddings
	m.insertTrianglePair(d, d2, dd2) ;
	m.template setOrbitEmbedding<VERTEX>(d, v1) ;
	m.template setOrbitEmbedding<VERTEX>(dd, v2) ;

	REAL err = 0 ;

//	REAL norm_deviation_1 = REAL(1) / fabs(norm * normal[v1]) ;
//	REAL norm_deviation_2 = REAL(1) / fabs(norm * normal[v2]) ;
//	err += norm_deviation_1 + norm_deviation_2 ;

	REAL mCurv_deviation_1 = fabs(mCurv - (kmax[v1] + kmin[v1] / REAL(2))) ;
	REAL mCurv_deviation_2 = fabs(mCurv - (kmax[v2] + kmin[v2] / REAL(2))) ;
	err += mCurv_deviation_1 + mCurv_deviation_2 ;

//	REAL cDir1_deviation_1 = REAL(1) / fabs(cDir1 * Kmax[v1]) ;
//	REAL cDir1_deviation_2 = REAL(1) / fabs(cDir1 * Kmax[v2]) ;
//	err += cDir1_deviation_1 + cDir1_deviation_2 ;

	einfo.it = edges.insert(std::make_pair(err, d)) ;
	einfo.valid = true ;
}

/************************************************************************************
 *                            CURVATURE TENSOR                                      *
 ************************************************************************************/

template <typename PFP>
bool EdgeSelector_CurvatureTensor<PFP>::init()
{
	typename PFP::MAP& m = this->m_map ;

	bool ok = false ;
	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = this->m_approximators.begin();
		it != this->m_approximators.end() && !ok;
		++it)
	{
		if((*it)->getApproximatedAttributeName() == "position")
		{
			assert((*it)->getType() == A_MidEdge || !"Only MidEdge Approximator is valid") ;
			m_positionApproximator = reinterpret_cast<Approximator<PFP, VEC3,EDGE>* >(*it) ;
			ok = true ;
		}
	}

	if(!ok)
		return false ;

	edges.clear() ;

	TraversorE<MAP> travE(m);
//	for(Dart dit = travE.begin() ; dit != travE.end() ; dit = travE.next())
//	{
//		computeEdgeMatrix(dit);
//	}

	for(Dart dit = travE.begin() ; dit != travE.end() ; dit = travE.next())
	{
		initEdgeInfo(dit) ;	// init "edgeInfo" and "edges"
	}

	cur = edges.begin() ; // init the current edge to the first one

	return true ;
}

template <typename PFP>
bool EdgeSelector_CurvatureTensor<PFP>::nextEdge(Dart& d)
{
	if(cur == edges.end() || edges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void EdgeSelector_CurvatureTensor<PFP>::updateBeforeCollapse(Dart d)
{
	typename PFP::MAP& m = this->m_map ;
	assert(!m.isBoundaryEdge(d));

	EdgeInfo* edgeE = &(edgeInfo[d]) ;
	if(edgeE->valid)
	{
		edges.erase(edgeE->it) ;
		edgeE->valid = false;
	}

	edgeE = &(edgeInfo[m.phi1(d)]) ;
	if(edgeE->valid)					// remove all
	{
		edges.erase(edgeE->it) ;
		edgeE->valid = false;
	}

	edgeE = &(edgeInfo[m.phi_1(d)]) ;	// the concerned edges
	if(edgeE->valid)
	{
		edges.erase(edgeE->it) ;
		edgeE->valid = false;
	}

									// from the multimap
	Dart dd = m.phi2(d) ;
	edgeE = &(edgeInfo[m.phi1(dd)]) ;
	if(edgeE->valid)
	{
		edges.erase(edgeE->it) ;
		edgeE->valid = false;
	}

	edgeE = &(edgeInfo[m.phi_1(dd)]) ;
	if(edgeE->valid)
	{
		edges.erase(edgeE->it) ;
		edgeE->valid = false;
	}
}

template <typename PFP>
void EdgeSelector_CurvatureTensor<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	typename PFP::MAP& m = this->m_map ;
	CellMarkerStore<EDGE> eMark (m);

	// update edge angles
	Traversor2VF<MAP> tf (m,d2);
	for(Dart dit = tf.begin(); dit != tf.end(); dit = tf.next())
	{
		Traversor2FE<MAP> te (m,dit);
		for(Dart dit2 = te.begin(); dit2 != te.end(); dit2=te.next())
		{
			if (!eMark.isMarked(dit2))
			{
				edgeangle[dit2] = Algo::Surface::Geometry::computeAngleBetweenNormalsOnEdge<PFP>(m, dit2, this->m_position) ;
				eMark.mark(dit2);
			}
		}
	}


	// update the multimap

	Traversor2VVaE<MAP> tv (m,d2);
	eMark.unmarkAll();

	for(Dart dit = tv.begin() ; dit != tv.end() ; dit = tv.next())
	{
		Traversor2VE<MAP> te2 (m,dit);
		for(Dart dit2 = te2.begin() ; dit2 != te2.end() ; dit2 = te2.next())
		{
			if (!eMark.isMarked(dit2))
			{
				updateEdgeInfo(dit2) ;
				eMark.mark(dit2);
			}
		}
	}

	cur = edges.begin() ; // set the current edge to the first one
}

template <typename PFP>
void EdgeSelector_CurvatureTensor<PFP>::initEdgeInfo(Dart d)
{
	typename PFP::MAP& m = this->m_map ;
	EdgeInfo einfo ;
	if(m.edgeCanCollapse(d))
		computeEdgeInfo(d, einfo) ;
	else
		einfo.valid = false ;
	edgeInfo[d] = einfo ;
}

template <typename PFP>
void EdgeSelector_CurvatureTensor<PFP>::updateEdgeInfo(Dart d)
{
	typename PFP::MAP& m = this->m_map ;
	EdgeInfo& einfo = edgeInfo[d] ;

	if(einfo.valid)
		edges.erase(einfo.it) ;		// remove the edge from the multimap

	if(m.edgeCanCollapse(d))
		computeEdgeInfo(d, einfo) ;
	else
		einfo.valid = false ;
}

template <typename PFP>
void EdgeSelector_CurvatureTensor<PFP>::computeEdgeInfo(Dart d, EdgeInfo& einfo)
{

	typedef Geom::Matrix<3,3,REAL> MATRIX;
//	typedef Eigen::Matrix<REAL,3,1> E_VEC3;
	typedef Eigen::Matrix<REAL,3,3,Eigen::RowMajor> E_MATRIX;

	typename PFP::MAP& m = this->m_map ;
	Dart dd = m.phi2(d) ;

	unsigned int v1 = m.template getEmbedding<VERTEX>(d) ;
	unsigned int v2 = m.template getEmbedding<VERTEX>(dd) ;

	m_positionApproximator->approximate(d) ;

	// compute tensor before collapse
	MATRIX tens1;
	Algo::Surface::Selection::Collector_OneRing_AroundEdge<PFP> col1 (m);
	col1.collectAll(d);
	col1.computeNormalCyclesTensor(this->m_position,edgeangle,tens1); // edgeangle is up to date here
	tens1 *= col1.computeArea(this->m_position); // mean tensor * area = integral of the tensor
	Algo::Surface::Geometry::normalCycles_SortTensor<PFP>(tens1);

	// temporary edge collapse
	Dart d2 = m.phi2(m.phi_1(d)) ;
	Dart dd2 = m.phi2(m.phi_1(dd)) ;
	m.extractTrianglePair(d) ;
	const unsigned int newV = m.template setOrbitEmbeddingOnNewCell<VERTEX>(d2) ;
	this->m_position[newV] = m_positionApproximator->getApprox(d) ;

	// compute tensor after collapse
	MATRIX tens2;
	Algo::Surface::Selection::Collector_OneRing<PFP> col2 (m);
	col2.collectAll(d);
	col2.computeNormalCyclesTensor(this->m_position,tens2); // edgeangle is not up to date here
	tens2 *= col2.computeArea(this->m_position); // mean tensor * area = integral of the tensor
	Algo::Surface::Geometry::normalCycles_SortTensor<PFP>(tens2);

	// vertex split to reset the initial connectivity and embeddings
	m.insertTrianglePair(d, d2, dd2) ;
	m.template setOrbitEmbedding<VERTEX>(d, v1) ;
	m.template setOrbitEmbedding<VERTEX>(dd, v2) ;

	// compute err from the tensors
	tens1 -= tens2;

	Eigen::SelfAdjointEigenSolver<E_MATRIX> solver (Utils::convertRef<E_MATRIX>(tens1),Eigen::EigenvaluesOnly);
	const VEC3& e_val = Utils::convertRef<VEC3>(solver.eigenvalues());

	REAL err = std::max(std::max(fabs(e_val[0]), fabs(e_val[1])) , fabs(e_val[2])) ;

//	if (v1 % 5000 == 0) CGoGNout << e_val << CGoGNendl << err << CGoGNendl ;

	// update the priority queue and edgeinfo
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
			assert((*it)->getType() != A_hQEM || !"Approximator(hQEM) and selector (EdgeSelector_MinDetail) are not compatible") ;
			assert((*it)->getType() != A_hHalfCollapse || !"Approximator(hHalfCollapse) and selector (EdgeSelector_MinDetail) are not compatible") ;
			assert((*it)->getType() != A_Lightfield || !"Approximator(hLightfield) and selector (EdgeSelector_MinDetail) are not compatible") ;
			m_positionApproximator = reinterpret_cast<Approximator<PFP, VEC3,EDGE>* >(*it) ;
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
	typename PFP::MAP& m = this->m_map ;

	EdgeInfo *edgeE = &(edgeInfo[d]) ;
	if(edgeE->valid)
		edges.erase(edgeE->it) ;

	edgeE = &(edgeInfo[m.phi1(d)]) ;
	if(edgeE->valid)					// remove all
		edges.erase(edgeE->it) ;

	edgeE = &(edgeInfo[m.phi_1(d)]) ;	// the concerned edges
	if(edgeE->valid)
		edges.erase(edgeE->it) ;
									// from the multimap
	Dart dd = m.phi2(d) ;
	if(dd != d)
	{
		edgeE = &(edgeInfo[m.phi1(dd)]) ;
		if(edgeE->valid)
			edges.erase(edgeE->it) ;

		edgeE = &(edgeInfo[m.phi_1(dd)]) ;
		if(edgeE->valid)
			edges.erase(edgeE->it) ;
	}
}

template <typename PFP>
void EdgeSelector_MinDetail<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	typename PFP::MAP& m = this->m_map ;

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
void EdgeSelector_MinDetail<PFP>::updateWithoutCollapse()
{
	EdgeInfo& einfo = edgeInfo[(*cur).second] ;
	einfo.valid = false ;
	edges.erase(einfo.it) ;

	//edges.erase(cur) ;
	cur = edges.begin();
}

template <typename PFP>
void EdgeSelector_MinDetail<PFP>::initEdgeInfo(Dart d)
{
	typename PFP::MAP& m = this->m_map ;
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
	typename PFP::MAP& m = this->m_map ;
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

/************************************************************************************
 *                         EDGESELECTOR COLOR PER VERTEX                            *
 ************************************************************************************/

template <typename PFP>
bool EdgeSelector_ColorNaive<PFP>::init()
{
	typename PFP::MAP& m = this->m_map ;

	// Verify availability of required approximators
	unsigned int ok = 0 ;
	for (unsigned int approxindex = 0 ; approxindex < this->m_approximators.size() ; ++approxindex)
	{
		assert(this->m_approximators[approxindex]->getType() != A_hQEM || !"Approximator(hQEM) and selector (ColorNaive) are not compatible") ;
		assert(this->m_approximators[approxindex]->getType() != A_hHalfCollapse || !"Approximator(hHalfCollapse) and selector (ColorNaive) are not compatible") ;
		assert(this->m_approximators[approxindex]->getType() != A_Lightfield || !"Approximator(hLightfield) and selector (ColorNaive) are not compatible") ;

		bool saved = false ;
		for (unsigned int attrindex = 0 ; attrindex < this->m_approximators[approxindex]->getNbApproximated() ; ++ attrindex)
		{
			// constraint : 2 approximators in specific order
			if(ok == 0 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "position")
			{
				++ok ;
				m_approxindex_pos = approxindex ;
				m_attrindex_pos = attrindex ;
				m_pos = this->m_position ;
				if (!saved)
				{
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3,EDGE>* >(this->m_approximators[approxindex])) ;
					saved = true ;
				}
			}
			else if(ok == 1 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "color")
			{
				++ok ;
				m_approxindex_color = approxindex ;
				m_attrindex_color = attrindex ;
				m_color = m.template getAttribute<typename PFP::VEC3, VERTEX>("color") ;
				assert(m_color.isValid() || !"EdgeSelector_ColorNaive: color attribute is not valid") ;
				if (!saved)
				{
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3,EDGE>* >(this->m_approximators[approxindex])) ;
					saved = true ;
				}
			}
		}
	}

	if(ok != 2)
		return false ;

	TraversorV<MAP> travV(m);
	for(Dart dit = travV.begin() ; dit != travV.end() ; dit = travV.next())
	{
		Utils::Quadric<REAL> q ;		// create one quadric
		m_quadric[dit] = q ;		// per vertex
	}

	// Compute quadric per vertex
	TraversorF<MAP> travF(m) ;
	for(Dart dit = travF.begin() ; dit != travF.end() ; dit = travF.next()) // init QEM quadrics
	{
		Dart d1 = m.phi1(dit) ;					// for each triangle,
		Dart d_1 = m.phi_1(dit) ;					// initialize the quadric of the triangle
		Utils::Quadric<REAL> q(this->m_position[dit], this->m_position[d1], this->m_position[d_1]) ;
		m_quadric[dit] += q ;						// and add the contribution of
		m_quadric[d1] += q ;						// this quadric to the ones
		m_quadric[d_1] += q ;						// of the 3 incident vertices
	}

	TraversorE<MAP> travE(m);
	for(Dart dit = travE.begin() ; dit != travE.end() ; dit = travE.next())
	{
		initEdgeInfo(dit) ; // init the edges with their optimal position
							// and insert them in the multimap according to their error
	}

	cur = edges.begin() ; // init the current edge to the first one

	return true ;
}

template <typename PFP>
bool EdgeSelector_ColorNaive<PFP>::nextEdge(Dart& d)
{
	if(cur == edges.end() || edges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void EdgeSelector_ColorNaive<PFP>::updateBeforeCollapse(Dart d)
{
	typename PFP::MAP& m = this->m_map ;

	EdgeInfo *edgeE = &(edgeInfo[d]) ;
	if(edgeE->valid)
		edges.erase(edgeE->it) ;

	edgeE = &(edgeInfo[m.phi1(d)]) ;
	if(edgeE->valid)						// remove all
		edges.erase(edgeE->it) ;

	edgeE = &(edgeInfo[m.phi_1(d)]) ;	// the edges that will disappear
	if(edgeE->valid)
		edges.erase(edgeE->it) ;
										// from the multimap
	Dart dd = m.phi2(d) ;
	if(dd != d)
	{
		edgeE = &(edgeInfo[m.phi1(dd)]) ;
		if(edgeE->valid)
			edges.erase(edgeE->it) ;

		edgeE = &(edgeInfo[m.phi_1(dd)]) ;
		if(edgeE->valid)
			edges.erase(edgeE->it) ;
	}
}

/*
 * Update quadric of a vertex
 * Discards quadrics of d and assigns freshly calculated
 * quadrics depending on the actual planes surrounding d
 * @param dart d
 */
template <typename PFP>
void EdgeSelector_ColorNaive<PFP>::recomputeQuadric(const Dart d, const bool recomputeNeighbors)
{
	Dart dFront,dBack ;
	Dart dInit = d ;

	// Init Front
	dFront = dInit ;

	m_quadric[d].zero() ;

   	do
   	{
   		// Make step
   		dBack = this->m_map.phi2(dFront) ;
       	dFront = this->m_map.phi2_1(dFront) ;

       	if (dBack != dFront)
       	{ // if dFront is no border
       		m_quadric[d] += Utils::Quadric<REAL>(this->m_position[d],this->m_position[this->m_map.phi2(dFront)],this->m_position[dBack]) ;
       	}
       	if (recomputeNeighbors)
       		recomputeQuadric(dBack, false) ;

    } while(dFront != dInit) ;
}

template <typename PFP>
void EdgeSelector_ColorNaive<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	typename PFP::MAP& m = this->m_map ;

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
void EdgeSelector_ColorNaive<PFP>::initEdgeInfo(Dart d)
{
	typename PFP::MAP& m = this->m_map ;
	EdgeInfo einfo ;
	if(m.edgeCanCollapse(d))
		computeEdgeInfo(d, einfo) ;
	else
		einfo.valid = false ;
	edgeInfo[d] = einfo ;
}

template <typename PFP>
void EdgeSelector_ColorNaive<PFP>::updateEdgeInfo(Dart d, bool recompute)
{
	typename PFP::MAP& m = this->m_map ;
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
void EdgeSelector_ColorNaive<PFP>::computeEdgeInfo(Dart d, EdgeInfo& einfo)
{
	typename PFP::MAP& m = this->m_map ;
	Dart dd = m.phi1(d) ;

	// New position
	Utils::Quadric<REAL> quad ;
	quad += m_quadric[d] ;	// compute the sum of the
	quad += m_quadric[dd] ;	// two vertices quadrics

	// compute all approximated attributes
	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = this->m_approximators.begin() ;
			it != this->m_approximators.end() ;
			++it)
	{
		(*it)->approximate(d) ;
	}

	// get pos
	const VEC3& newPos = this->m_approx[m_approxindex_pos]->getApprox(d,m_attrindex_pos) ; // get newPos
	// get col
	const VEC3& newCol = this->m_approx[m_approxindex_color]->getApprox(d,m_attrindex_color) ; // get newPos

	// compute error
	VEC3 colDiff1 = newCol ;
	VEC3 colDiff2 = newCol ;
	colDiff1 -= m_color[d] ;
	colDiff2 -= m_color[dd] ;
	const VEC3& colDiff = colDiff1 + colDiff2 ;

	// sum of QEM metric and squared difference between new color and old colors
	REAL err = quad(newPos) + colDiff.norm() ;

	einfo.it = edges.insert(std::make_pair(err, d)) ;
	einfo.valid = true ;
}

/************************************************************************************
 *                         EDGESELECTOR GEOM+COL OPT GRADIENT                       *
 ************************************************************************************/

template <typename PFP>
bool EdgeSelector_GeomColOptGradient<PFP>::init()
{
	typename PFP::MAP& m = this->m_map ;

	// Verify availability of required approximators
	unsigned int ok = 0 ;
	for (unsigned int approxindex = 0 ; approxindex < this->m_approximators.size() ; ++approxindex)
	{
		bool saved = false ;
		for (unsigned int attrindex = 0 ; attrindex < this->m_approximators[approxindex]->getNbApproximated() ; ++ attrindex)
		{
			// constraint : 2 approximators in specific order
			if(ok == 0 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "position")
			{
				++ok ;
				m_approxindex_pos = approxindex ;
				m_attrindex_pos = attrindex ;
				m_pos = this->m_position ;
				if (!saved)
				{
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3,EDGE>* >(this->m_approximators[approxindex])) ;
					saved = true ;
				}
			}
			else if(ok == 1 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "color")
			{
				++ok ;
				m_approxindex_color = approxindex ;
				m_attrindex_color = attrindex ;
				m_color = m.template getAttribute<typename PFP::VEC3, VERTEX>("color") ;
				assert(m_color.isValid() || !"EdgeSelector_GeomColOptGradient: color attribute is not valid") ;
				if (!saved)
				{
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3,EDGE>* >(this->m_approximators[approxindex])) ;
					saved = true ;
				}
			}
		}
	}

	if(ok != 2)
		return false ;

	TraversorV<MAP> travV(m);
	for(Dart dit = travV.begin() ; dit != travV.end() ; dit = travV.next())
	{
		Utils::Quadric<REAL> q ;	// create one quadric
		m_quadric[dit] = q ;		// per vertex
	}

	// Compute quadric per vertex
	TraversorF<MAP> travF(m) ;
	for(Dart dit = travF.begin() ; dit != travF.end() ; dit = travF.next()) // init QEM quadrics
	{
		Dart d1 = m.phi1(dit) ;					// for each triangle,
		Dart d_1 = m.phi_1(dit) ;					// initialize the quadric of the triangle
		Utils::Quadric<REAL> q(this->m_position[dit], this->m_position[d1], this->m_position[d_1]) ;
		m_quadric[dit] += q ;						// and add the contribution of
		m_quadric[d1] += q ;						// this quadric to the ones
		m_quadric[d_1] += q ;						// of the 3 incident vertices
	}

	TraversorE<MAP> travE(m);
	for(Dart dit = travE.begin() ; dit != travE.end() ; dit = travE.next())
	{
		initEdgeInfo(dit) ; // init the edges with their optimal position
							// and insert them in the multimap according to their error
	}

	cur = edges.begin() ; // init the current edge to the first one

	return true ;
}

template <typename PFP>
bool EdgeSelector_GeomColOptGradient<PFP>::nextEdge(Dart& d)
{
	if(cur == edges.end() || edges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void EdgeSelector_GeomColOptGradient<PFP>::updateBeforeCollapse(Dart d)
{
	typedef typename PFP::MAP MAP ;
	MAP& m = this->m_map ;

	const Dart& v0 = d ;
	const Dart& v1 = m.phi2(d) ;

	// remove all the edges that will disappear from the multimap
	// namely : all edges adjacent to a vertex which is adjacent
	// to either v0 or v1

	// collect vertices (1-ring)
	std::vector<Dart> vertices ;

	CellMarker<VERTEX> cvm(m) ;
	Traversor2VVaE<MAP> tv0(m,v0) ;
	for (Dart v = tv0.begin() ; v != tv0.end() ; v = tv0.next())
	{
		if (!cvm.isMarked(v))
		{
			vertices.push_back(v) ;
			cvm.mark(v) ;
		}
	}
	Traversor2VVaE<MAP> tv1(m,v1) ;
	for (Dart v = tv1.begin() ; v != tv1.end() ; v = tv1.next())
	{
		if (!cvm.isMarked(v))
		{
			vertices.push_back(v) ;
			cvm.mark(v) ;
		}
	}

	// apply to all adjacent edges (2-ring w/o border)
	CellMarker<EDGE> cem(m) ;
	for (std::vector<Dart>::const_iterator it = vertices.begin() ; it != vertices.end() ; ++it)
	{
		const Dart& v = *it ;
		Traversor2VE<MAP> te(m,v) ;
		for (Dart e = te.begin() ; e != te.end() ; e = te.next())
		{
			if (!cem.isMarked(e))
			{
				if(edgeInfo[e].valid)
				{
					edges.erase(edgeInfo[e].it) ;
					edgeInfo[e].valid = false ;
				}

				cem.mark(e) ;
			}
		}
	}
}

/*
 * Update quadric of a vertex
 * Discards quadrics of d and assigns freshly calculated
 * quadrics depending on the actual planes surrounding d
 * @param dart d
 */
template <typename PFP>
void EdgeSelector_GeomColOptGradient<PFP>::recomputeQuadric(const Dart d, const bool recomputeNeighbors)
{
	Dart dFront,dBack ;
	Dart dInit = d ;

	// Init Front
	dFront = dInit ;

	m_quadric[d].zero() ;

	do
	{
		// Make step
		dBack = this->m_map.phi2(dFront) ;
		dFront = this->m_map.phi2_1(dFront) ;

		if (dBack != dFront)
		{ // if dFront is no border
			m_quadric[d] += Utils::Quadric<REAL>(this->m_position[d],this->m_position[this->m_map.phi2(dFront)],this->m_position[dBack]) ;
		}
		if (recomputeNeighbors)
			recomputeQuadric(dBack, false) ;

	} while(dFront != dInit) ;
}

template <typename PFP>
void EdgeSelector_GeomColOptGradient<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	typename PFP::MAP& m = this->m_map ;

	// update quadrics
	recomputeQuadric(d2, true) ;

	// update the multimap
	Traversor2VVaE<MAP> tv (m,d2);
	CellMarker<EDGE> eMark (m);

	for(Dart dit = tv.begin() ; dit != tv.end() ; dit = tv.next())
	{
		Traversor2VE<MAP> te2 (m,dit);
		for(Dart dit2 = te2.begin() ; dit2 != te2.end() ; dit2 = te2.next())
		{
			if (!eMark.isMarked(dit2))
			{
				updateEdgeInfo(dit2) ;
				eMark.mark(dit2);
			}
		}
	}

	cur = edges.begin() ; // set the current edge to the first one
}

template <typename PFP>
void EdgeSelector_GeomColOptGradient<PFP>::initEdgeInfo(Dart d)
{
	typename PFP::MAP& m = this->m_map ;
	EdgeInfo einfo ;
	if(m.edgeCanCollapse(d))
		computeEdgeInfo(d, einfo) ;
	else
		einfo.valid = false ;
	edgeInfo[d] = einfo ;
}

template <typename PFP>
void EdgeSelector_GeomColOptGradient<PFP>::updateEdgeInfo(Dart d)
{
	typename PFP::MAP& m = this->m_map ;
	EdgeInfo& einfo = edgeInfo[d] ;

	if(einfo.valid)
		edges.erase(einfo.it) ;		// remove the edge from the multimap

	if(m.edgeCanCollapse(d))
		computeEdgeInfo(d, einfo) ;
	else
		einfo.valid = false ;
}

template <typename PFP>
void EdgeSelector_GeomColOptGradient<PFP>::computeEdgeInfo(Dart d, EdgeInfo& einfo)
{
	typename PFP::MAP& m = this->m_map ;
	Dart dd = m.phi1(d) ;

	// New position
	Utils::Quadric<REAL> quad ;
	quad += m_quadric[d] ;	// compute the sum of the
	quad += m_quadric[dd] ;	// two vertices quadrics

	// compute all approximated attributes
	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = this->m_approximators.begin() ;
			it != this->m_approximators.end() ;
			++it)
	{
		(*it)->approximate(d) ;
	}

	// get pos
	const VEC3& newPos = this->m_approx[m_approxindex_pos]->getApprox(d,m_attrindex_pos) ; // get newPos
	// get col
	const VEC3& newCol = this->m_approx[m_approxindex_color]->getApprox(d,m_attrindex_color) ; // get newPos


	// sum of QEM metric and color gradient metric
	const REAL t = 0.01 ;
	const REAL err = t*quad(newPos) + (1-t)*(computeEdgeGradientColorError(d,newPos,newCol) + computeEdgeGradientColorError(m.phi2(d),newPos,newCol)).norm() / sqrt(3) ;

	einfo.it = edges.insert(std::make_pair(err, d)) ;
	einfo.valid = true ;
}

template <typename PFP>
typename PFP::VEC3
EdgeSelector_GeomColOptGradient<PFP>::computeEdgeGradientColorError(const Dart& v0, const VEC3& P, const VEC3& newCol)
{
	MAP& m = this->m_map ;

	Traversor2VF<MAP> tf(m,v0) ; // all faces around vertex v0

	const VEC3& P0 = m_pos[v0] ;
	const VEC3& c0 = m_color[v0] ;
	const VEC3 d = P - P0 ; // displacement vector

	VEC3 count ;
	REAL areaSum = 0 ;
	for (Dart fi = tf.begin() ; fi != tf.end() ; fi = tf.next()) // foreach "blue" face
	{
		// get the data
		const Dart& vi = m.phi1(fi) ;
		const Dart& vj = m.phi_1(fi) ;
		const VEC3& Pi = this->m_position[vi] ;
		const VEC3& Pj = this->m_position[vj] ;
		const VEC3& ci = m_color[vi] ;
		const VEC3& cj = m_color[vj] ;

		// utils
		const VEC3 ei = P0 - Pj ;
		const VEC3 ej = Pi - P0 ;
		//const VEC3 e0 = Pj - Pi ;

		const REAL areaIJ0sq = (ei ^ ej).norm2() ;
		const REAL areaIJ0 = sqrt(areaIJ0sq)/2. ;
		areaSum += areaIJ0 ;

		// per-channel treatment
		for (unsigned int c = 0 ; c < 3 ;  ++c)
		{
			// color gradient for channel i
			VEC3 grad = (ei.norm2()*(ci[c]-newCol[c]) + (ei*ej)*(cj[c]-newCol[c]))*ej ;
			grad -= (ej.norm2()*(cj[c]-newCol[c]) + (ei*ej)*(ci[c]-newCol[c]))*ei ;
			const REAL denom = areaIJ0sq ;
			if (denom < 1e-9) // case flat triangles
				grad = VEC3(0,0,0) ;
			else
				grad /= denom ;

			// displacement error for channel i
			const REAL displacementE = areaIJ0 * pow(d*grad,2) ; // area x <disp,grad>

			count[c] += displacementE ;
		}
	}

	const VEC3 colDiff = newCol - c0 ;
	for (unsigned int c = 0 ; c < 3 ; ++c)
		count[c] += pow(colDiff[c],2) * areaSum ;

	return count ;
}


/************************************************************************************
 *                         EDGESELECTOR QEMext for Color                            *
 ************************************************************************************/
template <typename PFP>
bool EdgeSelector_QEMextColor<PFP>::init()
{
	typename PFP::MAP& m = this->m_map ;

	// Verify availability of required approximators
	unsigned int ok = 0 ;
	for (unsigned int approxindex = 0 ; approxindex < this->m_approximators.size() ; ++approxindex)
	{
		assert(this->m_approximators[approxindex]->getType() != A_hQEM || !"Approximator(hQEM) and selector (EdgeSelector_QEMextColor) are not compatible") ;
		assert(this->m_approximators[approxindex]->getType() != A_hHalfCollapse || !"Approximator(hHalfCollapse) and selector (EdgeSelector_QEMextColor) are not compatible") ;
		assert(this->m_approximators[approxindex]->getType() != A_hLightfieldHalf || !"Approximator(hLightfieldHalf) and selector (EdgeSelector_QEMextColor) are not compatible") ;

		bool saved = false ;
		for (unsigned int attrindex = 0 ; attrindex < this->m_approximators[approxindex]->getNbApproximated() ; ++ attrindex)
		{
			// constraint : 2 approximators in specific order
			if(ok == 0 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "position")
			{
				++ok ;
				m_approxindex_pos = approxindex ;
				m_attrindex_pos = attrindex ;
				m_pos = this->m_position ;
				if (!saved)
				{
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3,EDGE>* >(this->m_approximators[approxindex])) ;
					saved = true ;
				}
			}
			else if(ok == 1 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "color")
			{
				++ok ;
				m_approxindex_color = approxindex ;
				m_attrindex_color = attrindex ;
				m_color = m.template getAttribute<typename PFP::VEC3, VERTEX>("color") ;
				assert(m_color.isValid() || !"EdgeSelector_QEMextColor: color attribute is not valid") ;
				if (!saved)
				{
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3,EDGE>* >(this->m_approximators[approxindex])) ;
					saved = true ;
				}
			}
		}
	}

	if(ok != 2)
		return false ;

	TraversorV<MAP> travV(m);
	for(Dart dit = travV.begin() ; dit != travV.end() ; dit = travV.next())
	{
		Utils::QuadricNd<REAL,6> q ;		// create one quadric
		m_quadric[dit] = q ;		// per vertex
	}

	// Compute quadric per vertex
	TraversorF<MAP> travF(m) ;
	for(Dart dit = travF.begin() ; dit != travF.end() ; dit = travF.next()) // init QEM quadrics
	{
		Dart d1 = m.phi1(dit) ;					// for each triangle,
		Dart d_1 = m.phi_1(dit) ;					// initialize the quadric of the triangle

   		VEC6 p0, p1, p2 ;
   		for (unsigned int i = 0 ; i < 3 ; ++i)
   		{
   			p0[i] = this->m_position[dit][i] ;
   			p0[i+3] = this->m_color[dit][i] ;
   			p1[i] = this->m_position[d1][i] ;
   			p1[i+3] = this->m_color[d1][i] ;
   			p2[i] = this->m_position[d_1][i] ;
   			p2[i+3] = this->m_color[d_1][i] ;
   		}
		Utils::QuadricNd<REAL,6> q(p0,p1,p2) ;
		m_quadric[dit] += q ;						// and add the contribution of
		m_quadric[d1] += q ;						// this quadric to the ones
		m_quadric[d_1] += q ;						// of the 3 incident vertices
	}

	edges.clear() ;

	TraversorE<MAP> travE(m);
	for(Dart dit = travE.begin() ; dit != travE.end() ; dit = travE.next())
	{
		initEdgeInfo(dit) ; // init the edges with their optimal position
							// and insert them in the multimap according to their error
	}

	cur = edges.begin() ; // init the current edge to the first one

	return true ;
}

template <typename PFP>
bool EdgeSelector_QEMextColor<PFP>::nextEdge(Dart& d)
{
	if(cur == edges.end() || edges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void EdgeSelector_QEMextColor<PFP>::updateBeforeCollapse(Dart d)
{
	typename PFP::MAP& m = this->m_map ;

	EdgeInfo *edgeE = &(edgeInfo[d]) ;
	if(edgeE->valid)
		edges.erase(edgeE->it) ;

	edgeE = &(edgeInfo[m.phi1(d)]) ;
	if(edgeE->valid)						// remove all
		edges.erase(edgeE->it) ;

	edgeE = &(edgeInfo[m.phi_1(d)]) ;	// the edges that will disappear
	if(edgeE->valid)
		edges.erase(edgeE->it) ;
										// from the multimap
	Dart dd = m.phi2(d) ;
	if(dd != d)
	{
		edgeE = &(edgeInfo[m.phi1(dd)]) ;
		if(edgeE->valid)
			edges.erase(edgeE->it) ;

		edgeE = &(edgeInfo[m.phi_1(dd)]) ;
		if(edgeE->valid)
			edges.erase(edgeE->it) ;
	}
}

/*
 * Update quadric of a vertex
 * Discards quadrics of d and assigns freshly calculated
 * quadrics depending on the actual planes surrounding d
 * @param dart d
 */
template <typename PFP>
void EdgeSelector_QEMextColor<PFP>::recomputeQuadric(const Dart d, const bool recomputeNeighbors)
{
	Dart dFront,dBack ;
	Dart dInit = d ;

	// Init Front
	dFront = dInit ;

	m_quadric[d].zero() ;

   	do
   	{
   		// Make step
   		dBack = this->m_map.phi2(dFront) ;
       	dFront = this->m_map.phi2_1(dFront) ;

       	if (dBack != dFront)
       	{ // if dFront is no border
   			Dart d2 = this->m_map.phi2(dFront) ;

       		VEC6 p0, p1, p2 ;
       		for (unsigned int i = 0 ; i < 3 ; ++i)
       		{

       			p0[i] = this->m_position[d][i] ;
       			p0[i+3] = this->m_color[d][i] ;
       			p1[i] = this->m_position[d2][i] ;
       			p1[i+3] = this->m_color[d2][i] ;
       			p2[i] = this->m_position[dBack][i] ;
       			p2[i+3] = this->m_color[dBack][i] ;
       		}
       		m_quadric[d] += Utils::QuadricNd<REAL,6>(p0,p1,p2) ;
       	}
       	if (recomputeNeighbors)
       		recomputeQuadric(dBack, false) ;

    } while(dFront != dInit) ;
}

template <typename PFP>
void EdgeSelector_QEMextColor<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	typename PFP::MAP& m = this->m_map ;

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
void EdgeSelector_QEMextColor<PFP>::initEdgeInfo(Dart d)
{
	typename PFP::MAP& m = this->m_map ;
	EdgeInfo einfo ;
	if(m.edgeCanCollapse(d))
		computeEdgeInfo(d, einfo) ;
	else
		einfo.valid = false ;
	edgeInfo[d] = einfo ;
}

template <typename PFP>
void EdgeSelector_QEMextColor<PFP>::updateEdgeInfo(Dart d, bool recompute)
{
	typename PFP::MAP& m = this->m_map ;
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
void EdgeSelector_QEMextColor<PFP>::computeEdgeInfo(Dart d, EdgeInfo& einfo)
{
	typename PFP::MAP& m = this->m_map ;
	Dart dd = m.phi1(d) ;

	// New position
	Utils::QuadricNd<REAL,6> quad ;
	quad += m_quadric[d] ;	// compute the sum of the
	quad += m_quadric[dd] ;	// two vertices quadrics

	// compute all approximated attributes
	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = this->m_approximators.begin() ;
			it != this->m_approximators.end() ;
			++it)
	{
		(*it)->approximate(d) ;
	}

	// get pos
	const VEC3& newPos = this->m_approx[m_approxindex_pos]->getApprox(d,m_attrindex_pos) ; // get newPos
	// get col
	const VEC3& newCol = this->m_approx[m_approxindex_color]->getApprox(d,m_attrindex_color) ; // get newCol

	// compute error
	VEC6 newEmb ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
	{
		newEmb[i] = newPos[i] ;
		newEmb[i+3] = newCol[i] ;
	}

	const REAL& err = quad(newEmb) ;

	// Check if errated values appear
	if (err < -1e-6)
		einfo.valid = false ;
	else
	{
		einfo.it = edges.insert(std::make_pair(std::max(err,REAL(0)), d)) ;
		einfo.valid = true ;
	}
}

/*****************************************************************************************************************
 *                                 LIGHTFIELD QUADRIC ERROR METRIC                                               *
 *****************************************************************************************************************
template <typename PFP>
bool EdgeSelector_Lightfield<PFP>::init()
{
	typename PFP::MAP& m = this->m_map ;

	// Verify availability of required approximators
	unsigned int ok = 0 ;
	for (unsigned int approxindex = 0 ; approxindex < this->m_approximators.size() ; ++approxindex)
	{
		assert(this->m_approximators[approxindex]->getType() != A_hQEM || !"Approximator(hQEM) and selector (EdgeSelector_Lightfield) are not compatible") ;
		assert(this->m_approximators[approxindex]->getType() != A_hHalfCollapse || !"Approximator(hHalfCollapse) and selector (EdgeSelector_Lightfield) are not compatible") ;
		assert(this->m_approximators[approxindex]->getType() != A_hLightfieldHalf || !"Approximator(hLightfieldHalf) and selector (EdgeSelector_Lightfield) are not compatible") ;

		unsigned int k = 0 ;
		bool saved = false ;
		for (unsigned int attrindex = 0 ; attrindex < this->m_approximators[approxindex]->getNbApproximated() ; ++ attrindex)
		{
			// constraint : 2 approximators in specific order
			if(ok == 0 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "position")
			{
				++ok ;
				m_approxindex_pos = approxindex ;
				m_attrindex_pos = attrindex ;
				m_pos = this->m_position ;
				if (!saved)
				{
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3,EDGE>* >(this->m_approximators[approxindex])) ;
					saved = true ;
				}
			}
			else if(ok == 1 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "frameT")
			{
				++ok ;
//				m_approxindex_FT = approxindex ;
//				m_attrindex_FT = attrindex ;
				m_frameT = m.template getAttribute<typename PFP::VEC3, VERTEX>("frameT") ;
				if (!saved)
				{
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3,EDGE>* >(this->m_approximators[approxindex])) ;
					assert(m_frameT.isValid() || !"EdgeSelector_QEMextColor: frameT attribute is not valid") ;
					saved = true ;
				}
			}
			else if(ok == 2 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "frameB")
			{
				++ok ;
//				m_approxindex_FB = approxindex ;
//				m_attrindex_FB = attrindex ;
				m_frameB = m.template getAttribute<typename PFP::VEC3, VERTEX>("frameB") ;
				assert(m_frameB.isValid() || !"EdgeSelector_QEMextColor: frameB attribute is not valid") ;
				if (!saved)
				{
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3,EDGE>* >(this->m_approximators[approxindex])) ;
					saved = true ;
				}
			}
			else if(ok == 3 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "frameN")
			{
				++ok ;
				m_approxindex_FN = approxindex ;
				m_attrindex_FN = attrindex ;
				m_frameN = m.template getAttribute<typename PFP::VEC3, VERTEX>("frameN") ;
				assert(m_frameN.isValid() || !"EdgeSelector_QEMextColor: frameN attribute is not valid") ;
				if (!saved)
				{
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3,EDGE>* >(this->m_approximators[approxindex])) ;
					saved = true ;
				}
			}
			else
			{
				std::stringstream s ;
				s << "PBcoefs" << k ;
				if(ok > 3 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == s.str().c_str())
				{
					++ok ;
					m_HF.push_back(m.template getAttribute<typename PFP::VEC3, VERTEX>(s.str().c_str())) ;
					if (m_HF[k++].isValid())
					{
						m_approxindex_HF.push_back(approxindex) ;
						m_attrindex_HF.push_back(attrindex) ;
						if (!saved)
						{
							m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3,EDGE>* >(this->m_approximators[approxindex])) ;
							saved = true ;
						}
					}
				}
			}
		}
		m_K = k ;
	}

	if(ok < 5)
		return false ;

	TraversorV<MAP> travV(m);
	for(Dart dit = travV.begin() ; dit != travV.end() ; dit = travV.next())
	{
		Utils::Quadric<REAL> q ;		// create one quadric
		m_quadricGeom[dit] = q ;		// per vertex
	}

	// Compute quadric per vertex
	TraversorF<MAP> travF(m) ;
	for(Dart dit = travF.begin() ; dit != travF.end() ; dit = travF.next()) // init QEM quadrics
	{
		Dart d1 = m.phi1(dit) ;					// for each triangle,
		Dart d_1 = m.phi_1(dit) ;					// initialize the quadric of the triangle

		Utils::Quadric<REAL> q(this->m_position[dit], this->m_position[d1], this->m_position[d_1]) ;
		m_quadricGeom[dit] += q ;						// and add the contribution of
		m_quadricGeom[d1] += q ;						// this quadric to the ones
		m_quadricGeom[d_1] += q ;						// of the 3 incident vertices
	}

	TraversorE<MAP> travE(m);
	for(Dart dit = travE.begin() ; dit != travE.end() ; dit = travE.next())
	{
		initEdgeInfo(dit) ; // init the edges with their optimal position
							// and insert them in the multimap according to their error
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
	typename PFP::MAP& m = this->m_map ;

	EdgeInfo *edgeE = &(edgeInfo[d]) ;
	if(edgeE->valid)
		edges.erase(edgeE->it) ;

	edgeE = &(edgeInfo[m.phi1(d)]) ;
	if(edgeE->valid)						// remove all
		edges.erase(edgeE->it) ;

	edgeE = &(edgeInfo[m.phi_1(d)]) ;	// the edges that will disappear
	if(edgeE->valid)
		edges.erase(edgeE->it) ;
										// from the multimap
	Dart dd = m.phi2(d) ;
	if(dd != d)
	{
		edgeE = &(edgeInfo[m.phi1(dd)]) ;
		if(edgeE->valid)
			edges.erase(edgeE->it) ;

		edgeE = &(edgeInfo[m.phi_1(dd)]) ;
		if(edgeE->valid)
			edges.erase(edgeE->it) ;
	}
}

/*
 * Update quadric of a vertex
 * Discards quadrics of d and assigns freshly calculated
 * quadrics depending on the actual planes surrounding d
 * @param dart d
 *
template <typename PFP>
void EdgeSelector_Lightfield<PFP>::recomputeQuadric(const Dart d, const bool recomputeNeighbors)
{
	Dart dFront,dBack ;
	Dart dInit = d ;

	// Init Front
	dFront = dInit ;

	m_quadricGeom[d].zero() ;

   	do
   	{
   		// Make step
   		dBack = this->m_map.phi2(dFront) ;
       	dFront = this->m_map.phi2_1(dFront) ;

       	if (dBack != dFront)
       	{ // if dFront is no border
       		m_quadricGeom[d] += Utils::Quadric<REAL>(m_pos[d],m_pos[this->m_map.phi1(dFront)],m_pos[dBack]) ;
       	}
       	if (recomputeNeighbors)
       		recomputeQuadric(dBack, false) ;

    } while(dFront != dInit) ;
}

template <typename PFP>
void EdgeSelector_Lightfield<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	typename PFP::MAP& m = this->m_map ;

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
void EdgeSelector_Lightfield<PFP>::initEdgeInfo(Dart d)
{
	typename PFP::MAP& m = this->m_map ;
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
	typename PFP::MAP& m = this->m_map ;
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
void EdgeSelector_Lightfield<PFP>::computeEdgeInfo(Dart d, EdgeInfo& einfo)
{
	typename PFP::MAP& m = this->m_map ;
	Dart dd = m.phi1(d) ;

	// New position
	Utils::Quadric<REAL> quad ;
	quad += m_quadricGeom[d] ;	// compute the sum of the
	quad += m_quadricGeom[dd] ;	// two vertices quadrics

	// compute all approximated attributes
	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = this->m_approximators.begin() ;
			it != this->m_approximators.end() ;
			++it)
	{
		(*it)->approximate(d) ;
	}

	// Get all approximated attributes
	// New position
	const VEC3& newPos = this->m_approx[m_approxindex_pos]->getApprox(d,m_attrindex_pos) ; // get newPos
	// New normal
	const VEC3& newFN = this->m_approx[m_approxindex_FN]->getApprox(d,m_attrindex_FN) ; // get new frameN

	// New function
	std::vector<VEC3> newHF ;
	newHF.resize(m_K) ;
	for (unsigned int k = 0 ; k < m_K ; ++k)
		newHF[k] = this->m_approx[m_approxindex_HF[k]]->getApprox(d,m_attrindex_HF[k]) ; // get newHFcoefsK

	// Compute errors
	// Position
	Utils::Quadric<REAL> quadGeom ;
	quadGeom += m_quadricGeom[d] ;	// compute the sum of the
	quadGeom += m_quadricGeom[dd] ;	// two vertices quadrics

	// hemisphere difference error
	double scal1 = double(m_frameN[d] * newFN) ;
	double alpha1 = acos(std::max(std::min(scal1, double(1)),double(-1))) ; // for epsilon normalization of newFN errors
	// angle 2
	double scal2 = double(m_frameN[dd] * newFN) ;
	double alpha2 = acos(std::max(std::min(scal2, double(1)),double(-1))) ; // for epsilon normalization of newFN errors

	double alpha = alpha1 + alpha2 ;

	assert(m_quadricHF.isValid() | !"EdgeSelector_Lightfield<PFP>::computeEdgeInfo: quadricHF is not valid") ;
	Utils::QuadricHF<REAL> quadHF = m_quadricHF[d] ;

	assert(m_avgColor.isValid()) ;
	VEC3 avgColDiff = m_avgColor[d] ;
	avgColDiff -= m_avgColor[dd] ;

	//std::cout << quadGeom(newPos) << " vs " << alpha/M_PI << " vs " << quadHF(newHF) << std::endl ;
	// sum of QEM metric and frame orientation difference
	const REAL& errG = quadGeom(newPos) ; // geom
	//const REAL& errAngle = alpha / M_PI ; // frame
	const REAL& errAngle = (alpha / M_PI) * avgColDiff.norm2() / 3. ; // avg color times covering area
	const REAL& errLF = quadHF(newHF) ; // function coefficients

	// Check if errated values appear
	if (errG < -1e-6 || errAngle < -1e-6 || errLF < -1e-6)
		einfo.valid = false ;
	else
	{
		einfo.it = edges.insert(std::make_pair(std::max(errG, REAL(0)) + 10*(errAngle + errLF), d)) ;
		//einfo.it = edges.insert(std::make_pair(errG+errLF+errAngle, d)) ;
		einfo.valid = true ;
	}
}
*/

/************************************************************************************
 *                         EDGESELECTOR LIGHTFIELD GRADIENT                         *
 ************************************************************************************

template <typename PFP>
bool EdgeSelector_LightfieldGradient<PFP>::init()
{
	MAP& m = this->m_map ;

	// Verify availability of required approximators
	unsigned int ok = 0 ;
	unsigned int k = 0 ;
	for (unsigned int approxindex = 0 ; approxindex < this->m_approximators.size() ; ++approxindex)
	{
		bool saved = false ;
		for (unsigned int attrindex = 0 ; attrindex < this->m_approximators[approxindex]->getNbApproximated() ; ++ attrindex)
		{
			// constraint : 2 approximators in specific order
			if(ok == 0 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "position")
			{
				++ok ;
				m_approxindex_pos = approxindex ;
				m_attrindex_pos = attrindex ;
				if (!saved)
				{
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3, EDGE>* >(this->m_approximators[approxindex])) ;
					saved = true ;
				}
			}
			else if(ok == 1 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "frameT")
			{
				++ok ;
				m_approxindex_FT = approxindex ;
				m_attrindex_FT = attrindex ;
				if (!saved)
				{
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3, EDGE>* >(this->m_approximators[approxindex])) ;
					assert(this->m_approx[approxindex]->getAttr(attrindex).isValid() || !"EdgeSelector_LightfieldGradient: frameT attribute is not valid") ;
					saved = true ;
				}
			}
			else if(ok == 2 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "frameB")
			{
				++ok ;
				m_approxindex_FB = approxindex ;
				m_attrindex_FB = attrindex ;
				if (!saved)
				{
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3, EDGE>* >(this->m_approximators[approxindex])) ;
					assert(this->m_approx[approxindex]->getAttr(attrindex).isValid() || !"EdgeSelector_LightfieldGradient: frameB attribute is not valid") ;
					saved = true ;
				}
			}
			else if(ok == 3 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "frameN")
			{
				++ok ;
				m_approxindex_FN = approxindex ;
				m_attrindex_FN = attrindex ;
				if (!saved)
				{
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3, EDGE>* >(this->m_approximators[approxindex])) ;
					assert(this->m_approx[approxindex]->getAttr(attrindex).isValid() || !"EdgeSelector_LightfieldGradient: frameN attribute is not valid") ;
					saved = true ;
				}
			}
			else
			{
				std::stringstream s ;
				s << "PBcoefs" << k ;
				if(ok > 3 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == s.str().c_str())
				{
					++ok ;
					if (!saved)
					{
						m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3, EDGE>* >(this->m_approximators[approxindex])) ;
						saved = true ;
					}
					if (this->m_approx[approxindex]->getAttr(attrindex).isValid())
					{
						++k ;
						m_approxindex_HF.push_back(approxindex) ;
						m_attrindex_HF.push_back(attrindex) ;
					}
				}
			}
		}
	}
	m_K = k ;

	if(ok < 6)
	{
		std::cerr << "EdgeSelector_LightfieldGradient<PFP>::init() not OK" << std::endl ;
		return false ;
	}

	m_integrator.Init(17) ;
	// allocate workspaces
	m_workspace = new double[59+12*m_K] ;

	TraversorV<MAP> travV(m);
	for(Dart dit = travV.begin() ; dit != travV.end() ; dit = travV.next())
	{
		Utils::Quadric<REAL> q ;		// create one quadric
		m_quadric[dit] = q ;		// per vertex
	}

	// Compute quadric per vertex
	TraversorF<MAP> travF(m) ;
	for(Dart dit = travF.begin() ; dit != travF.end() ; dit = travF.next()) // init QEM quadrics
	{
		Dart d1 = m.phi1(dit) ;					// for each triangle,
		Dart d_1 = m.phi_1(dit) ;					// initialize the quadric of the triangle

		Utils::Quadric<REAL> q(this->m_position[dit], this->m_position[d1], this->m_position[d_1]) ;
		m_quadric[dit] += q ;						// and add the contribution of
		m_quadric[d1] += q ;						// this quadric to the ones
		m_quadric[d_1] += q ;						// of the 3 incident vertices
	}

	TraversorE<MAP> travE(m);
	for(Dart dit = travE.begin() ; dit != travE.end() ; dit = travE.next())
	{
		initEdgeInfo(dit) ; // init the edges with their optimal position
							// and insert them in the multimap according to their error
	}

	cur = edges.begin() ; // init the current edge to the first one

	return true ;
}

template <typename PFP>
bool EdgeSelector_LightfieldGradient<PFP>::nextEdge(Dart& d)
{
	if(cur == edges.end() || edges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void EdgeSelector_LightfieldGradient<PFP>::updateBeforeCollapse(Dart d)
{
	typedef typename PFP::MAP MAP ;
	MAP& m = this->m_map ;

	const Dart& v0 = d ;
	const Dart& v1 = m.phi2(d) ;

	// remove all the edges that will disappear from the multimap
	// namely : all edges adjacent to a vertex which is adjacent
	// to either v0 or v1

	// collect vertices (1-ring)
	std::vector<Dart> vertices ;

	CellMarker<VERTEX> cvm(m) ;
	Traversor2VVaE<MAP> tv0(m,v0) ;
	for (Dart v = tv0.begin() ; v != tv0.end() ; v = tv0.next())
	{
		if (!cvm.isMarked(v))
		{
			vertices.push_back(v) ;
			cvm.mark(v) ;
		}
	}
	Traversor2VVaE<MAP> tv1(m,v1) ;
	for (Dart v = tv1.begin() ; v != tv1.end() ; v = tv1.next())
	{
		if (!cvm.isMarked(v))
		{
			vertices.push_back(v) ;
			cvm.mark(v) ;
		}
	}

	// apply to all adjacent edges (2-ring w/o border)
	CellMarker<EDGE> cem(m) ;
	for (std::vector<Dart>::const_iterator it = vertices.begin() ; it != vertices.end() ; ++it)
	{
		const Dart& v = *it ;
		Traversor2VE<MAP> te(m,v) ;
		for (Dart e = te.begin() ; e != te.end() ; e = te.next())
		{
			if (!cem.isMarked(e))
			{
				if(edgeInfo[e].valid)
				{
					edges.erase(edgeInfo[e].it) ;
					edgeInfo[e].valid = false ;
				}

				cem.mark(e) ;
			}
		}
	}
}

/*
 * Update quadric of a vertex
 * Discards quadrics of d and assigns freshly calculated
 * quadrics depending on the actual planes surrounding d
 * @param dart d
 *
template <typename PFP>
void EdgeSelector_LightfieldGradient<PFP>::recomputeQuadric(const Dart d, const bool recomputeNeighbors)
{
	Dart dFront,dBack ;
	Dart dInit = d ;

	// Init Front
	dFront = dInit ;

	m_quadric[d].zero() ;

	do
	{
		// Make step
		dBack = this->m_map.phi2(dFront) ;
		dFront = this->m_map.phi2_1(dFront) ;

		if (dBack != dFront)
		{ // if dFront is no border
			m_quadric[d] += Utils::Quadric<REAL>(this->m_position[d],this->m_position[this->m_map.phi2(dFront)],this->m_position[dBack]) ;
		}
		if (recomputeNeighbors)
			recomputeQuadric(dBack, false) ;

	} while(dFront != dInit) ;
}

template <typename PFP>
void EdgeSelector_LightfieldGradient<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	typename PFP::MAP& m = this->m_map ;

	// update quadrics
	recomputeQuadric(d2, true) ;

	// update the multimap
	Traversor2VVaE<MAP> tv (m,d2);
	CellMarker<EDGE> eMark (m);

	for(Dart dit = tv.begin() ; dit != tv.end() ; dit = tv.next())
	{
		Traversor2VE<MAP> te2 (m,dit);
		for(Dart dit2 = te2.begin() ; dit2 != te2.end() ; dit2 = te2.next())
		{
			if (!eMark.isMarked(dit2))
			{
				updateEdgeInfo(dit2) ;
				eMark.mark(dit2);
			}
		}
	}

	cur = edges.begin() ; // set the current edge to the first one
}

template <typename PFP>
void EdgeSelector_LightfieldGradient<PFP>::initEdgeInfo(Dart d)
{
	typename PFP::MAP& m = this->m_map ;
	EdgeInfo einfo ;
	if(m.edgeCanCollapse(d))
		computeEdgeInfo(d, einfo) ;
	else
		einfo.valid = false ;
	edgeInfo[d] = einfo ;
}

template <typename PFP>
void EdgeSelector_LightfieldGradient<PFP>::updateEdgeInfo(Dart d)
{
	typename PFP::MAP& m = this->m_map ;
	EdgeInfo& einfo = edgeInfo[d] ;

	if(einfo.valid)
		edges.erase(einfo.it) ;		// remove the edge from the multimap

	if(m.edgeCanCollapse(d))
		computeEdgeInfo(d, einfo) ;
	else
		einfo.valid = false ;
}

template <typename PFP>
void EdgeSelector_LightfieldGradient<PFP>::computeEdgeInfo(Dart d, EdgeInfo& einfo)
{
	typename PFP::MAP& m = this->m_map ;
	Dart dd = m.phi1(d) ;

	// New position
	Utils::Quadric<REAL> quad ;
	quad += m_quadric[d] ;	// compute the sum of the
	quad += m_quadric[dd] ;	// two vertices quadrics

	// compute all approximated attributes
	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = this->m_approximators.begin() ;
			it != this->m_approximators.end() ;
			++it)
	{
		(*it)->approximate(d) ;
	}

	// get pos
	const VEC3& newPos = this->m_approx[m_approxindex_pos]->getApprox(d,m_attrindex_pos) ; // get newPos

	// WARNING : in the following, we consider the edge contraction (v0,v1) --> v
	const Dart& v0 = d ;

	// Compute errors
	// Position
	Utils::Quadric<REAL> quadGeom ;
	quadGeom += m_quadric[d] ;	// compute the sum of the
	quadGeom += m_quadric[dd] ;	// two vertices quadrics

	// sum of QEM metric and LF
	const REAL t = 0.01 ;
	const REAL err =
			t*quadGeom(newPos) + // geom
			(1-t)*computeDirDerivativeLFerror(v0).norm()/sqrt(3) // lf
			//(1-t)*computeGradientLFerror(v0,v1).norm()/sqrt(3) // lf
			;
	//std::cout << computeLightfieldError(v0,v1) / quadGeom(newPos) << std::endl ;

	// Check if errated values appear
	if (err < -1e-6)
		einfo.valid = false ;
	else
	{
		einfo.it = this->edges.insert(std::make_pair(std::max(err,REAL(0)), d)) ;
		einfo.valid = true ;
	}
}

template <typename PFP>
typename PFP::VEC3
EdgeSelector_LightfieldGradient<PFP>::computeDirDerivativeLFerror(const Dart& v) const
{
	MAP& m = this->m_map ;

	const VEC3& P0 = this->m_approx[m_approxindex_pos]->getAttr(m_attrindex_pos)[v] ;
	const VEC3& P = this->m_approx[m_approxindex_pos]->getApprox(v,m_attrindex_pos) ;
	const VEC3& N = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[m.phi1(v)] ;

	const VEC3& T = this->m_approx[m_approxindex_FT]->getApprox(v,m_attrindex_FT) ;
	const VEC3& B = this->m_approx[m_approxindex_FB]->getApprox(v,m_attrindex_FB) ;
	const VEC3& Nopt = this->m_approx[m_approxindex_FN]->getApprox(v,m_attrindex_FN) ;
	const VEC3& T0 = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[v] ;
	const VEC3& B0 = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[v] ;
	const VEC3& N0 = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[v] ;
	VEC3 coefs[m_K], coefs0[m_K] ;
	for (unsigned int k = 0 ; k < m_K ; ++k)
	{
		coefs0[k] = this->m_approx[m_approxindex_HF[k]]->getAttr(m_attrindex_HF[k])[v] ;
		coefs[k] = this->m_approx[m_approxindex_HF[k]]->getApprox(v,m_attrindex_HF[k]) ;
	}
	const VEC3& avgCol = coefs[0] ; // TO ADD IN APPROXIMATOR

	std::vector<Dart> faces ;
	CellMarker<FACE> fmark(m) ;
	Traversor2VF<MAP> tfv0(m,v) ; // all faces around vertex v0
	for (Dart fi = tfv0.begin() ; fi != tfv0.end() ; fi = tfv0.next()) // foreach "blue" face
	{
		if (!fmark.isMarked(fi))
			faces.push_back(fi) ;
		fmark.mark(fi) ;
	}
	Traversor2VF<MAP> tfv1(m,m.phi1(v)) ; // all faces around vertex v1
	for (Dart fi = tfv1.begin() ; fi != tfv1.end() ; fi = tfv1.next()) // foreach "blue" face
	{
		if (!fmark.isMarked(fi))
			faces.push_back(fi) ;
		fmark.mark(fi) ;
	}

	VEC3 error ;
	const VEC3 d = P - P0 ; // displacement vector

	for (unsigned int i = 0 ; i < faces.size() ; ++i) // foreach "blue" face
	{
		Dart fi = faces[i] ;
		// get the data
		const Dart& vi = m.phi1(fi) ;
		const Dart& vj = m.phi_1(fi) ;
		const VEC3& Pi = this->m_position[vi] ;
		const VEC3& Pj = this->m_position[vj] ;

		const VEC3& Ti = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[vi] ;
		const VEC3& Bi = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[vi] ;
		const VEC3& Ni = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[vi] ;
		const VEC3& Tj = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[vj] ;
		const VEC3& Bj = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[vj] ;
		const VEC3& Nj = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[vj] ;

		VEC3 coefsi[m_K], coefsj[m_K] ;
		for (unsigned int k = 0 ; k < m_K ; ++k)
		{
			coefsi[k] = this->m_approx[m_approxindex_HF[k]]->getAttr(m_attrindex_HF[k])[vi] ;
			coefsj[k] = this->m_approx[m_approxindex_HF[k]]->getAttr(m_attrindex_HF[k])[vj] ;
		}

		// utils
		const VEC3 ei = P - Pj ;
		const VEC3 ej = Pi - P ;

		// area x integration
		error += (ei ^ ej).norm()/2 * integrateDlf(d, m_K, N, ei, ej,
													  coefs0, T0, B0, N0, m_avgColor[v],
													  coefs, T, B, Nopt, avgCol,
													  coefsi, Ti, Bi, Ni, m_avgColor[vi],
													  coefsj, Tj, Bj, Nj, m_avgColor[vj]) ;
	}

	return error ;
}

template <typename PFP>
typename PFP::VEC3
EdgeSelector_LightfieldGradient<PFP>::integrateDlf(const VEC3& d, const unsigned int& K, const VEC3& N, const VEC3& ei, const VEC3& ej,
												   const VEC3* coefs0, const VEC3& T0, const VEC3& B0, const VEC3& N0, const VEC3& avg0,
												   const VEC3* coefs1, const VEC3& T1, const VEC3& B1, const VEC3& N1, const VEC3& avg1,
												   const VEC3* coefsi, const VEC3& Ti, const VEC3& Bi, const VEC3& Ni, const VEC3& avgi,
												   const VEC3* coefsj, const VEC3& Tj, const VEC3& Bj, const VEC3& Nj, const VEC3& avgj) const
{
	// set domain of integration
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_n[i] = N[i] ;

	// prepare workspace
	for (unsigned int i = 0 ; i < 3 ; ++i)	// d
		m_workspace[i] = d[i] ;
	// nbCoefs
	m_workspace[3] = K ;
	for (unsigned int i = 0 ; i < 3 ; ++i)	// ei
		m_workspace[4+i] = ei[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)	// ej
		m_workspace[7+i] = ej[i] ;
	// avg1, avgi, avgj
	m_workspace[11] = avg0[0] ;
	m_workspace[12] = avg1[0] ;
	m_workspace[13] = avgi[0] ;
	m_workspace[14] = avgj[0] ;
	m_workspace[15] = avg0[1] ;
	m_workspace[16] = avg1[1] ;
	m_workspace[17] = avgi[1] ;
	m_workspace[18] = avgj[1] ;
	m_workspace[19] = avg0[2] ;
	m_workspace[20] = avg1[2] ;
	m_workspace[21] = avgi[2] ;
	m_workspace[22] = avgj[2] ;

	// N1, Ni, Nj
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[23+i] = N0[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[26+i] = N1[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[29+i] = Ni[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[32+i] = Nj[i] ;
	// T1, Ti, Tj
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[35+i] = T0[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[38+i] = T1[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[41+i] = Ti[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[44+i] = Tj[i] ;
	// B1, Bi, Bj
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[47+i] = B0[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[50+i] = B1[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[53+i] = Bi[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[56+i] = Bj[i] ;

	// coefs1, coefsi, coefsj
	for (unsigned int c = 0 ; c < 3 ; ++c)
		for (unsigned int k = 0 ; k < K ; ++k)
		{
			m_workspace[59+c*K+k] = coefs0[k][c] ;
			m_workspace[59+(3+c)*K+k] = coefs1[k][c] ;
			m_workspace[59+(6+c)*K+k] = coefsi[k][c] ;
			m_workspace[59+(9+c)*K+k] = coefsj[k][c] ;
		}

	VEC3 res ;
	for (unsigned int c = 0 ; c < 3 ; ++c)
	{
		m_workspace[10] = c ;

		// exec integrator
		double integral, area ;
		m_integrator.Compute(&integral, &area, &dlf, m_workspace, &isInDomain, m_n) ;
		res[c] = integral/(2*M_PI) ;
	}

	return res ;
}

// fonction qui retourne bool est_dans_domaine en fonction de f et omega
template <typename PFP>
bool
EdgeSelector_LightfieldGradient<PFP>::isInDomain(double x, double y, double z, void *data)
{
	// (x,y,z) vecteur de vue
	// n vecteur normal
	double *n = (double*)data ;

	// vrai ssi n*(x,y,z) > 0
	return n[0]*x+n[1]*y+n[2]*z >= 0 ;
}

template <typename PFP>
double
EdgeSelector_LightfieldGradient<PFP>::dlf (double x, double y, double z, void* data)
{
	double *myData = (double*)data ;

	// decode workspace
	double *d = &(myData[0]) ;
	unsigned int K = myData[3] ;
	double *ei = &(myData[4]) ;
	double *ej = &(myData[7]) ;
	unsigned int channel = myData[10] ;
	double *avg = &(myData[11+4*channel]) ;
	// (11=avg0_c0,avg1_c0,avgI_c0,avgJ_c0,avg0_c1,avg1_c1,avgI_c1,avgJ_c1,avg0_c2,avg1_c2,avgI_c2,avgJ_c2)
	double *N0 = &(myData[23]) ;
	double *N1 = &(myData[26]) ;
	double *Ni = &(myData[29]) ;
	double *Nj = &(myData[32]) ;
	double *T0 = &(myData[35]) ;
	double *T1 = &(myData[38]) ;
	double *Ti = &(myData[41]) ;
	double *Tj = &(myData[44]) ;
	double *B0 = &(myData[47]) ;
	double *B1 = &(myData[50]) ;
	double *Bi = &(myData[53]) ;
	double *Bj = &(myData[56]) ;
	double *coefs0 = &(myData[59+K*channel]) ; // (coefs1_c0, ... coefsK_c0,coefs1_c1, ... coefsK_c1, coefs1_c2, coefsK_c2)
	double *coefs1 = &(myData[59+3*K+K*channel]) ;
	double *coefsi = &(myData[59+6*K+K*channel]) ;
	double *coefsj = &(myData[59+9*K+K*channel]) ;

	double vectorProd[3] ;
	vectorProd[0] = ei[1]*ej[2] - ei[2]*ej[1] ;
	vectorProd[1] = ei[2]*ej[0] - ei[0]*ej[2] ;
	vectorProd[2] = ei[0]*ej[1] - ei[1]*ej[0] ;
	double areaSq = vectorProd[0]*vectorProd[0] + vectorProd[1]*vectorProd[1] + vectorProd[2]*vectorProd[2] ;

	if (areaSq < 1e-9)
		return 0 ;

	double einorm2 = ei[0]*ei[0] + ei[1]*ei[1] + ei[2]*ei[2] ;
	double ejnorm2 = ej[0]*ej[0] + ej[1]*ej[1] + ej[2]*ej[2] ;
	double eiej = ei[0]*ej[0] + ei[1]*ej[1] + ei[2] * ej[2] ;

	double f0 = evalF(N0,&(avg[0]),K,T0,B0,coefs0,x,y,z) ;
	double f1 = evalF(N1,&(avg[1]),K,T1,B1,coefs1,x,y,z) ;
	double fi = evalF(Ni,&(avg[2]),K,Ti,Bi,coefsi,x,y,z) ;
	double fj = evalF(Nj,&(avg[3]),K,Tj,Bj,coefsj,x,y,z) ;

	double t1 = einorm2 * (fi-f1) + eiej*(fj-f1) ;
	double t2 = ejnorm2 * (fj-f1) + eiej*(fi-f1) ;

	double grad[3] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		grad[i] = (t1*ej[i] - t2*ei[i]) / areaSq ;

	// for second metric
	return pow(f0 - f1 + d[0]*grad[0] + d[1]*grad[1] + d[2]*grad[2],2) ;
}

template <typename PFP>
double
EdgeSelector_LightfieldGradient<PFP>::evalF(double* N, double* avg, unsigned int nb, double* T, double* B, double* coefs, double& x, double& y, double& z)
{
	if (N[0]*x+N[1]*y+N[2]*z < 0)
		return *avg ;

	// tangente
	double u = T[0]*x + T[1]*y + T[2]*z ;

	// binormale
	double v = B[0]*x + B[1]*y + B[2]*z ;

	// eval en ((xyz)*t,(xyz)*b)
	double res = 0 ;
	if (nb > 0)
	{
		// coefs
		res = coefs[0] ;
		if (nb > 1)
		{
			res += coefs[1]*v ;
			res += coefs[2]*u ;
			if (nb > 3)
			{
				res += coefs[3]*u*v ;
				res += coefs[4]*v*v ;
				res += coefs[5]*u*u ;
				if (nb > 6)
				{
					res += coefs[6]*u*v*v ;
					res += coefs[7]*v*u*u ;
					res += coefs[8]*v*v*v ;
					res += coefs[9]*u*u*u ;
					if (nb > 10)
					{
						res += coefs[10]*v*v*u*u ;
						res += coefs[11]*v*v*v*u ;
						res += coefs[12]*v*u*u*u ;
						res += coefs[13]*v*v*v*v ;
						res += coefs[14]*u*u*u*u ;
					}
				}
			}
		}
	}

	return res ;
}
*/

} // namespace Decimation

}

} // namespace Algo

} // namespace CGoGN
