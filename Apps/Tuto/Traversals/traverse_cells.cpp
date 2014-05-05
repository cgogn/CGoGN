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
#include "Algo/Geometry/area.h"

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
	Algo::Surface::Tilings::Square::Grid<PFP> grid(myMap, 4, 4, true);

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


	//using foreach function (C++11 lambda expression)
	foreach_cell<VERTEX>(myMap,[&](Vertex v) // for each Vertex v of the MAP myMap
	{
		std::cout << v << " : " << position[v]<< " / ";
	});
	// warning here v is a Vertex and not a Dart (but can cast automatically into)

	std::cout <<  std::endl;


	// example of parallel traversal of cells
	Parallel::foreach_cell<VERTEX>(myMap,[&](Vertex v, unsigned int thread) // for each Vertex v of the MAP myMap
	{
		position[v] += VEC3(0.0,0.0,PFP::REAL(thread)*0.1f);
		// WARNING thread vary here from 1 to 4 (and not from 0 to 3) !!!!
	},4,false); // 4:4 thread, false for no need for markers in threaded code.


	std::cout << "After // processing"<< std::endl;
	foreach_cell<VERTEX>(myMap,[&](Vertex v) // for each Vertex v of the MAP myMap
	{
		std::cout << position[v]<< " / ";
	});
	std::cout <<  std::endl;


	// Example with // accumulation
	// computing the sum of area faces
	// init nbthread values with 0
	float surf[4]={0.0f,0.0f,0.0f,0.0f};
	// traverse face in //
	Parallel::foreach_cell<FACE>(myMap,[&](Face f, unsigned int thr)
	{
		// for each face add surface to accumulator (-1 because counter between 1-4 not 0-3)
		surf[thr-1] += Algo::Surface::Geometry::convexFaceArea<PFP>(myMap,f,position);
	},4,false);

	std::cout << surf[0]<< "/"<< surf[1]<< "/"<< surf[2]<< "/"<< surf[3]<< "/"<< std::endl;
	std::cout << "Total="<<surf[0]+surf[1]+surf[2]+surf[3]<< std::endl;


	return 0;
}
