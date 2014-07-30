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
typedef PFP::VEC3 VEC3 ;			// type of R³ vector 


int main()
{
	// declare a map to handle the mesh
	MAP myMap;

	// add position attribute on vertices and get handler on it
	VertexAttribute<VEC3, MAP> position = myMap.addAttribute<VEC3, VERTEX, MAP>("position");

	// create a topo grid of 2x2 squares
	Algo::Surface::Tilings::Square::Grid<PFP> grid(myMap, 4, 4, true);

	// and embed it using position attribute
    grid.embedIntoGrid(position, 1.,1.,0.);
	
	// traversal with begin/end/next on attribute handler
	for (unsigned int id = position.begin(); id != position.end(); position.next(id))
	{
		std::cout << id << " : " << position[id]<< " / ";
	}
	std::cout << std::endl;

	//using foreach function (C++11 lambda expression)
	foreach_attribute(position, [&] (unsigned int id) // for each element of position
	{
		std::cout << id << " : " << position[id]<< " / ";
	});

	// using parallel foreach
	// parameter position must be captured explicitly even if it used as first parameter of foreach !
	Parallel::foreach_attribute(position, [&position] (unsigned int id, unsigned int /*thread*/) // for each elt of the position attribute
	{
		position[id] *= 2.0f;
	}, 4); // 4:4 thread, false for no need for markers in threaded code.

	return 0;
}
