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
#include "Topology/map/embeddedMap3.h"
#include "Algo/Tiling/Volume/cubic.h"
#include "Algo/Geometry/volume.h"


using namespace CGoGN ;

/**
 * Struct that contains some informations about the types of the manipulated objects
 * Mainly here to be used by the algorithms that are parameterized by it
 */
struct PFP: public PFP_STANDARD
{
	// definition of the type of the map
	typedef EmbeddedMap3 MAP;
	typedef double REAL;
	typedef Geom::Vector<3,REAL> VEC3;
};

typedef PFP::MAP MAP;
typedef PFP::MAP::IMPL MAP_IMPL;
typedef PFP::VEC3 VEC3;

int main()
{
	// declare a map to handle the mesh
	MAP myMap;

	// add position attribute on vertices and get handler on it
	VertexAttribute<VEC3, MAP> position = myMap.addAttribute<VEC3, VERTEX, MAP>("position");
	constexpr int nb = 2;
	Algo::Volume::Tilings::Cubic::Grid<PFP> cubic(myMap, nb, nb, nb);
	cubic.embedIntoGrid(position, 10.0f, 10.0f, 10.0f);

	VolumeAttribute<VEC3,MAP> color = myMap.addAttribute<VEC3, VOLUME, MAP>("color");

	foreach_cell<VOLUME>(myMap, [&](Vol w)
	{
		color[w] = position[w.dart] + VEC3(0.5,0.5,0.5);
	});

	CGoGNout.toStd(false);
	CGoGNout.toFile("map1.csv");
	myMap.dumpCSV();

	myMap.saveMapBin("pipo.map");

	MAP myMap2;

	myMap2.loadMapBin("pipo.map");

	CGoGNout.toFile("map2.csv");
	myMap2.dumpCSV();

	std::cout << " Volume Total =" << Algo::Geometry::totalVolume<PFP>(myMap2, position)<< std::endl;


	return 0;
}
