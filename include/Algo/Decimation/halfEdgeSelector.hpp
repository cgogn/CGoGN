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

namespace Surface
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
			assert((*it)->getType() == A_hQEM || (*it)->getType() == A_hHalfCollapse || (*it)->getType() != A_Lightfield || !"Approximator for selector (HalfEdgeSelector_QEMml) must be of a half-edge approximator") ;
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

	HalfEdgeInfo* edgeE = &(halfEdgeInfo[d]) ;
	if(edgeE->valid)
		halfEdges.erase(edgeE->it) ;

	edgeE = &(halfEdgeInfo[m.phi1(d)]) ;
	if(edgeE->valid)						// remove all
		halfEdges.erase(edgeE->it) ;

	edgeE = &(halfEdgeInfo[m.phi_1(d)]) ;	// the halfedges that will disappear
	if(edgeE->valid)
		halfEdges.erase(edgeE->it) ;
										// from the multimap
	Dart dd = m.phi2(d) ;
	assert(dd != d) ;
	if(dd != d)
	{
		edgeE = &(halfEdgeInfo[dd]) ;
		if(edgeE->valid)
			halfEdges.erase(edgeE->it) ;

		edgeE = &(halfEdgeInfo[m.phi1(dd)]) ;
		if(edgeE->valid)
			halfEdges.erase(edgeE->it) ;

		edgeE = &(halfEdgeInfo[m.phi_1(dd)]) ;
		if(edgeE->valid)
			halfEdges.erase(edgeE->it) ;
	}
}

/*
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
		if (d != vit)
			updateHalfEdgeInfo(d, true) ;

		updateHalfEdgeInfo(m.phi1(vit), true) ;
		d = m.phi2(m.phi1(vit)) ;
		if (d != m.phi1(vit))
			updateHalfEdgeInfo(d, true) ;

		Dart stop = m.phi2(vit) ;
		assert (stop != vit) ;
		Dart vit2 = m.phi12(m.phi1(vit)) ;
		do {
			updateHalfEdgeInfo(vit2, true) ;
			d = m.phi2(vit2) ;
			if (d != vit2)
				updateHalfEdgeInfo(d, true) ;

			updateHalfEdgeInfo(m.phi1(vit2), false) ;
			d = m.phi2(m.phi1(vit2)) ;
			if (d != m.phi1(vit2))
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
		assert(this->m_approximators[approxindex]->getType() == A_hQEM
				|| this->m_approximators[approxindex]->getType() == A_hHalfCollapse
				|| !"Approximator for selector (HalfEdgeSelector_QEMextColor) must be of a half-edge approximator") ;

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

	HalfEdgeInfo* edgeE = &(halfEdgeInfo[d]) ;
	if(edgeE->valid)
		halfEdges.erase(edgeE->it) ;

	edgeE = &(halfEdgeInfo[m.phi1(d)]) ;
	if(edgeE->valid)						// remove all
		halfEdges.erase(edgeE->it) ;

	edgeE = &(halfEdgeInfo[m.phi_1(d)]) ;	// the halfedges that will disappear
	if(edgeE->valid)
		halfEdges.erase(edgeE->it) ;
										// from the multimap
	Dart dd = m.phi2(d) ;
	assert(dd != d) ;
	if(dd != d)
	{
		edgeE = &(halfEdgeInfo[dd]) ;
		if(edgeE->valid)
			halfEdges.erase(edgeE->it) ;

		edgeE = &(halfEdgeInfo[m.phi1(dd)]) ;
		if(edgeE->valid)
			halfEdges.erase(edgeE->it) ;

		edgeE = &(halfEdgeInfo[m.phi_1(dd)]) ;
		if(edgeE->valid)
			halfEdges.erase(edgeE->it) ;
	}
}

/*
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
		if (d != vit)
			updateHalfEdgeInfo(d, true) ;

		updateHalfEdgeInfo(m.phi1(vit), true) ;
		d = m.phi2(m.phi1(vit)) ;
		if (d != m.phi1(vit))
			updateHalfEdgeInfo(d, true) ;

		Dart stop = m.phi2(vit) ;
		assert (stop != vit) ;
		Dart vit2 = m.phi12(m.phi1(vit)) ;
		do {
			updateHalfEdgeInfo(vit2, true) ;
			d = m.phi2(vit2) ;
			if (d != vit2)
				updateHalfEdgeInfo(d, true) ;

			updateHalfEdgeInfo(m.phi1(vit2), false) ;
			d = m.phi2(m.phi1(vit2)) ;
			if (d != m.phi1(vit2))
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
	unsigned int k = 0 ;
	for (unsigned int approxindex = 0 ; approxindex < this->m_approximators.size() ; ++approxindex)
	{
		assert(this->m_approximators[approxindex]->getType() == A_hQEM
				|| this->m_approximators[approxindex]->getType() == A_hHalfCollapse
				|| this->m_approximators[approxindex]->getType() != A_Lightfield
				|| !"Approximator for selector (HalfEdgeSelector_Lightfield) must be of a half-edge approximator") ;

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
					assert(m_frameT.isValid() || !"HalfEdgeSelector_Lightfield: frameT attribute is not valid") ;
					saved = true ;
				}
			}
			else if(ok == 2 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "frameB")
			{
				++ok ;
//				m_approxindex_FB = approxindex ;
//				m_attrindex_FB = attrindex ;
				m_frameB = m.template getAttribute<typename PFP::VEC3, VERTEX>("frameB") ;
				assert(m_frameB.isValid() || !"HalfEdgeSelector_Lightfield: frameB attribute is not valid") ;
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
				assert(m_frameN.isValid() || !"HalfEdgeSelector_Lightfield: frameN attribute is not valid") ;
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
	}
	m_K = k ;

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

	HalfEdgeInfo* edgeE = &(halfEdgeInfo[d]) ;
	if(edgeE->valid)
		halfEdges.erase(edgeE->it) ;

	edgeE = &(halfEdgeInfo[m.phi1(d)]) ;
	if(edgeE->valid)						// remove all
		halfEdges.erase(edgeE->it) ;

	edgeE = &(halfEdgeInfo[m.phi_1(d)]) ;	// the halfedges that will disappear
	if(edgeE->valid)
		halfEdges.erase(edgeE->it) ;
										// from the multimap
	Dart dd = m.phi2(d) ;
	assert(dd != d) ;
	if(dd != d)
	{
		edgeE = &(halfEdgeInfo[dd]) ;
		if(edgeE->valid)
			halfEdges.erase(edgeE->it) ;

		edgeE = &(halfEdgeInfo[m.phi1(dd)]) ;
		if(edgeE->valid)
			halfEdges.erase(edgeE->it) ;

		edgeE = &(halfEdgeInfo[m.phi_1(dd)]) ;
		if(edgeE->valid)
			halfEdges.erase(edgeE->it) ;
	}
}

/*
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
		if (d != vit)
			updateHalfEdgeInfo(d, true) ;

		updateHalfEdgeInfo(m.phi1(vit), true) ;
		d = m.phi2(m.phi1(vit)) ;
		if (d != m.phi1(vit))
			updateHalfEdgeInfo(d, true) ;

		Dart stop = m.phi2(vit) ;
		assert (stop != vit) ;
		Dart vit2 = m.phi12(m.phi1(vit)) ;
		do {
			updateHalfEdgeInfo(vit2, true) ;
			d = m.phi2(vit2) ;
			if (d != vit2)
				updateHalfEdgeInfo(d, true) ;

			updateHalfEdgeInfo(m.phi1(vit2), false) ;
			d = m.phi2(m.phi1(vit2)) ;
			if (d != m.phi1(vit2))
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

	assert(m_quadricHF.isValid() | !"EdgeSelector_Lightfield<PFP>::computeEdgeInfo: quadricHF is not valid") ;
	Utils::QuadricHF<REAL> quadHF = m_quadricHF[d] ;

	//std::cout << quadGeom(newPos) / (alpha/M_PI + quadHF(newHF)) << std::endl ;
	// sum of QEM metric and frame orientation difference
	const REAL& err =
			quadGeom(newPos) + // geom
			alpha / M_PI + // frame
			quadHF(newHF) // function coefficients
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

/************************************************************************************
 *              HALF EDGE LIGHTFIELD SELECTOR (using QEMml half-edge and avg color) *
 ************************************************************************************/

template <typename PFP>
bool HalfEdgeSelector_LightfieldAvgColor<PFP>::init()
{
	MAP& m = this->m_map ;

	// Verify availability of required approximators
	unsigned int ok = 0 ;
	unsigned int k = 0 ;
	for (unsigned int approxindex = 0 ; approxindex < this->m_approximators.size() ; ++approxindex)
	{
		assert(this->m_approximators[approxindex]->getType() == A_hQEM
				|| this->m_approximators[approxindex]->getType() == A_hHalfCollapse
				|| this->m_approximators[approxindex]->getType() != A_Lightfield
				|| !"Approximator for selector (HalfEdgeSelector_Lightfield) must be of a half-edge approximator") ;

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
					assert(m_frameT.isValid() || !"HalfEdgeSelector_LightfieldExp: frameT attribute is not valid") ;
					saved = true ;
				}
			}
			else if(ok == 2 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "frameB")
			{
				++ok ;
//				m_approxindex_FB = approxindex ;
//				m_attrindex_FB = attrindex ;
				m_frameB = m.template getAttribute<typename PFP::VEC3, VERTEX>("frameB") ;
				assert(m_frameB.isValid() || !"HalfEdgeSelector_LightfieldExp: frameB attribute is not valid") ;
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
				assert(m_frameN.isValid() || !"HalfEdgeSelector_LightfieldExp: frameN attribute is not valid") ;
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
	}
	m_K = k ;

	if(ok < 6)
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

	for(Dart d = m.begin() ; d != m.end() ; m.next(d))
	{
		initHalfEdgeInfo(d) ; // init the edges with their optimal position
	}						// and insert them in the multimap according to their error

	cur = halfEdges.begin() ; 	// init the current edge to the first one

	return true ;
}

template <typename PFP>
bool HalfEdgeSelector_LightfieldAvgColor<PFP>::nextEdge(Dart& d)
{
	if(cur == halfEdges.end() || halfEdges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void HalfEdgeSelector_LightfieldAvgColor<PFP>::updateBeforeCollapse(Dart d)
{
	MAP& m = this->m_map ;

	HalfEdgeInfo* edgeE = &(halfEdgeInfo[d]) ;
	if(edgeE->valid)
		halfEdges.erase(edgeE->it) ;

	edgeE = &(halfEdgeInfo[m.phi1(d)]) ;
	if(edgeE->valid)						// remove all
		halfEdges.erase(edgeE->it) ;

	edgeE = &(halfEdgeInfo[m.phi_1(d)]) ;	// the halfedges that will disappear
	if(edgeE->valid)
		halfEdges.erase(edgeE->it) ;
										// from the multimap
	Dart dd = m.phi2(d) ;
	assert(dd != d) ;
	if(dd != d)
	{
		edgeE = &(halfEdgeInfo[dd]) ;
		if(edgeE->valid)
			halfEdges.erase(edgeE->it) ;

		edgeE = &(halfEdgeInfo[m.phi1(dd)]) ;
		if(edgeE->valid)
			halfEdges.erase(edgeE->it) ;

		edgeE = &(halfEdgeInfo[m.phi_1(dd)]) ;
		if(edgeE->valid)
			halfEdges.erase(edgeE->it) ;
	}
}

/*
 * Update quadric of a vertex
 * Discards quadrics of d and assigns freshly calculated
 * quadrics depending on the actual planes surrounding d
 * @param dart d
 */
template <typename PFP>
void HalfEdgeSelector_LightfieldAvgColor<PFP>::recomputeQuadric(const Dart d, const bool recomputeNeighbors)
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
void HalfEdgeSelector_LightfieldAvgColor<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	MAP& m = this->m_map ;

	recomputeQuadric(d2, true) ;

	Dart vit = d2 ;
	do
	{
		updateHalfEdgeInfo(vit, true) ;
		Dart d = m.phi2(vit) ;
		if (d != vit)
			updateHalfEdgeInfo(d, true) ;

		updateHalfEdgeInfo(m.phi1(vit), true) ;
		d = m.phi2(m.phi1(vit)) ;
		if (d != m.phi1(vit))
			updateHalfEdgeInfo(d, true) ;

		Dart stop = m.phi2(vit) ;
		assert (stop != vit) ;
		Dart vit2 = m.phi12(m.phi1(vit)) ;
		do {
			updateHalfEdgeInfo(vit2, true) ;
			d = m.phi2(vit2) ;
			if (d != vit2)
				updateHalfEdgeInfo(d, true) ;

			updateHalfEdgeInfo(m.phi1(vit2), false) ;
			d = m.phi2(m.phi1(vit2)) ;
			if (d != m.phi1(vit2))
				updateHalfEdgeInfo(d, false) ;

			vit2 = m.phi12(vit2) ;
		} while (stop != vit2) ;
		vit = m.phi2_1(vit) ;
	} while(vit != d2) ;

	cur = halfEdges.begin() ; // set the current edge to the first one
}

template <typename PFP>
void HalfEdgeSelector_LightfieldAvgColor<PFP>::initHalfEdgeInfo(Dart d)
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
void HalfEdgeSelector_LightfieldAvgColor<PFP>::updateHalfEdgeInfo(Dart d, bool recompute)
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
void HalfEdgeSelector_LightfieldAvgColor<PFP>::computeHalfEdgeInfo(Dart d, HalfEdgeInfo& heinfo)
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

	assert(m_avgColor.isValid()) ;
	double alpha = alpha1 + alpha2 ;
	VEC3 avgColDiff = m_avgColor[d] ;
	avgColDiff -= m_avgColor[dd] ;
	assert(m_quadricHF.isValid() | !"HalfEdgeSelector_LightfieldAvgColor<PFP>::computeEdgeInfo: quadricHF is not valid") ;
	Utils::QuadricHF<REAL> quadHF = m_quadricHF[d] ;

	//std::cout << quadGeom(newPos) / (alpha/M_PI + quadHF(newHF)) << std::endl ;
	// sum of QEM metric and frame orientation difference
	const REAL& err =
			quadGeom(newPos) + // geom
			(alpha / M_PI) * avgColDiff.norm2() / 3. + // average color times covering area
			//alpha / M_PI + // frame
			quadHF(newHF) // function coefficients
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

/************************************************************************************
 *              HALF EDGE LIGHTFIELD SELECTOR (using QEMml half-edge) KCL-like      *
 ************************************************************************************/

template <typename PFP>
bool HalfEdgeSelector_LightfieldKCL<PFP>::init()
{
	MAP& m = this->m_map ;

	// Verify availability of required approximators
	unsigned int ok = 0 ;
	unsigned int k = 0 ;
	for (unsigned int approxindex = 0 ; approxindex < this->m_approximators.size() ; ++approxindex)
	{
		if (this->m_approximators[approxindex]->getType() != A_hHalfCollapse)
		{
			std::cerr << "Approximator for selector (HalfEdgeSelector_LightfieldKCL) must be of A_hHalfCollapse" << std::endl ;
			return false ;
		}
		assert(this->m_approximators[approxindex]->getType() == A_hHalfCollapse
				|| !"Approximator for selector (HalfEdgeSelector_Lightfield) must be of A_hHalfCollapse") ;

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
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3, DART>* >(this->m_approximators[approxindex])) ;
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
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3, DART>* >(this->m_approximators[approxindex])) ;
					assert(this->m_approx[approxindex]->getAttr(attrindex).isValid() || !"HalfEdgeSelector_LightfieldExp: frameT attribute is not valid") ;
					saved = true ;
				}
			}
			else if(ok == 2 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "frameB")
			{
				++ok ;
				m_approxindex_FB = approxindex ;
				m_attrindex_FB = attrindex ;
				assert(this->m_approx[approxindex]->getAttr(attrindex).isValid() || !"HalfEdgeSelector_LightfieldExp: frameB attribute is not valid") ;
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
				assert(this->m_approx[approxindex]->getAttr(attrindex).isValid() || !"HalfEdgeSelector_LightfieldExp: frameN attribute is not valid") ;
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
					if (this->m_approx[approxindex]->getAttr(attrindex).isValid())
					{
						++k ;
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
	}
	m_K = k ;

	if(ok < 6)
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

	// compute vertex importance
	for(Dart v0 = travV.begin() ; v0 != travV.end() ; v0 = travV.next())
	{
		m_visualImportance[v0] = 0 ;
		REAL max = 0 ;
		Traversor2VVaE<MAP> tv(m,v0) ;
		unsigned int t = 0 ;
		for(Dart vi = tv.begin() ; vi != tv.end() ; vi = tv.next())
		{
			const REAL& err = sqrt(computeSquaredLightfieldDifference(v0,vi)) ;
			m_visualImportance[v0] += err ;
			++t ;
			if (err > max)
				max = err ;
		}
		if (max == 0)
			m_visualImportance[v0] = 0 ;
		else
			m_visualImportance[v0] /= (max*t) ;
	}

	// Init multimap for each Half-edge
	halfEdges.clear() ;

	for(Dart d = m.begin() ; d != m.end() ; m.next(d))
	{
		initHalfEdgeInfo(d) ; // init the edges with their optimal position
	}						// and insert them in the multimap according to their error

	cur = halfEdges.begin() ; 	// init the current edge to the first one

	return true ;
}

template <typename PFP>
bool HalfEdgeSelector_LightfieldKCL<PFP>::nextEdge(Dart& d)
{
	if(cur == halfEdges.end() || halfEdges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void HalfEdgeSelector_LightfieldKCL<PFP>::updateBeforeCollapse(Dart d)
{
	MAP& m = this->m_map ;

	HalfEdgeInfo* edgeE = &(halfEdgeInfo[d]) ;
	if(edgeE->valid)
		halfEdges.erase(edgeE->it) ;

	edgeE = &(halfEdgeInfo[m.phi1(d)]) ;
	if(edgeE->valid)						// remove all
		halfEdges.erase(edgeE->it) ;

	edgeE = &(halfEdgeInfo[m.phi_1(d)]) ;	// the halfedges that will disappear
	if(edgeE->valid)
		halfEdges.erase(edgeE->it) ;
										// from the multimap
	Dart dd = m.phi2(d) ;
	assert(dd != d) ;
	if(dd != d)
	{
		edgeE = &(halfEdgeInfo[dd]) ;
		if(edgeE->valid)
			halfEdges.erase(edgeE->it) ;

		edgeE = &(halfEdgeInfo[m.phi1(dd)]) ;
		if(edgeE->valid)
			halfEdges.erase(edgeE->it) ;

		edgeE = &(halfEdgeInfo[m.phi_1(dd)]) ;
		if(edgeE->valid)
			halfEdges.erase(edgeE->it) ;
	}

	tmpVisualImportance = m_visualImportance[d] + m_visualImportance[dd] ;
}

/*
 * Update quadric of a vertex
 * Discards quadrics of d and assigns freshly calculated
 * quadrics depending on the actual planes surrounding d
 * @param dart d
 */
template <typename PFP>
void HalfEdgeSelector_LightfieldKCL<PFP>::recomputeQuadric(const Dart d, const bool recomputeNeighbors)
{
	Dart dFront,dBack ;
	Dart dInit = d ;

	// Init Front
	dFront = dInit ;

	m_quadricGeom[d].zero() ;
	const VertexAttribute<VEC3>& m_pos = this->m_approx[m_approxindex_pos]->getAttr(m_attrindex_pos) ;

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
void HalfEdgeSelector_LightfieldKCL<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	MAP& m = this->m_map ;

	recomputeQuadric(d2, true) ;

	m_visualImportance[d2] = tmpVisualImportance ;

	Dart vit = d2 ;
	do
	{
		updateHalfEdgeInfo(vit, true) ;
		Dart d = m.phi2(vit) ;
		if (d != vit)
			updateHalfEdgeInfo(d, true) ;

		updateHalfEdgeInfo(m.phi1(vit), true) ;
		d = m.phi2(m.phi1(vit)) ;
		if (d != m.phi1(vit))
			updateHalfEdgeInfo(d, true) ;

		Dart stop = m.phi2(vit) ;
		assert (stop != vit) ;
		Dart vit2 = m.phi12(m.phi1(vit)) ;
		do {
			updateHalfEdgeInfo(vit2, true) ;
			d = m.phi2(vit2) ;
			if (d != vit2)
				updateHalfEdgeInfo(d, true) ;

			updateHalfEdgeInfo(m.phi1(vit2), false) ;
			d = m.phi2(m.phi1(vit2)) ;
			if (d != m.phi1(vit2))
				updateHalfEdgeInfo(d, false) ;

			vit2 = m.phi12(vit2) ;
		} while (stop != vit2) ;
		vit = m.phi2_1(vit) ;
	} while(vit != d2) ;

	cur = halfEdges.begin() ; // set the current edge to the first one
}

template <typename PFP>
void HalfEdgeSelector_LightfieldKCL<PFP>::initHalfEdgeInfo(Dart d)
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
void HalfEdgeSelector_LightfieldKCL<PFP>::updateHalfEdgeInfo(Dart d, bool recompute)
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
void HalfEdgeSelector_LightfieldKCL<PFP>::computeHalfEdgeInfo(Dart d, HalfEdgeInfo& heinfo)
{
	MAP& m = this->m_map ;
	Dart dd = m.phi1(d) ;

	// compute all approximated attributes
	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = this->m_approximators.begin() ;
			it != this->m_approximators.end() ;
			++it)
	{
		(*it)->approximate(d) ;
	}

	// Compute errors
	// Position
	Utils::Quadric<REAL> quadGeom ;
	quadGeom += m_quadricGeom[d] ;	// compute the sum of the
	quadGeom += m_quadricGeom[dd] ;	// two vertices quadrics
	// New position
	const VEC3& newPos = (this->m_approx[m_approxindex_pos]->getAttr(m_attrindex_pos))[d] ; // get newPos

	const REAL& geomErr = quadGeom(newPos) ;
//	const REAL& visualI = m_visualImportance[dd] ;
	const REAL& lferr = computeLightfieldError(d) ;

	//std::cout << lferr/geomErr << std::endl ;
	const REAL& err =
			geomErr + // geom
			//visualI + // visual importance of vertex that will disappear
			lferr / 10.
			 ;

	// Check if errated values appear
	if (lferr < 0 || err < -1e-6 || isnan(err))
	{
		std::cerr << "HalfEdgeSelector_LightfieldKCL<PFP>::computeHalfEdgeInfo: " << err << std::endl ;
		heinfo.valid = false ;
	}
	else
	{
		heinfo.it = this->halfEdges.insert(std::make_pair(std::max(err,REAL(0)), d)) ;
		heinfo.valid = true ;
	}
}

template <typename PFP>
typename PFP::REAL HalfEdgeSelector_LightfieldKCL<PFP>::computeLightfieldError(Dart v0) const
{
	Dart v1 = this->m_map.phi1(v0) ;

	REAL err = 0 ;
	/*Traversor2VVaE<MAP> tv0(this->m_map,v0) ; // all vertices surrounding vertex v0
	for (Dart vi = tv0.begin() ; vi != tv0.end() ; vi = tv0.next())
	{
		if (vi != v1)
		{
			err += computeSquaredLightfieldDifference(v1,vi) ;
		}
	}
	return err ;*/


	// return computeSquaredLightfieldDifference(v0,v1) ;
	Traversor2VVaE<MAP> tv(this->m_map,v1) ; // all vertices surrounding vertex v0
	for (Dart vi = tv.begin() ; vi != tv.end() ; vi = tv.next())
	{
		VEC3 edgeL = this->m_position[v1] - this->m_position[vi] ;
		err += sqrt(computeSquaredLightfieldDifference(v1,vi)) * edgeL.norm() ;
		//std::cout << "1 : " <<  edgeL.norm() << std::endl ;
		edgeL = this->m_position[v0] - this->m_position[vi] ;
		//std::cout << "2 : " << edgeL.norm() << std::endl ;
		err -= sqrt(computeSquaredLightfieldDifference(v0,vi)) * edgeL.norm() ;
	}
	return fabs(err) ;
}

template <typename PFP>
typename PFP::REAL HalfEdgeSelector_LightfieldKCL<PFP>::computeSquaredLightfieldDifference(Dart d1, Dart d2) const
{
	// get two frames
	const VEC3& T1 = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[d1] ;
	const VEC3& T2 = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[d2] ;
	const VEC3& B1 = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[d1] ;
	const VEC3& B2 = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[d2] ;
	const VEC3& N1 = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[d1] ;
	const VEC3& N2 = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[d2] ;

	// compute new frame
	const VEC3& N = N1 ;
	VEC3 T ;
	if (N2 != N1)
		T = N2 ^ N1 ; // i is perpendicular to newNormal
	else
		T = N1 ^ VEC3(1,2,3) ; // second random vector
	T.normalize() ;

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

	double alpha = fabs(alpha1 + alpha2) ;

	// get coefs of v1 and v2
	std::vector<VEC3> coefs1, coefs2, coefs ;
	coefs1.resize(m_K) ; coefs2.resize(m_K) ;
	for (unsigned int i = 0 ; i < m_K ; ++i)
	{
		coefs1[i] = this->m_approx[m_approxindex_HF[i]]->getAttr(m_attrindex_HF[i])[d1] ;
		coefs2[i] = this->m_approx[m_approxindex_HF[i]]->getAttr(m_attrindex_HF[i])[d2] ;
	}

	Utils::QuadricHF<REAL> q(coefs2, gamma2, alpha2) ;
	bool opt = q.findOptimizedCoefs(coefs) ; // coefs of d1's lightfield rotated around new local axis
	q += Utils::QuadricHF<REAL>(coefs1, gamma1, alpha1) ;

	if (!opt)
	{
		std::cerr << "HalfEdgeSelector_LightfieldKCL<PFP>::Optimization failed (should never happen since no optim is done)" << std::endl ;
		std::cout << alpha2 << std::endl ;
	}

	const VEC3 avgColDiff = m_avgColor[d1] - m_avgColor[d2] ;

	REAL err = q(coefs) ;
	if (fabs(err) < 1e-6)
		err = 0 ;

	return (alpha / M_PI) * avgColDiff.norm2()/3. + err ;

}

/************************************************************************************
 *                           COLOR EXPERIMENTAL                                     *
 ************************************************************************************/

template <typename PFP>
bool HalfEdgeSelector_ColorExperimental<PFP>::init()
{
	MAP& m = this->m_map ;


	// Verify availability of required approximators
	unsigned int ok = 0 ;
	for (unsigned int approxindex = 0 ; approxindex < this->m_approximators.size() ; ++approxindex)
	{
		assert(this->m_approximators[approxindex]->getType() == A_hQEM
				|| this->m_approximators[approxindex]->getType() == A_hHalfCollapse
				|| !"Approximator for selector (HalfEdgeSelector_ColorExperimental) must be of a half-edge approximator") ;

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

	CellMarker<VERTEX> vMark(m) ;
	for(Dart d = m.begin(); d != m.end(); m.next(d))
	{
		if(!vMark.isMarked(d))
		{
			Utils::Quadric<REAL> q ;	// create one quadric
			m_quadric[d] = q ;	// per vertex
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
			m_quadric[d] += q ;					// and add the contribution of
			m_quadric[d1] += q ;					// this quadric to the ones
			m_quadric[d_1] += q ;					// of the 3 incident vertices
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
bool HalfEdgeSelector_ColorExperimental<PFP>::nextEdge(Dart& d)
{
	if(cur == halfEdges.end() || halfEdges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void HalfEdgeSelector_ColorExperimental<PFP>::updateBeforeCollapse(Dart d)
{
	MAP& m = this->m_map ;

	const Dart& v0 = m.phi1(d) ;

	Traversor2VVaE<MAP> tv(m,v0) ;
	for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
	{
		Traversor2VE<MAP> te(m,v) ;
		for (Dart he = te.begin() ; he != te.end() ; he = te.next())
		{
			HalfEdgeInfo* edgeE = &(halfEdgeInfo[he]) ;
			if(edgeE->valid)
			{
				edgeE->valid = false ;
				halfEdges.erase(edgeE->it) ;
			}
			Dart de = m.phi2(he) ;
			edgeE = &(halfEdgeInfo[de]) ;
			if(edgeE->valid)
			{
				edgeE->valid = false ;
				halfEdges.erase(edgeE->it) ;
			}
		}
	}

//	HalfEdgeInfo* edgeE = &(halfEdgeInfo[d]) ;
//	if(edgeE->valid)
//		halfEdges.erase(edgeE->it) ;
//
//	edgeE = &(halfEdgeInfo[m.phi1(d)]) ;
//	if(edgeE->valid)						// remove all
//		halfEdges.erase(edgeE->it) ;
//
//	edgeE = &(halfEdgeInfo[m.phi_1(d)]) ;	// the halfedges that will disappear
//	if(edgeE->valid)
//		halfEdges.erase(edgeE->it) ;
//										// from the multimap
//	Dart dd = m.phi2(d) ;
//	assert(dd != d) ;
//	if(dd != d)
//	{
//		edgeE = &(halfEdgeInfo[dd]) ;
//		if(edgeE->valid)
//			halfEdges.erase(edgeE->it) ;
//
//		edgeE = &(halfEdgeInfo[m.phi1(dd)]) ;
//		if(edgeE->valid)
//			halfEdges.erase(edgeE->it) ;
//
//		edgeE = &(halfEdgeInfo[m.phi_1(dd)]) ;
//		if(edgeE->valid)
//			halfEdges.erase(edgeE->it) ;
//	}
}

/*
 * Update quadric of a vertex
 * Discards quadrics of d and assigns freshly calculated
 * quadrics depending on the actual planes surrounding d
 * @param dart d
 */
template <typename PFP>
void HalfEdgeSelector_ColorExperimental<PFP>::recomputeQuadric(const Dart d)
{
	Dart dFront,dBack ;
	Dart dInit = d ;

	// Init Front
	dFront = dInit ;

	m_quadric[d].zero() ;

   	do {
   		// Make step
   		dBack = this->m_map.phi1(dFront) ;
       	dFront = this->m_map.phi2_1(dFront) ;

       	if (this->m_map.phi2(dFront) != dFront) { // if dFront is no border
       		m_quadric[d] += Utils::Quadric<REAL>(this->m_position[d],this->m_position[dBack],this->m_position[this->m_map.phi1(dFront)]) ;
       	}
    } while(dFront != dInit) ;
}

template <typename PFP>
void HalfEdgeSelector_ColorExperimental<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	MAP& m = this->m_map ;

	const Dart& v1 = d2 ;

	recomputeQuadric(v1) ;
	Traversor2VVaE<MAP> tv(m,v1) ;
	for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
	{
		recomputeQuadric(v) ;
	}

	for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
	{
		Traversor2VE<MAP> te(m,v) ;
		for (Dart e = te.begin() ; e != te.end() ; e = te.next())
		{
			updateHalfEdgeInfo(e) ;
			updateHalfEdgeInfo(m.phi2(e)) ;
		}
	}

	cur = halfEdges.begin() ; // set the current edge to the first one
}

template <typename PFP>
void HalfEdgeSelector_ColorExperimental<PFP>::initHalfEdgeInfo(Dart d)
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
void HalfEdgeSelector_ColorExperimental<PFP>::updateHalfEdgeInfo(Dart d)
{
	MAP& m = this->m_map ;
	HalfEdgeInfo& heinfo = halfEdgeInfo[d] ;

	if(!heinfo.valid && m.edgeCanCollapse(d))
		computeHalfEdgeInfo(d, heinfo) ;
}

template <typename PFP>
void HalfEdgeSelector_ColorExperimental<PFP>::computeHalfEdgeInfo(Dart d, HalfEdgeInfo& heinfo)
{
	MAP& m = this->m_map ;
	Dart dd = m.phi1(d) ;

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

	// Get all approximated attributes
	// New position
	const VEC3& newPos = this->m_approx[m_approxindex_pos]->getApprox(d,m_attrindex_pos) ; // get newPos
	// New normal
//	const VEC3& newColor = this->m_approx[m_approxindex_color]->getApprox(d,m_attrindex_color) ; // get new color

	const Dart& v0 = dd ;
	const Dart& v1 = d ;

	assert(newPos == m_pos[v1]) ;
	assert( this->m_approx[m_approxindex_color]->getApprox(d,m_attrindex_color) == m_color[v1]) ;

	// Compute errors
	// Position
	Utils::Quadric<REAL> quadGeom ;
	quadGeom += m_quadric[d] ;	// compute the sum of the
	quadGeom += m_quadric[dd] ;	// two vertices quadrics

	// sum of QEM metric and frame orientation difference
	const REAL& err =
			quadGeom(newPos) + // geom
			10*computeExperimentalColorError(v0,v1).norm()/sqrt(3) // color
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

template <typename PFP>
typename PFP::VEC3
HalfEdgeSelector_ColorExperimental<PFP>::computeExperimentalColorError(const Dart& v0, const Dart& v1) const
{
	MAP& m = this->m_map ;

	Traversor2VF<MAP> tf(m,v0) ; // all faces around vertex v0
	const unsigned int& v1_vertexId = m.template getEmbedding<VERTEX>(v1) ;

	const VEC3& P0 = m_pos[v0] ;
	const VEC3& P1 = m_pos[v1] ;
	const VEC3& c0 = m_color[v0] ;
	const VEC3& c1 = m_color[v1] ;

	REAL res1 = 0, res2 = 0 ;
	unsigned int count1 = 0, count2 = 0 ;
	for (Dart fi = tf.begin() ; fi != tf.end() ; fi = tf.next())
	{
		const Dart& vi = m.phi1(fi) ;
		const Dart& vj = m.phi_1(fi) ;

		const VEC3& Pi = this->m_position[vi] ;
		const VEC3& Pj = this->m_position[vj] ;

		const VEC3& ci = m_color[vi] ;
		const VEC3& cj = m_color[vj] ;

		const VEC3 Pi_P0 = P0 - Pi ;
		//const VEC3 PjP0 = P0 - Pj ;
		const VEC3 Pi_Pj = Pj - Pi ;
		const REAL b = Pi_Pj.norm() ;

		const VEC3 P0proj2D = Pi + (Pi_P0 * Pi_Pj)*Pi_Pj ;
		const VEC3 P0_P0proj2D = P0proj2D - P0 ;
		const REAL h0 = P0_P0proj2D.norm() ;

		const VEC3 coldiff01 = c1 - c0 ;
		res1 += coldiff01.norm() * b * h0 / (6. * sqrt(3)) ;
		++count1 ;

		// test if v1 is adjacent to this face
		bool adjacent = false ;
		Traversor2FV<MAP> tv(m,fi) ;
		for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
		{
				adjacent |= v1_vertexId == m.template getEmbedding<VERTEX>(v) ;
		}

		// if v1 is not adjacent to this face
		if (!adjacent)
		{
			const VEC3 P1proj3D = P1 ;
			const VEC3 Pi_P1 = P1proj3D - Pi ;
			//const VEC3 PjP1 = newP1 - Pj ;

			const VEC3 P1proj2D = Pi + (Pi_P1 * Pi_Pj)*Pi_Pj ;
			const VEC3 P1proj2D_P1proj3D = P1proj2D - P1proj3D ;
			const REAL h1 = P1proj2D_P1proj3D.norm() ;

			const VEC3 coldiff0i = ci - c0 ;
			const VEC3 coldiff1i = ci - c1 ;
			const VEC3 coldiff0j = cj - c0 ;
			const VEC3 coldiff1j = cj - c1 ;

			res2 += (coldiff0i.norm() + coldiff0j.norm() + coldiff1i.norm() + coldiff1j.norm())
					* b * std::fabs(h1 - h0)
					/ (12. * sqrt(3)) ;
			++count2 ;
		}
	}

	assert(count1-2==count2) ;

	return res1 / count1 + res2 / count2 ;
}

/************************************************************************************
 *                           COLOR GRADIENT ERR                                     *
 ************************************************************************************/

template <typename PFP>
bool HalfEdgeSelector_ColorGradient<PFP>::init()
{
	MAP& m = this->m_map ;


	// Verify availability of required approximators
	unsigned int ok = 0 ;
	for (unsigned int approxindex = 0 ; approxindex < this->m_approximators.size() ; ++approxindex)
	{
		assert(this->m_approximators[approxindex]->getType() == A_hQEM
				|| this->m_approximators[approxindex]->getType() == A_hHalfCollapse
				|| !"Approximator for selector (HalfEdgeSelector_ColorExperimental) must be of a half-edge approximator") ;

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

	CellMarker<VERTEX> vMark(m) ;
	for(Dart d = m.begin(); d != m.end(); m.next(d))
	{
		if(!vMark.isMarked(d))
		{
			Utils::Quadric<REAL> q ;	// create one quadric
			m_quadric[d] = q ;	// per vertex
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
			m_quadric[d] += q ;					// and add the contribution of
			m_quadric[d1] += q ;					// this quadric to the ones
			m_quadric[d_1] += q ;					// of the 3 incident vertices
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
bool HalfEdgeSelector_ColorGradient<PFP>::nextEdge(Dart& d)
{
	if(cur == halfEdges.end() || halfEdges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void HalfEdgeSelector_ColorGradient<PFP>::updateBeforeCollapse(Dart d)
{
	MAP& m = this->m_map ;

	const Dart& v0 = m.phi1(d) ;

	Traversor2VVaE<MAP> tv(m,v0) ;
	for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
	{
		Traversor2VE<MAP> te(m,v) ;
		for (Dart he = te.begin() ; he != te.end() ; he = te.next())
		{
			HalfEdgeInfo* edgeE = &(halfEdgeInfo[he]) ;
			if(edgeE->valid)
			{
				edgeE->valid = false ;
				halfEdges.erase(edgeE->it) ;
			}
			Dart de = m.phi2(he) ;
			edgeE = &(halfEdgeInfo[de]) ;
			if(edgeE->valid)
			{
				edgeE->valid = false ;
				halfEdges.erase(edgeE->it) ;
			}
		}
	}

//	HalfEdgeInfo* edgeE = &(halfEdgeInfo[d]) ;
//	if(edgeE->valid)
//		halfEdges.erase(edgeE->it) ;
//
//	edgeE = &(halfEdgeInfo[m.phi1(d)]) ;
//	if(edgeE->valid)						// remove all
//		halfEdges.erase(edgeE->it) ;
//
//	edgeE = &(halfEdgeInfo[m.phi_1(d)]) ;	// the halfedges that will disappear
//	if(edgeE->valid)
//		halfEdges.erase(edgeE->it) ;
//										// from the multimap
//	Dart dd = m.phi2(d) ;
//	assert(dd != d) ;
//	if(dd != d)
//	{
//		edgeE = &(halfEdgeInfo[dd]) ;
//		if(edgeE->valid)
//			halfEdges.erase(edgeE->it) ;
//
//		edgeE = &(halfEdgeInfo[m.phi1(dd)]) ;
//		if(edgeE->valid)
//			halfEdges.erase(edgeE->it) ;
//
//		edgeE = &(halfEdgeInfo[m.phi_1(dd)]) ;
//		if(edgeE->valid)
//			halfEdges.erase(edgeE->it) ;
//	}
}

/*
 * Update quadric of a vertex
 * Discards quadrics of d and assigns freshly calculated
 * quadrics depending on the actual planes surrounding d
 * @param dart d
 */
template <typename PFP>
void HalfEdgeSelector_ColorGradient<PFP>::recomputeQuadric(const Dart d)
{
	Dart dFront,dBack ;
	Dart dInit = d ;

	// Init Front
	dFront = dInit ;

	m_quadric[d].zero() ;

	do {
		// Make step
		dBack = this->m_map.phi1(dFront) ;
		dFront = this->m_map.phi2_1(dFront) ;

		if (this->m_map.phi2(dFront) != dFront) { // if dFront is no border
			m_quadric[d] += Utils::Quadric<REAL>(this->m_position[d],this->m_position[dBack],this->m_position[this->m_map.phi1(dFront)]) ;
		}
	} while(dFront != dInit) ;
}

template <typename PFP>
void HalfEdgeSelector_ColorGradient<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	MAP& m = this->m_map ;

	const Dart& v1 = d2 ;

	recomputeQuadric(v1) ;
	Traversor2VVaE<MAP> tv(m,v1) ;
	for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
	{
		recomputeQuadric(v) ;
	}

	for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
	{
		Traversor2VE<MAP> te(m,v) ;
		for (Dart e = te.begin() ; e != te.end() ; e = te.next())
		{
			updateHalfEdgeInfo(e) ;
			updateHalfEdgeInfo(m.phi2(e)) ;
		}
	}

	cur = halfEdges.begin() ; // set the current edge to the first one
}

template <typename PFP>
void HalfEdgeSelector_ColorGradient<PFP>::initHalfEdgeInfo(Dart d)
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
void HalfEdgeSelector_ColorGradient<PFP>::updateHalfEdgeInfo(Dart d)
{
	MAP& m = this->m_map ;
	HalfEdgeInfo& heinfo = halfEdgeInfo[d] ;

	if(!heinfo.valid && m.edgeCanCollapse(d))
		computeHalfEdgeInfo(d, heinfo) ;
}

template <typename PFP>
void HalfEdgeSelector_ColorGradient<PFP>::computeHalfEdgeInfo(Dart d, HalfEdgeInfo& heinfo)
{
	MAP& m = this->m_map ;
	Dart dd = m.phi1(d) ;

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

	// Get all approximated attributes
	// New position
	const VEC3& newPos = this->m_approx[m_approxindex_pos]->getApprox(d,m_attrindex_pos) ; // get newPos
	// New normal
//	const VEC3& newColor = this->m_approx[m_approxindex_color]->getApprox(d,m_attrindex_color) ; // get new color

	const Dart& v0 = dd ;
	const Dart& v1 = d ;

	assert(newPos == m_pos[v1]) ;
	assert( this->m_approx[m_approxindex_color]->getApprox(d,m_attrindex_color) == m_color[v1]) ;

	// Compute errors
	// Position
	Utils::Quadric<REAL> quadGeom ;
	quadGeom += m_quadric[d] ;	// compute the sum of the
	quadGeom += m_quadric[dd] ;	// two vertices quadrics

	//std::cout << quadGeom(newPos) / (alpha/M_PI + quadHF(newHF)) << std::endl ;
	// sum of QEM metric and color gradient metric
	const REAL t = 0.01 ;
	const REAL& err =
			t*quadGeom(newPos) + // geom
			(1-t)*computeGradientColorError(v0,v1).norm()/sqrt(3) // color
			;

	/*std::cout << quadGeom(newPos) << std::endl ;
	std::cerr << computeGradientColorError(v0,v1).norm()/sqrt(3) << std::endl ;*/

	// Check if errated values appear
	if (err < -1e-6)
		heinfo.valid = false ;
	else
	{
		heinfo.it = this->halfEdges.insert(std::make_pair(std::max(err,REAL(0)), d)) ;
		heinfo.valid = true ;
	}
}

template <typename PFP>
typename PFP::VEC3
HalfEdgeSelector_ColorGradient<PFP>::computeGradientColorError(const Dart& v0, const Dart& v1) const
{
	MAP& m = this->m_map ;

	Traversor2VF<MAP> tf(m,v0) ; // all faces around vertex v0

	const VEC3& P0 = m_pos[v0] ;
	const VEC3& P1 = m_pos[v1] ;
	const VEC3& c0 = m_color[v0] ;
	const VEC3& c1 = m_color[v1] ;
	const VEC3 d = P1 - P0 ; // displacement vector

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
			VEC3 grad = (ei.norm2()*(ci[c]-c1[c]) + (ei*ej)*(cj[c]-c1[c]))*ej ;
			grad -= (ej.norm2()*(cj[c]-c1[c]) + (ei*ej)*(ci[c]-c1[c]))*ei ;
			const REAL denom = areaIJ0sq ;
			if (denom < 1e-9) // case flat triangles
				grad = VEC3(0,0,0) ;
			else
				grad /= denom ;

			// color change error for channel i
			count[c] += areaIJ0 * pow(d*grad,2) ;
		}
	}

	const VEC3 colDiff = c1 - c0 ;
	for (unsigned int c = 0 ; c < 3 ; ++c)
	{
		count[c] += pow(colDiff[c],2) * areaSum ;
	}

	return count ;
}

/************************************************************************************
 *                           LIGHTFIELD EXPERIMENTAL                                *
 ************************************************************************************

template <typename PFP>
bool HalfEdgeSelector_LFexperimental<PFP>::init()
{
	MAP& m = this->m_map ;

	// Verify availability of required approximators
	unsigned int ok = 0 ;
	unsigned int k = 0 ;
	for (unsigned int approxindex = 0 ; approxindex < this->m_approximators.size() ; ++approxindex)
	{
		if (this->m_approximators[approxindex]->getType() != A_hHalfCollapse)
		{
			std::cerr << "Approximator for selector (HalfEdgeSelector_LFexperimental) must be of A_hHalfCollapse" << std::endl ;
			return false ;
		}
		assert(this->m_approximators[approxindex]->getType() == A_hHalfCollapse
				|| !"Approximator for selector (HalfEdgeSelector_LFexperimental) must be of A_hHalfCollapse") ;

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
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3, DART>* >(this->m_approximators[approxindex])) ;
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
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3, DART>* >(this->m_approximators[approxindex])) ;
					assert(this->m_approx[approxindex]->getAttr(attrindex).isValid() || !"HalfEdgeSelector_LFexperimental: frameT attribute is not valid") ;
					saved = true ;
				}
			}
			else if(ok == 2 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "frameB")
			{
				++ok ;
				m_approxindex_FB = approxindex ;
				m_attrindex_FB = attrindex ;
				assert(this->m_approx[approxindex]->getAttr(attrindex).isValid() || !"HalfEdgeSelector_LFexperimental: frameB attribute is not valid") ;
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
				assert(this->m_approx[approxindex]->getAttr(attrindex).isValid() || !"HalfEdgeSelector_LFexperimental: frameN attribute is not valid") ;
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
					if (this->m_approx[approxindex]->getAttr(attrindex).isValid())
					{
						++k ;
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
	}
	m_K = k ;

	if(ok < 6)
	{
		std::cerr << "HalfEdgeSelector_LFexperimental<PFP>::init() not OK" << std::endl ;
		return false ;
	}

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

	// Init multimap for each Half-edge
	halfEdges.clear() ;

	for(Dart d = m.begin() ; d != m.end() ; m.next(d))
	{
		initHalfEdgeInfo(d) ; // init the edges with their optimal position
	}						// and insert them in the multimap according to their error

	cur = halfEdges.begin() ; 	// init the current edge to the first one

	return true ;
}

template <typename PFP>
bool HalfEdgeSelector_LFexperimental<PFP>::nextEdge(Dart& d)
{
	if(cur == halfEdges.end() || halfEdges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void HalfEdgeSelector_LFexperimental<PFP>::updateBeforeCollapse(Dart d)
{
	MAP& m = this->m_map ;

	const Dart& v0 = m.phi1(d) ;

	Traversor2VVaE<MAP> tv(m,v0) ;
	for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
	{
		Traversor2VE<MAP> te(m,v) ;
		for (Dart he = te.begin() ; he != te.end() ; he = te.next())
		{
			HalfEdgeInfo* edgeE = &(halfEdgeInfo[he]) ;
			if(edgeE->valid)
			{
				edgeE->valid = false ;
				halfEdges.erase(edgeE->it) ;
			}
			Dart de = m.phi2(he) ;
			edgeE = &(halfEdgeInfo[de]) ;
			if(edgeE->valid)
			{
				edgeE->valid = false ;
				halfEdges.erase(edgeE->it) ;
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
void HalfEdgeSelector_LFexperimental<PFP>::recomputeQuadric(const Dart d)
{
	Dart dFront,dBack ;
	Dart dInit = d ;

	// Init Front
	dFront = dInit ;

	m_quadric[d].zero() ;

   	do {
   		// Make step
   		dBack = this->m_map.phi1(dFront) ;
       	dFront = this->m_map.phi2_1(dFront) ;

       	if (this->m_map.phi2(dFront) != dFront) { // if dFront is no border
       		m_quadric[d] += Utils::Quadric<REAL>(this->m_position[d],this->m_position[dBack],this->m_position[this->m_map.phi1(dFront)]) ;
       	}
    } while(dFront != dInit) ;
}

template <typename PFP>
void HalfEdgeSelector_LFexperimental<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	MAP& m = this->m_map ;

	const Dart& v1 = d2 ;

	recomputeQuadric(v1) ;
	Traversor2VVaE<MAP> tv(m,v1) ;
	for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
	{
		recomputeQuadric(v) ;
	}

	for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
	{
		Traversor2VE<MAP> te(m,v) ;
		for (Dart e = te.begin() ; e != te.end() ; e = te.next())
		{
			updateHalfEdgeInfo(e) ;
			updateHalfEdgeInfo(m.phi2(e)) ;
		}
	}

	cur = halfEdges.begin() ; // set the current edge to the first one
}

template <typename PFP>
void HalfEdgeSelector_LFexperimental<PFP>::initHalfEdgeInfo(Dart d)
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
void HalfEdgeSelector_LFexperimental<PFP>::updateHalfEdgeInfo(Dart d)
{
	MAP& m = this->m_map ;
	HalfEdgeInfo& heinfo = halfEdgeInfo[d] ;

	if(!heinfo.valid && m.edgeCanCollapse(d))
		computeHalfEdgeInfo(d, heinfo) ;
}

template <typename PFP>
void HalfEdgeSelector_LFexperimental<PFP>::computeHalfEdgeInfo(Dart d, HalfEdgeInfo& heinfo)
{
	MAP& m = this->m_map ;
	Dart dd = m.phi1(d) ;

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

	// Get all approximated attributes
	// New position
	const VEC3& newPos = this->m_approx[m_approxindex_pos]->getApprox(d,m_attrindex_pos) ; // get newPos

	const Dart& v0 = dd ;
	const Dart& v1 = d ;

	assert(newPos == this->m_position[v1]) ;

	// Compute errors
	// Position
	Utils::Quadric<REAL> quadGeom ;
	quadGeom += m_quadric[d] ;	// compute the sum of the
	quadGeom += m_quadric[dd] ;	// two vertices quadrics

	// sum of QEM metric
	const REAL& err =
			quadGeom(newPos) + // geom
			1000*computeLightfieldError(v0,v1) // lf
			;
	//std::cout << computeLightfieldError(v0,v1) / quadGeom(newPos) << std::endl ;

	// Check if errated values appear
	if (err < -1e-6)
		heinfo.valid = false ;
	else
	{
		heinfo.it = this->halfEdges.insert(std::make_pair(std::max(err,REAL(0)), d)) ;
		heinfo.valid = true ;
	}
}

template <typename PFP>
typename PFP::REAL
HalfEdgeSelector_LFexperimental<PFP>::computeLightfieldError(const Dart& v0, const Dart& v1) const
{
	MAP& m = this->m_map ;

	Traversor2VF<MAP> tf(m,v0) ;
	const unsigned int& v1_vertexId = m.template getEmbedding<VERTEX>(v1) ;

	const VEC3& P0 = this->m_position[v0] ;
	const VEC3& P1 = this->m_position[v1] ;

	REAL res1 = 0, res2 = 0 ;
	unsigned int count = 0 ;
	for (Dart fi = tf.begin() ; fi != tf.end() ; fi = tf.next())
	{
		const Dart& vi = m.phi1(fi) ;
		const Dart& vj = m.phi_1(fi) ;

		const VEC3& Pi = this->m_position[vi] ;
		const VEC3& Pj = this->m_position[vj] ;

		const VEC3 Pi_P0 = P0 - Pi ;
		//const VEC3 PjP0 = P0 - Pj ;
		const VEC3 Pi_Pj = Pj - Pi ;
		const REAL b = Pi_Pj.norm() ;

		const VEC3 P0proj2D = Pi + (Pi_P0 * Pi_Pj)*Pi_Pj ;
		const VEC3 P0_P0proj2D = P0proj2D - P0 ;
		const REAL h0 = P0_P0proj2D.norm() ;

		const REAL lfdiff01 = computeSquaredLightfieldDifference(v0,v1) ;
		res1 += lfdiff01 * b * h0 / (6.*sqrt(3)) ;

		// test if v1 is adjacent to this face
		bool adjacent = false ;
		Traversor2FV<MAP> tv(m,fi) ;
		for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
		{
			if (v1_vertexId == m.template getEmbedding<VERTEX>(v))
				adjacent = true ;
		}

		// if v1 is not adjacent to this face
		if (!adjacent)
		{
			const VEC3 P1proj3D = P1 ;
			const VEC3 Pi_P1 = P1proj3D - Pi ;
			//const VEC3 PjP1 = newP1 - Pj ;

			const VEC3 P1proj2D = Pi + (Pi_P1 * Pi_Pj)*Pi_Pj ;
			const VEC3 P1proj2D_P1proj3D = P1proj2D - P1proj3D ;
			const REAL h1 = P1proj2D_P1proj3D.norm() ;

			const REAL lfdiff0i = computeSquaredLightfieldDifference(vi,v0) ;
			const REAL lfdiff1i = computeSquaredLightfieldDifference(vi,v1) ;
			const REAL lfdiff0j = computeSquaredLightfieldDifference(vj,v0) ;
			const REAL lfdiff1j = computeSquaredLightfieldDifference(vj,v1) ;

			res2 += (lfdiff0i + lfdiff0j + lfdiff1i + lfdiff1j) * b * std::fabs(h1 - h0) / (12.*sqrt(3)) ;
		}
		++count ;
	}

	return res1 / count + res2 / (count-2) ;
}

template <typename PFP>
typename PFP::REAL HalfEdgeSelector_LFexperimental<PFP>::computeSquaredLightfieldDifference(const Dart& v0, const Dart& v1) const
{
	// get two frames
	const VEC3& T1 = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[v0] ;
	const VEC3& T2 = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[v1] ;
	const VEC3& B1 = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[v0] ;
	const VEC3& B2 = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[v1] ;
	const VEC3& N1 = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[v0] ;
	const VEC3& N2 = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[v1] ;

	// Get new frame
	// precondition : this->m_approx[m_approxindex_FN]->approximate should have been called !
	const VEC3& N = N2 ; // assumption : half-collapse

	// compute new frame
	VEC3 T ;
	if (N2 != N1)
		T = N2 ^ N1 ; // i is perpendicular to newNormal
	else
		T = N1 ^ VEC3(1,2,3) ; // second random vector
	T.normalize() ;

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

	double alpha = fabs(alpha1 + alpha2) ;

	// get coefs of v0 and v1
	std::vector<VEC3> coefs1, coefs2, coefs ;
	coefs1.resize(m_K) ; coefs2.resize(m_K) ;
	for (unsigned int i = 0 ; i < m_K ; ++i)
	{
		coefs1[i] = this->m_approx[m_approxindex_HF[i]]->getAttr(m_attrindex_HF[i])[v0] ;
		coefs2[i] = this->m_approx[m_approxindex_HF[i]]->getAttr(m_attrindex_HF[i])[v1] ;
	}

	Utils::QuadricHF<REAL> q(coefs2, gamma2, alpha2) ;
	bool opt = q.findOptimizedCoefs(coefs) ; // coefs of d1's lightfield rotated around new local axis
	q += Utils::QuadricHF<REAL>(coefs1, gamma1, alpha1) ;

	if (!opt)
	{
		std::cerr << "HalfEdgeSelector_LFexperimental::Optimization failed (should never happen since no optim is done)" << std::endl ;
		std::cout << alpha2 << std::endl ;
	}

	const VEC3 avgColDiff = m_avgColor[v0] - m_avgColor[v1] ;

	REAL err = q(coefs) ;
	if (fabs(err) < 1e-6)
		err = 0 ;

	return (alpha / M_PI) * avgColDiff.norm2()/3. + err ;
}

/************************************************************************************
 *                           LIGHTFIELD GRADIENT                                    *
 ************************************************************************************

template <typename PFP>
bool
HalfEdgeSelector_LFgradient<PFP>::init()
{
	MAP& m = this->m_map ;

	// Verify availability of required approximators
	unsigned int ok = 0 ;
	unsigned int k = 0 ;
	for (unsigned int approxindex = 0 ; approxindex < this->m_approximators.size() ; ++approxindex)
	{
		if (this->m_approximators[approxindex]->getType() != A_hHalfCollapse)
		{
			std::cerr << "Approximator for selector (HalfEdgeSelector_LFexperimental) must be of A_hHalfCollapse" << std::endl ;
			return false ;
		}
		assert(this->m_approximators[approxindex]->getType() == A_hHalfCollapse
				|| !"Approximator for selector (HalfEdgeSelector_LFexperimental) must be of A_hHalfCollapse") ;

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
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3, DART>* >(this->m_approximators[approxindex])) ;
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
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3, DART>* >(this->m_approximators[approxindex])) ;
					assert(this->m_approx[approxindex]->getAttr(attrindex).isValid() || !"HalfEdgeSelector_LFexperimental: frameT attribute is not valid") ;
					saved = true ;
				}
			}
			else if(ok == 2 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "frameB")
			{
				++ok ;
				m_approxindex_FB = approxindex ;
				m_attrindex_FB = attrindex ;
				assert(this->m_approx[approxindex]->getAttr(attrindex).isValid() || !"HalfEdgeSelector_LFexperimental: frameB attribute is not valid") ;
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
				assert(this->m_approx[approxindex]->getAttr(attrindex).isValid() || !"HalfEdgeSelector_LFexperimental: frameN attribute is not valid") ;
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
					if (!saved)
					{
						m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3, DART>* >(this->m_approximators[approxindex])) ;
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

	if(ok <= 6)
	{
		std::cerr << "HalfEdgeSelector_LFgradient<PFP>::init() not OK" << std::endl ;
		return false ;
	}

	unsigned int ruleId = m_K <= 6 ? 17 : (m_K <= 10) ? 23 : 29 ;
	//ruleId = 29 ;
	m_integrator.Init(ruleId) ;

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

	// Init multimap for each Half-edge
	halfEdges.clear() ;

	for(Dart d = m.begin() ; d != m.end() ; m.next(d))
	{
		initHalfEdgeInfo(d) ; // init the edges with their optimal position
	}						// and insert them in the multimap according to their error

	cur = halfEdges.begin() ; 	// init the current edge to the first one

	return true ;
}

template <typename PFP>
bool
HalfEdgeSelector_LFgradient<PFP>::nextEdge(Dart& d)
{
	if(cur == halfEdges.end() || halfEdges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void
HalfEdgeSelector_LFgradient<PFP>::updateBeforeCollapse(Dart d)
{
	MAP& m = this->m_map ;

	const Dart& v0 = m.phi1(d) ;

	Traversor2VVaE<MAP> tv(m,v0) ;
	for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
	{
		Traversor2VE<MAP> te(m,v) ;
		for (Dart he = te.begin() ; he != te.end() ; he = te.next())
		{
			HalfEdgeInfo* edgeE = &(halfEdgeInfo[he]) ;
			if(edgeE->valid)
			{
				edgeE->valid = false ;
				halfEdges.erase(edgeE->it) ;
			}
			Dart de = m.phi2(he) ;
			edgeE = &(halfEdgeInfo[de]) ;
			if(edgeE->valid)
			{
				edgeE->valid = false ;
				halfEdges.erase(edgeE->it) ;
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
void
HalfEdgeSelector_LFgradient<PFP>::recomputeQuadric(const Dart d)
{
	Dart dFront,dBack ;
	Dart dInit = d ;

	// Init Front
	dFront = dInit ;

	m_quadric[d].zero() ;

	do {
		// Make step
		dBack = this->m_map.phi1(dFront) ;
		dFront = this->m_map.phi2_1(dFront) ;

		if (this->m_map.phi2(dFront) != dFront) { // if dFront is no border
			m_quadric[d] += Utils::Quadric<REAL>(this->m_position[d],this->m_position[dBack],this->m_position[this->m_map.phi1(dFront)]) ;
		}
	} while(dFront != dInit) ;
}

template <typename PFP>
void
HalfEdgeSelector_LFgradient<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	MAP& m = this->m_map ;

	const Dart& v1 = d2 ;

	recomputeQuadric(v1) ;
	Traversor2VVaE<MAP> tv(m,v1) ;
	for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
	{
		recomputeQuadric(v) ;
	}

	for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
	{
		Traversor2VE<MAP> te(m,v) ;
		for (Dart e = te.begin() ; e != te.end() ; e = te.next())
		{
			updateHalfEdgeInfo(e) ;
			updateHalfEdgeInfo(m.phi2(e)) ;
		}
	}

	cur = halfEdges.begin() ; // set the current edge to the first one
}

template <typename PFP>
void
HalfEdgeSelector_LFgradient<PFP>::initHalfEdgeInfo(Dart d)
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
void
HalfEdgeSelector_LFgradient<PFP>::updateHalfEdgeInfo(Dart d)
{
	MAP& m = this->m_map ;
	HalfEdgeInfo& heinfo = halfEdgeInfo[d] ;

	if(!heinfo.valid && m.edgeCanCollapse(d))
		computeHalfEdgeInfo(d, heinfo) ;
}

template <typename PFP>
void
HalfEdgeSelector_LFgradient<PFP>::computeHalfEdgeInfo(Dart d, HalfEdgeInfo& heinfo)
{
	MAP& m = this->m_map ;
	Dart dd = m.phi1(d) ;

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

	// Get all approximated attributes
	// New position
	const VEC3& newPos = this->m_approx[m_approxindex_pos]->getApprox(d,m_attrindex_pos) ; // get newPos

	// WARNING : in the following, we consider the half-edge contraction v0 --> v1
	const Dart& v0 = dd ;
	const Dart& v1 = d ;
	assert(newPos == this->m_position[v1]) ;

	// Compute errors
	// Position
	Utils::Quadric<REAL> quadGeom ;
	quadGeom += m_quadric[d] ;	// compute the sum of the
	quadGeom += m_quadric[dd] ;	// two vertices quadrics

	// sum of QEM metric and LF
	const REAL t = 0.01 ;
	const REAL err =
			t*quadGeom(newPos) + // geom
			(1-t)*computeDirDerivativeLFerror(v0,v1).norm()/sqrt(3) // lf
			//(1-t)*computeGradientLFerror(v0,v1).norm()/sqrt(3) // lf
			;
	//std::cout << computeLightfieldError(v0,v1) / quadGeom(newPos) << std::endl ;

	// Check if errated values appear
	if (err < -1e-6)
		heinfo.valid = false ;
	else
	{
		heinfo.it = this->halfEdges.insert(std::make_pair(std::max(err,REAL(0)), d)) ;
		heinfo.valid = true ;
	}
}

template <typename PFP>
typename PFP::VEC3
HalfEdgeSelector_LFgradient<PFP>::computeGradientLFerror(const Dart& v0, const Dart& v1) const
{
	MAP& m = this->m_map ;

	const VEC3& P0 = this->m_approx[m_approxindex_pos]->getAttr(m_attrindex_pos)[v0] ;
	const VEC3& P1 = this->m_approx[m_approxindex_pos]->getAttr(m_attrindex_pos)[v1] ;
	const VEC3& N = this->m_approx[m_approxindex_FN]->getApprox(v1,m_attrindex_FN) ;

	const VEC3& T1 = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[v1] ;
	const VEC3& B1 = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[v1] ;
	const VEC3& N1 = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[v1] ;
	VEC3 coefs1[m_K] ;
	for (unsigned int k = 0 ; k < m_K ; ++k)
	{
		coefs1[k] = this->m_approx[m_approxindex_HF[k]]->getAttr(m_attrindex_HF[k])[v1] ;
	}

	Traversor2VF<MAP> tf(m,v0) ; // all faces around vertex v0

	VEC3 count ;
	REAL areaSum = 0 ;
	const VEC3 d = P1 - P0 ; // displacement vector

	for (Dart fi = tf.begin() ; fi != tf.end() ; fi = tf.next()) // foreach "blue" face
	{
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
		const VEC3 ei = P0 - Pj ;
		const VEC3 ej = Pi - P0 ;
		//const VEC3 e0 = Pj - Pi ;

		const REAL areaIJ0sq = (ei ^ ej).norm2() ;
		const REAL areaIJ0 = sqrt(areaIJ0sq)/2. ;
		areaSum += areaIJ0 ;

		const VEC3 displacementE = integrateDscalGrad(d, m_K, N, ei, ej,
													  coefs1, T1, B1, N1, m_avgColor[v1],
													  coefsi, Ti, Bi, Ni, m_avgColor[vi],
													  coefsj, Tj, Bj, Nj, m_avgColor[vj]) ;

		count += displacementE * areaIJ0 ;
	}

	const VEC3 lfDiff = computeSquaredLightfieldDifferenceNumerical(v0,v1,N) ;

	return lfDiff*areaSum + count ;
}

template <typename PFP>
typename PFP::VEC3
HalfEdgeSelector_LFgradient<PFP>::computeDirDerivativeLFerror(const Dart& v0, const Dart& v1)
{
	MAP& m = this->m_map ;

	const VEC3& P0 = this->m_approx[m_approxindex_pos]->getAttr(m_attrindex_pos)[v0] ;
	const VEC3& P1 = this->m_approx[m_approxindex_pos]->getAttr(m_attrindex_pos)[v1] ;
	const VEC3& N = this->m_approx[m_approxindex_FN]->getApprox(v1,m_attrindex_FN) ;

	const VEC3& T1 = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[v1] ;
	const VEC3& B1 = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[v1] ;
	const VEC3& N1 = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[v1] ;
	const VEC3& T0 = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[v0] ;
	const VEC3& B0 = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[v0] ;
	const VEC3& N0 = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[v0] ;
	std::vector<VEC3*> coefs0, coefs1 ;
	coefs0.resize(m_K) ;
	coefs1.resize(m_K) ;
	for (unsigned int k = 0 ; k < m_K ; ++k)
	{
		coefs0[k] = &(this->m_approx[m_approxindex_HF[k]]->getAttr(m_attrindex_HF[k])[v0]) ;
		coefs1[k] = &(this->m_approx[m_approxindex_HF[k]]->getAttr(m_attrindex_HF[k])[v1]) ;
	}

	Traversor2VF<MAP> tf(m,v0) ; // all faces around vertex v0

	VEC3 error ;
	const VEC3 d = P1 - P0 ; // displacement vector

	for (Dart fi = tf.begin() ; fi != tf.end() ; fi = tf.next()) // foreach "blue" face
	{
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
		std::vector<VEC3*> coefsi, coefsj ;
		coefsi.resize(m_K) ;
		coefsj.resize(m_K) ;
		//VEC3 coefsi[m_K], coefsj[m_K] ;
		for (unsigned int k = 0 ; k < m_K ; ++k)
		{
			coefsi[k] = &(this->m_approx[m_approxindex_HF[k]]->getAttr(m_attrindex_HF[k])[vi]) ;
			coefsj[k] = &(this->m_approx[m_approxindex_HF[k]]->getAttr(m_attrindex_HF[k])[vj]) ;
		}

		// utils
		const VEC3 ei = P1 - Pj ;
		const VEC3 ej = Pi - P1 ;

		// area x integration
		error += (ei ^ ej).norm()/2 * integrateDlf(d, m_K, N, ei, ej,
													  coefs0, T0, B0, N0, m_avgColor[v0],
													  coefs1, T1, B1, N1, m_avgColor[v1],
													  coefsi, Ti, Bi, Ni, m_avgColor[vi],
													  coefsj, Tj, Bj, Nj, m_avgColor[vj]) ;
	}

	return error ;
}

template <typename PFP>
typename PFP::VEC3
HalfEdgeSelector_LFgradient<PFP>::computeSquaredLightfieldDifferenceNumerical(const Dart& v0, const Dart& v1, const VEC3& N) const
{
	// get two frames
	const VEC3& T1 = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[v0] ;
	const VEC3& T2 = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[v1] ;
	const VEC3& B1 = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[v0] ;
	const VEC3& B2 = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[v1] ;
	const VEC3& N1 = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[v0] ;
	const VEC3& N2 = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[v1] ;

	// get coefs of v1 and v2
	std::vector<VEC3> coefs1, coefs2 ;
	coefs1.resize(m_K) ; coefs2.resize(m_K) ;
	for (unsigned int i = 0 ; i < m_K ; ++i)
	{
		coefs1[i] = this->m_approx[m_approxindex_HF[i]]->getAttr(m_attrindex_HF[i])[v0] ;
		coefs2[i] = this->m_approx[m_approxindex_HF[i]]->getAttr(m_attrindex_HF[i])[v1] ;
	}

	// fill workspace
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[i] = double(N1[i]) ; // n
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[4+i] = double(T1[i]) ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[7+i] = double(B1[i]) ;
	m_workspace[10] = double(m_K) ;

	// fill workspace 2
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[11+m_K+i] = double(N2[i]) ; // n
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[15+m_K+i] = double(T2[i]) ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[18+m_K+i] = double(B2[i]) ;
	m_workspace[21+m_K] = double(m_K) ;

	// Get new frame
	// precondition : this->m_approx[m_approxindex_FN]->approximate should have been called !
	//const VEC3& N = this->m_approx[m_approxindex_FN]->getApprox(v1,m_attrindex_FN) ; // get new N
	//assert(N == N2 || !"HalfEdgeSelector_LFgradient<PFP>::computeSquaredLightfieldDifferenceNumerical: only works with half-collapse") ;
	m_n[0] = N[0] ;
	m_n[1] = N[1] ;
	m_n[2] = N[2] ;

	// call integral function

	double integ, ar ;
	VEC3 integral ;
	VEC3 area ;
	for (unsigned int channel = 0 ; channel < 3 ; ++channel)
	{
		m_workspace[3] = m_avgColor[v0][channel];
		m_workspace[14+m_K] = m_avgColor[v1][channel];
		for (unsigned int i = 0 ; i < m_K ; ++i)
		{
			m_workspace[11+i] = coefs1[i][channel] ;
			m_workspace[22+m_K+i] = coefs2[i][channel] ;
		}
		m_integrator.Compute(&integ, &ar, &SquaredDifferenceOfCartesianFunctions, m_workspace, &isInDomain, m_n) ;
		integral[channel] = integ/(4*M_PI) ;
		area[channel] = ar ;
	}

	return integral ;
}

template <typename PFP>
typename PFP::VEC3
HalfEdgeSelector_LFgradient<PFP>::computeSquaredLightfieldDifferenceAnalytical(const Dart& v0, const Dart& v1) const
{
	// get two frames
	const VEC3& T1 = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[v0] ;
	const VEC3& T2 = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[v1] ;
	const VEC3& B1 = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[v0] ;
	const VEC3& B2 = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[v1] ;
	const VEC3& N1 = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[v0] ;
	const VEC3& N2 = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[v1] ;

	// Get new frame
	// precondition : this->m_approx[m_approxindex_FN]->approximate should have been called !
	const VEC3& N = this->m_approx[m_approxindex_FN]->getApprox(v1,m_attrindex_FN) ; // get new N
	assert(N == N2 || !"HalfEdgeSelector_LFgradient<PFP>::computeSquaredLightfieldDifferenceAnalytical: only works with half-collapse") ;

	// compute new frame
	VEC3 T ;
	if (N2 != N1)
		T = N2 ^ N1 ; // i is perpendicular to newNormal
	else
		T = N1 ^ VEC3(1,2,3) ; // second random vector
	T.normalize() ;

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

	double alpha = fabs(alpha1 + alpha2) ;

	// get coefs of v1 and v2
	std::vector<VEC3> coefs1, coefs2, coefs ;
	coefs1.resize(m_K) ; coefs2.resize(m_K) ;
	for (unsigned int i = 0 ; i < m_K ; ++i)
	{
		coefs1[i] = this->m_approx[m_approxindex_HF[i]]->getAttr(m_attrindex_HF[i])[v0] ;
		coefs2[i] = this->m_approx[m_approxindex_HF[i]]->getAttr(m_attrindex_HF[i])[v1] ;
	}

	Utils::QuadricHF<REAL> q(coefs2, gamma2, alpha2) ;
	bool opt = q.findOptimizedCoefs(coefs) ; // coefs of d1's lightfield rotated around new local axis
	q += Utils::QuadricHF<REAL>(coefs1, gamma1, alpha1) ;

	if (!opt)
	{
		std::cerr << "HalfEdgeSelector_LFgradient::Optimization failed (should never happen since no optim is done)" << std::endl ;
		std::cout << alpha1 << std::endl ;
	}

	VEC3 avgColDiff = m_avgColor[v0] - m_avgColor[v1] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		avgColDiff[i] = pow(avgColDiff[i],2) ;

	const VEC3 LFerr = q.evalR3(coefs) ;
	VEC3 err = (alpha / M_PI) * avgColDiff + LFerr ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		err[i] = std::max(REAL(0),err[i]) ;

	return err ;
}

template <typename PFP>
typename PFP::VEC3
HalfEdgeSelector_LFgradient<PFP>::computeGradient(const VEC3& P0, const VEC3& Pi, const VEC3& Pj, const Dart& v0, const Dart& v1, const Dart& vi, const Dart& vj, unsigned int channel) const
{
	const VEC3 ei = P0 - Pj ;
	const VEC3 ej = Pi - P0 ;

	// domaine sur lequel intégrer : hémisphère défini par n
	const VEC3& ni = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[vi] ;
	const VEC3& nj = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[vj] ;
	const VEC3& n0 = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[v0] ;
	VEC3 normal = (ni+nj+n0)/3. ;	normal.normalize() ;

	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_n[i] = normal[i] ;
	std::vector<Dart> vertices ;	vertices.resize(3) ;
	vertices[0] = vi ;
	vertices[1] = vj ;
	vertices[2] = v1 ;

	std::vector<double> coefs ;	coefs.resize(m_K) ;
	VEC3 res ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
	{
		const Dart& d = vertices[i] ;
		const VEC3& t = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[d] ;
		const VEC3& b = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[d] ;
		const VEC3& n_d = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[d] ;

		double avgCol = m_avgColor[d][channel] ;
		for (unsigned int k = 0 ; k < m_K ; ++k)
		{
			coefs[k] = this->m_approx[m_approxindex_HF[k]]->getAttr(m_attrindex_HF[k])[d][channel] ;
		}
		res[i] = computeIntegral(&avgCol, t, b, n_d, m_K, coefs) ;
	}

	const REAL& ci = res[0] ;
	const REAL& cj = res[1] ;
	const REAL& c1 = res[2] ;

	return (ei.norm2()*fabs(ci-c1) + (ei*ej)*fabs(cj-c1))*ej - (ej.norm2()*fabs(cj-c1) + (ei*ej)*fabs(ci-c1))*ei ;
}

template <typename PFP>
typename PFP::VEC3
HalfEdgeSelector_LFgradient<PFP>::integrateDscalGrad(const VEC3& d, const unsigned int& K, const VEC3& N, const VEC3& ei, const VEC3& ej,
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
	m_workspace[11] = avg1[0] ;
	m_workspace[12] = avgi[0] ;
	m_workspace[13] = avgj[0] ;
	m_workspace[14] = avg1[1] ;
	m_workspace[15] = avgi[1] ;
	m_workspace[16] = avgj[1] ;
	m_workspace[17] = avg1[2] ;
	m_workspace[18] = avgi[2] ;
	m_workspace[19] = avgj[2] ;

	// N1, Ni, Nj
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[20+i] = N1[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[23+i] = Ni[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[26+i] = Nj[i] ;
	// T1, Ti, Tj
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[29+i] = T1[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[32+i] = Ti[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[35+i] = Tj[i] ;
	// B1, Bi, Bj
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[38+i] = B1[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[41+i] = Bi[i] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[44+i] = Bj[i] ;

	// coefs1, coefsi, coefsj
	for (unsigned int c = 0 ; c < 3 ; ++c)
		for (unsigned int k = 0 ; k < K ; ++k)
		{
			m_workspace[47+c*K+k] = coefs1[k][c] ;
			m_workspace[47+(3+c)*K+k] = coefsi[k][c] ;
			m_workspace[47+(6+c)*K+k] = coefsj[k][c] ;
		}

	VEC3 res ;
	for (unsigned int c = 0 ; c < 3 ; ++c)
	{
		m_workspace[10] = c ;

		// exec integrator
		double integral, area ;
		m_integrator.Compute(&integral, &area, &dispScalGrad, m_workspace, &isInDomain, m_n) ;
		res[c] = integral/(4*M_PI) ;
	}

	return res ;
}

template <typename PFP>
typename PFP::VEC3
HalfEdgeSelector_LFgradient<PFP>::integrateDlf(const VEC3& d, const unsigned int& K, const VEC3& N, const VEC3& ei, const VEC3& ej,
												   const std::vector<VEC3*> coefs0, const VEC3& T0, const VEC3& B0, const VEC3& N0, const VEC3& avg0,
												   const std::vector<VEC3*> coefs1, const VEC3& T1, const VEC3& B1, const VEC3& N1, const VEC3& avg1,
												   const std::vector<VEC3*> coefsi, const VEC3& Ti, const VEC3& Bi, const VEC3& Ni, const VEC3& avgi,
												   const std::vector<VEC3*> coefsj, const VEC3& Tj, const VEC3& Bj, const VEC3& Nj, const VEC3& avgj) const
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
			m_workspace[59+c*K+k] = coefs0[k]->operator[](c) ;
			m_workspace[59+(3+c)*K+k] = coefs1[k]->operator[](c) ;
			m_workspace[59+(6+c)*K+k] = coefsi[k]->operator[](c) ;
			m_workspace[59+(9+c)*K+k] = coefsj[k]->operator[](c) ;
		}

	VEC3 res ;
	for (unsigned int c = 0 ; c < 3 ; ++c)
	{
		m_workspace[10] = c ;

		// exec integrator
		double integral, area ;
		m_integrator.Compute(&integral, &area, &dlf, m_workspace, &isInDomain, m_n) ;
		res[c] = integral/(4*M_PI) ;
	}

	return res ;
}

template <typename PFP>
typename PFP::REAL
HalfEdgeSelector_LFgradient<PFP>::computeIntegral(const double *avgi, const VEC3& ti, const VEC3& bi, const VEC3& ni, unsigned int nbCoefs, const std::vector<double>& coefs) const
{
	// n is filled

	// fill workspace
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[i] = double(ni[i]) ; // n
	m_workspace[3] = *avgi ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[4+i] = double(ti[i]) ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		m_workspace[7+i] = double(bi[i]) ;
	m_workspace[10] = double(nbCoefs) ;
	for (unsigned int i = 0 ; i < nbCoefs ; ++i)
		m_workspace[11+i] = coefs[i] ;

	// call integral function
	double integral ;
	double area ;
	m_integrator.Compute(&integral, &area, &CartesianFunction, m_workspace, &isInDomain, m_n) ;

	return integral*4*M_PI ;
}

// fonction qui retourne bool est_dans_domaine en fonction de f et omega
template <typename PFP>
bool
HalfEdgeSelector_LFgradient<PFP>::isInDomain(double x, double y, double z, void *data)
{
	// (x,y,z) vecteur de vue
	// n vecteur normal
	double *n = (double*)data ;

	// vrai ssi n*(x,y,z) > 0
	return n[0]*x+n[1]*y+n[2]*z >= 0 ;
}

// fonction qui retourne col en fonction de f et de omega
template <typename PFP>
double
HalfEdgeSelector_LFgradient<PFP>::CartesianFunction (double x, double y, double z, void* data)
{
	// (x,y,z) vecteur de vue
	// data = {n,avgCol,t,b,nbCoefs,coefs}
	double *myData = (double*)data ;

	// normale
	if (myData[0]*x + myData[1]*y + myData[2]*z < 0)
	{
		return myData[3] ;
	}

	// tangente
	double u = myData[4]*x + myData[5]*y + myData[6]*z ;

	// binormale
	double v = myData[7]*x + myData[8]*y + myData[9]*z ;

	// nbcoefs
	unsigned int nb = (unsigned int)(myData[10]) ;

	// eval en ((xyz)*t,(xyz)*b)
	double res = 0 ;
	if (nb > 0)
	{
		// coefs
		double *coefs = &(myData[11]) ;
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

template <typename PFP>
double
HalfEdgeSelector_LFgradient<PFP>::dispScalGrad (double x, double y, double z, void* data)
{
	double *myData = (double*)data ;

	// decode workspace
	double *d = &(myData[0]) ;
	unsigned int K = myData[3] ;
	double *ei = &(myData[4]) ;
	double *ej = &(myData[7]) ;
	unsigned int channel = myData[10] ;
	double *avg = &(myData[11+3*channel]) ;
	// (11=avg1_c0,avgI_c0,avgJ_c0,avg1_c1,avgI_c2,avgJ_c3,avg1_c1,avgI_c2,avgJ_c3)
	double *N1 = &(myData[20]) ;
	double *Ni = &(myData[23]) ;
	double *Nj = &(myData[26]) ;
	double *T1 = &(myData[29]) ;
	double *Ti = &(myData[32]) ;
	double *Tj = &(myData[35]) ;
	double *B1 = &(myData[38]) ;
	double *Bi = &(myData[41]) ;
	double *Bj = &(myData[44]) ;
	double *coefs1 = &(myData[47+K*channel]) ; // (coefs1_c0, ... coefsK_c0,coefs1_c1, ... coefsK_c1, coefs1_c2, coefsK_c2)
	double *coefsi = &(myData[47+3*K+K*channel]) ;
	double *coefsj = &(myData[47+6*K+K*channel]) ;

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

	double f1 = evalF(N1,&(avg[0]),K,T1,B1,coefs1,x,y,z) ;
	double fi = evalF(Ni,&(avg[1]),K,Ti,Bi,coefsi,x,y,z) ;
	double fj = evalF(Nj,&(avg[2]),K,Tj,Bj,coefsj,x,y,z) ;

	double t1 = einorm2 * (fi-f1) + eiej*(fj-f1) ;
	double t2 = ejnorm2 * (fj-f1) + eiej*(fi-f1) ;

	double grad[3] ;
	for (unsigned int i = 0 ; i < 3 ; ++i)
		grad[i] = (t1*ej[i] - t2*ei[i]) / areaSq ;

	return pow(d[0]*grad[0] + d[1]*grad[1] + d[2]*grad[2],2) ;
}

template <typename PFP>
double
HalfEdgeSelector_LFgradient<PFP>::dlf (double x, double y, double z, void* data)
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
HalfEdgeSelector_LFgradient<PFP>::evalF(double* N, double* avg, unsigned int nb, double* T, double* B, double* coefs, double& x, double& y, double& z)
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

// fonction qui retourne col en fonction de f et de omega
template <typename PFP>
double
HalfEdgeSelector_LFgradient<PFP>::SquaredDifferenceOfCartesianFunctions (double x, double y, double z, void* data)
{
	// (x,y,z) vecteur de vue
	// data = {n1,avgCol1,t1,b1,nbCoefs1,coefs1, n2,avgCol2,t2,b2,nbCoefs2,coefs2}
	double *myData1 = (double*)data ;
	unsigned int nb = (unsigned int)(myData1[10]) ;
	double *myData2 = &(myData1[11+nb]) ;

	return pow(CartesianFunction(x,y,z,myData1) - CartesianFunction(x,y,z,myData2),2) ;
}

/************************************************************************************
 *                           COLOR PER FACE                                         *
 ************************************************************************************

template <typename PFP>
bool HalfEdgeSelector_ColorPerFace<PFP>::init()
{
	MAP& m = this->m_map ;


	// Verify availability of required approximators
	unsigned int ok = 0 ;
	for (unsigned int approxindex = 0 ; approxindex < this->m_approximators.size() ; ++approxindex)
	{
		assert(this->m_approximators[approxindex]->getType() == A_hQEM
				|| this->m_approximators[approxindex]->getType() == A_hHalfCollapse
				|| !"Approximator for selector (HalfEdgeSelector_ColorExperimental) must be of a half-edge approximator") ;

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
		}
	}

	m_facecolor = m.template getAttribute<typename PFP::VEC3, FACE>("color") ;
	assert(m_facecolor.isValid() || !"EdgeSelector_QEMextColor: color attribute is not valid") ;

	if(ok != 1 || !m_facecolor.isValid())
			return false ;

	CellMarker<VERTEX> vMark(m) ;
	for(Dart d = m.begin(); d != m.end(); m.next(d))
	{
		if(!vMark.isMarked(d))
		{
			Utils::Quadric<REAL> q ;	// create one quadric
			m_quadric[d] = q ;	// per vertex
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
			m_quadric[d] += q ;					// and add the contribution of
			m_quadric[d1] += q ;					// this quadric to the ones
			m_quadric[d_1] += q ;					// of the 3 incident vertices
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
bool HalfEdgeSelector_ColorPerFace<PFP>::nextEdge(Dart& d)
{
	if(cur == halfEdges.end() || halfEdges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void HalfEdgeSelector_ColorPerFace<PFP>::updateBeforeCollapse(Dart d)
{
	MAP& m = this->m_map ;

	const Dart& dd = m.phi2(d) ; // dart that will disappear after collapse

	// both darts of current edge become invalid
	HalfEdgeInfo* edgeE = &(halfEdgeInfo[d]) ;
	if(edgeE->valid)
	{
		edgeE->valid = false ;
		halfEdges.erase(edgeE->it) ;
	}
	edgeE = &(halfEdgeInfo[dd]) ;
	if(edgeE->valid)
	{
		edgeE->valid = false ;
		halfEdges.erase(edgeE->it) ;
	}

	Traversor2EF<MAP> tf(m,d) ; // for all triangles incident to this dart
	for (Dart f = tf.begin() ; f != tf.end() ; f = tf.next())
	{
		Traversor2FFaE<MAP> tff(m,f) ; // for all triangles incident to the current triangle by an edge
		for (Dart ff = tff.begin() ; ff != tff.end() ; ff = tff.next())
		{
			Traversor2FE<MAP> te(m,ff) ; // for all edges composing this triangle
			for (Dart e = te.begin() ; e != te.end() ; e = te.next())
			{
				// make both darts of the edge invalid
				edgeE = &(halfEdgeInfo[e]) ;
				if(edgeE->valid)
				{
					edgeE->valid = false ;
					halfEdges.erase(edgeE->it) ;
				}
				edgeE = &(halfEdgeInfo[m.phi2(e)]) ;
				if(edgeE->valid)
				{
					edgeE->valid = false ;
					halfEdges.erase(edgeE->it) ;
				}

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
void HalfEdgeSelector_ColorPerFace<PFP>::recomputeQuadric(const Dart d)
{
	Dart dFront,dBack ;
	Dart dInit = d ;

	// Init Front
	dFront = dInit ;

	m_quadric[d].zero() ;

	do {
		// Make step
		dBack = this->m_map.phi1(dFront) ;
		dFront = this->m_map.phi2_1(dFront) ;

		if (this->m_map.phi2(dFront) != dFront) { // if dFront is no border
			m_quadric[d] += Utils::Quadric<REAL>(this->m_position[d],this->m_position[dBack],this->m_position[this->m_map.phi1(dFront)]) ;
		}
	} while(dFront != dInit) ;
}

template <typename PFP>
void HalfEdgeSelector_ColorPerFace<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	MAP& m = this->m_map ;

	const Dart& v1 = d2 ;

	recomputeQuadric(v1) ;
	Traversor2VVaE<MAP> tv(m,v1) ;
	for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
	{
		recomputeQuadric(v) ;
	}

	Traversor2EF<MAP> tt(m,d2) ; // for all triangles incident to the edge d2
	for (Dart t = tt.begin() ; t != tt.end() ; t = tt.next())
	{
		Traversor2FE<MAP> te(m,t) ;	// for all edges incident to these triangles
		for (Dart e = te.begin() ; e != te.end() ; e = te.next())
		{
			// recompute error
			updateHalfEdgeInfo(e) ;
			updateHalfEdgeInfo(m.phi2(e)) ;
		}
	}

	Traversor2EF<MAP> tt2(m,dd2) ; // for all triangles incident to the edge dd2
	for (Dart t = tt2.begin() ; t != tt2.end() ; t = tt2.next())
	{
		Traversor2FE<MAP> te(m,t) ;	// for all edges incident to these triangles
		for (Dart e = te.begin() ; e != te.end() ; e = te.next())
		{
			// recompute error
			updateHalfEdgeInfo(e) ;
			updateHalfEdgeInfo(m.phi2(e)) ;
		}
	}

	cur = halfEdges.begin() ; // set the current edge to the first one
}

template <typename PFP>
void HalfEdgeSelector_ColorPerFace<PFP>::initHalfEdgeInfo(Dart d)
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
void HalfEdgeSelector_ColorPerFace<PFP>::updateHalfEdgeInfo(Dart d)
{
	MAP& m = this->m_map ;
	HalfEdgeInfo& heinfo = halfEdgeInfo[d] ;

	if(!heinfo.valid && m.edgeCanCollapse(d))
		computeHalfEdgeInfo(d, heinfo) ;
}

template <typename PFP>
void HalfEdgeSelector_ColorPerFace<PFP>::computeHalfEdgeInfo(Dart d, HalfEdgeInfo& heinfo)
{
	MAP& m = this->m_map ;
	Dart dd = m.phi1(d) ;

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

	// Get all approximated attributes
	// New position
	const VEC3& newPos = this->m_approx[m_approxindex_pos]->getApprox(d,m_attrindex_pos) ; // get newPos

	assert(newPos == m_pos[d]) ;

	// Compute errors
	// Position
	Utils::Quadric<REAL> quadGeom ;
	quadGeom += m_quadric[d] ;	// compute the sum of the
	quadGeom += m_quadric[dd] ;	// two vertices quadrics

	// sum of QEM metric and frame orientation difference
	const Dart& f0 = d ;
	const Dart& f1 = m.phi2(d) ;
	const REAL& err =
			100*quadGeom(newPos) + // geom
			computeFaceColorError(f0,f1) // color
			;
	//std::cout << computeFaceColorError(f0,f1) << std::endl ;

	// Check if errated values appear
	if (err < -1e-6)
		heinfo.valid = false ;
	else
	{
		heinfo.it = this->halfEdges.insert(std::make_pair(std::max(err,REAL(0)), d)) ;
		heinfo.valid = true ;
	}
}

template <typename PFP>
typename PFP::REAL
HalfEdgeSelector_ColorPerFace<PFP>::computeFaceColorError(const Dart& f0, const Dart& f1)
{
	MAP& m = this->m_map ;

	REAL resF0 = 0, resF1 = 0 ;

	// distance f0 to its neighboring faces
	Traversor2FFaE<MAP> tf0(m,f0) ; // all faces around face f0
	for (Dart fi = tf0.begin() ; fi != tf0.end() ; fi = tf0.next())
	{
		// except f1
		if (!m.sameFace(fi,f1))
			resF0 += (m_facecolor[fi] - m_facecolor[f0]).norm() / sqrt(3) ;
	}

	// distance f1 to its neighboring faces
	Traversor2FFaE<MAP> tf1(m,f1) ; // all faces around face f1
	for (Dart fi = tf1.begin() ; fi != tf1.end() ; fi = tf1.next())
	{
		// except f0
		if (!m.sameFace(fi,f0))
			resF1 += (m_facecolor[fi] - m_facecolor[f1]).norm() / sqrt(3) ;
	}

	if (resF0 > resF1)
	{
		return 99 ;
	}

	return (resF0 + resF1) / 4. ;
}

/************************************************************************************
 *                           LIGHTFIELD PER FACE                                    *
 ************************************************************************************

template <typename PFP>
bool HalfEdgeSelector_LFperFace<PFP>::init()
{
	MAP& m = this->m_map ;

	// Verify availability of required approximators
	unsigned int ok = 0 ;
	unsigned int k = 0 ;
	for (unsigned int approxindex = 0 ; approxindex < this->m_approximators.size() ; ++approxindex)
	{
		if (this->m_approximators[approxindex]->getType() != A_hHalfCollapse)
		{
			std::cerr << "Approximator for selector (HalfEdgeSelector_LFexperimental) must be of A_hHalfCollapse" << std::endl ;
			return false ;
		}
		assert(this->m_approximators[approxindex]->getType() == A_hHalfCollapse
				|| !"Approximator for selector (HalfEdgeSelector_LFexperimental) must be of A_hHalfCollapse") ;

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
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3, DART>* >(this->m_approximators[approxindex])) ;
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
					m_approx.push_back(reinterpret_cast<Approximator<PFP, VEC3, DART>* >(this->m_approximators[approxindex])) ;
					assert(this->m_approx[approxindex]->getAttr(attrindex).isValid() || !"HalfEdgeSelector_LFexperimental: frameT attribute is not valid") ;
					saved = true ;
				}
			}
			else if(ok == 2 && this->m_approximators[approxindex]->getApproximatedAttributeName(attrindex) == "frameB")
			{
				++ok ;
				m_approxindex_FB = approxindex ;
				m_attrindex_FB = attrindex ;
				assert(this->m_approx[approxindex]->getAttr(attrindex).isValid() || !"HalfEdgeSelector_LFexperimental: frameB attribute is not valid") ;
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
				assert(this->m_approx[approxindex]->getAttr(attrindex).isValid() || !"HalfEdgeSelector_LFexperimental: frameN attribute is not valid") ;
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
					if (this->m_approx[approxindex]->getAttr(attrindex).isValid())
					{
						++k ;
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
	}
	m_K = k ;

	if(ok < 6)
	{
		std::cerr << "HalfEdgeSelector_LFexperimental<PFP>::init() not OK" << std::endl ;
		return false ;
	}

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

	// Init multimap for each Half-edge
	halfEdges.clear() ;

	for(Dart d = m.begin() ; d != m.end() ; m.next(d))
	{
		initHalfEdgeInfo(d) ; // init the edges with their optimal position
	}						// and insert them in the multimap according to their error

	cur = halfEdges.begin() ; 	// init the current edge to the first one

	return true ;
}

template <typename PFP>
bool HalfEdgeSelector_LFperFace<PFP>::nextEdge(Dart& d)
{
	if(cur == halfEdges.end() || halfEdges.empty())
		return false ;
	d = (*cur).second ;
	return true ;
}

template <typename PFP>
void HalfEdgeSelector_LFperFace<PFP>::updateBeforeCollapse(Dart d)
{
	MAP& m = this->m_map ;

	const Dart& v0 = m.phi1(d) ;

	Traversor2VVaE<MAP> tv(m,v0) ;
	for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
	{
		Traversor2VE<MAP> te(m,v) ;
		for (Dart he = te.begin() ; he != te.end() ; he = te.next())
		{
			HalfEdgeInfo* edgeE = &(halfEdgeInfo[he]) ;
			if(edgeE->valid)
			{
				edgeE->valid = false ;
				halfEdges.erase(edgeE->it) ;
			}
			Dart de = m.phi2(he) ;
			edgeE = &(halfEdgeInfo[de]) ;
			if(edgeE->valid)
			{
				edgeE->valid = false ;
				halfEdges.erase(edgeE->it) ;
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
void HalfEdgeSelector_LFperFace<PFP>::recomputeQuadric(const Dart d)
{
	Dart dFront,dBack ;
	Dart dInit = d ;

	// Init Front
	dFront = dInit ;

	m_quadric[d].zero() ;

	do {
		// Make step
		dBack = this->m_map.phi1(dFront) ;
		dFront = this->m_map.phi2_1(dFront) ;

		if (this->m_map.phi2(dFront) != dFront) { // if dFront is no border
			m_quadric[d] += Utils::Quadric<REAL>(this->m_position[d],this->m_position[dBack],this->m_position[this->m_map.phi1(dFront)]) ;
		}
	} while(dFront != dInit) ;
}

template <typename PFP>
void HalfEdgeSelector_LFperFace<PFP>::updateAfterCollapse(Dart d2, Dart dd2)
{
	MAP& m = this->m_map ;

	const Dart& v1 = d2 ;

	recomputeQuadric(v1) ;
	Traversor2VVaE<MAP> tv(m,v1) ;
	for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
	{
		recomputeQuadric(v) ;
	}

	for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
	{
		Traversor2VE<MAP> te(m,v) ;
		for (Dart e = te.begin() ; e != te.end() ; e = te.next())
		{
			updateHalfEdgeInfo(e) ;
			updateHalfEdgeInfo(m.phi2(e)) ;
		}
	}

	cur = halfEdges.begin() ; // set the current edge to the first one
}

template <typename PFP>
void HalfEdgeSelector_LFperFace<PFP>::initHalfEdgeInfo(Dart d)
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
void HalfEdgeSelector_LFperFace<PFP>::updateHalfEdgeInfo(Dart d)
{
	MAP& m = this->m_map ;
	HalfEdgeInfo& heinfo = halfEdgeInfo[d] ;

	if(!heinfo.valid && m.edgeCanCollapse(d))
		computeHalfEdgeInfo(d, heinfo) ;
}

template <typename PFP>
void HalfEdgeSelector_LFperFace<PFP>::computeHalfEdgeInfo(Dart d, HalfEdgeInfo& heinfo)
{
	MAP& m = this->m_map ;
	Dart dd = m.phi1(d) ;

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

	// Get all approximated attributes
	// New position
	const VEC3& newPos = this->m_approx[m_approxindex_pos]->getApprox(d,m_attrindex_pos) ; // get newPos

	const Dart& v0 = dd ;
	const Dart& v1 = d ;

	assert(newPos == this->m_position[v1]) ;

	// Compute errors
	// Position
	Utils::Quadric<REAL> quadGeom ;
	quadGeom += m_quadric[d] ;	// compute the sum of the
	quadGeom += m_quadric[dd] ;	// two vertices quadrics

	// sum of QEM metric
	const REAL& err =
			quadGeom(newPos) + // geom
			100*computeLightfieldError(v0,v1) // lf
			;
	//std::cout << computeLightfieldError(v0,v1) / quadGeom(newPos) << std::endl ;

	// Check if errated values appear
	if (err < -1e-6)
		heinfo.valid = false ;
	else
	{
		heinfo.it = this->halfEdges.insert(std::make_pair(std::max(err,REAL(0)), d)) ;
		heinfo.valid = true ;
	}
}

template <typename PFP>
typename PFP::REAL
HalfEdgeSelector_LFperFace<PFP>::computeLightfieldError(const Dart& v0, const Dart& v1) const
{
	MAP& m = this->m_map ;

	Traversor2VF<MAP> tf(m,v0) ;
	const unsigned int& v1_vertexId = m.template getEmbedding<VERTEX>(v1) ;

	const VEC3& P0 = this->m_position[v0] ;
	const VEC3& P1 = this->m_position[v1] ;

	REAL res1 = 0, res2 = 0 ;
	unsigned int count = 0 ;
	for (Dart fi = tf.begin() ; fi != tf.end() ; fi = tf.next())
	{
		const Dart& vi = m.phi1(fi) ;
		const Dart& vj = m.phi_1(fi) ;

		const VEC3& Pi = this->m_position[vi] ;
		const VEC3& Pj = this->m_position[vj] ;

		const VEC3 Pi_P0 = P0 - Pi ;
		//const VEC3 PjP0 = P0 - Pj ;
		const VEC3 Pi_Pj = Pj - Pi ;
		const REAL b = Pi_Pj.norm() ;

		const VEC3 P0proj2D = Pi + (Pi_P0 * Pi_Pj)*Pi_Pj ;
		const VEC3 P0_P0proj2D = P0proj2D - P0 ;
		const REAL h0 = P0_P0proj2D.norm() ;

		const REAL lfdiff01 = computeSquaredLightfieldDifference(v0,v1) ;
		res1 += lfdiff01 * b * h0 / (6.*sqrt(3)) ;

		// test if v1 is adjacent to this face
		bool adjacent = false ;
		Traversor2FV<MAP> tv(m,fi) ;
		for (Dart v = tv.begin() ; v != tv.end() ; v = tv.next())
		{
			if (v1_vertexId == m.template getEmbedding<VERTEX>(v))
				adjacent = true ;
		}

		// if v1 is not adjacent to this face
		if (!adjacent)
		{
			const VEC3 P1proj3D = P1 ;
			const VEC3 Pi_P1 = P1proj3D - Pi ;
			//const VEC3 PjP1 = newP1 - Pj ;

			const VEC3 P1proj2D = Pi + (Pi_P1 * Pi_Pj)*Pi_Pj ;
			const VEC3 P1proj2D_P1proj3D = P1proj2D - P1proj3D ;
			const REAL h1 = P1proj2D_P1proj3D.norm() ;

			const REAL lfdiff0i = computeSquaredLightfieldDifference(vi,v0) ;
			const REAL lfdiff1i = computeSquaredLightfieldDifference(vi,v1) ;
			const REAL lfdiff0j = computeSquaredLightfieldDifference(vj,v0) ;
			const REAL lfdiff1j = computeSquaredLightfieldDifference(vj,v1) ;

			res2 += (lfdiff0i + lfdiff0j + lfdiff1i + lfdiff1j) * b * std::fabs(h1 - h0) / (12.*sqrt(3)) ;
		}
		++count ;
	}

	return res1 / count + res2 / (count-2) ;
}

template <typename PFP>
typename PFP::REAL
HalfEdgeSelector_LFperFace<PFP>::computeSquaredLightfieldDifference(const Dart& d1, const Dart& d2) const
{
	// get two frames
	const VEC3& T1 = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[d1] ;
	const VEC3& T2 = this->m_approx[m_approxindex_FT]->getAttr(m_attrindex_FT)[d2] ;
	const VEC3& B1 = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[d1] ;
	const VEC3& B2 = this->m_approx[m_approxindex_FB]->getAttr(m_attrindex_FB)[d2] ;
	const VEC3& N1 = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[d1] ;
	const VEC3& N2 = this->m_approx[m_approxindex_FN]->getAttr(m_attrindex_FN)[d2] ;

	// compute new frame
	const VEC3& N = N1 ;
	VEC3 T ;
	if (N2 != N1)
		T = N2 ^ N1 ; // i is perpendicular to newNormal
	else
		T = N1 ^ VEC3(1,2,3) ; // second random vector
	T.normalize() ;

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

	double alpha = fabs(alpha1 + alpha2) ;

	// get coefs of v1 and v2
	std::vector<VEC3> coefs1, coefs2, coefs ;
	coefs1.resize(m_K) ; coefs2.resize(m_K) ;
	for (unsigned int i = 0 ; i < m_K ; ++i)
	{
		coefs1[i] = this->m_approx[m_approxindex_HF[i]]->getAttr(m_attrindex_HF[i])[d1] ;
		coefs2[i] = this->m_approx[m_approxindex_HF[i]]->getAttr(m_attrindex_HF[i])[d2] ;
	}

	Utils::QuadricHF<REAL> q(coefs2, gamma2, alpha2) ;
	bool opt = q.findOptimizedCoefs(coefs) ; // coefs of d1's lightfield rotated around new local axis
	q += Utils::QuadricHF<REAL>(coefs1, gamma1, alpha1) ;

	if (!opt)
	{
		std::cerr << "HalfEdgeSelector_LFperFace::Optimization failed (should never happen since no optim is done)" << std::endl ;
		std::cout << alpha2 << std::endl ;
	}

	const VEC3 avgColDiff = m_avgColor[d1] - m_avgColor[d2] ;

	REAL err = q(coefs) ;
	if (fabs(err) < 1e-6)
		err = 0 ;

	return (alpha / M_PI) * avgColDiff.norm2()/3. + err ;

}
*/

} // namespace Decimation

} // namespace Surface

} // namespace Algo

} // namespace CGoGN
