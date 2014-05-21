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
typedef PFP::MAP::IMPL MAP_IMPL ;	// map implementation
typedef PFP::VEC3 VEC3 ;			// type of R³ vector 


int main()
{
	// declare a map to handle the mesh
	MAP myMap;

	// add position attribute on vertices and get handler on it
	VertexAttribute<VEC3, MAP_IMPL> position = myMap.addAttribute<VEC3, VERTEX>("position");

	// create a triangle
	Face f1 = myMap.newFace(3);
	// create a quad
	Face f2 = myMap.newFace(4);

	// sew them together
	myMap.sewFaces(f1,f2);

	// and set the position of the vertices
	Dart d1 = f1;
	position[d1] = VEC3(0, 0, 0);
	d1 = myMap.phi1(d1);
	position[d1] = VEC3(2, 0, 0);
	d1 = myMap.phi1(d1);
	position[d1] = VEC3(1, 2, 0);

	position[myMap.phi<11>(f2.dart)] = VEC3(0, -2, 0);
	position[myMap.phi_1(f2.dart)] = VEC3(2, -2, 0);

	return 0;
}
