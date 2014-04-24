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


#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Algo/Tiling/Surface/square.h"
#include "Topology/generic/cells_macros.h"


using namespace CGoGN ;

/**
 * Struct that contains some informations about the types of the manipulated objects
 * Mainly here to be used by the algorithms that are parameterized by it
 */
struct PFP: public PFP_STANDARD
{
	// definition of the type of the map
	typedef EmbeddedMap2 MAP;
};

// some typedef shortcuts
typedef PFP::MAP MAP ;				// map type
typedef PFP::MAP::IMPL MAP_IMPL ;	// map implementation
typedef PFP::VEC3 VEC3 ;			// type of R³ vector 


int main()
{
	// declare a map to handle the mesh
	MAP myMap;

	// add position attribute on vertices and get handler on it
	VertexAttribute<VEC3, MAP_IMPL> position = myMap.addAttribute<VEC3, VERTEX>("position");

	// create a topo grid of 2x2 squares
	Algo::Surface::Tilings::Square::Grid<PFP> grid(myMap, 2, 2, true);

	// and embed it using position attribute
    grid.embedIntoGrid(position, 1.,1.,0.);


	
	// ALGO:
	// foreach vertex of the map
	//    print position of the vertex

	// 4 versions 	

	// low level traversal using pure topo
	DartMarker<MAP> dm(myMap);
	for (Dart d = myMap.begin(); d != myMap.end(); myMap.next(d)) // traverse all darts of map
	{
		if (!dm.isMarked(d))			// is dart not marked ?
		{
			dm.markOrbit<VERTEX>(d);	// mark all dart of vertex orbit to be sure to not traverse it again
			std::cout << d << " : " << position[d]<< " / ";
		}
	}
	std::cout <<  std::endl;


	// low level traversal using cell marking (use marker on embedding is available)
	CellMarker<MAP,VERTEX> cm(myMap);
	for (Dart d = myMap.begin(); d != myMap.end(); myMap.next(d))
	{
		if (!cm.isMarked(d))	// is dart of the vertex cell not marked ?
		{
			cm.mark(d);			// 
			std::cout << d << " : " << position[d]<< " / ";
		}
	}
	std::cout <<  std::endl;


	// using traversal
	TraversorV<MAP> tv(myMap);	// alias for Traversor<MAP,VERTEX> 
	for (Dart d=tv.begin(); d!=tv.end(); d=tv.next())// traverse all vertices (d one dart of each vertex)
		std::cout << d << " : " << position[d]<< " / ";
	std::cout <<  std::endl;


	//using foreach macro
	foreachCell(VERTEX,v,MAP,myMap)	// for each Vertex v of the MAP myMap
		std::cout << v << " : " << position[v]<< " / ";
	std::cout <<  std::endl;
	// warning here v is a Vertex and not a Dart (but can cast automatically into)

	return 0;
}
