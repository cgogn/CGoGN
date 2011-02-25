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

#include "Topology/map/map1.h"

namespace CGoGN
{

/*! @name Generator and Deletor
 *  To generate or delete faces in a 1-map
 *************************************************************************/

Dart Map1::newOrientedFace(unsigned nbEdges)
{
	assert(nbEdges > 0 || !"Cannot create a face with no edge") ;
	Dart d = newDart() ;			// Create the first edge
	for (unsigned i=1 ; i<nbEdges ; ++i)
		Map1::cutEdge(d) ;				// Subdivide nbEdges-1 times this edge
	return d ;
}

void Map1::deleteOrientedFace(Dart d)
{
	Dart e = phi1(d) ;
	while (e != d)
	{
		Dart f = phi1(e) ;
		deleteDart(e) ;
		e = f ;
	}
	deleteDart(d) ;
}

/*! @name Topological Operators
 *  Topological operations on 1-maps
 *************************************************************************/

void Map1::reverseFace(Dart d)
{
	Dart e = phi1(d) ;			// Dart e is the first edge of the new face
	if (e == d) return ;		// Only one edge: nothing to do
	if (phi1(e) == d) return ;	// Only two edges: nothing to do

	phi1unsew(d) ;				// Detach e from the face of d

	Dart dNext = phi1(d) ;		// While the face of d contains more than two edges
	while (dNext != d)
	{
		phi1unsew(d) ;			// Unsew the edge after d
		phi1sew(e,dNext) ;		// Sew it after e (thus in reverse order)
		dNext = phi1(d) ;
	}
	phi1sew(e,d) ;				// Sew the last edge
}

/*! @name Topological Queries
 *  Return or set various topological information
 *************************************************************************/

bool Map1::sameOrientedFace(Dart d, Dart e)
{
	Dart dNext = d ;
	do
	{
		if (dNext == e)
			return true ;
		dNext = phi1(dNext) ;
	} while (dNext != d) ;
	return false ;
}

unsigned int Map1::faceDegree(Dart d)
{
	unsigned int count = 0 ;
	Dart dNext = d ;
	do
	{
		++count ;
		dNext = phi1(dNext) ;
	} while (dNext != d) ;
	return count ;
}

bool Map1::isFaceTriangle(Dart d)
{
	return (phi1(d) != d) && (phi1(phi1(phi1(d))) == d) ;
}

/*! @name Cell Functors
 *  Apply functors to all darts of a cell
 *************************************************************************/

bool Map1::foreach_dart_of_oriented_face(Dart d, FunctorType& f, unsigned int thread)
{
	Dart dNext = d ;
	do
	{
		if (f(dNext))
			return true ;
		dNext = phi1(dNext) ;
	} while (dNext != d) ;
	return false ;
}

} // namespace CGoGN
