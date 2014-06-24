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

#include "Topology/generic/cellmarker.h"

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


// example of cell marking with CellMarker for a simple traversal
template<unsigned int ORBIT>
void simpleTraversal(MAP& map) // NEVER COPY THE MAP, ALWAYS USE REFERENCE !!
{
	CellMarker<MAP, ORBIT> cm(map);
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (!cm.isMarked(d))	// is the cell (of dart d) not marked ?
		{
			std::cout << orbitName(ORBIT)<< " of dart "<< d << std::endl;
			cm.mark(d);			// mark the cell
		}
	}
	std::cout <<  std::endl;

	// all cells are unmarked automatically during the destruction of the CellMarker
}



// example of cell marking with CellMarkerNoUnmark for a double traversal
template<unsigned int ORBIT>
void doubleTraversal(MAP& map) // NEVER COPY THE MAP, ALWAYS USE REFERENCE !!
{
	CellMarkerNoUnmark<MAP,ORBIT> cm(map);
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (!cm.isMarked(d))	// is the cell (of dart d) not marked ?
		{
			std::cout << "First Pass" << orbitName(ORBIT)<< " of dart "<< d << std::endl;
			cm.mark(d);			// mark the cell
		}
	}
	std::cout <<  std::endl;

	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (cm.isMarked(d))	// is the cell (of dart d) marked ?
		{
			std::cout << "second Pass" << orbitName(ORBIT)<< " of dart "<< d << std::endl;
			cm.unmark(d);			// unmark the cell
		}
	}
	std::cout <<  std::endl;

	// destructor does not clean the markers
	// user MUST ensure that he has unmark all he has marked
}




// example of usage of CellMarkerStore
void negativePositions(MAP& map, VertexAttribute<VEC3,MAP>& position) // NEVER COPY THE MAP, ALWAYS USE REFERENCE !!
{
	// if user knows that small numbers of cell will be marked
	// it is more efficient to store them instead of traverse
	// all darts for cleanning. CellMarkerStore do it for you.

	CellMarkerStore<MAP,VERTEX> cms(map);

	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (!cms.isMarked(d) && (position[d][0]<= 0.0) && (position[d][1]<= 0.0))
		{
			std::cout << "position["<<d<<"] < (0,0)" << std::endl;
			cms.mark(d);
		}
	}
	std::cout <<  std::endl;
}



int main()
{
	// declare a map to handle the mesh
	MAP myMap;

	// add position attribute on vertices and get handler on it
	VertexAttribute<VEC3, MAP> position = myMap.addAttribute<VEC3, VERTEX, MAP>("position");
	// create a topo grid of 2x2 squares
	Algo::Surface::Tilings::Square::Grid<PFP> grid(myMap, 2, 2, true);
	// and embed it using position attribute
    grid.embedIntoGrid(position, 1.,1.,0.);


	simpleTraversal<VERTEX>(myMap);

	doubleTraversal<FACE>(myMap);

	negativePositions(myMap, position);


	return 0;
}
