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
struct PFP2: public PFP_STANDARD
{
	// definition of the type of the map
	typedef EmbeddedMap2 MAP;
};


typedef PFP2::MAP MAP;
typedef PFP2::VEC3 VEC3;


int main()
{
	// declare a map to handle the mesh
	MAP myMap;

	// add position attribute on vertices and get handler on it
	VertexAttribute<VEC3, MAP> position = myMap.addAttribute<VEC3, VERTEX, MAP>("position");
	Algo::Surface::Tilings::Square::Cube<PFP2> cube(myMap, 2, 2, 2);
	cube.embedIntoCube(position, 10.0f, 10.0f, 10.0f);

	FaceAttribute<VEC3, MAP> color = myMap.addAttribute<VEC3, FACE, MAP>("colorFace");

	foreach_cell<FACE>(myMap, [&](Face f)
	{
		color[f] =(Algo::Surface::Geometry::faceCentroid<PFP2>(myMap,f,position)+ VEC3(5,5,5))/10.0f;
	});


	CGoGNout.toStd(false);
	CGoGNout.toFile("compact_init.csv");
	myMap.dumpCSV();
	std::cout << "MAP dumped in compact_init.csv"<< std::endl;

	// mark central vertex of each face of the cube
	CellMarker<MAP,VERTEX> cm(myMap);
	foreach_cell<VERTEX>(myMap, [&](Vertex v)
	{
		int nb=0;
		if (position[v][0] == 0.0f)
			nb++;
		if (position[v][1] == 0.0f)
			nb++;
		if (position[v][2] == 0.0f)
			nb++;

		if (nb == 2)
			cm.mark(v);
	});

	// remove central vertex of each face of the cube
	foreach_cell<VERTEX>(myMap, [&](Vertex v)
	{
		if (cm.isMarked(v))
		{
			std::cout << "delete vertex"<< std::endl;
			myMap.deleteVertex(v);
		}
	});

	// remove middle points of each edge of the cube
	foreach_cell<EDGE>(myMap, [&](Edge e)
	{
		Vertex v(e.dart);
		if (myMap.vertexDegree(v)==2)
			myMap.uncutEdge(myMap.phi_1(v));
	});


	CGoGNout.toFile("compact_before.csv");
	myMap.dumpCSV();
	std::cout << "MAP with holes dumped in compact_before.csv"<< std::endl;

	std::cout << "Total area "<< Algo::Surface::Geometry::totalArea<PFP2>(myMap,position)<< std::endl;


	myMap.compact();

	CGoGNout.toFile("compact_after.csv");
	myMap.dumpCSV();
	std::cout << "Compacted MAP dumped in compact_after.csv"<< std::endl;

	std::cout << "Total area "<< Algo::Surface::Geometry::totalArea<PFP2>(myMap,position)<< std::endl;

	myMap.saveMapBin("compacted_map2.map");

	return 0;
}
