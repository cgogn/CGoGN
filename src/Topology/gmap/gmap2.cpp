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

#include "Topology/gmap/gmap2.h"

namespace CGoGN
{

/*! @name Constructors and Destructors
 *  To generate or delete cells in a 2-G-map
 *************************************************************************/

void GMap2::deleteFace(Dart d)
{
	Dart e = d ;
	do
	{
		phi2unsew(e) ;	// unsew the face of d
		e = phi1(e) ;	// from all its adjacent faces
	} while (e != d) ;
	GMap1::deleteFace(d);	// delete the face
}

/*! @name Topological Operators
 *  Topological operations on 2-G-maps
 *************************************************************************/

void GMap2::splitVertex(Dart d, Dart e)
{
	assert(sameOrientedVertex(d, e));
	Dart dd = phi2(d) ;
	Dart ee = phi2(e) ;
	GMap1::cutEdge(dd);			// Cut the edge of dd (make a new edge)
	GMap1::cutEdge(ee);			// Cut the edge of ee (make a new edge)
	phi2sew(phi1(dd),phi1(ee));	// Sew the two faces along the new edge
}

bool GMap2::deleteVertex(Dart d)
{
	if(isBoundaryVertex(d))
		return false ;

	Dart vit = d ;
	do
	{
		Dart f = phi_1(phi2(vit)) ;
		phi1sew(vit, f) ;
		vit = alpha1(vit) ;
	} while(vit != d) ;
	GMap1::deleteFace(d) ;

	return true ;
}

void GMap2::cutEdge(Dart d)
{
	GMap1::cutEdge(d);		// Cut the edge of d
	Dart nd = phi1(d);
	Dart e = beta2(d);
	if (e != d)				// Test if an opposite edge exists
	{
		GMap1::cutEdge(e);	// Cut the opposite edge
		Dart ne = phi1(e);
		phi2sew(nd, beta0(ne));	// sew the two new edges
	}
}

Dart GMap2::collapseEdge(Dart d, bool delDegenerateFaces = true)
{
	Dart resV ;

	Dart e = phi2(d);
	if (e != d)			// Test if an opposite edge exists
	{
		phi2unsew(d);	// Unlink the opposite edges
		Dart f = phi1(e) ;
		Dart g = phi_1(e) ;

		if(f != d && !isFaceTriangle(e))
			resV = f ;
		else if(phi2(g) != g)
			resV = phi2(g) ;
		else if(f != d && phi2(f) != f)
			resV = phi1(phi2(f)) ;

		if (f != e && delDegenerateFaces)
		{
			GMap1::collapseEdge(e);		// Collapse edge e
			collapseDegeneratedFace(f);	// and collapse its face if degenerated
		}
		else
			GMap1::collapseEdge(e);	// Just collapse edge e
	}

	Dart f = phi1(d) ;
	Dart g = phi_1(d) ;

	if(resV == Dart::nil())
	{
		if(!isFaceTriangle(d))
			resV = f ;
		else if(phi2(g) != g)
			resV = phi2(g) ;
		else if(phi2(f) != f)
			resV = phi1(phi2(f)) ;
	}

	if (f != d && delDegenerateFaces)
	{
		GMap1::collapseEdge(d);		// Collapse edge d
		collapseDegeneratedFace(f);	// and collapse its face if degenerated
	}
	else
		GMap1::collapseEdge(d);	// Just collapse edge d

	return resV ;
}

bool GMap2::flipEdge(Dart d)
{
	Dart e = phi2(d);			// Test if an opposite
	if (e != d)					// edge exists
	{
		Dart dNext = phi1(d);
		Dart eNext = phi1(e);
		Dart dPrev = phi_1(d);
		Dart ePrev = phi_1(e);
		phi1sew(d, ePrev);		// Detach the two
		phi1sew(e, dPrev);		// vertices of the edge
		phi1sew(d, dNext);		// Insert the edge in its
		phi1sew(e, eNext);		// new vertices after flip
		return true ;
	}
	return false ; // cannot flip a border edge
}

bool GMap2::flipBackEdge(Dart d)
{
	Dart e = phi2(d);			// Test if an opposite
	if (e != d)					// edge exists
	{
		Dart dNext = phi1(d);
		Dart eNext = phi1(e);
		Dart dPrev = phi_1(d);
		Dart ePrev = phi_1(e);
		phi1sew(d, ePrev);			// Detach the two
		phi1sew(e, dPrev);			// vertices of the edge
		phi1sew(e, phi_1(dPrev));	// Insert the edge in its
		phi1sew(d, phi_1(ePrev));	// new vertices after flip
		return true ;
	}
	return false ; // cannot flip a border edge
}

void GMap2::sewFaces(Dart d, Dart e)
{
	phi2sew(d, e);
}

void GMap2::unsewFaces(Dart d)
{
	phi2unsew(d);
}

bool GMap2::collapseDegeneratedFace(Dart d)
{
	Dart e = phi1(d);				// Check if the face is a loop
	if (phi1(e) == d)				// Yes: it contains one or two edge(s)
	{
		Dart d2 = beta2(d);			// Check opposite edges
		Dart e2 = beta2(e);
		if (d2 != d) phi2unsew(d);	// Update phi2-links if needed
		if (e2 != e) phi2unsew(e);
		if (d2 != d && e2 != e)
			phi2sew(d2, e2);
		GMap1::deleteFace(d);		// Delete the single edge or two edges of the loop
		return true ;
	}
	return false ;
}

void GMap2::splitFace(Dart d, Dart e)
{
	GMap1::splitFace(d, e);			// Split the face
	phi2sew(beta1(d), beta1(e));	// Sew the two resulting faces along the new edge
}

bool GMap2::mergeFaces(Dart d)
{
	Dart e = phi2(d) ;
	if(e != d)
	{
		phi2unsew(d);			// unsew the face of d
		GMap1::mergeFaces(d, e); // merge the two faces along edges of d and e
		return true ;
	}
	return false ;
}

void GMap2::extractTrianglePair(Dart d)
{
	assert(isFaceTriangle(d)) ;
	Dart e = phi2(d) ;
	if(e != d)
	{
		assert(isFaceTriangle(e)) ;
		Dart e1 = phi2(phi1(e)) ;
		Dart e2 = phi2(phi_1(e)) ;
		phi2unsew(e1) ;
		phi2unsew(e2) ;
		phi2sew(e1, e2) ;
	}
	Dart d1 = phi2(phi1(d)) ;
	Dart d2 = phi2(phi_1(d)) ;
	phi2unsew(d1) ;
	phi2unsew(d2) ;
	phi2sew(d1, d2) ;
}

void GMap2::insertTrianglePair(Dart d, Dart v1, Dart v2)
{
	assert(v1 != v2 && sameOrientedVertex(v1, v2)) ;
	assert(isFaceTriangle(d) && phi2(phi1(d)) == phi1(d) && phi2(phi_1(d)) == phi_1(d)) ;
	Dart e = phi2(d) ;
	if(e != d)
	{
		assert(isFaceTriangle(e) && phi2(phi1(e)) == phi1(e) && phi2(phi_1(e)) == phi_1(e)) ;
		Dart vv2 = phi2(v2) ;
		phi2unsew(v2) ;
		phi2sew(phi_1(e), v2) ;
		phi2sew(phi1(e), vv2) ;
	}
	Dart vv1 = phi2(v1) ;
	phi2unsew(v1) ;
	phi2sew(phi_1(d), v1) ;
	phi2sew(phi1(d), vv1) ;
}

//	TODO check this function
bool GMap2::mergeVolumes(Dart d, Dart e)
{
	// First traversal of both faces to check the face sizes
	// and stored their edges to efficiently access them further
	std::vector<Dart> dDarts;
	std::vector<Dart> eDarts;
	dDarts.reserve(16);	// usual faces have less than 16 edges
	eDarts.reserve(16);

	Dart dFit = d ;
	Dart eFit = phi1(e) ; 	// must take phi1 because of the use
	do						// of reverse iterator for sewing loop
	{
		dDarts.push_back(dFit) ;
		dFit = phi1(dFit) ;
	} while(dFit != d) ;
	do
	{
		eDarts.push_back(eFit) ;
		eFit = phi1(eFit) ;
	} while(eFit != phi1(e)) ;

	if(dDarts.size() != eDarts.size())
		return false ;

	// Make the sewing: take darts in initial order (clockwise) in first face
	// and take darts in reverse order (counter-clockwise) in the second face
	std::vector<Dart>::iterator dIt;
	std::vector<Dart>::reverse_iterator eIt;
	for (dIt = dDarts.begin(), eIt = eDarts.rbegin(); dIt != dDarts.end(); ++dIt, ++eIt)
	{
		Dart d2 = phi2(*dIt);			// Search the faces adjacent to dNext and eNext
		Dart e2 = phi2(*eIt);
		if (d2 != *dIt) phi2unsew(d2);	// Unlink the two adjacent faces from dNext and eNext
		if (e2 != *eIt) phi2unsew(e2);
		if (d2 != *dIt && e2 != *eIt) phi2sew(d2,e2); // Link the two adjacent faces together if they exists
	}
	GMap1::deleteFace(d);	// Delete the two alone faces
	GMap1::deleteFace(e);

	return true ;
}

//	TODO check this function
unsigned int GMap2::closeHole(Dart d)
{
	assert(phi2(d) == d);	// Nothing to close

	Dart first = GMap1::newEdge();	// First edge of the face that will fill the hole
	unsigned int countEdges = 1;

	phi2sew(d, first);	// phi2-link the new edge to the hole

	Dart dNext = d;	// Turn around the hole
	Dart dPhi1;		// to complete the face
	do
	{
		do
		{
			dPhi1 = phi1(dNext);	// Search and put in dNext
			dNext = phi2(dPhi1);	// the next dart of the hole
		} while (dNext != dPhi1 && dPhi1 != d);

		if (dPhi1 != d)
		{
			Dart next = newDart();	// Add a new edge there and link it to the face
			++countEdges;
			phi1sew(first, next);	// the edge is linked to the face
			phi2sew(dNext, next);	// the face is linked to the hole
		}
	} while (dPhi1 != d);

	if (countEdges == 2)
	{
		countEdges = 0 ;
		collapseDegeneratedFace(first); // if the closing face is 2-sided, collapse it
	}

	return countEdges ;
}

void GMap2::closeMap(DartMarker& marker)
{
	// Search the map for topological holes (fixed point for phi2)
	for (Dart d = begin(); d != end(); next(d))
	{
		if (phi2(d) == d)
		{
			closeHole(d);
			marker.markOrbit(FACE, phi2(d)) ;
		}
	}
}

//DartMarker GMap2::markCCOrientation(Dart d)
//{
//	// lock a marker
//	DartMarker markCC(*this);
//
//	// init algo with parameter dart
//	std::list<Dart> darts_list;
//	darts_list.push_back(d);
//	markCC.mark(d);
//
//	// use iterator for begin of not yet treated darts
//	std::list<Dart>::iterator beg = darts_list.begin();
//
//	// until all darts treated
//	while (beg != darts_list.end())
//	{
//		Dart d1 = *beg;
//		// add phi<1> and phi<2> successor if they are not yet marked
//		Dart d2 = beta2(beta0(d1));
//		Dart d3 = beta1(beta0(d1));
//
//		if (!markCC.isMarked(d2))
//		{
//			darts_list.push_back(d2);
//			markCC.mark(d2);
////			markCC.mark(beta0(d2));
//		}
//		if (!markCC.isMarked(d3))
//		{
//			darts_list.push_back(d3);
//			markCC.mark(d3);
////			markCC.mark(beta0(d3));
//		}
//		// step to next dart of list
//		beg++;
//	}
//	return markCC;
//}

/*! @name Topological Queries
 *  Return or set various topological information
 *************************************************************************/

bool GMap2::sameOrientedVertex(Dart d, Dart e)
{
	Dart dNext = d;				// Foreach dart dNext in the vertex of d
	do
	{
		if (dNext == e)			// Test equality with e
			return true;
		dNext = alpha1(dNext);
	} while (dNext != d);
	return false;				// None is equal to e => vertices are distinct
}

bool GMap2::sameVertex(Dart d, Dart e)
{
	Dart dNext = d;				// Foreach dart dNext in the vertex of d
	do
	{
		if (dNext == e || beta2(dNext) == e)	// Test equality with e
			return true;
		dNext = alpha1(dNext);
	} while (dNext != d);
	return false;				// None is equal to e => vertices are distinct
}

unsigned int GMap2::vertexDegree(Dart d)
{
	unsigned int count = 0 ;
	Dart dNext = d ;
	do
	{
		++count ;
		dNext = alpha1(dNext) ;
	} while (dNext != d) ;
	return count ;
}

bool GMap2::isBoundaryVertex(Dart d)
{
	Dart dNext = d ;
	do
	{
		if(beta2(dNext) == dNext)
			return true ;
		dNext = alpha1(dNext) ;
	} while (dNext != d) ;
	return false ;
}

bool GMap2::check()
{
	CGoGNout << "Check: topology begin" << CGoGNendl;
	for(Dart d = begin(); d != end(); next(d))
	{
		Dart dd = beta0(d);
		if (beta0(dd) != d)	// beta0 involution ?
		{
			CGoGNout << "Check: beta0 is not an involution" << CGoGNendl;
			return false;
		}

		dd = beta1(d);
		if (beta1(dd) != d)	// beta1 involution ?
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
	}

	CGoGNout << "Check: topology ok" << CGoGNendl;
	return true;
}

/*! @name Cell Functors
 *  Apply functors to all darts of a cell
 *************************************************************************/

bool GMap2::foreach_dart_of_oriented_vertex(Dart d, FunctorType& f, unsigned int thread)
{
	Dart dNext = d;
	do
	{
		if (f(dNext))
			return true;
		dNext = alpha1(dNext);
 	} while (dNext != d);
 	return false;
}

bool GMap2::foreach_dart_of_vertex(Dart d, FunctorType& f, unsigned int thread)
{
	return foreach_dart_of_oriented_vertex(d, f, thread) || foreach_dart_of_oriented_vertex(beta1(d), f, thread) ;
}

bool GMap2::foreach_dart_of_edge(Dart d, FunctorType& f, unsigned int thread)
{
	if (f(d))
		return true ;
	Dart e = beta0(d) ;
	if (f(e))
		return true ;

	Dart d2 = beta2(d) ;
	if (d2 != d)
	{
		if (f(d2))
			return true ;
		e = beta0(d2) ;
		if (f(e))
			return true ;
	}
	return false ;
}

bool GMap2::foreach_dart_of_cc(Dart d, FunctorType& f, unsigned int thread)
{
	bool found = false;
	// lock a marker
	DartMarkerStore markCC(*this,thread);

	// init algo with parameter dart
	std::list<Dart> darts_list;
	darts_list.push_back(d);
	markCC.mark(d);

	// use iterator for begin of not yet treated darts
	std::list<Dart>::iterator dit = darts_list.begin();

	// until all darts treated
	while (dit != darts_list.end())
	{
		Dart d1 = *dit;
		// add phi<1> and phi<2> successor if they are not yet marked
		Dart d2 = beta0(d1);
		Dart d3 = beta1(d1);
		Dart d4 = beta2(d1);
		if (!markCC.isMarked(d2))
		{
			darts_list.push_back(d2);
			markCC.mark(d2);
		}
		if (!markCC.isMarked(d3))
		{
			darts_list.push_back(d3);
			markCC.mark(d3);
		}
		if (!markCC.isMarked(d4))
		{
			darts_list.push_back(d4);
			markCC.mark(d4);
		}
		// step to next dart of list
		dit++;

		// apply functor
		if (f(d1))
		{	// set dit to stop traversal
			dit = darts_list.end();
			found = true;
		}
	}
	return found;
}

} // namespace CGoGN
