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
	Algo::Surface::Tilings::Square::Grid<PFP> grid(myMap, 3, 3, true);

	// and embed it using position attribute
    grid.embedIntoGrid(position, 1.,1.,0.);


	// traverse all darts of map and test if they belong to the boundary
	for (Dart d = myMap.begin(); d != myMap.end(); myMap.next(d))
	{
		if (myMap.isBoundaryMarked<2>(d)) // 2 because myMap is Map2 (can use isBoundaryMarkedCurrent)
			std::cout << "Dart of boundary" << std::endl;
	}
	std::cout <<  std::endl;


	// traverse all vertices of mesh and test if they belong to the boundary
	foreach_cell<VERTEX>(myMap,[&myMap](Vertex v) // for all edge e of map do
	{
		if (myMap.isBoundaryVertex(v))
			std::cout << "vertex of boundary"<< std::endl;
		else
			std::cout << "normal vertex"<< std::endl;
	});
	std::cout <<  std::endl;

	// traverse all edges of mesh and test if they belong to the boundary
	foreach_cell<EDGE>(myMap,[&myMap](Edge e) // for all edge e of map do
	{
		if (myMap.isBoundaryEdge(e))
			std::cout << "edge of boundary"<< std::endl;
		else
			std::cout << "normal edge"<< std::endl;
	});
	std::cout <<  std::endl;


	//It does not work with FACE because foreach and Traversors do not traverse the boundary faces
	foreach_cell<FACE>(myMap,[&myMap](Face v) // for all edge e of map do
	{
		if (myMap.isBoundaryMarked<2>(v.dart))
			std::cout << "boundary face"<< std::endl;
		else
			std::cout << "normal face"<< std::endl;
	});
	std::cout <<  std::endl;

	return 0;
}
