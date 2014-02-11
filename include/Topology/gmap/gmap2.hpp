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

template <class MAP>
inline void GMap2<MAP>::init()
{
	MAP::addInvolution() ;
}

template <class MAP>
inline GMap2<MAP>::GMap2() : GMap1<MAP>()
{
	init() ;
}

template <class MAP>
inline std::string GMap2<MAP>::mapTypeName() const
{
	return "GMap2";
}

template <class MAP>
inline unsigned int GMap2<MAP>::dimension() const
{
	return 2;
}

template <class MAP>
inline void GMap2<MAP>::clear(bool removeAttrib)
{
	ParentMap::clear(removeAttrib) ;
	if (removeAttrib)
		init() ;
}

template <class MAP>
inline void GMap2<MAP>::update_topo_shortcuts()
{
	ParentMap::update_topo_shortcuts();
//	m_beta2 = getRelation("beta2");
}

/*! @name Basic Topological Operators
 * Access and Modification
 *************************************************************************/

template <class MAP>
inline Dart GMap2<MAP>::beta2(Dart d) const
{
	return MAP::template getInvolution<2>(d);
}

template <class MAP>
template <int N>
inline Dart GMap2<MAP>::beta(const Dart d) const
{
	assert( (N > 0) || !"negative parameters not allowed in template multi-beta");
	if (N<10)
	{
		switch(N)
		{
			case 0 : return this->beta0(d) ;
			case 1 : return this->beta1(d) ;
			case 2 : return beta2(d) ;
			default : assert(!"Wrong multi-beta relation value") ;
		}
	}
	switch(N%10)
	{
		case 0 : return beta0(beta<N/10>(d)) ;
		case 1 : return beta1(beta<N/10>(d)) ;
		case 2 : return beta2(beta<N/10>(d)) ;
		default : assert(!"Wrong multi-beta relation value") ;
	}
}

template <class MAP>
inline Dart GMap2<MAP>::phi2(Dart d) const
{
	return beta2(this->beta0(d)) ;
}

template <class MAP>
template <int N>
inline Dart GMap2<MAP>::phi(Dart d) const
{
	assert( (N >0) || !"negative parameters not allowed in template multi-phi");
	if (N<10)
	{
		switch(N)
		{
			case 1 : return this->phi1(d) ;
			case 2 : return phi2(d) ;
			default : assert(!"Wrong multi-phi relation value") ; return d ;
		}
	}
	switch(N%10)
	{
		case 1 : return phi1(phi<N/10>(d)) ;
		case 2 : return phi2(phi<N/10>(d)) ;
		default : assert(!"Wrong multi-phi relation value") ; return d ;
	}
}

template <class MAP>
inline Dart GMap2<MAP>::alpha0(Dart d) const
{
	return beta2(this->beta0(d)) ;
}

template <class MAP>
inline Dart GMap2<MAP>::alpha1(Dart d) const
{
	return beta2(this->beta1(d)) ;
}

template <class MAP>
inline Dart GMap2<MAP>::alpha_1(Dart d) const
{
	return beta1(beta2(d)) ;
}

template <class MAP>
inline void GMap2<MAP>::beta2sew(Dart d, Dart e)
{
	MAP::template involutionSew<2>(d,e);
}

template <class MAP>
inline void GMap2<MAP>::beta2unsew(Dart d)
{
	MAP::template involutionUnsew<2>(d);
}

/*! @name Constructors and Destructors
 *  To generate or delete faces in a 2-G-map
 *************************************************************************/

template <class MAP>
Dart GMap2<MAP>::newFace(unsigned int nbEdges, bool withBoundary)
{
	Dart d = ParentMap::newCycle(nbEdges);
	if (withBoundary)
	{
		Dart e = newBoundaryCycle(nbEdges);

		Dart it = d;
		do
		{
			beta2sew(it, this->beta0(e));
			beta2sew(this->beta0(it), e);
			it = this->phi1(it);
			e = this->phi_1(e);
		} while (it != d);
	}
	return d;
}

template <class MAP>
void GMap2<MAP>::deleteFace(Dart d)
{
	assert(!this->isBoundaryMarked2(d)) ;
	Dart it = d ;
	do
	{
		if(!isBoundaryEdge(it))
			unsewFaces(it) ;
		it = this->phi1(it) ;
	} while(it != d) ;
	Dart dd = phi2(d) ;
	ParentMap::deleteCycle(d) ;
	ParentMap::deleteCycle(dd) ;
}

template <class MAP>
void GMap2<MAP>::deleteCC(Dart d)
{
	DartMarkerStore mark(*this);

	std::vector<Dart> visited;
	visited.reserve(1024) ;
	visited.push_back(d);
	mark.mark(d) ;

	for(unsigned int i = 0; i < visited.size(); ++i)
	{
		Dart d0 = this->beta0(visited[i]) ;
		if(!mark.isMarked(d0))
		{
			visited.push_back(d0) ;
			mark.mark(d0);
		}
		Dart d1 = this->beta1(visited[i]) ;
		if(!mark.isMarked(d1))
		{
			visited.push_back(d1) ;
			mark.mark(d1);
		}
		Dart d2 = beta2(visited[i]) ;
		if(!mark.isMarked(d2))
		{
			visited.push_back(d2) ;
			mark.mark(d2);
		}
	}

	for(std::vector<Dart>::iterator it = visited.begin(); it != visited.end(); ++it)
		this->deleteDart(*it) ;
}

template <class MAP>
void GMap2<MAP>::fillHole(Dart d)
{
	assert(isBoundaryEdge(d)) ;
	Dart dd = d ;
	if(!this->isBoundaryMarked2(dd))
		dd = phi2(dd) ;
	this->template boundaryUnmarkOrbit<FACE,2>(dd) ;
}

/*! @name Topological Operators
 *  Topological operations on 2-G-maps
 *************************************************************************/

template <class MAP>
void GMap2<MAP>::splitVertex(Dart d, Dart e)
{
	assert(sameVertex(d, e));

	if(!sameOrientedVertex(d, e))
		e = beta2(e) ;

	Dart dd = phi2(d) ;
	Dart ee = phi2(e) ;
	beta2unsew(d) ;
	beta2unsew(e) ;

	ParentMap::cutEdge(dd);	// Cut the edge of dd (make a new edge)
	ParentMap::cutEdge(ee);	// Cut the edge of ee (make a new edge)

	beta2sew(this->phi1(dd), this->beta0(this->phi1(ee)));	// Sew the two faces along the new edge
	beta2sew(this->phi1(ee), this->beta0(this->phi1(dd)));
	beta2sew(d, this->beta0(dd)) ;
	beta2sew(e, this->beta0(ee)) ;
}

template <class MAP>
Dart GMap2<MAP>::deleteVertex(Dart d)
{
	if(isBoundaryVertex(d))
		return NIL ;

	Dart res = NIL ;
	Dart vit = d ;
	do
	{
		if(res == NIL && this->phi1(this->phi1(d)) != d)
			res = this->phi1(d) ;

		Dart d0 = this->beta0(vit) ;
		Dart d02 = beta2(d0) ;
		Dart d01 = this->beta1(d0) ;
		Dart d021 = this->beta1(d02) ;
		this->beta1unsew(d0) ;
		this->beta1unsew(d02) ;
		this->beta1sew(d0, d02) ;
		this->beta1sew(d01, d021) ;

		vit = alpha1(vit) ;
	} while(vit != d) ;
	ParentMap::deleteCycle(d) ;
	return res ;
}

template <class MAP>
Dart GMap2<MAP>::cutEdge(Dart d)
{
	Dart e = phi2(d) ;
	Dart nd = ParentMap::cutEdge(d) ;
	Dart ne = ParentMap::cutEdge(e) ;

	beta2sew(nd, this->beta1(ne)) ;
	beta2sew(ne, this->beta1(nd)) ;

	return nd ;
}

template <class MAP>
bool GMap2<MAP>::uncutEdge(Dart d)
{
	if(vertexDegree(this->phi1(d)) == 2)
	{
		ParentMap::uncutEdge(d) ;
		ParentMap::uncutEdge(beta2(d)) ;
		return true ;
	}
	return false ;
}

template <class MAP>
Dart GMap2<MAP>::collapseEdge(Dart d, bool delDegenerateFaces)
{
	Dart resV = NIL ;

	Dart e = phi2(d);
	beta2unsew(d);	// Unlink the opposite edges
	beta2unsew(e);

	Dart f = this->phi1(e) ;
	Dart h = alpha1(e);

	if (h != e)
		resV = h;

	if (f != e && delDegenerateFaces)
	{
		ParentMap::collapseEdge(e) ;	// Collapse edge e
		collapseDegeneratedFace(f) ;	// and collapse its face if degenerated
	}
	else
		ParentMap::collapseEdge(e) ;	// Just collapse edge e

	f = this->phi1(d) ;
	if(resV == NIL)
	{
		h = alpha1(d);
		if (h != d)
			resV = h;
	}

	if (f != d && delDegenerateFaces)
	{
		ParentMap::collapseEdge(d) ;	// Collapse edge d
		collapseDegeneratedFace(f) ;	// and collapse its face if degenerated
	}
	else
		ParentMap::collapseEdge(d) ;	// Just collapse edge d

	return resV ;
}

template <class MAP>
bool GMap2<MAP>::flipEdge(Dart d)
{
	if (!isBoundaryEdge(d))
	{
		Dart e = phi2(d);
		Dart dNext = this->phi1(d);
		Dart eNext = this->phi1(e);
		Dart dPrev = this->phi_1(d);
		Dart ePrev = this->phi_1(e);
		Dart dNext2 = this->phi1(dNext);
		Dart eNext2 = this->phi1(eNext);

		this->beta1unsew(d) ;
		this->beta1unsew(eNext) ;
		this->beta1unsew(e) ;
		this->beta1unsew(dNext) ;
		this->beta1unsew(dNext2) ;
		this->beta1unsew(eNext2) ;

		this->beta1sew(this->beta0(e), eNext2) ;
		this->beta1sew(d, this->beta0(eNext)) ;
		this->beta1sew(this->beta0(d), dNext2) ;
		this->beta1sew(e, this->beta0(dNext)) ;
		this->beta1sew(eNext, this->beta0(dPrev)) ;
		this->beta1sew(dNext, this->beta0(ePrev)) ;

		return true ;
	}
	return false ; // cannot flip a border edge
}

template <class MAP>
bool GMap2<MAP>::flipBackEdge(Dart d)
{
	if (!isBoundaryEdge(d))
	{
		Dart e = phi2(d);
		Dart dNext = this->phi1(d);
		Dart eNext = this->phi1(e);
		Dart dPrev = this->phi_1(d);
		Dart ePrev = this->phi_1(e);
		Dart dPrev1 = this->beta1(dPrev) ;
		Dart ePrev1 = this->beta1(ePrev) ;

		this->beta1unsew(d) ;
		this->beta1unsew(eNext) ;
		this->beta1unsew(e) ;
		this->beta1unsew(dNext) ;
		this->beta1unsew(dPrev) ;
		this->beta1unsew(ePrev) ;

		this->beta1sew(this->beta0(e), dPrev) ;
		this->beta1sew(d, dPrev1) ;
		this->beta1sew(this->beta0(d), ePrev) ;
		this->beta1sew(e, ePrev1) ;
		this->beta1sew(eNext, this->beta0(dPrev)) ;
		this->beta1sew(dNext, this->beta0(ePrev)) ;

		return true ;
	}
	return false ; // cannot flip a border edge
}

//template <class MAP>
//void GMap2<MAP>::insertEdgeInVertex(Dart d, Dart e)
//{
//	assert(!sameVertex(d,e) && phi2(e)==phi_1(e));
//
//	phi1sew(phi_1(d),phi_1(e));
//}
//
//template <class MAP>
//void GMap2<MAP>::removeEdgeFromVertex(Dart d)
//{
//	assert(phi2(d)!=d);
//
//	phi1sew(phi_1(d),phi2(d));
//}

template <class MAP>
void GMap2<MAP>::sewFaces(Dart d, Dart e, bool withBoundary)
{
	// if sewing with fixed points
	if (!withBoundary)
	{
		assert(beta2(d) == d && beta2(this->beta0(d)) == this->beta0(d) && beta2(e) == e && beta2(this->beta0(e)) == this->beta0(e)) ;
		beta2sew(d, this->beta0(e)) ;
		beta2sew(e, this->beta0(d)) ;
		return ;
	}

	assert(isBoundaryEdge(d) && isBoundaryEdge(e)) ;

	Dart dd = phi2(d) ;
	Dart ee = phi2(e) ;

	beta2unsew(d) ;	// unsew faces from boundary
	beta2unsew(this->beta0(d)) ;
	beta2unsew(e) ;
	beta2unsew(this->beta0(e)) ;

	if (ee != this->phi_1(dd))
	{
		Dart eeN = this->phi1(ee) ;		// remove the boundary edge
		Dart dd1 = this->beta1(dd) ;
		this->beta1unsew(eeN) ;
		this->beta1unsew(dd1) ;
		this->beta1sew(this->beta0(ee), dd) ;
		this->beta1sew(eeN, dd1) ;
	}
	if (dd != this->phi_1(ee))
	{
		Dart ddN = this->phi1(dd) ;		// and properly close incident boundaries
		Dart ee1 = this->beta1(ee) ;
		this->beta1unsew(ddN) ;
		this->beta1unsew(ee1) ;
		this->beta1sew(this->beta0(dd), ee) ;
		this->beta1sew(ddN, ee1) ;
	}
	ParentMap::deleteCycle(dd) ;

	beta2sew(d, this->beta0(e)) ; // sew the faces
	beta2sew(e, this->beta0(d)) ;
}

template <class MAP>
void GMap2<MAP>::unsewFaces(Dart d)
{
	assert(!isBoundaryEdge(d)) ;

	Dart dd = phi2(d);

	Dart e = newBoundaryCycle(2);
	Dart ee = this->phi1(e) ;

	Dart f = findBoundaryEdgeOfVertex(d) ;
	if (f != NIL)
	{
		Dart f1 = this->beta1(f) ;
		this->beta1unsew(ee) ;
		this->beta1unsew(f) ;
		this->beta1sew(f, this->beta0(e)) ;
		this->beta1sew(f1, ee) ;
	}

	f = findBoundaryEdgeOfVertex(dd) ;
	if (f != NIL)
	{
		Dart f1 = this->beta1(f) ;
		this->beta1unsew(e) ;
		this->beta1unsew(f) ;
		this->beta1sew(f, this->beta0(ee)) ;
		this->beta1sew(f1, e) ;
	}

	beta2unsew(d) ;
	beta2unsew(this->beta0(d)) ;
	beta2unsew(dd) ;
	beta2unsew(this->beta0(dd)) ;

	beta2sew(d, this->beta0(e)) ;	// sew faces
	beta2sew(e, this->beta0(d)) ;
	beta2sew(dd, this->beta0(ee)) ;	// to the boundary
	beta2sew(ee, this->beta0(dd)) ;
}

template <class MAP>
bool GMap2<MAP>::collapseDegeneratedFace(Dart d)
{
	Dart e = this->phi1(d);		// Check if the face is a loop
	if (this->phi1(e) == d)		// Yes: it contains one or two edge(s)
	{
		Dart d2 = phi2(d) ;		// Check opposite edges
		Dart e2 = phi2(e) ;
		beta2unsew(d) ;
		beta2unsew(this->beta0(d)) ;
		if(d != e)
		{
			beta2unsew(e) ;
			beta2unsew(this->beta0(e)) ;
			beta2sew(d2, this->beta0(e2)) ;
			beta2sew(e2, this->beta0(d2)) ;
		}
		else
		{
			Dart d21 = this->beta1(d2) ;
			Dart d2N = this->phi1(d2) ;
			this->beta1unsew(d2) ;
			this->beta1unsew(d2N) ;
			this->beta1sew(d21, d2N) ;
			this->beta1sew(d2, this->beta0(d2)) ;
			ParentMap::deleteCycle(d2) ;
		}
		ParentMap::deleteCycle(d) ;
		return true ;
	}
	return false ;
}

template <class MAP>
void GMap2<MAP>::splitFace(Dart d, Dart e)
{
	assert(d != e && sameFace(d, e)) ;

	if(!sameOrientedFace(d, e))
		e = this->beta1(e) ;

	Dart dprev = this->phi_1(d) ;
	Dart eprev = this->phi_1(e) ;

	// required to unsew and resew because we use GMap1 cutEdge
	// which insert new darts within the cut edge
	beta2unsew(this->beta1(d)) ;
	beta2unsew(this->beta1(e)) ;

	Dart dd = ParentMap::cutEdge(this->phi_1(d)) ;
	Dart ee = ParentMap::cutEdge(this->phi_1(e)) ;
	ParentMap::splitCycle(dd, ee) ;
	beta2sew(dd, this->beta1(e)) ;
	beta2sew(ee, this->beta1(d)) ;

	beta2sew(this->beta0(dprev), this->beta0(beta2(dprev))) ;
	beta2sew(this->beta0(eprev), this->beta0(beta2(eprev))) ;
}

template <class MAP>
bool GMap2<MAP>::mergeFaces(Dart d)
{
	if (!isBoundaryEdge(d))
	{
		Dart e = phi2(d) ;
		beta2unsew(d) ;
		beta2unsew(e) ;
		ParentMap::mergeCycles(d, this->phi1(e)) ;
		ParentMap::splitCycle(e, this->phi1(d)) ;
		ParentMap::deleteCycle(d) ;
		return true ;
	}
	return false ;
}

template <class MAP>
void GMap2<MAP>::extractTrianglePair(Dart d)
{
	Dart e = phi2(d) ;

	assert(!isBoundaryFace(d) && !isBoundaryFace(e)) ;
	assert(faceDegree(d) == 3 && faceDegree(e) == 3) ;

	Dart d1 = phi2(this->phi1(d)) ;
	Dart d2 = phi2(this->phi_1(d)) ;
	beta2unsew(d1) ;
	beta2unsew(this->beta0(d1)) ;
	beta2unsew(d2) ;
	beta2unsew(this->beta0(d2)) ;
	beta2sew(d1, this->beta0(d2)) ;
	beta2sew(d2, this->beta0(d1)) ;

	Dart e1 = phi2(this->phi1(e)) ;
	Dart e2 = phi2(this->phi_1(e)) ;
	beta2unsew(e1) ;
	beta2unsew(this->beta0(e1)) ;
	beta2unsew(e2) ;
	beta2unsew(this->beta0(e2)) ;
	beta2sew(e1, this->beta0(e2)) ;
	beta2sew(e2, this->beta0(e1)) ;
}

template <class MAP>
void GMap2<MAP>::insertTrianglePair(Dart d, Dart v1, Dart v2)
{
	Dart e = phi2(d) ;

	assert(v1 != v2 && sameOrientedVertex(v1, v2)) ;
	assert(faceDegree(d) == 3 && faceDegree(phi2(d)) == 3) ;

	Dart vv1 = phi2(v1) ;
	beta2unsew(v1) ;
	beta2unsew(vv1) ;
	beta2sew(this->phi_1(d), this->beta0(v1)) ;
	beta2sew(this->beta1(d), v1) ;
	beta2sew(this->phi1(d), this->beta0(vv1)) ;
	beta2sew(this->beta0(this->phi1(d)), vv1) ;

	Dart vv2 = phi2(v2) ;
	beta2unsew(v2) ;
	beta2unsew(vv2) ;
	beta2sew(this->phi_1(e), this->beta0(v2)) ;
	beta2sew(this->beta1(e), v2) ;
	beta2sew(this->phi1(e), this->beta0(vv2)) ;
	beta2sew(this->beta0(this->phi1(e)), vv2) ;
}

template <class MAP>
bool GMap2<MAP>::mergeVolumes(Dart d, Dart e)
{
	assert(!this->isBoundaryMarked2(d) && !this->isBoundaryMarked2(e)) ;

	if (GMap2::isBoundaryFace(d) || GMap2::isBoundaryFace(e))
		return false;

	// First traversal of both faces to check the face sizes
	// and store their edges to efficiently access them further

	std::vector<Dart> dDarts;
	std::vector<Dart> eDarts;
	dDarts.reserve(16);		// usual faces have less than 16 edges
	eDarts.reserve(16);

	Dart dFit = d ;
	Dart eFit = this->phi1(e) ;	// must take phi1 because of the use
	do							// of reverse iterator for sewing loop
	{
		dDarts.push_back(dFit) ;
		dFit = this->phi1(dFit) ;
	} while(dFit != d) ;
	do
	{
		eDarts.push_back(eFit) ;
		eFit = this->phi1(eFit) ;
	} while(eFit != this->phi1(e)) ;

	if(dDarts.size() != eDarts.size())
		return false ;

	// Make the sewing: take darts in initial order (clockwise) in first face
	// and take darts in reverse order (counter-clockwise) in the second face
	std::vector<Dart>::iterator dIt;
	std::vector<Dart>::reverse_iterator eIt;
	for (dIt = dDarts.begin(), eIt = eDarts.rbegin(); dIt != dDarts.end(); ++dIt, ++eIt)
	{
		Dart d2 = phi2(*dIt);	// Search the faces adjacent to dNext and eNext
		Dart e2 = phi2(*eIt);
		beta2unsew(d2);		// Unlink the two adjacent faces from dNext and eNext
		beta2unsew(this->beta0(d2));
		beta2unsew(e2);
		beta2unsew(this->beta0(e2));
		beta2sew(d2, this->beta0(e2));	// Link the two adjacent faces together
		beta2sew(e2, this->beta0(d2));
	}
	ParentMap::deleteCycle(d);		// Delete the two alone faces
	ParentMap::deleteCycle(e);

	return true ;
}

template <class MAP>
void GMap2<MAP>::splitSurface(std::vector<Dart>& vd, bool firstSideClosed, bool secondSideClosed)
{
	//assert(checkSimpleOrientedPath(vd)) ;
	Dart e = vd.front() ;
	Dart e2 = phi2(e) ;

	//unsew the edge path
	for(std::vector<Dart>::iterator it = vd.begin() ; it != vd.end() ; ++it)
	{
		if(!GMap2::isBoundaryEdge(*it))
			unsewFaces(*it) ;
	}

	if(firstSideClosed)
		GMap2::fillHole(e) ;

	if(secondSideClosed)
		GMap2::fillHole(e2) ;
}

/*! @name Topological Queries
 *  Return or set various topological information
 *************************************************************************/

template <class MAP>
bool GMap2<MAP>::sameOrientedVertex(Dart d, Dart e) const
{
	Dart it = d;
	do
	{
		if (it == e)	// Test equality with e
			return true;
		it = alpha1(it);
	} while (it != d);
	return false;		// None is equal to e => vertices are distinct
}

template <class MAP>
inline bool GMap2<MAP>::sameVertex(Dart d, Dart e) const
{
	return sameOrientedVertex(d, e) || sameOrientedVertex(beta2(d), e) ;
}

template <class MAP>
unsigned int GMap2<MAP>::vertexDegree(Dart d) const
{
	unsigned int count = 0 ;
	Dart it = d ;
	do
	{
		++count ;
		it = alpha1(it) ;
	} while (it != d) ;
	return count ;
}

template <class MAP>
int GMap2<MAP>::checkVertexDegree(Dart d, unsigned int vd) const
{
	unsigned int count = 0 ;
	Dart it = d ;
	do
	{
		++count ;
		it = alpha1(it) ;
	} while ((count<=vd) && (it != d)) ;

	return count-vd;
}

template <class MAP>
bool GMap2<MAP>::isBoundaryVertex(Dart d) const
{
	Dart it = d ;
	do
	{
		if (this->isBoundaryMarked2(it))
			return true ;
		it = alpha1(it) ;
	} while (it != d) ;
	return false ;
}

template <class MAP>
Dart GMap2<MAP>::findBoundaryEdgeOfVertex(Dart d) const
{
	Dart it = d ;
	do
	{
		if (this->isBoundaryMarked2(it))
			return it ;
		it = alpha1(it) ;
	} while (it != d) ;
	return NIL ;
}

template <class MAP>
inline bool GMap2<MAP>::sameEdge(Dart d, Dart e) const
{
	return d == e || beta2(d) == e || this->beta0(d) == e || beta2(this->beta0(d)) == e ;
}

template <class MAP>
inline bool GMap2<MAP>::isBoundaryEdge(Dart d) const
{
	return this->isBoundaryMarked2(d) || this->isBoundaryMarked2(beta2(d));
}

template <class MAP>
inline bool GMap2<MAP>::sameOrientedFace(Dart d, Dart e) const
{
	return ParentMap::sameOrientedCycle(d, e) ;
}

template <class MAP>
inline bool GMap2<MAP>::sameFace(Dart d, Dart e) const
{
	return ParentMap::sameCycle(d, e) ;
}

template <class MAP>
inline unsigned int GMap2<MAP>::faceDegree(Dart d) const
{
	return ParentMap::cycleDegree(d) ;
}

template <class MAP>
inline int GMap2<MAP>::checkFaceDegree(Dart d, unsigned int le) const
{
	return ParentMap::checkCycleDegree(d,le) ;
}

template <class MAP>
bool GMap2<MAP>::isBoundaryFace(Dart d) const
{
	Dart it = d ;
	do
	{
		if (this->isBoundaryMarked2(beta2(it)))
			return true ;
		it = this->phi1(it) ;
	} while (it != d) ;
	return false ;
}

template <class MAP>
bool GMap2<MAP>::sameOrientedVolume(Dart d, Dart e) const
{
	DartMarkerStore mark(*this);	// Lock a marker

	std::list<Dart> visitedFaces;	// Faces that are traversed
	visitedFaces.push_back(d);		// Start with the face of d
	std::list<Dart>::iterator face;

	// For every face added to the list
	for (face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		if (!this->isBoundaryMarked2(*face) && !mark.isMarked(*face))	// Face has not been visited yet
		{
			Dart it = *face ;
			do
			{
				if(it == e)
					return true;

				mark.mark(it);						// Mark
				Dart adj = phi2(it);				// Get adjacent face
				if (!this->isBoundaryMarked2(adj) && !mark.isMarked(adj))
					visitedFaces.push_back(adj);	// Add it
				it = this->phi1(it);
			} while(it != *face);
		}
	}
	return false;
}

template <class MAP>
inline bool GMap2<MAP>::sameVolume(Dart d, Dart e) const
{
	return sameOrientedVolume(d, e) || sameOrientedVolume(beta2(d), e) ;
}

template <class MAP>
unsigned int GMap2<MAP>::volumeDegree(Dart d) const
{
	unsigned int count = 0;
	DartMarkerStore mark(*this);		// Lock a marker

	std::vector<Dart> visitedFaces;		// Faces that are traversed
	visitedFaces.reserve(16);
	visitedFaces.push_back(d);			// Start with the face of d
	std::vector<Dart>::iterator face;

	// For every face added to the list
	for (unsigned int i = 0; i != visitedFaces.size(); ++i)
	{
		Dart df = visitedFaces[i];
		if (!this->isBoundaryMarked2(df) && !mark.isMarked(df))	// Face has not been visited yet
		{
			++count;
			Dart it = df ;
			do
			{
				mark.mark(it);					// Mark
				Dart adj = phi2(it);			// Get adjacent face
				if ( !this->isBoundaryMarked2(adj) && !mark.isMarked(adj) )
					visitedFaces.push_back(adj);// Add it
				it = this->phi1(it);
			} while(it != df);
		}
	}

	return count;
}

template <class MAP>
int GMap2<MAP>::checkVolumeDegree(Dart d, unsigned int volDeg) const
{
	unsigned int count = 0;
	DartMarkerStore mark(*this);		// Lock a marker

	std::vector<Dart> visitedFaces;		// Faces that are traversed
	visitedFaces.reserve(16);
	visitedFaces.push_back(d);			// Start with the face of d
	std::vector<Dart>::iterator face;

	// For every face added to the list
	for (unsigned int i = 0; i != visitedFaces.size(); ++i)
	{
		Dart df = visitedFaces[i];
		if (!this->isBoundaryMarked2(df) && !mark.isMarked(df))	// Face has not been visited yet
		{
			++count;
			Dart it = df ;
			do
			{
				mark.mark(it);					// Mark
				Dart adj = phi2(it);			// Get adjacent face
				if ( !this->isBoundaryMarked2(adj) && !mark.isMarked(adj) )
					visitedFaces.push_back(adj);// Add it
				it = this->phi1(it);
			} while(it != df);
		}
		if (count > volDeg)
			break;
	}

	return count - volDeg;
}

template <class MAP>
bool GMap2<MAP>::isTriangular() const
{
	TraversorF<GMap2> t(*this) ;
	for(Dart d = t.begin(); d != t.end(); d = t.next())
	{
		if(faceDegree(d) != 3)
			return false ;
	}
	return true ;
}

template <class MAP>
bool GMap2<MAP>::check() const
{
	CGoGNout << "Check: topology begin" << CGoGNendl;
	for(Dart d = this->begin(); d != this->end(); this->next(d))
	{
		Dart dd = this->beta0(d);
		if (this->beta0(dd) != d)	// beta0 involution ?
		{
			CGoGNout << "Check: beta0 is not an involution" << CGoGNendl;
			return false;
		}

		dd = this->beta1(d);
		if (this->beta1(dd) != d)	// beta1 involution ?
		{
			CGoGNout << "Check: beta1 is not an involution" << CGoGNendl;
			return false;
		}

		dd = beta2(d);
		if (beta2(dd) != d)	// beta2 involution ?
		{
			CGoGNout << "Check: beta2 is not an involution" << CGoGNendl;
			return false;
		}
		if(dd == d)
			CGoGNout << "Check (warning): beta2 has fix points" << CGoGNendl;
	}

	CGoGNout << "Check: topology ok" << CGoGNendl;

	return true;
}

template <class MAP>
bool GMap2<MAP>::checkSimpleOrientedPath(std::vector<Dart>& vd)
{
	DartMarkerStore dm(*this) ;
	for(std::vector<Dart>::iterator it = vd.begin() ; it != vd.end() ; ++it)
	{
		if(dm.isMarked(*it))
			return false ;

		dm.markOrbit<VERTEX>(*it) ;

		std::vector<Dart>::iterator prev ;
		if(it == vd.begin())
			prev = vd.end() - 1 ;
		else
			prev = it - 1 ;

		if(!sameVertex(*it, this->phi1(*prev)))
			return false ;
	}
	return true ;
}

/*! @name Cell Functors
 *  Apply functors to all darts of a cell
 *************************************************************************/

template <class MAP>
bool GMap2<MAP>::foreach_dart_of_oriented_vertex(Dart d, FunctorType& f, unsigned int /*thread*/) const
{
	Dart it = d;
	do
	{
		if (f(it))
			return true;
		it = alpha1(it);
	} while (it != d);
	return false;
}

template <class MAP>
inline bool GMap2<MAP>::foreach_dart_of_vertex(Dart d, FunctorType& f, unsigned int thread) const
{
	return GMap2<MAP>::foreach_dart_of_oriented_vertex(d, f, thread) || GMap2<MAP>::foreach_dart_of_oriented_vertex(this->beta1(d), f, thread) ;
}

template <class MAP>
bool GMap2<MAP>::foreach_dart_of_oriented_edge(Dart d, FunctorType& f, unsigned int /*thread*/) const
{
	if (f(d))
		return true ;
	Dart e = beta2(this->beta0(d)) ;
	if (f(e))
		return true ;

	return false ;
}

template <class MAP>
bool GMap2<MAP>::foreach_dart_of_edge(Dart d, FunctorType& f, unsigned int /*thread*/) const
{
	if (f(d))
		return true ;
	Dart e = this->beta0(d) ;
	if (f(e))
		return true ;
	e = beta2(d) ;
	if (f(e))
		return true ;
	e = this->beta0(e) ;
	if (f(e))
		return true ;

	return false ;
}

template <class MAP>
inline bool GMap2<MAP>::foreach_dart_of_oriented_face(Dart d, FunctorType& f, unsigned int thread) const
{
	return ParentMap::foreach_dart_of_oriented_cc(d, f, thread);
}

template <class MAP>
inline bool GMap2<MAP>::foreach_dart_of_face(Dart d, FunctorType& f, unsigned int thread) const
{
	return ParentMap::foreach_dart_of_cc(d, f, thread);
}

template <class MAP>
bool GMap2<MAP>::foreach_dart_of_oriented_cc(Dart d, FunctorType& f, unsigned int thread) const
{
	DartMarkerStore mark(*this, thread);	// Lock a marker
	bool found = false;				// Last functor return value

	std::vector<Dart> visitedFaces;	// Faces that are traversed
	visitedFaces.reserve(1024) ;
	visitedFaces.push_back(d);		// Start with the face of d

	// For every face added to the list
	for(unsigned int i = 0; !found && i < visitedFaces.size(); ++i)
	{
		if (!mark.isMarked(visitedFaces[i]))		// Face has not been visited yet
		{
			// Apply functor to the darts of the face
			found = foreach_dart_of_oriented_face(visitedFaces[i], f);

			// If functor returns false then mark visited darts (current face)
			// and add non visited adjacent faces to the list of face
			if (!found)
			{
				Dart e = visitedFaces[i] ;
				do
				{
					mark.mark(e);					// Mark
					Dart adj = phi2(e);				// Get adjacent face
					if (!mark.isMarked(adj))
						visitedFaces.push_back(adj);	// Add it
					e = this->phi1(e);
				} while(e != visitedFaces[i]);
			}
		}
	}
	return found;
}

template <class MAP>
inline bool GMap2<MAP>::foreach_dart_of_cc(Dart d, FunctorType& f, unsigned int thread) const
{
	return GMap2<MAP>::foreach_dart_of_oriented_cc(d, f, thread) || GMap2<MAP>::foreach_dart_of_oriented_cc(this->beta0(d), f, thread) ;
}

template <class MAP>
inline bool GMap2<MAP>::foreach_dart_of_vertex1(Dart d, FunctorType& f, unsigned int thread) const
{
	return ParentMap::foreach_dart_of_vertex(d,f,thread);
}

template <class MAP>
inline bool GMap2<MAP>::foreach_dart_of_edge1(Dart d, FunctorType& f, unsigned int thread) const
{
	return ParentMap::foreach_dart_of_edge(d,f,thread);
}

/*! @name Close map after import or creation
 *  These functions must be used with care, generally only by import/creation algorithms
 *************************************************************************/

template <class MAP>
Dart GMap2<MAP>::newBoundaryCycle(unsigned int nbE)
{
	Dart d = ParentMap::newCycle(nbE);
	this->template boundaryMarkOrbit<FACE,2>(d);
	return d;
}

template <class MAP>
unsigned int GMap2<MAP>::closeHole(Dart d, bool forboundary)
{
	assert(beta2(d) == d);		// Nothing to close

	Dart first = ParentMap::newEdge();	// First edge of the face that will fill the hole
	unsigned int countEdges = 1;

	beta2sew(d, this->beta0(first));	// sew the new edge to the hole
	beta2sew(first, this->beta0(d));

	Dart prev = first ;
	Dart dNext = d;	// Turn around the hole
	Dart dPhi1;		// to complete the face
	do
	{
		dPhi1 = this->phi1(dNext) ;
		dNext = beta2(dPhi1) ;
		while(dNext != dPhi1 && dPhi1 != d)
		{
			dPhi1 = this->beta1(dNext) ;	// Search and put in dNext
			dNext = beta2(dPhi1) ;			// the next dart of the hole
		}

		if (dPhi1 != d)
		{
			Dart next = ParentMap::newEdge();		// Add a new edge there and link it to the face
			++countEdges;
			this->beta1sew(this->beta0(next), prev);// the edge is linked to the face
			prev = next ;
			beta2sew(dNext, this->beta0(next));		// the face is linked to the hole
			beta2sew(next, this->beta0(dNext));
		}
	} while (dPhi1 != d);

	this->beta1sew(prev, this->beta0(first)) ;

	if(forboundary)
		this->template boundaryMarkOrbit<FACE,2>(phi2(d));

	return countEdges ;
}

template <class MAP>
unsigned int GMap2<MAP>::closeMap()
{
	// Search the map for topological holes (fix points of phi2)
	unsigned int nb = 0 ;
	for (Dart d = this->begin(); d != this->end(); this->next(d))
	{
		if (beta2(d) == d)
		{
			++nb ;
			closeHole(d);
		}
	}
	return nb ;
}

/*! @name Compute dual
 * These functions compute the dual mesh
 *************************************************************************/

template <class MAP>
void GMap2<MAP>::computeDual()
{
//	DartAttribute<Dart> old_beta0 = this->getAttribute<Dart, DART>("beta0");
//	DartAttribute<Dart> old_beta2 = this->getAttribute<Dart, DART>("beta2") ;
//
//	swapAttributes<Dart>(old_beta0, old_beta2) ;
//
//	swapEmbeddingContainers(VERTEX, FACE) ;
//
//	//boundary management ?
}

} // namespace CGoGN
