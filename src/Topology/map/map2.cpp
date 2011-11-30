/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2011, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.u-strasbg.fr/                                         *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include "Topology/map/map2.h"
#include "Topology/generic/traversorCell.h"
#include "Topology/generic/traversor2CC.h"

namespace CGoGN
{

/*! @name Boundary marker management
 *  Function used to merge boundary faces properly
 *************************************************************************/

//void Map2::mergeBoundaryFaces(Dart dd, Dart ee)
//{
//	if (ee != phi_1(dd))
//		phi1sew(ee, phi_1(dd)) ;
//	if (dd != phi_1(ee))
//		phi1sew(dd, phi_1(ee)) ;
//	deleteCycle(dd);
//}

//void Map2::mergeFaceWithBoundary(Dart d)
//{
//	std::vector<Dart> storeForLinkVertex;
//	std::vector<Dart> storeForLinkFace;
//
//	Dart it = d ;
//	do	// foreach vertex/edge of face
//	{
//		Dart e = phi2(it) ;
//		if(isBoundaryMarked(e))	// check if connection by edge
//		{
//			storeForLinkFace.push_back(it);
//			storeForLinkFace.push_back(e);
//		}
//		else
//		{
//			Dart f = findBoundaryEdgeOfVertex(alpha1(it));	// check if connection by vertex
//			if (f != it)
//			{
//				storeForLinkVertex.push_back(phi_1(it));
//				storeForLinkVertex.push_back(phi_1(f));
//			}
//		}
//		it = phi1(it) ;
//	} while (it != d) ;
//
//	// merge by vertices
//	while (!storeForLinkVertex.empty())
//	{
//		Dart a = storeForLinkVertex.back() ;
//		storeForLinkVertex.pop_back() ;
//		Dart b = storeForLinkVertex.back() ;
//		storeForLinkVertex.pop_back() ;
//		phi1sew(a, b);
//	}
//	//merge by faces
//	while (!storeForLinkFace.empty())
//	{
//		Dart a = storeForLinkVertex.back() ;
//		storeForLinkVertex.pop_back() ;
//		Dart b = storeForLinkVertex.back() ;
//		storeForLinkVertex.pop_back() ;
//		mergeBoundaryFaces(a, b);
//	}
//}

/*! @name Generator and Deletor
 *  To generate or delete faces in a 2-map
 *************************************************************************/

Dart Map2::newFace(unsigned int nbEdges, bool withBoundary)
{
	Dart d = Map1::newCycle(nbEdges);
	if (withBoundary)
	{
		Dart e = Map1::newBoundaryCycle(nbEdges);

		Dart it = d;
		do
		{
			phi2sew(it, e);
			it = phi1(it);
			e = phi_1(e);
		} while (it != d);
	}
	return d;
}

void Map2::deleteFace(Dart d)
{
	assert(!isBoundaryMarked(d)) ;
	Dart it = d ;
	do
	{
		if(!isBoundaryEdge(it))
			unsewFaces(it) ;
		it = phi1(it) ;
	} while(it != d) ;
	Dart dd = phi2(d) ;
	Map1::deleteCycle(d) ;
	Map1::deleteCycle(dd) ;
}

void Map2::deleteCC(Dart d)
{
	DartMarkerStore mark(*this);

	std::vector<Dart> visited;
	visited.reserve(1024) ;
	visited.push_back(d);
	std::vector<Dart>::iterator it;

	for (it = visited.begin(); it != visited.end(); ++it)
	{
		if (!mark.isMarked(*it))
		{
			Dart d1 = phi1(*it) ;
			if(!mark.isMarked(d1))
			{
				mark.mark(d1);
				visited.push_back(d1) ;
			}
			Dart d2 = phi2(*it) ;
			if(!mark.isMarked(d2))
			{
				mark.mark(d2);
				visited.push_back(d2) ;
			}
		}
	}

	for(it = visited.begin(); it != visited.end(); ++it)
		deleteDart(*it) ;
}

void Map2::fillHole(Dart d)
{
	assert(isBoundaryMarked(d)) ;
	boundaryUnmarkOrbit(FACE, d) ;
}

/*! @name Topological Operators
 *  Topological operations on 2-maps
 *************************************************************************/

void Map2::splitVertex(Dart d, Dart e)
{
	assert(sameOrientedVertex(d, e));
	Dart dd = phi2(d) ;
	Dart ee = phi2(e) ;
	Map1::cutEdge(dd);			// Cut the edge of dd (make a new half edge)
	Map1::cutEdge(ee);			// Cut the edge of ee (make a new half edge)
	phi2sew(phi1(dd), phi1(ee));// Sew the two faces along the new edge
}

Dart Map2::deleteVertex(Dart d)
{
	if(isBoundaryVertex(d))
		return NIL ;

	Dart res = NIL ;
	Dart vit = d ;
	do
	{
		if(res == NIL && phi1(phi1(d)) != d)
			res = phi1(d) ;

		Dart f = phi_1(phi2(vit)) ;
		phi1sew(vit, f) ;

		vit = alpha1(vit) ;
	} while(vit != d) ;
	deleteCycle(d) ;
	return res ;
}

void Map2::cutEdge(Dart d)
{
	Dart e = phi2(d);
	phi2unsew(d);			// remove old phi2 links
	Map1::cutEdge(d);		// Cut the 1-edge of d
	Map1::cutEdge(e);		// Cut the 1-edge of phi2(d)

	phi2sew(d, phi1(e));			// Correct the phi2 links
	phi2sew(e, phi1(d));
}

bool Map2::uncutEdge(Dart d)
{
	if(vertexDegree(phi1(d)) == 2)
	{
		Dart ne = phi2(d) ;
		Dart nd = phi1(d) ;
		Dart e = phi_1(ne) ;
		phi2unsew(e) ;
		phi2unsew(d) ;
		Map1::collapseEdge(nd) ;
		Map1::collapseEdge(ne) ;
		phi2sew(d, e) ;
		return true ;
	}
	return false ;
}

Dart Map2::collapseEdge(Dart d, bool delDegenerateFaces)
{
	Dart resV = NIL ;

	Dart e = phi2(d);
	phi2unsew(d);	// Unlink the opposite edges

	Dart f = phi1(e) ;
	Dart h = alpha1(e);

	if (h != e)
		resV = h;

	if (f != e && delDegenerateFaces)
	{
		Map1::collapseEdge(e) ;		// Collapse edge e
		collapseDegeneratedFace(f) ;// and collapse its face if degenerated
	}
	else
		Map1::collapseEdge(e) ;	// Just collapse edge e

	f = phi1(d) ;
	if(resV == NIL)
	{
		h = alpha1(d);
		if (h != d)
			resV = h;
	}

	if (f != d && delDegenerateFaces)
	{
		Map1::collapseEdge(d) ;		// Collapse edge d
		collapseDegeneratedFace(f) ;// and collapse its face if degenerated
	}
	else
		Map1::collapseEdge(d) ;	// Just collapse edge d

	return resV ;
}

bool Map2::flipEdge(Dart d)
{
	if (!isBoundaryEdge(d))
	{
		Dart e = phi2(d);
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

bool Map2::flipBackEdge(Dart d)
{
	if (!isBoundaryEdge(d))
	{
		Dart e = phi2(d);
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

//void Map2::insertEdgeInVertex(Dart d, Dart e)
//{
//	assert(!sameVertex(d,e) && phi2(e) == phi_1(e));
//	phi1sew(phi_1(d), phi_1(e));
//}
//
//bool Map2::removeEdgeFromVertex(Dart d)
//{
//	if (!isBoundaryEdge(d))
//	{
//		phi1sew(phi_1(d), phi2(d)) ;
//		return true ;
//	}
//	return false ;
//}

void Map2::sewFaces(Dart d, Dart e, bool withBoundary)
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

	if (ee != phi_1(dd))
		phi1sew(ee, phi_1(dd)) ;	// remove the boundary edge
	if (dd != phi_1(ee))
		phi1sew(dd, phi_1(ee)) ;	// and properly close incident boundaries
	Map1::deleteCycle(dd) ;

	phi2sew(d, e) ; // sew the faces
}

void Map2::unsewFaces(Dart d)
{
	assert(!isBoundaryEdge(d)) ;

	Dart dd = phi2(d);

	Dart e = newBoundaryCycle(2);
	Dart ee = phi1(e) ;

	if (isBoundaryVertex(d))
	{
		Dart f = findBoundaryEdgeOfVertex(d);
		phi1sew(e, phi_1(f));
	}

	if (isBoundaryVertex(dd))
	{
		Dart f = findBoundaryEdgeOfVertex(dd);
		phi1sew(ee, phi_1(f));
	}

	phi2unsew(d);

	phi2sew(d, e);		// sew faces
	phi2sew(dd, ee);	// to the boundary
}

bool Map2::collapseDegeneratedFace(Dart d)
{
	Dart e = phi1(d) ;				// Check if the face is degenerated
	if (phi1(e) == d)				// Yes: it contains one or two edge(s)
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
			phi1sew(d2, phi_1(d2)) ;
			Map1::deleteCycle(d2) ;
		}
		Map1::deleteCycle(d) ;
		return true ;
	}
	return false ;
}

void Map2::splitFace(Dart d, Dart e)
{
	assert(d != e && sameFace(d, e)) ;
	Map1::cutEdge(phi_1(d)) ;
	Map1::cutEdge(phi_1(e)) ;
	Map1::splitCycle(phi_1(d), phi_1(e)) ;
	phi2sew(phi_1(d), phi_1(e));
}

bool Map2::mergeFaces(Dart d)
{
	if (!isBoundaryEdge(d))
	{
		Dart e = phi2(d) ;
		phi2unsew(d) ;
		Map1::mergeCycles(d, phi1(e)) ;
		Map1::splitCycle(e, phi1(d)) ;
		Map1::deleteCycle(d) ;
		return true ;
	}
	return false ;
}

void Map2::extractTrianglePair(Dart d)
{
	Dart e = phi2(d) ;

	assert(!isBoundaryFace(d) && !isBoundaryFace(e)) ;
	assert(faceDegree(d) == 3 && faceDegree(e) == 3) ;

	Dart d1 = phi2(phi1(d)) ;
	Dart d2 = phi2(phi_1(d)) ;
	phi2unsew(d1) ;
	phi2unsew(d2) ;
	phi2sew(d1, d2) ;

	Dart e1 = phi2(phi1(e)) ;
	Dart e2 = phi2(phi_1(e)) ;
	phi2unsew(e1) ;
	phi2unsew(e2) ;
	phi2sew(e1, e2) ;
}

void Map2::insertTrianglePair(Dart d, Dart v1, Dart v2)
{
	Dart e = phi2(d) ;

	assert(v1 != v2 && sameOrientedVertex(v1, v2)) ;
	assert(faceDegree(d) == 3 && faceDegree(phi2(d)) == 3) ;

	Dart vv1 = phi2(v1) ;
	phi2unsew(v1) ;
	phi2sew(phi_1(d), v1) ;
	phi2sew(phi1(d), vv1) ;

	Dart vv2 = phi2(v2) ;
	phi2unsew(v2) ;
	phi2sew(phi_1(e), v2) ;
	phi2sew(phi1(e), vv2) ;
}

void Map2::unsewAroundVertex(Dart d)
{
	Dart it = d ;
	do
	{
		Dart temp = phi1(it) ;
		Dart e_1 = phi_1(it) ;

		do
		{
			unsewFaces(temp) ;
			temp = phi1(temp) ;
		} while(temp != e_1);

		it = alpha1(it);
	}
	while(it != d);
}

bool Map2::mergeVolumes(Dart d, Dart e)
{
	assert(!isBoundaryMarked(d) && !isBoundaryMarked(e)) ;

	if (isBoundaryFace(d) || isBoundaryFace(e))
		return false;

	// First traversal of both faces to check the face sizes
	// and store their edges to efficiently access them further

	std::vector<Dart> dDarts;
	std::vector<Dart> eDarts;
	dDarts.reserve(16);		// usual faces have less than 16 edges
	eDarts.reserve(16);

	Dart dFit = d ;
	Dart eFit = phi1(e) ;	// must take phi1 because of the use
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
		phi2unsew(d2);		// Unlink the two adjacent faces from dNext and eNext
		phi2unsew(e2);
		phi2sew(d2, e2);	// Link the two adjacent faces together
	}
	deleteCycle(d);		// Delete the two alone faces
	deleteCycle(e);

	return true ;
}

/*! @name Topological Queries
 *  Return or set various topological information
 *************************************************************************/

bool Map2::sameOrientedVertex(Dart d, Dart e)
{
	Dart it = d;				// Foreach dart dNext in the vertex of d
	do
	{
		if (it == e)			// Test equality with e
			return true;
		it = alpha1(it);
	} while (it != d);
	return false;				// None is equal to e => vertices are distinct
}

unsigned int Map2::vertexDegree(Dart d)
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

bool Map2::isBoundaryVertex(Dart d)
{
	Dart it = d ;
	do
	{
		if (isBoundaryMarked(it))
			return true ;
		it = alpha1(it) ;
	} while (it != d) ;
	return false ;
}

Dart Map2::findBoundaryEdgeOfVertex(Dart d)
{
	Dart it = d ;
	do
	{
		if (isBoundaryMarked(it))
			return it ;
		it = alpha1(it) ;
	} while (it != d) ;
	return NIL ;
}

bool Map2::isBoundaryEdge(Dart d)
{
	Dart e = phi2(d);
	return isBoundaryMarked(e) || isBoundaryMarked(d);
}

bool Map2::isBoundaryFace(Dart d)
{
	Dart it = d ;
	do
	{
		if (isBoundaryMarked(phi2(it)))
			return true ;
		it = phi1(it) ;
	} while (it != d) ;
	return false ;
}

bool Map2::sameOrientedVolume(Dart d, Dart e)
{
	DartMarkerStore mark(*this);	// Lock a marker

	std::list<Dart> visitedFaces;	// Faces that are traversed
	visitedFaces.push_back(d);		// Start with the face of d
	std::list<Dart>::iterator face;

	// For every face added to the list
	for (face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		if (!isBoundaryMarked(*face) && !mark.isMarked(*face))		// Face has not been visited yet
		{
			Dart it = *face ;
			do
			{
				if(it == e)
					return true;

				mark.mark(it);						// Mark
				Dart adj = phi2(it);				// Get adjacent face
				if (!isBoundaryMarked(adj) && !mark.isMarked(adj))
					visitedFaces.push_back(adj);	// Add it
				it = phi1(it);
			} while(it != *face);
		}
	}
	return false;
}

unsigned int Map2::volumeDegree(Dart d)
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
		if (!isBoundaryMarked(df) && !mark.isMarked(df))		// Face has not been visited yet
		{
			++count;
			Dart it = df ;
			do
			{
				mark.mark(it);					// Mark
				Dart adj = phi2(it);			// Get adjacent face
				if ( !isBoundaryMarked(adj) && !mark.isMarked(adj) )
					visitedFaces.push_back(adj);// Add it
				it = phi1(it);
			} while(it != df);
		}
	}

	return count;
}

bool Map2::isTriangular()
{
	TraversorF<Map2> t(*this) ;
	for(Dart d = t.begin(); d != t.end(); d = t.next())
	{
		if(faceDegree(d) != 3)
			return false ;
	}
	return true ;
}

bool Map2::check()
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

		Dart d1 = phi1(d);
		if (phi_1(d1) != d)	// phi1 a une image correcte ?
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
		if (phi1(d1) == d)
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

/*! @name Cell Functors
 *  Apply functors to all darts of a cell
 *************************************************************************/

bool Map2::foreach_dart_of_vertex(Dart d, FunctorType& f, unsigned int thread)
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

bool Map2::foreach_dart_of_edge(Dart d, FunctorType& fonct, unsigned int thread)
{
	if (fonct(d))
		return true;
	return fonct(phi2(d));
}

bool Map2::foreach_dart_of_oriented_volume(Dart d, FunctorType& f, unsigned int thread)
{
	DartMarkerStore mark(*this, thread);	// Lock a marker
	bool found = false;				// Last functor return value

	std::list<Dart> visitedFaces;	// Faces that are traversed
	visitedFaces.push_back(d);		// Start with the face of d
	std::list<Dart>::iterator face;

	// For every face added to the list
	for (face = visitedFaces.begin(); !found && face != visitedFaces.end(); ++face)
	{
		if (!isBoundaryMarked(*face) && !mark.isMarked(*face))		// Face has not been visited yet
		{
			// Apply functor to the darts of the face
			found = foreach_dart_of_oriented_face(*face, f);

			// If functor returns false then mark visited darts (current face)
			// and add non visited adjacent faces to the list of face
			if (!found)
			{
				Dart it = *face ;
				do
				{
					mark.mark(it);					// Mark
					Dart adj = phi2(it);				// Get adjacent face
					if (!isBoundaryMarked(adj) && !mark.isMarked(adj))
						visitedFaces.push_back(adj);	// Add it
					it = phi1(it);
				} while(it != *face);
			}
		}
	}
	return found;
}

/*! @name Close map after import or creation
 *  These functions must be used with care, generally only by import/creation algorithms
 *************************************************************************/

unsigned int Map2::closeHole(Dart d, bool forboundary)
{
	assert(phi2(d) == d);		// Nothing to close

	Dart first = newDart();		// First edge of the face that will fill the hole
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

	if (countEdges < 3)
	{
		countEdges = 0 ;
		collapseDegeneratedFace(first); // if the closing face is 2-sided, collapse it
	}
	else
	{
		if(forboundary)
			boundaryMarkOrbit(FACE, phi2(d));
	}

	return countEdges ;
}

void Map2::closeMap()
{
	// Search the map for topological holes (fix points of phi2)
	for (Dart d = begin(); d != end(); next(d))
	{
		if (phi2(d) == d)
			closeHole(d);
	}
}

} // namespace CGoGN
