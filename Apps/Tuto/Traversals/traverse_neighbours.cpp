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
	Algo::Surface::Tilings::Square::Grid<PFP> grid(myMap, 4, 4, true);
	// and embed it using position attribute
    grid.embedIntoGrid(position, 1.,1.,0.);

	// easy way to find the central vertex of the grid	
	Vertex v;
//	findCell(VERTEX,v,MAP,myMap, position[v] == VEC3(0,0,0) );
	find_cell<VERTEX>(myMap,v, [&](Vertex v)
	{
		return position[v] == VEC3(0,0,0);
	});

	// must test of find ok (if not v.dart is NIL)
	if (! v.valid())
		std::cerr << "could not find a vertex with position (0,0,0)" << std::endl;


// WITH TRAVERSORS:
	
	// find incident faces to vertex
	Traversor2VF<MAP> trvf(myMap,v.dart);
	for (Dart e = trvf.begin(); e != trvf.end(); e = trvf.next())
	{
		std::cout << "Face of dart "<<e<< " incident to vertex of dart " << v.dart<< std::endl;
	}

	// find adjacent vertices thru a face
	Traversor2VVaF<MAP> trvvaf(myMap,v.dart);
	for (Dart e = trvvaf.begin(); e != trvvaf.end(); e = trvvaf.next())
	{
		std::cout << "vertex of dart "<<e<< " adjacent to vertex of dart " << v.dart<< " by a face" << std::endl;
	}

// WITH FOREACH FUNCTION (C++11 lambda expression)

	// find incident faces to vertex
	foreach_incident2<FACE>(myMap,v,[&](Face f)
	{
		std::cout << "Face of dart "<<f<< " incident to vertex of dart " << v.dart<< std::endl;
	});

	// find adjacent vertices thru a face
	foreach_adjacent2<FACE>(myMap,v,[&](Vertex x)
	{
		std::cout << "vertex of dart "<<x<< " adjacent to vertex of dart " << v.dart<< " by a face" << std::endl;	
	});

	return 0;
}
