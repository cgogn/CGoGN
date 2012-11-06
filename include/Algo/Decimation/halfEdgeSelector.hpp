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
			m_positionApproximator = reinterpret_cast<Approximator<PFP, VEC3,DART>* >(*it) ;
			ok = true ;
		}
	}

	if(!ok)
		return false ;

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
	assert(dd != d) ;
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
       	dFront = this->m_map.phi2_1(dFront) ;

       	if (this->m_map.phi2(dFront) != dFront) { // if dFront is no border
           	quadric[d] += Utils::Quadric<REAL>(this->m_position[d],this->m_position[dBack],this->m_position[this->m_map.phi1(dFront)]) ;
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
		Dart d = m.phi2(vit) ;
		if (d != vit) ;
			updateHalfEdgeInfo(d, true) ;

		updateHalfEdgeInfo(m.phi1(vit), true) ;
		d = m.phi2(m.phi1(vit)) ;
		if (d != m.phi1(vit)) ;
			updateHalfEdgeInfo(d, true) ;

		Dart stop = m.phi2(vit) ;
		assert (stop != vit) ;
		Dart vit2 = m.phi12(m.phi1(vit)) ;
		do {
			updateHalfEdgeInfo(vit2, true) ;
			d = m.phi2(vit2) ;
			if (d != vit2) ;
				updateHalfEdgeInfo(d, true) ;

			updateHalfEdgeInfo(m.phi1(vit2), false) ;
			d = m.phi2(m.phi1(vit2)) ;
			if (d != m.phi1(vit2)) ;
				updateHalfEdgeInfo(d, false) ;

			vit2 = m.phi12(vit2) ;
		} while (stop != vit2) ;
		vit = m.phi2_1(vit) ;
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
	Dart dd = m.phi1(d) ;

	Utils::Quadric<REAL> quad ;
	quad += quadric[d] ;	// compute the sum of the
	quad += quadric[dd] ;	// two vertices quadrics

	m_positionApproximator->approximate(d) ;

	REAL err = quad(m_positionApproximator->getApprox(d)) ;
	heinfo.it = halfEdges.insert(std::make_pair(err, d)) ;
	heinfo.valid = true ;
}

/************************************************************************************
 *                        EDGESELECTOR QEMext for Color                             *
 ************************************************************************************/

template <typename PFP>
bool HalfEdgeSelector_QEMextColor<PFP>::init()
{
	MAP& m = this->m_map ;

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
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3, DART>* >(this->m_approximators[approxindex])) ;
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
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3, DART>* >(this->m_approximators[approxindex])) ;
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
bool HalfEdgeSelector_QEMextColor<PFP>::nextEdge(Dart& d)
{
	if(cur == halfEdges.end() || halfEdges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void HalfEdgeSelector_QEMextColor<PFP>::updateBeforeCollapse(Dart d)
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
	assert(dd != d) ;
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
void HalfEdgeSelector_QEMextColor<PFP>::recomputeQuadric(const Dart d, const bool recomputeNeighbors) {
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
void HalfEdgeSelector_QEMextColor<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	MAP& m = this->m_map ;

	recomputeQuadric(d2, true) ;

	Dart vit = d2 ;
	do
	{
		updateHalfEdgeInfo(vit, true) ;
		Dart d = m.phi2(vit) ;
		if (d != vit) ;
			updateHalfEdgeInfo(d, true) ;

		updateHalfEdgeInfo(m.phi1(vit), true) ;
		d = m.phi2(m.phi1(vit)) ;
		if (d != m.phi1(vit)) ;
			updateHalfEdgeInfo(d, true) ;

		Dart stop = m.phi2(vit) ;
		assert (stop != vit) ;
		Dart vit2 = m.phi12(m.phi1(vit)) ;
		do {
			updateHalfEdgeInfo(vit2, true) ;
			d = m.phi2(vit2) ;
			if (d != vit2) ;
				updateHalfEdgeInfo(d, true) ;

			updateHalfEdgeInfo(m.phi1(vit2), false) ;
			d = m.phi2(m.phi1(vit2)) ;
			if (d != m.phi1(vit2)) ;
				updateHalfEdgeInfo(d, false) ;

			vit2 = m.phi12(vit2) ;
		} while (stop != vit2) ;
		vit = m.phi2_1(vit) ;
	} while(vit != d2) ;

	cur = halfEdges.begin() ; // set the current edge to the first one
}

template <typename PFP>
void HalfEdgeSelector_QEMextColor<PFP>::initHalfEdgeInfo(Dart d)
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
void HalfEdgeSelector_QEMextColor<PFP>::updateHalfEdgeInfo(Dart d, bool recompute)
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
void HalfEdgeSelector_QEMextColor<PFP>::computeHalfEdgeInfo(Dart d, HalfEdgeInfo& heinfo)
{
	MAP& m = this->m_map ;
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
		heinfo.valid = false ;
	else
	{
		heinfo.it = this->halfEdges.insert(std::make_pair(std::max(err,REAL(0)), d)) ;
		heinfo.valid = true ;
	}
}

/************************************************************************************
 *              HALF EDGE LIGHTFIELD SELECTOR (using QEMml half-edge)               *
 ************************************************************************************/

template <typename PFP>
bool HalfEdgeSelector_Lightfield<PFP>::init()
{
	MAP& m = this->m_map ;

	// Verify availability of required approximators
	unsigned int ok = 0 ;
	for (unsigned int approxindex = 0 ; approxindex < this->m_approximators.size() ; ++approxindex)
	{
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
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3, DART>* >(this->m_approximators[approxindex])) ;
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
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3, DART>* >(this->m_approximators[approxindex])) ;
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
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3, DART>* >(this->m_approximators[approxindex])) ;
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
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3, DART>* >(this->m_approximators[approxindex])) ;
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
							m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3, DART>* >(this->m_approximators[approxindex])) ;
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

	// Init multimap for each Half-edge
	halfEdges.clear() ;

	TraversorE<MAP> travE(m);
	for(Dart dit = travE.begin() ; dit != travE.end() ; dit = travE.next())
	{
		initHalfEdgeInfo(dit) ; // init the edges with their optimal position
							// and insert them in the multimap according to their error
	}
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
	assert(dd != d) ;
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
void HalfEdgeSelector_Lightfield<PFP>::recomputeQuadric(const Dart d, const bool recomputeNeighbors)
{
	Dart dFront,dBack ;
	Dart dInit = d ;

	// Init Front
	dFront = dInit ;

	m_quadricGeom[d].zero() ;

   	do {
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
void HalfEdgeSelector_Lightfield<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	MAP& m = this->m_map ;

	recomputeQuadric(d2, true) ;

	Dart vit = d2 ;
	do
	{
		updateHalfEdgeInfo(vit, true) ;
		Dart d = m.phi2(vit) ;
		if (d != vit) ;
			updateHalfEdgeInfo(d, true) ;

		updateHalfEdgeInfo(m.phi1(vit), true) ;
		d = m.phi2(m.phi1(vit)) ;
		if (d != m.phi1(vit)) ;
			updateHalfEdgeInfo(d, true) ;

		Dart stop = m.phi2(vit) ;
		assert (stop != vit) ;
		Dart vit2 = m.phi12(m.phi1(vit)) ;
		do {
			updateHalfEdgeInfo(vit2, true) ;
			d = m.phi2(vit2) ;
			if (d != vit2) ;
				updateHalfEdgeInfo(d, true) ;

			updateHalfEdgeInfo(m.phi1(vit2), false) ;
			d = m.phi2(m.phi1(vit2)) ;
			if (d != m.phi1(vit2)) ;
				updateHalfEdgeInfo(d, false) ;

			vit2 = m.phi12(vit2) ;
		} while (stop != vit2) ;
		vit = m.phi2_1(vit) ;
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

	if (isnan(alpha))
		std::cerr << "Nan: " << m_frameN[d] << " ; " << m_frameN[dd] << " ; " << newFN << std::endl ;

	assert(m_quadricHF.isValid() | !"EdgeSelector_Lightfield<PFP>::computeEdgeInfo: quadricHF is not valid") ;
	Utils::QuadricHF<REAL> quadHF = m_quadricHF[d] ;

	//std::cout << quadGeom(newPos) << " vs " << alpha/M_PI << " vs " << quadHF(newHF) << std::endl ;
	// sum of QEM metric and frame orientation difference
	const REAL& err =
			quadGeom(newPos) // geom
			+ alpha / M_PI // frame
			+ quadHF(newHF) // function coefficients
			 ;

	// Check if errated values appear
	if (err < -1e-6)
		heinfo.valid = false ;
	else
	{
		heinfo.it = this->halfEdges.insert(std::make_pair(std::max(err,REAL(0)), d)) ;
		heinfo.valid = true ;
	}
}

///************************************************************************************
// *                         HALFEDGESELECTOR LIGHTFIELD                              *
// ************************************************************************************/
//template <typename PFP>
//bool HalfEdgeSelector_Lightfield<PFP>::init()
//{
//	// TODO
//	MAP& m = this->m_map ;
//
//	// Verify availability of required approximators
//	unsigned int ok = 0 ;
//	for (unsigned int approxindex = 0 ; approxindex < this->m_approximators.size() ; ++approxindex)
//	{
//		bool saved = false ;
//		for (unsigned int attrindex = 0 ; attrindex < this->m_approximators[approxindex]->getNbApproximated() ; ++ attrindex)
//		{
//			// constraint : 2 approximators in specific order
//			if(ok == 0 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "position")
//			{
//				++ok ;
//				m_approxindex_pos = approxindex ;
//				m_attrindex_pos = attrindex ;
//				m_pos = this->m_position ;
//				if (!saved)
//				{
//					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3>* >(this->m_approximators[approxindex])) ;
//					saved = true ;
//				}
//			}
//			else if(ok == 1 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "frameT")
//			{
//				++ok ;
////				m_approxindex_FT = approxindex ;
////				m_attrindex_FT = attrindex ;
//				m_frameT = m.template getAttribute<typename PFP::VEC3, VERTEX>("frameT") ;
//				if (!saved)
//				{
//					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3>* >(this->m_approximators[approxindex])) ;
//					assert(m_frameT.isValid() || !"EdgeSelector_QEMextColor: frameT attribute is not valid") ;
//					saved = true ;
//				}
//			}
//			else if(ok == 2 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "frameB")
//			{
//				++ok ;
////				m_approxindex_FB = approxindex ;
////				m_attrindex_FB = attrindex ;
//				m_frameB = m.template getAttribute<typename PFP::VEC3, VERTEX>("frameB") ;
//				assert(m_frameB.isValid() || !"EdgeSelector_QEMextColor: frameB attribute is not valid") ;
//				if (!saved)
//				{
//					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3>* >(this->m_approximators[approxindex])) ;
//					saved = true ;
//				}
//			}
//			else if(ok == 3 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "frameN")
//			{
//				++ok ;
//				m_approxindex_FN = approxindex ;
//				m_attrindex_FN = attrindex ;
//				m_frameN = m.template getAttribute<typename PFP::VEC3, VERTEX>("frameN") ;
//				assert(m_frameN.isValid() || !"EdgeSelector_QEMextColor: frameN attribute is not valid") ;
//				if (!saved)
//				{
//					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3>* >(this->m_approximators[approxindex])) ;
//					saved = true ;
//				}
//			}
//		}
//	}
//
//	if(ok != 4)
//		return false ;
//
//	// Set quadric per vertex
//	CellMarker<VERTEX> vMark(m) ;
//	for(Dart d = m.begin(); d != m.end(); m.next(d))
//	{
//		if(!vMark.isMarked(d))
//		{
//			Utils::Quadric<REAL> q ;		// create one quadric
//			m_quadricGeom[d] = q ;		// per vertex
//			vMark.mark(d) ;
//		}
//	}
//
//	// Compute quadric per vertex
//	DartMarker mark(m) ;
//
//	for(Dart d = m.begin(); d != m.end(); m.next(d)) // init QEM quadrics
//	{
//		if(!mark.isMarked(d))
//		{
//			Dart d1 = m.phi1(d) ;					// for each triangle,
//			Dart d_1 = m.phi_1(d) ;					// initialize the quadric of the triangle
//			Utils::Quadric<REAL> q(this->m_position[d], this->m_position[d1], this->m_position[d_1]) ;
//			m_quadricGeom[d] += q ;						// and add the contribution of
//			m_quadricGeom[d1] += q ;						// this quadric to the ones
//			m_quadricGeom[d_1] += q ;						// of the 3 incident vertices
//			mark.markOrbit<FACE>(d) ;
//		}
//	}
//
//	// Init multimap for each Half-edge
//	halfEdges.clear() ;
//
//	for(Dart d = m.begin(); d != m.end(); m.next(d))
//	{
//		initHalfEdgeInfo(d) ;	// init the edges with their optimal info
//	}							// and insert them in the multimap according to their error
//
//	cur = halfEdges.begin() ; 	// init the current edge to the first one
//
//	return true ;
//}
//
//template <typename PFP>
//bool HalfEdgeSelector_Lightfield<PFP>::nextEdge(Dart& d)
//{
//	if(cur == halfEdges.end() || halfEdges.empty())
//		return false ;
//	d = (*cur).second ;
//	return true ;
//}
//
//template <typename PFP>
//void HalfEdgeSelector_Lightfield<PFP>::updateBeforeCollapse(Dart d)
//{
//	MAP& m = this->m_map ;
//
//	HalfEdgeInfo& edgeE = halfEdgeInfo[d] ;
//	if(edgeE.valid)
//		halfEdges.erase(edgeE.it) ;
//
//	edgeE = halfEdgeInfo[m.phi1(d)] ;
//	if(edgeE.valid)						// remove all
//		halfEdges.erase(edgeE.it) ;
//
//	edgeE = halfEdgeInfo[m.phi_1(d)] ;	// the halfedges that will disappear
//	if(edgeE.valid)
//		halfEdges.erase(edgeE.it) ;
//										// from the multimap
//	Dart dd = m.phi2(d) ;
//	if(dd != d)
//	{
//		edgeE = halfEdgeInfo[dd] ;
//		if(edgeE.valid)
//			halfEdges.erase(edgeE.it) ;
//
//		edgeE = halfEdgeInfo[m.phi1(dd)] ;
//		if(edgeE.valid)
//			halfEdges.erase(edgeE.it) ;
//
//		edgeE = halfEdgeInfo[m.phi_1(dd)] ;
//		if(edgeE.valid)
//			halfEdges.erase(edgeE.it) ;
//	}
//}
//
///**
// * Update quadric of a vertex
// * Discards quadrics of d and assigns freshly calculated
// * quadrics depending on the actual planes surrounding d
// * @param dart d
// */
//template <typename PFP>
//void HalfEdgeSelector_Lightfield<PFP>::recomputeQuadric(const Dart d, const bool recomputeNeighbors)
//{	// TODO
//	Dart dFront,dBack ;
//	Dart dInit = d ;
//
//	// Init Front
//	dFront = dInit ;
//
//	m_quadricGeom[d].zero() ;
//
//   	do
//   	{
//   		// Make step
//   		dBack = this->m_map.phi1(dFront) ;
//       	dFront = this->m_map.phi2_1(dFront) ;
//
//       	if (dBack != dFront)
//       	{ // if dFront is no border
//       		m_quadricGeom[d] += Utils::Quadric<REAL>(m_pos[d],m_pos[this->m_map.phi1(dFront)],m_pos[dBack]) ;
//       	}
//       	if (recomputeNeighbors)
//       		recomputeQuadric(dBack, false) ;
//
//    } while(dFront != dInit) ;
//}
//
//template <typename PFP>
//void HalfEdgeSelector_Lightfield<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
//{
//	MAP& m = this->m_map ;
//
//	// TODO
//	recomputeQuadric(d2, true) ;
//
//	Dart vit = d2 ;
//	do
//	{
//		updateHalfEdgeInfo(vit, true) ;
//		Dart d = m.phi2(vit) ;
//		if (d != vit) ;
//			updateHalfEdgeInfo(d, true) ;
//
//		updateHalfEdgeInfo(m.phi1(vit), true) ;
//		d = m.phi2(m.phi1(vit)) ;
//		if (d != m.phi1(vit)) ;
//			updateHalfEdgeInfo(d, true) ;
//
//		Dart stop = m.phi2(vit) ;
//		assert (stop != vit) ;
//		Dart vit2 = m.phi12(m.phi1(vit)) ;
//		do {
//			updateHalfEdgeInfo(vit2, true) ;
//			d = m.phi2(vit2) ;
//			if (d != vit2) ;
//				updateHalfEdgeInfo(d, true) ;
//
//			updateHalfEdgeInfo(m.phi1(vit2), false) ;
//			d = m.phi2(m.phi1(vit2)) ;
//			if (d != m.phi1(vit2)) ;
//				updateHalfEdgeInfo(d, false) ;
//
//			vit2 = m.phi12(vit2) ;
//		} while (stop != vit2) ;
//		vit = m.phi2_1(vit) ;
//	} while(vit != d2) ;
//
//	cur = halfEdges.begin() ; // set the current edge to the first one
//}
//
//template <typename PFP>
//void HalfEdgeSelector_Lightfield<PFP>::initHalfEdgeInfo(Dart d)
//{
//	MAP& m = this->m_map ;
//	HalfEdgeInfo heinfo ;
//	if(m.edgeCanCollapse(d))
//		computeHalfEdgeInfo(d, heinfo) ;
//	else
//		heinfo.valid = false ;
//
//	halfEdgeInfo[d] = heinfo ;
//}
//
//template <typename PFP>
//void HalfEdgeSelector_Lightfield<PFP>::updateHalfEdgeInfo(Dart d, bool recompute)
//{
//	MAP& m = this->m_map ;
//	HalfEdgeInfo& heinfo = halfEdgeInfo[d] ;
//	if(recompute)
//	{
//		if(heinfo.valid)
//			halfEdges.erase(heinfo.it) ;			// remove the edge from the multimap
//		if(m.edgeCanCollapse(d))
//			computeHalfEdgeInfo(d, heinfo) ;
//		else
//			heinfo.valid = false ;
//	}
//	else
//	{
//		if(m.edgeCanCollapse(d))
//		{								 	// if the edge can be collapsed now
//			if(!heinfo.valid)				 // but it was not before
//				computeHalfEdgeInfo(d, heinfo) ;
//		}
//		else
//		{								 // if the edge cannot be collapsed now
//			if(heinfo.valid)				 // and it was before
//			{
//				halfEdges.erase(heinfo.it) ;
//				heinfo.valid = false ;
//			}
//		}
//	}
//}
//
//template <typename PFP>
//void HalfEdgeSelector_Lightfield<PFP>::computeHalfEdgeInfo(Dart d, HalfEdgeInfo& heinfo)
//{
//	MAP& m = this->m_map ;
//	Dart dd = m.phi1(d) ;
//
//	// compute all approximated attributes
//	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = this->m_approximators.begin() ;
//			it != this->m_approximators.end() ;
//			++it)
//	{
//		(*it)->approximate(d) ;
//	}
//
//	// Get all approximated attributes
//	// New position
//	const VEC3& newPos = this->m_approx[m_approxindex_pos]->getApprox(d,m_attrindex_pos) ; // get newPos
//	// New normal
//	const VEC3& newFN = this->m_approx[m_approxindex_FN]->getApprox(d,m_attrindex_FN) ; // get new frameN
//
//	// New function
//	// todo const VEC3& hfcoefs0 = this->m_approximators[m_approxindex_hf]->getApprox(d,m_attrindex_hf[0]) ; // get newHFcoefs0
//	// todo const VEC3& hfcoefs1 = this->m_approximators[m_approxindex_hf]->getApprox(d,m_attrindex_hf[1]) ; // get newHFcoefs1
//	// todo const VEC3& hfcoefs2 = this->m_approximators[m_approxindex_hf]->getApprox(d,m_attrindex_hf[2]) ; // get newHFcoefs2
//	// ...
//	// todo const VEC3& hfcoefsK = this->m_approximators[m_approxindex_hf]->getApprox(d,m_attrindex_hf[K]) ; // get newHFcoefsK
//
//	// Compute errors
//	// Position
//	Utils::Quadric<REAL> quadGeom ;
//	quadGeom += m_quadricGeom[d] ;	// compute the sum of the
//	quadGeom += m_quadricGeom[dd] ;	// two vertices quadrics
//
//	// hemisphere difference error
//	double scal1 = abs(double(m_frameN[d] * newFN)) ;
//	scal1 = std::min(scal1, double(1)) ; // for epsilon normalization of newFN errors
//	// angle 2
//	double scal2 = abs(double(m_frameN[dd] * newFN)) ;
//	scal2 = std::min(scal2, double(1)) ;
//	// Important: sum of abs values works only if newFN is in-between the two old ones (interpolated).
//	// This avoids computation of the sign of alpha1 and alpha2.
//	double alpha = acos(scal1 + scal2) ;
//
//	std::cout << quadGeom(newPos) << " vs " << alpha/M_PI << std::endl ;
//	// sum of QEM metric and frame orientation difference
//	REAL err =
//			quadGeom(newPos) // geom
//			+ alpha / M_PI // frame
//			// todo+ quadHF(newHF) // function coefficients
//			 ;
//
//	heinfo.it = halfEdges.insert(std::make_pair(err, d)) ;
//	heinfo.valid = true ;
//}

///************************************************************************************
// *                         HALFEDGESELECTOR LIGHTFIELD                              *
// ************************************************************************************/
//
//template <typename PFP>
//bool HalfEdgeSelector_Lightfield_deprecated<PFP>::init()
//{
//	MAP& m = this->m_map ;
//
//	// Verify availability of required approximators
//	char ok = 0 ;
//	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = this->m_approximators.begin();
//		it != this->m_approximators.end();
//		++it)
//	{
//		// constraint : 3 approximators in specific order
//		if(ok == 0 && (*it)->getApproximatedAttributeName() == "position")
//		{
//			m_positionApproximator = reinterpret_cast<Approximator<PFP, VEC3>* >(*it) ; // 1) position
//			assert(m_positionApproximator->getType() != A_QEM) ; // A_QEM is not compatible for half-edge crit
//			++ok ;
//		}
//		else if( ok == 1 && (*it)->getApproximatedAttributeName() == "frame")
//		{
//			m_frameApproximator = reinterpret_cast<Approximator<PFP, MATRIX33>* >(*it) ; // 2) frame (needs position)
//			++ok ;
//		}
//		else if(ok == 2 && (*it)->getApproximatedAttributeName() == "colorPTM")
//		{
//			m_RGBfunctionsApproximator = reinterpret_cast<Approximator<PFP, MATRIX36>* >(*it) ; // 3) functions (needs frame)
//			++ok ;
//		}
//	}
//
//	if(ok != 3)
//		return false ;
//
//	// Set quadric per vertex
//	CellMarker<VERTEX> vMark(m) ;
//	for(Dart d = m.begin(); d != m.end(); m.next(d))
//	{
//		if(!vMark.isMarked(d))
//		{
//			Utils::Quadric<REAL> q ;		// create one quadric
//			quadric[d] = q ;		// per vertex
//			vMark.mark(d) ;
//		}
//	}
//
//	// Compute quadric per vertex
//	DartMarker mark(m) ;
//
//	for(Dart d = m.begin(); d != m.end(); m.next(d)) // init QEM quadrics
//	{
//		if(!mark.isMarked(d))
//		{
//			Dart d1 = m.phi1(d) ;					// for each triangle,
//			Dart d_1 = m.phi_1(d) ;					// initialize the quadric of the triangle
//			Utils::Quadric<REAL> q(this->m_position[d], this->m_position[d1], this->m_position[d_1]) ;
//			quadric[d] += q ;						// and add the contribution of
//			quadric[d1] += q ;						// this quadric to the ones
//			quadric[d_1] += q ;						// of the 3 incident vertices
//			mark.markOrbit<FACE>(d) ;
//		}
//	}
//
//	// Init multimap for each Half-edge
//	halfEdges.clear() ;
//
//	for(Dart d = m.begin(); d != m.end(); m.next(d))
//	{
//		initHalfEdgeInfo(d) ;	// init the edges with their optimal info
//	}							// and insert them in the multimap according to their error
//
//	cur = halfEdges.begin() ; 	// init the current edge to the first one
//
//	return true ;
//}
//
//template <typename PFP>
//bool HalfEdgeSelector_Lightfield_deprecated<PFP>::nextEdge(Dart& d)
//{
//	if(cur == halfEdges.end() || halfEdges.empty())
//		return false ;
//	d = (*cur).second ;
//	return true ;
//}
//
//template <typename PFP>
//void HalfEdgeSelector_Lightfield_deprecated<PFP>::updateBeforeCollapse(Dart d)
//{
//	MAP& m = this->m_map ;
//
//	HalfEdgeInfo& edgeE = halfEdgeInfo[d] ;
//	if(edgeE.valid)
//		halfEdges.erase(edgeE.it) ;
//
//	edgeE = halfEdgeInfo[m.phi1(d)] ;
//	if(edgeE.valid)						// remove all
//		halfEdges.erase(edgeE.it) ;
//
//	edgeE = halfEdgeInfo[m.phi_1(d)] ;	// the halfedges that will disappear
//	if(edgeE.valid)
//		halfEdges.erase(edgeE.it) ;
//										// from the multimap
//	Dart dd = m.phi2(d) ;
//	if(dd != d)
//	{
//		edgeE = halfEdgeInfo[dd] ;
//		if(edgeE.valid)
//			halfEdges.erase(edgeE.it) ;
//
//		edgeE = halfEdgeInfo[m.phi1(dd)] ;
//		if(edgeE.valid)
//			halfEdges.erase(edgeE.it) ;
//
//		edgeE = halfEdgeInfo[m.phi_1(dd)] ;
//		if(edgeE.valid)
//			halfEdges.erase(edgeE.it) ;
//	}
//}
//
///**
// * Update quadric of a vertex
// * Discards quadrics of d and assigns freshly calculated
// * quadrics depending on the actual planes surrounding d
// * @param dart d
// */
//template <typename PFP>
//void HalfEdgeSelector_Lightfield_deprecated<PFP>::recomputeQuadric(const Dart d, const bool recomputeNeighbors) {
//	Dart dFront,dBack ;
//	Dart dInit = d ;
//
//	// Init Front
//	dFront = dInit ;
//
//	quadric[d].zero() ;
//
//   	do {
//   		// Make step
//   		dBack = this->m_map.phi1(dFront) ;
//       	dFront = this->m_map.phi2_1(dFront) ;
//
//       	if (dBack != dFront) { // if dFront is no border
//           	quadric[d] += Utils::Quadric<REAL>(this->m_position[d],this->m_position[this->m_map.phi1(dFront)],this->m_position[dBack]) ;
//       	}
//       	if (recomputeNeighbors)
//       		recomputeQuadric(dBack, false) ;
//
//    } while(dFront != dInit) ;
//}
//
//template <typename PFP>
//void HalfEdgeSelector_Lightfield_deprecated<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
//{
//	MAP& m = this->m_map ;
//
//	recomputeQuadric(d2, true) ;
//
//	Dart vit = d2 ;
//	do
//	{
//		updateHalfEdgeInfo(vit, true) ;
//		Dart d = m.phi2(vit) ;
//		if (d != vit) ;
//			updateHalfEdgeInfo(d, true) ;
//
//		updateHalfEdgeInfo(m.phi1(vit), true) ;
//		d = m.phi2(m.phi1(vit)) ;
//		if (d != m.phi1(vit)) ;
//			updateHalfEdgeInfo(d, true) ;
//
//		Dart stop = m.phi2(vit) ;
//		assert (stop != vit) ;
//		Dart vit2 = m.phi12(m.phi1(vit)) ;
//		do {
//			updateHalfEdgeInfo(vit2, true) ;
//			d = m.phi2(vit2) ;
//			if (d != vit2) ;
//				updateHalfEdgeInfo(d, true) ;
//
//			updateHalfEdgeInfo(m.phi1(vit2), false) ;
//			d = m.phi2(m.phi1(vit2)) ;
//			if (d != m.phi1(vit2)) ;
//				updateHalfEdgeInfo(d, false) ;
//
//			vit2 = m.phi12(vit2) ;
//		} while (stop != vit2) ;
//		vit = m.phi2_1(vit) ;
//	} while(vit != d2) ;
//
//
//	cur = halfEdges.begin() ; // set the current edge to the first one
//}
//
//template <typename PFP>
//void HalfEdgeSelector_Lightfield_deprecated<PFP>::initHalfEdgeInfo(Dart d)
//{
//	MAP& m = this->m_map ;
//	HalfEdgeInfo heinfo ;
//	if(m.edgeCanCollapse(d))
//		computeHalfEdgeInfo(d, heinfo) ;
//	else
//		heinfo.valid = false ;
//
//	halfEdgeInfo[d] = heinfo ;
//}
//
//template <typename PFP>
//void HalfEdgeSelector_Lightfield_deprecated<PFP>::updateHalfEdgeInfo(Dart d, bool recompute)
//{
//	MAP& m = this->m_map ;
//	HalfEdgeInfo& heinfo = halfEdgeInfo[d] ;
//	if(recompute)
//	{
//		if(heinfo.valid)
//			halfEdges.erase(heinfo.it) ;			// remove the edge from the multimap
//		if(m.edgeCanCollapse(d))
//			computeHalfEdgeInfo(d, heinfo) ;
//		else
//			heinfo.valid = false ;
//	}
//	else
//	{
//		if(m.edgeCanCollapse(d))
//		{								 	// if the edge can be collapsed now
//			if(!heinfo.valid)				 // but it was not before
//				computeHalfEdgeInfo(d, heinfo) ;
//		}
//		else
//		{								 // if the edge cannot be collapsed now
//			if(heinfo.valid)				 // and it was before
//			{
//				halfEdges.erase(heinfo.it) ;
//				heinfo.valid = false ;
//			}
//		}
//	}
//}
//
//template <typename PFP>
//void HalfEdgeSelector_Lightfield_deprecated<PFP>::computeHalfEdgeInfo(Dart d, HalfEdgeInfo& heinfo)
//{
//	MAP& m = this->m_map ;
//	Dart dd = m.phi1(d) ;
//
//	// New position
//	Utils::Quadric<REAL> quad ;
//	quad += quadric[d] ;	// compute the sum of the
//	quad += quadric[dd] ;	// two vertices quadrics
//
//	this->m_positionApproximator->approximate(d) ; 		// sets newPos
//	VEC3 newPos = this->m_positionApproximator->getApprox(d) ; // get newPos
//
//	// New Frame
//	this->m_frameApproximator->approximate(d) ; 		// sets newF
//	MATRIX33 newFrame = this->m_frameApproximator->getApprox(d) ; // get newF
//
//	VEC3 n1,n2 ;
//	if (! m_frame[d].getSubVectorH(2,0,n1))	{ // get the normals
//		CGoGNout << "EdgeSelector_LightField::computeEdgeInfo --> getSubVectorH 1 failed " << CGoGNendl;
//		exit(2) ;
//	}
//	if (!m_frame[dd].getSubVectorH(2,0,n2))	{ // of the two vertices
//		CGoGNout << "EdgeSelector_LightField::computeEdgeInfo --> getSubVectorH 2 failed " << CGoGNendl;
//		exit(3) ;
//	}
//
//	// New function
//	this->m_RGBfunctionsApproximator->approximate(d) ; 	// sets quadricRGBf and newRGBf
//	MATRIX36 newRGBf = this->m_RGBfunctionsApproximator->getApprox(d) ; // get newRGBf
//
//	// Compute error
//	REAL err = quad(newPos) + (2 * acos (n1 * n2)) + quadricRGBfunctions[d](newRGBf) ;
//
//	heinfo.it = halfEdges.insert(std::make_pair(err, d)) ;
//	heinfo.valid = true ;
//}
//
///************************************************************************************
// *                             EDGESELECTOR LIGHTFIELD                              *
// ************************************************************************************
//
//template <typename PFP>
//bool EdgeSelector_Lightfield<PFP>::init()
//{
//	MAP& m = this->m_map ;
//
//	// Verify availability of required approximators
//	char ok = 0 ;
//	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = this->m_approximators.begin();
//		it != this->m_approximators.end();
//		++it)
//	{
//		// constraint : 3 approximators in specific order
//		if(ok == 0 && (*it)->getApproximatedAttributeName() == "position")
//		{
//			m_positionApproximator = reinterpret_cast<Approximator<PFP, VEC3>* >(*it) ; // 1) position
//			++ok ;
//		}
//		else if( ok == 1 && (*it)->getApproximatedAttributeName() == "frame")
//		{
//			m_frameApproximator = reinterpret_cast<Approximator<PFP, FRAME>* >(*it) ; // 2) frame (needs position)
//			++ok ;
//		}
//		else if(ok == 2 && (*it)->getApproximatedAttributeName() == "colorPTM")
//		{
//			m_RGBfunctionsApproximator = reinterpret_cast<Approximator<PFP, RGBFUNCTIONS>* >(*it) ; // 3) functions (needs frame)
//			++ok ;
//		}
//	}
//
//	if(ok != 3)
//		return false ;
//
//	edges.clear() ;
//
//	CellMarker<VERTEX> vMark(m) ;
//	for(Dart d = m.begin(); d != m.end(); m.next(d))
//	{
//		if(!vMark.isMarked(d))
//		{
//			Utils::Quadric<REAL> q ;		// create one quadric
//			quadric[d] = q ;		// per vertex
//			vMark.mark(d) ;
//		}
//	}
//
//	DartMarker mark(m) ;
//
//	for(Dart d = m.begin(); d != m.end(); m.next(d)) // init QEM quadrics
//	{
//		if(!mark.isMarked(d))
//		{
//			Dart d1 = m.phi1(d) ;					// for each triangle,
//			Dart d_1 = m.phi_1(d) ;					// initialize the quadric of the triangle
//			Utils::Quadric<REAL> q(this->m_position[d], this->m_position[d1], this->m_position[d_1]) ;
//			quadric[d] += q ;						// and add the contribution of
//			quadric[d1] += q ;						// this quadric to the ones
//			quadric[d_1] += q ;						// of the 3 incident vertices
//			mark.markOrbit<FACE>(d) ;
//		}
//	}
//
//	CellMarker<EDGE> eMark(m) ;
//	for(Dart d = m.begin(); d != m.end(); m.next(d))
//	{
//		if(!eMark.isMarked(d))
//		{
//			initEdgeInfo(d) ;	// init the edges with their optimal info
//			eMark.mark(d) ;		// and insert them in the multimap according to their error
//		}
//	}
//
//	cur = edges.begin() ; // init the current edge to the first one
//
//	return true ;
//}
//
//template <typename PFP>
//bool EdgeSelector_Lightfield<PFP>::nextEdge(Dart& d)
//{
//	if(cur == edges.end() || edges.empty())
//		return false ;
//	d = (*cur).second ;
//	return true ;
//}
//
//template <typename PFP>
//void EdgeSelector_Lightfield<PFP>::updateBeforeCollapse(Dart d)
//{
//	MAP& m = this->m_map ;
//
//	EdgeInfo& edgeE = edgeInfo[d] ;
//	if(edgeE.valid)
//		edges.erase(edgeE.it) ;
//
//	edgeE = edgeInfo[m.phi1(d)] ;
//	if(edgeE.valid)					// remove all
//		edges.erase(edgeE.it) ;
//
//	edgeE = edgeInfo[m.phi_1(d)] ;	// the concerned edges
//	if(edgeE.valid)
//		edges.erase(edgeE.it) ;
//									// from the multimap
//	Dart dd = m.phi2(d) ;
//	if(dd != d)
//	{
//		edgeE = edgeInfo[m.phi1(dd)] ;
//		if(edgeE.valid)
//			edges.erase(edgeE.it) ;
//
//		edgeE = edgeInfo[m.phi_1(dd)] ;
//		if(edgeE.valid)
//			edges.erase(edgeE.it) ;
//	}
//
//	tmpQ.zero() ;			// compute quadric for the new
//	tmpQ += quadric[d] ;	// vertex as the sum of those
//	tmpQ += quadric[dd] ;	// of the contracted vertices
//}
//
//template <typename PFP>
//void EdgeSelector_Lightfield<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
//{
//	MAP& m = this->m_map ;
//
//	quadric[d2] = tmpQ ;
//
//	Dart vit = d2 ;
//	do
//	{
//		updateEdgeInfo(m.phi1(vit), false) ;			// must recompute some edge infos in the
//		if(vit == d2 || vit == dd2)						// neighborhood of the collapsed edge
//		{
//			initEdgeInfo(vit) ;							// various optimizations are applied here by
//														// treating differently :
//			Dart vit2 = m.phi12(m.phi1(vit)) ;		// - edges for which the criteria must be recomputed
//			Dart stop = m.phi2(vit) ;					// - edges that must be re-embedded
//			do											// - edges for which only the collapsibility must be re-tested
//			{
//				updateEdgeInfo(vit2, false) ;
//				updateEdgeInfo(m.phi1(vit2), false) ;
//				vit2 = m.phi12(vit2) ;
//			} while(vit2 != stop) ;
//		}
//		else
//			updateEdgeInfo(vit, true) ;
//
//		vit = m.phi2_1(vit) ;
//	} while(vit != d2) ;
//
//	cur = edges.begin() ; // set the current edge to the first one
//}
//
//template <typename PFP>
//void EdgeSelector_Lightfield<PFP>::initEdgeInfo(Dart d)
//{
//	MAP& m = this->m_map ;
//	EdgeInfo einfo ;
//	if(m.edgeCanCollapse(d))
//		computeEdgeInfo(d, einfo) ;
//	else
//		einfo.valid = false ;
//	edgeInfo[d] = einfo ;
//}
//
//template <typename PFP>
//void EdgeSelector_Lightfield<PFP>::updateEdgeInfo(Dart d, bool recompute)
//{
//	MAP& m = this->m_map ;
//	EdgeInfo& einfo = edgeInfo[d] ;
//	if(recompute)
//	{
//		if(einfo.valid)
//			edges.erase(einfo.it) ;			// remove the edge from the multimap
//		if(m.edgeCanCollapse(d))
//			computeEdgeInfo(d, einfo) ;
//		else
//			einfo.valid = false ;
//	}
//	else
//	{
//		if(m.edgeCanCollapse(d))
//		{								 	// if the edge can be collapsed now
//			if(!einfo.valid)				 // but it was not before
//				computeEdgeInfo(d, einfo) ;
//		}
//		else
//		{								 // if the edge cannot be collapsed now
//			if(einfo.valid)				 // and it was before
//			{
//				edges.erase(einfo.it) ;
//				einfo.valid = false ;
//			}
//		}
//	}
//}
//
//template <typename PFP>
//void EdgeSelector_Lightfield<PFP>::computeEdgeInfo(Dart d, EdgeInfo& einfo)
//{
//	MAP& m = this->m_map ;
//	Dart dd = m.phi2(d) ;
//
//	// New position
//	Utils::Quadric<REAL> quad ;
//	quad += quadric[d] ;	// compute the sum of the
//	quad += quadric[dd] ;	// two vertices quadrics
//
//	this->m_positionApproximator->approximate(d) ; 		// sets newPos
//	VEC3 newPos = this->m_positionApproximator->getApprox(d) ; // get newPos
//
//	// New Frame
//	this->m_frameApproximator->approximate(d) ; 		// sets newF
//	MATRIX33 newFrame = this->m_frameApproximator->getApprox(d) ; // get newF
//
//	VEC3 n1,n2 ;
//	if (! m_frame[d].getSubVectorH(2,0,n1))	{ // get the normals
//		CGoGNout << "EdgeSelector_LightField::computeEdgeInfo --> getSubVectorH 1 failed " << CGoGNendl;
//		exit(2) ;
//	}
//	if (!m_frame[dd].getSubVectorH(2,0,n2))	{ // of the two vertices
//		CGoGNout << "EdgeSelector_LightField::computeEdgeInfo --> getSubVectorH 2 failed " << CGoGNendl;
//		exit(3) ;
//	}
//
//	// New function
//	this->m_RGBfunctionsApproximator->approximate(d) ; 	// sets quadricRGBf and newRGBf
//	MATRIX36 newRGBf = this->m_RGBfunctionsApproximator->getApprox(d) ; // get newRGBf
//
//	QuadricRGBfunctions<typename PFP::REAL> quadRGBf = quadricRGBfunctions[d]; // get quadricRGBf
//
//	// Compute error
//	REAL err = quad(newPos) + (2 * acos (n1 * n2)) + quadRGBf(newRGBf) ;
////	REAL t1 = quad(newPos) ;
////	REAL t2 = (2 * acos (n1 * n2)) ;
////	REAL t3 = quadRGBf(newRGBf) ;
//
//	einfo.it = edges.insert(std::make_pair(err, d)) ;
//	einfo.valid = true ;
//}*/

} // namespace Decimation

} // namespace Algo

} // namespace CGoGN
