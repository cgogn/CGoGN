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
#include "Topology/map/embeddedMap3.h"
#include "Algo/Tiling/Surface/square.h"
#include "Algo/Geometry/volume.h"

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

struct PFP3: public PFP_STANDARD
{
	// definition of the type of the map
	typedef EmbeddedMap3 MAP;
};


typedef PFP2::MAP MAP2;
typedef PFP2::VEC3 VEC3;
typedef PFP3::MAP MAP3;

int main()
{
	// declare a map to handle the mesh
	MAP2 myMap;

	// add position attribute on vertices and get handler on it
	VertexAttribute<VEC3, MAP2> position = myMap.addAttribute<VEC3, VERTEX, MAP2>("position");
	Algo::Surface::Tilings::Square::Cube<PFP2> cube(myMap, 2, 2, 2);
	cube.embedIntoCube(position, 10.0f, 10.0f, 10.0f);

	FaceAttribute<VEC3,MAP2> color = myMap.addAttribute<VEC3, FACE, MAP2>("color");

	foreach_cell<FACE>(myMap, [&](Face f)
	{
		color[f] = (position[f.dart])*0.1f;
	});

	CGoGNout.toStd(false);
	CGoGNout.toFile("mf_map2.csv");
	myMap.dumpCSV();
	std::cout << "MAP2 dumped in mf_map2.csv"<< std::endl;

	MAP3 myMap2;
	VertexAttribute<VEC3, MAP3> position2 = myMap2.addAttribute<VEC3, VERTEX, MAP3>("position");

	myMap2.moveFrom(myMap);

	// DartMarker and CellMarker must be updated after copy
	if (!position2.isValid())
	{
		std::cout << "Attribute handlers are invalid after load, copy or move,  get it agins"<< std::endl;
		// get it again (here attribute created in move)
		position2 = myMap2.getAttribute<VEC3, VERTEX, MAP3>("position");
	}

	CGoGNout.toFile("mf_map3.csv");
	myMap2.dumpCSV();
	std::cout << "MAP3 dumped in mf_map3.csv"<< std::endl;

	// some computation to show that map is ok
	std::cout << " Volume Total =" << Algo::Geometry::totalVolume<PFP3>(myMap2, position2)<< std::endl;

	// save maps
	myMap2.saveMapBin("mf_pipo.map");

	return 0;
}
