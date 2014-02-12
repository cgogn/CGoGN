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
inline void Map2<MAP>::init()
{
	MAP::addInvolution() ;
}

template <class MAP>
inline Map2<MAP>::Map2() : Map1<MAP>()
{
	init() ;
}

template <class MAP>
inline std::string Map2<MAP>::mapTypeName() const
{
	return "Map2" ;
}

template <class MAP>
inline unsigned int Map2<MAP>::dimension() const
{
	return 2 ;
}

template <class MAP>
inline void Map2<MAP>::clear(bool removeAttrib)
{
	ParentMap::clear(removeAttrib) ;
	if (removeAttrib)
		init() ;
}

template <class MAP>
inline void Map2<MAP>::update_topo_shortcuts()
{
	ParentMap::update_topo_shortcuts();
//	m_phi2 = getRelation("phi2");
}

/*! @name Basic Topological Operators
 * Access and Modification
 *************************************************************************/

template <class MAP>
inline Dart Map2<MAP>::phi2(Dart d) const
{
	return MAP::template getInvolution<0>(d);
}

template <class MAP>
template <int N>
inline Dart Map2<MAP>::phi(Dart d) const
{
	assert( (N > 0) || !"negative parameters not allowed in template multi-phi");
	if (N < 10)
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
inline Dart Map2<MAP>::alpha0(Dart d) const
{
	return phi2(d) ;
}

template <class MAP>
inline Dart Map2<MAP>::alpha1(Dart d) const
{
	return phi2(this->phi_1(d)) ;
}

template <class MAP>
inline Dart Map2<MAP>::alpha_1(Dart d) const
{
	return this->phi1(phi2(d)) ;
}

template <class MAP>
inline Dart Map2<MAP>::phi2_1(Dart d) const
{
	return phi2(this->phi_1(d)) ;
}

template <class MAP>
inline Dart Map2<MAP>::phi12(Dart d) const
{
	return phi1(phi2(d)) ;
}

template <class MAP>
inline void Map2<MAP>::phi2sew(Dart d, Dart e)
{
	MAP::template involutionSew<0>(d,e);
}

template <class MAP>
inline void Map2<MAP>::phi2unsew(Dart d)
{
	MAP::template involutionUnsew<0>(d);
}

/*! @name Generator and Deletor
 *  To generate or delete faces in a 2-map
 *************************************************************************/

template <class MAP>
Dart Map2<MAP>::newPolyLine(unsigned int nbEdges)
{
	Dart d = ParentMap::newCycle(2*nbEdges);
	{
		Dart it1 = d;
		Dart it2 = this->phi_1(d);
		for(unsigned int i = 0; i < nbEdges ; ++i)
		{
			phi2sew(it1, it2);
			it1 = this->phi1(it1);
			it2 = this->phi_1(it2);
		}
	}
	return d;
}

template <class MAP>
Dart Map2<MAP>::newFace(unsigned int nbEdges, bool withBoundary)
{
	Dart d = ParentMap::newCycle(nbEdges);
	if (withBoundary)
	{
		Dart e = newBoundaryCycle(nbEdges);

		Dart it = d;
		do
		{
			phi2sew(it, e);
			it = this->phi1(it);
			e = this->phi_1(e);
		} while (it != d);
	}
	return d;
}

template <class MAP>
void Map2<MAP>::deleteFace(Dart d, bool withBoundary)
{
	assert(!this->isBoundaryMarked2(d)) ;
	if (withBoundary)
	{
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
		return;
	}
	//else with remove the face and create fixed points
	Dart it = d ;
	do
	{
		phi2unsew(it);
		it = this->phi1(it) ;
	} while(it != d) ;
	ParentMap::deleteCycle(d);
}

template <class MAP>
void Map2<MAP>::deleteCC(Dart d)
{
	DartMarkerStore mark(*this);

	std::vector<Dart> visited;
	visited.reserve(1024) ;
	visited.push_back(d);
	mark.mark(d) ;

	for(unsigned int i = 0; i < visited.size(); ++i)
	{
		Dart d1 = this->phi1(visited[i]) ;
		if(!mark.isMarked(d1))
		{
			visited.push_back(d1) ;
			mark.mark(d1);
		}
		Dart d2 = phi2(visited[i]) ;
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
void Map2<MAP>::fillHole(Dart d)
{
	assert(isBoundaryEdge(d)) ;
	Dart dd = d ;
	if(!this->isBoundaryMarked2(dd))
		dd = phi2(dd) ;
	this->template boundaryUnmarkOrbit<FACE,2>(dd) ;
}

template <class MAP>
void Map2<MAP>::createHole(Dart d)
{
	assert(!isBoundaryEdge(d)) ;
	this->template boundaryMarkOrbit<FACE,2>(d) ;
}

/*! @name Topological Operators
 *  Topological operations on 2-maps
 *************************************************************************/

template <class MAP>
void Map2<MAP>::splitVertex(Dart d, Dart e)
{
	assert(sameVertex(d, e)) ;
	Dart d2 = phi2(d) ; assert(d != d2) ;
	Dart e2 = phi2(e) ; assert(e != e2) ;
	Dart nd = ParentMap::cutEdge(d2) ;	// Cut the edge of dd (make a new half edge)
	Dart ne = ParentMap::cutEdge(e2) ;	// Cut the edge of ee (make a new half edge)
	phi2sew(nd, ne) ;					// Sew the two faces along the new edge
}

template <class MAP>
Dart Map2<MAP>::deleteVertex(Dart d)
{
	//TODO utile ?
	if(isBoundaryVertex(d))
		return NIL ;

	Dart res = NIL ;
	Dart vit = d ;
	do
	{
		if(res == NIL && this->phi1(this->phi1(d)) != d)
			res = this->phi1(d) ;

		Dart f = this->phi_1(phi2(vit)) ;
		this->phi1sew(vit, f) ;

		vit = phi2(this->phi_1(vit)) ;
	} while(vit != d) ;
	ParentMap::deleteCycle(d) ;
	return res ;
}

template <class MAP>
Dart Map2<MAP>::cutEdge(Dart d)
{
	Dart e = phi2(d);
	phi2unsew(d);						// remove old phi2 links
	Dart nd = ParentMap::cutEdge(d);	// Cut the 1-edge of d
	Dart ne = ParentMap::cutEdge(e);	// Cut the 1-edge of phi2(d)
	phi2sew(d, ne);						// Correct the phi2 links
	phi2sew(e, nd);
	return nd;
}

template <class MAP>
bool Map2<MAP>::uncutEdge(Dart d)
{
	if(vertexDegree(this->phi1(d)) == 2)
	{
		Dart e = phi2(this->phi1(d)) ;
		phi2unsew(e) ;
		phi2unsew(d) ;
		ParentMap::uncutEdge(d) ;
		ParentMap::uncutEdge(e) ;
		phi2sew(d, e) ;
		return true ;
	}
	return false ;
}

template <class MAP>
Dart Map2<MAP>::collapseEdge(Dart d, bool delDegenerateFaces)
{
	Dart resV = NIL ;

	Dart e = phi2(d);
	phi2unsew(d);	// Unlink the opposite edges

	Dart f = this->phi1(e) ;
	Dart h = phi2(this->phi_1(e));

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
		h = phi2(this->phi_1(d));
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
bool Map2<MAP>::flipEdge(Dart d)
{
	if (!isBoundaryEdge(d))
	{
		Dart e = phi2(d);
		Dart dNext = this->phi1(d);
		Dart eNext = this->phi1(e);
		Dart dPrev = this->phi_1(d);
		Dart ePrev = this->phi_1(e);
		this->phi1sew(d, ePrev);	// Detach the two
		this->phi1sew(e, dPrev);	// vertices of the edge
		this->phi1sew(d, dNext);	// Insert the edge in its
		this->phi1sew(e, eNext);	// new vertices after flip
		return true ;
	}
	return false ; // cannot flip a border edge
}

template <class MAP>
bool Map2<MAP>::flipBackEdge(Dart d)
{
	if (!isBoundaryEdge(d))
	{
		Dart e = phi2(d);
		Dart dPrev = this->phi_1(d);
		Dart ePrev = this->phi_1(e);
		this->phi1sew(d, ePrev);		// Detach the two
		this->phi1sew(e, dPrev);		// vertices of the edge
		this->phi1sew(e, this->phi_1(dPrev));	// Insert the edge in its
		this->phi1sew(d, this->phi_1(ePrev));	// new vertices after flip
		return true ;
	}
	return false ; // cannot flip a border edge
}

template <class MAP>
void Map2<MAP>::swapEdges(Dart d, Dart e)
{
	assert(!Map2::isBoundaryEdge(d) && !Map2::isBoundaryEdge(e));

	Dart d2 = phi2(d);
	Dart e2 = phi2(e);

	phi2unsew(d);
	phi2unsew(e) ;

	phi2sew(d, e);
	phi2sew(d2, e2);
}

template <class MAP>
void Map2<MAP>::insertEdgeInVertex(Dart d, Dart e)
{
	assert(!sameVertex(d,e));
	assert(phi2(e) == this->phi_1(e));
	this->phi1sew(this->phi_1(d), this->phi_1(e));
}

template <class MAP>
bool Map2<MAP>::removeEdgeFromVertex(Dart d)
{
	if (!isBoundaryEdge(d))
	{
		this->phi1sew(this->phi_1(d), phi2(d)) ;
		return true ;
	}
	return false ;
}

template <class MAP>
void Map2<MAP>::sewFaces(Dart d, Dart e, bool withBoundary)
{
	// if sewing with fixed points
	if (!withBoundary)
	{
		assert(phi2(d) == d && phi2(e) == e) ;
		phi2sew(d, e) ;
		return ;
	}

	assert(isBoundaryEdge(d) && isBoundaryEdge(e)) ;

	Dart dd = phi2(d) ;
	Dart ee = phi2(e) ;

	phi2unsew(d) ;	// unsew faces from boundary
	phi2unsew(e) ;

	if (ee != this->phi_1(dd))
		this->phi1sew(ee, this->phi_1(dd)) ;	// remove the boundary edge
	if (dd != this->phi_1(ee))
		this->phi1sew(dd, this->phi_1(ee)) ;	// and properly close incident boundaries
	ParentMap::deleteCycle(dd) ;

	phi2sew(d, e) ; // sew the faces
}

template <class MAP>
void Map2<MAP>::unsewFaces(Dart d, bool withBoundary)
{
	if (!withBoundary)
	{
		phi2unsew(d) ;
		return ;
	}

	assert(!Map2::isBoundaryEdge(d)) ;

	Dart dd = phi2(d) ;

	Dart e = newBoundaryCycle(2) ;
	Dart ee = this->phi1(e) ;

	Dart f = findBoundaryEdgeOfVertex(d) ;
	Dart ff = findBoundaryEdgeOfVertex(dd) ;

	if(f != NIL)
		this->phi1sew(e, this->phi_1(f)) ;

	if(ff != NIL)
		this->phi1sew(ee, this->phi_1(ff)) ;

	phi2unsew(d) ;

	phi2sew(d, e) ;		// sew faces
	phi2sew(dd, ee) ;	// to the boundary
}

template <class MAP>
bool Map2<MAP>::collapseDegeneratedFace(Dart d)
{
	Dart e = this->phi1(d) ;		// Check if the face is degenerated
	if (this->phi1(e) == d)			// Yes: it contains one or two edge(s)
	{
		Dart d2 = phi2(d) ;			// Check opposite edges
		Dart e2 = phi2(e) ;
		phi2unsew(d) ;
		if(d != e)
		{
			phi2unsew(e) ;
			phi2sew(d2, e2) ;
		}
		else
		{
			this->phi1sew(d2, this->phi_1(d2)) ;
			ParentMap::deleteCycle(d2) ;
		}
		ParentMap::deleteCycle(d) ;
		return true ;
	}
	return false ;
}

template <class MAP>
void Map2<MAP>::splitFace(Dart d, Dart e)
{
	assert(d != e && Map2<MAP>::sameFace(d, e)) ;
	Dart dd = ParentMap::cutEdge(this->phi_1(d)) ;
	Dart ee = ParentMap::cutEdge(this->phi_1(e)) ;
	ParentMap::splitCycle(dd, ee) ;
	phi2sew(dd, ee);
}

template <class MAP>
bool Map2<MAP>::mergeFaces(Dart d)
{
	if (!isBoundaryEdge(d))
	{
		Dart e = phi2(d) ;
		phi2unsew(d) ;
		ParentMap::mergeCycles(d, this->phi1(e)) ;
		ParentMap::splitCycle(e, this->phi1(d)) ;
		ParentMap::deleteCycle(d) ;
		return true ;
	}
	return false ;
}

template <class MAP>
void Map2<MAP>::extractTrianglePair(Dart d)
{
	Dart e = phi2(d) ;

	assert(!isBoundaryFace(d) && !isBoundaryFace(e)) ;
	assert(faceDegree(d) == 3 && faceDegree(e) == 3) ;

	Dart d1 = phi2(this->phi1(d)) ;
	Dart d2 = phi2(this->phi_1(d)) ;
	phi2unsew(d1) ;
	phi2unsew(d2) ;
	phi2sew(d1, d2) ;

	Dart e1 = phi2(this->phi1(e)) ;
	Dart e2 = phi2(this->phi_1(e)) ;
	phi2unsew(e1) ;
	phi2unsew(e2) ;
	phi2sew(e1, e2) ;
}

template <class MAP>
void Map2<MAP>::insertTrianglePair(Dart d, Dart v1, Dart v2)
{
	Dart e = phi2(d) ;

	assert(v1 != v2 && sameOrientedVertex(v1, v2)) ;
	assert(faceDegree(d) == 3 && faceDegree(phi2(d)) == 3) ;

	Dart vv1 = phi2(v1) ;
	phi2unsew(v1) ;
	phi2sew(this->phi_1(d), v1) ;
	phi2sew(this->phi1(d), vv1) ;

	Dart vv2 = phi2(v2) ;
	phi2unsew(v2) ;
	phi2sew(this->phi_1(e), v2) ;
	phi2sew(this->phi1(e), vv2) ;
}

template <class MAP>
bool Map2<MAP>::mergeVolumes(Dart d, Dart e, bool deleteFace)
{
	assert(!this->isBoundaryMarked2(d) && !this->isBoundaryMarked2(e)) ;

	if (Map2<MAP>::isBoundaryFace(d) || Map2<MAP>::isBoundaryFace(e))
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
		phi2unsew(d2);		// Unlink the two adjacent faces from dNext and eNext
		phi2unsew(e2);
		phi2sew(d2, e2);	// Link the two adjacent faces together
	}

	if(deleteFace)
	{
		ParentMap::deleteCycle(d);		// Delete the two alone faces
		ParentMap::deleteCycle(e);
	}

	return true ;
}

template <class MAP>
void Map2<MAP>::splitSurface(std::vector<Dart>& vd, bool firstSideClosed, bool secondSideClosed)
{
//	assert(checkSimpleOrientedPath(vd)) ;

	Dart e = vd.front() ;
	Dart e2 = phi2(e) ;

	//unsew the edge path
	for(std::vector<Dart>::iterator it = vd.begin() ; it != vd.end() ; ++it)
	{
		//if(!Map2<MAP>::isBoundaryEdge(*it))
			unsewFaces(*it) ;
	}

	if(firstSideClosed)
		Map2<MAP>::fillHole(e) ;

	if(secondSideClosed)
		Map2<MAP>::fillHole(e2) ;
}

/*! @name Topological Queries
 *  Return or set various topological information
 *************************************************************************/

template <class MAP>
bool Map2<MAP>::sameOrientedVertex(Dart d, Dart e) const
{
	Dart it = d;				// Foreach dart dNext in the vertex of d
	do
	{
		if (it == e)			// Test equality with e
			return true;
		it = phi2(this->phi_1(it));
	} while (it != d);
	return false;				// None is equal to e => vertices are distinct
}

template <class MAP>
inline bool Map2<MAP>::sameVertex(Dart d, Dart e) const
{
	return sameOrientedVertex(d, e) ;
}

template <class MAP>
unsigned int Map2<MAP>::vertexDegree(Dart d) const
{
	unsigned int count = 0 ;
	Dart it = d ;
	do
	{
		++count ;
		it = phi2(this->phi_1(it)) ;
	} while (it != d) ;
	return count ;
}

template <class MAP>
int Map2<MAP>::checkVertexDegree(Dart d, unsigned int vd) const
{
	unsigned int count = 0 ;
	Dart it = d ;
	do
	{
		++count ;
		it = phi2(this->phi_1(it)) ;
	} while ((count <= vd) && (it != d)) ;

	return count - vd;
}

template <class MAP>
bool Map2<MAP>::isBoundaryVertex(Dart d) const
{
	Dart it = d ;
	do
	{
		if (this->isBoundaryMarked2(it))
			return true ;
		it = phi2(this->phi_1(it)) ;
	} while (it != d) ;
	return false ;
}

template <class MAP>
Dart Map2<MAP>::findBoundaryEdgeOfVertex(Dart d) const
{
	Dart it = d ;
	do
	{
		if (this->isBoundaryMarked2(it))
			return it ;
		it = phi2(this->phi_1(it)) ;
	} while (it != d) ;
	return NIL ;
}

template <class MAP>
inline bool Map2<MAP>::sameEdge(Dart d, Dart e) const
{
	return d == e || phi2(d) == e ;
}

template <class MAP>
inline bool Map2<MAP>::isBoundaryEdge(Dart d) const
{
	return this->isBoundaryMarked2(d) || this->isBoundaryMarked2(phi2(d));
}

template <class MAP>
inline bool Map2<MAP>::sameOrientedFace(Dart d, Dart e) const
{
	return ParentMap::sameCycle(d, e) ;
}

template <class MAP>
inline bool Map2<MAP>::sameFace(Dart d, Dart e) const
{
	return sameOrientedFace(d, e) ;
}

template <class MAP>
inline unsigned int Map2<MAP>::faceDegree(Dart d) const
{
	return ParentMap::cycleDegree(d) ;
}

template <class MAP>
inline int Map2<MAP>::checkFaceDegree(Dart d, unsigned int le) const
{
	return ParentMap::checkCycleDegree(d,le) ;
}

template <class MAP>
bool Map2<MAP>::isBoundaryFace(Dart d) const
{
	Dart it = d ;
	do
	{
		if (this->isBoundaryMarked2(phi2(it)))
			return true ;
		it = this->phi1(it) ;
	} while (it != d) ;
	return false ;
}

template <class MAP>
Dart Map2<MAP>::findBoundaryEdgeOfFace(Dart d) const
{
	Dart it = d ;
	do
	{
		if (this->isBoundaryMarked2(phi2(it)))
			return phi2(it) ;
		it = this->phi1(it) ;
	} while (it != d) ;
	return NIL ;
}

template <class MAP>
bool Map2<MAP>::sameOrientedVolume(Dart d, Dart e) const
{
	DartMarkerStore mark(*this);	// Lock a marker

	std::list<Dart> visitedFaces;	// Faces that are traversed
	visitedFaces.push_back(d);		// Start with the face of d
	std::list<Dart>::iterator face;

	// For every face added to the list
	for (face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		if (!this->isBoundaryMarked2(*face) && !mark.isMarked(*face))		// Face has not been visited yet
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
inline bool Map2<MAP>::sameVolume(Dart d, Dart e) const
{
	return sameOrientedVolume(d, e) ;
}

template <class MAP>
unsigned int Map2<MAP>::volumeDegree(Dart d) const
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
		if (!this->isBoundaryMarked2(df) && !mark.isMarked(df))		// Face has not been visited yet
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
int Map2<MAP>::checkVolumeDegree(Dart d, unsigned int volDeg) const
{
	unsigned int count = 0;
	DartMarkerStore mark(*this);		// Lock a marker

	std::vector<Dart> visitedFaces;		// Faces that are traversed
	visitedFaces.reserve(16);
	visitedFaces.push_back(d);			// Start with the face of d

	// For every face added to the list
	for (unsigned int i = 0; i != visitedFaces.size(); ++i)
	{
		Dart df = visitedFaces[i];
		if (!this->isBoundaryMarked2(df) && !mark.isMarked(df))		// Face has not been visited yet
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
bool Map2<MAP>::isTriangular() const
{
	TraversorF<Map2<MAP> > t(*this) ;
	for(Dart d = t.begin(); d != t.end(); d = t.next())
	{
		if(faceDegree(d) != 3)
			return false ;
	}
	return true ;
}

template <class MAP>
bool Map2<MAP>::check() const
{
	CGoGNout << "Check: topology begin" << CGoGNendl;
	DartMarker m(*this);
	for(Dart d = Map2::begin(); d != Map2::end(); Map2::next(d))
	{
		Dart d2 = phi2(d);
		if (phi2(d2) != d)	// phi2 involution ?
		{
			CGoGNout << "Check: phi2 is not an involution" << CGoGNendl;
			return false;
		}
		if(d2 == d)
		{
			CGoGNout << "Check: phi2 fixed point" << CGoGNendl;
			return false;
		}

		Dart d1 = this->phi1(d);
		if (this->phi_1(d1) != d)	// phi1 a une image correcte ?
		{
			CGoGNout << "Check: inconsistent phi_1 link" << CGoGNendl;
			return false;
		}

		if (m.isMarked(d1))	// phi1 a un seul antécédent ?
		{
			CGoGNout << "Check: dart with two phi1 predecessors" << CGoGNendl;
			return false;
		}
		m.mark(d1);

		if (d1 == d)
			CGoGNout << "Check: (warning) face loop (one edge)" << CGoGNendl;
		if (this->phi1(d1) == d)
			CGoGNout << "Check: (warning) face with only two edges" << CGoGNendl;
		if (phi2(d1) == d)
			CGoGNout << "Check: (warning) dangling edge" << CGoGNendl;
	}

	for(Dart d = Map2::begin(); d != Map2::end(); Map2::next(d))
	{
		if (!m.isMarked(d))	// phi1 a au moins un antécédent ?
		{
			CGoGNout << "Check: dart with no phi1 predecessor" << CGoGNendl;
			return false;
		}
	}

	CGoGNout << "Check: topology ok" << CGoGNendl;

	return true;
}

template <class MAP>
bool Map2<MAP>::checkSimpleOrientedPath(std::vector<Dart>& vd)
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
bool Map2<MAP>::foreach_dart_of_vertex(Dart d, FunctorType& f, unsigned int /*thread*/) const
{
	Dart dNext = d;
	do
	{
		if (f(dNext))
			return true;
		dNext = phi2(this->phi_1(dNext));
	} while (dNext != d);
	return false;
}

template <class MAP>
bool Map2<MAP>::foreach_dart_of_edge(Dart d, FunctorType& fonct, unsigned int /*thread*/) const
{
	if (fonct(d))
		return true;
	return fonct(phi2(d));
}

template <class MAP>
inline bool Map2<MAP>::foreach_dart_of_face(Dart d, FunctorType& f, unsigned int thread) const
{
	return ParentMap::foreach_dart_of_cc(d, f, thread);
}

template <class MAP>
inline bool Map2<MAP>::foreach_dart_of_volume(Dart d, FunctorType& f, unsigned int thread) const
{
	return foreach_dart_of_cc(d, f, thread);
}

template <class MAP>
bool Map2<MAP>::foreach_dart_of_cc(Dart d, FunctorType& f, unsigned int thread) const
{
	DartMarkerStore mark(*this, thread);	// Lock a marker
	bool found = false;				// Last functor return value

	std::vector<Dart> visitedFaces;	// Faces that are traversed
	visitedFaces.reserve(1024) ;
	visitedFaces.push_back(d);		// Start with the face of d

	// For every face added to the list
	for(unsigned int i = 0; !found && i < visitedFaces.size(); ++i)
	{
		if (!mark.isMarked(visitedFaces[i]))	// Face has not been visited yet
		{
			// Apply functor to the darts of the face
			found = Map2::foreach_dart_of_face(visitedFaces[i], f);

			// If functor returns false then mark visited darts (current face)
			// and add non visited adjacent faces to the list of face
			if (!found)
			{
				Dart e = visitedFaces[i] ;
				do
				{
					mark.mark(e);				// Mark
					Dart adj = phi2(e);			// Get adjacent face
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
inline bool Map2<MAP>::foreach_dart_of_vertex1(Dart d, FunctorType& f, unsigned int thread) const
{
	return ParentMap::foreach_dart_of_vertex(d,f,thread);
}

template <class MAP>
inline bool Map2<MAP>::foreach_dart_of_edge1(Dart d, FunctorType& f, unsigned int thread) const
{
	return ParentMap::foreach_dart_of_edge(d,f,thread);
}

/*! @name Close map after import or creation
 *  These functions must be used with care, generally only by import/creation algorithms
 *************************************************************************/

template <class MAP>
Dart Map2<MAP>::newBoundaryCycle(unsigned int nbE)
{
	Dart d = ParentMap::newCycle(nbE);
	this->template boundaryMarkOrbit<FACE,2>(d);
	return d;
}

template <class MAP>
unsigned int Map2<MAP>::closeHole(Dart d, bool forboundary)
{
	assert(phi2(d) == d);		// Nothing to close

	Dart first = this->newDart();	// First edge of the face that will fill the hole
	unsigned int countEdges = 1;

	phi2sew(d, first);	// phi2-link the new edge to the hole

	Dart dNext = d;	// Turn around the hole
	Dart dPhi1;		// to complete the face
	do
	{
		do
		{
			dPhi1 = this->phi1(dNext);	// Search and put in dNext
			dNext = phi2(dPhi1);		// the next dart of the hole
		} while (dNext != dPhi1 && dPhi1 != d);

		if (dPhi1 != d)
		{
			Dart next = this->newDart();	// Add a new edge there and link it to the face
			++countEdges;
			this->phi1sew(first, next);	// the edge is linked to the face
			phi2sew(dNext, next);		// the face is linked to the hole
		}
	} while (dPhi1 != d);

	if (forboundary)
		this->template boundaryMarkOrbit<FACE,2>(phi2(d));

	return countEdges ;
}

template <class MAP>
unsigned int Map2<MAP>::closeMap(bool forboundary)
{
	// Search the map for topological holes (fix points of phi2)
	unsigned int nb = 0 ;
	for (Dart d = this->begin(); d != this->end(); this->next(d))
	{
		if (phi2(d) == d)
		{
			++nb ;
			closeHole(d, forboundary);
		}
	}
	return nb ;
}

/*! @name Compute dual
 * These functions compute the dual mesh
 *************************************************************************/

template <class MAP>
void Map2<MAP>::reverseOrientation()
{
	DartAttribute<unsigned int> emb0(this, this->template getEmbeddingAttributeVector<VERTEX>()) ;
	if(emb0.isValid())
	{
		DartAttribute<unsigned int> new_emb0 = this->template addAttribute<unsigned int, DART>("new_EMB_0") ;
		for(Dart d = this->begin(); d != this->end(); this->next(d))
			new_emb0[d] = emb0[this->phi1(d)] ;

		this->template swapAttributes<unsigned int>(emb0, new_emb0) ;
		this->removeAttribute(new_emb0) ;
	}

	DartAttribute<Dart> n_phi1 = this->template getAttribute<Dart, DART>("phi1") ;
	DartAttribute<Dart> n_phi_1 = this->template getAttribute<Dart, DART>("phi_1") ;
	this->template swapAttributes<Dart>(n_phi1, n_phi_1) ;
}

template <class MAP>
void Map2<MAP>::computeDual()
{
	DartAttribute<Dart> old_phi1 = this->template getAttribute<Dart, DART>("phi1");
	DartAttribute<Dart> old_phi_1 = this->template getAttribute<Dart, DART>("phi_1") ;
	DartAttribute<Dart> new_phi1 = this->template addAttribute<Dart, DART>("new_phi1") ;
	DartAttribute<Dart> new_phi_1 = this->template addAttribute<Dart, DART>("new_phi_1") ;

	for(Dart d = this->begin(); d != this->end(); this->next(d))
	{
		Dart dd = this->phi1(phi2(d));

		new_phi1[d] = dd ;
		new_phi_1[dd] = d ;
	}

	this->template swapAttributes<Dart>(old_phi1, new_phi1) ;
	this->template swapAttributes<Dart>(old_phi_1, new_phi_1) ;

	this->removeAttribute(new_phi1) ;
	this->removeAttribute(new_phi_1) ;

	this->swapEmbeddingContainers(VERTEX, FACE) ;

	reverseOrientation() ;

	// boundary management
	for(Dart d = this->begin(); d != this->end(); this->next(d))
	{
		if(this->isBoundaryMarked2(d))
		{
			this->template boundaryMarkOrbit<FACE,2>(deleteVertex(phi2(d)));
		}
	}
}

} // namespace CGoGN
