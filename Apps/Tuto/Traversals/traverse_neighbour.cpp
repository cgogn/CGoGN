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

	Vertex v;
	findCell(VERTEX,v,MAP,myMap, position[v] == VEC3(0,0,0) );
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
	Traversor2VVaF<MAP> trvvaf(myMap,d);
	for (Dart e = trvvaf.begin(); e != trvvaf.end(); e = trvvaf.next())
	{
		std::cout << "vertex of dart "<<e<< " adjacent to vertex of dart " << v.dart<< " by a face" << std::endl;
	}

	// WITH FOREACH MACRO

	// find incident faces to vertex
	foreachIncident2(VERTEX,v,FACE,f,MAP,myMap)
		std::cout << "Face of dart "<<e<< " incident to vertex of dart " << v.dart<< std::endl;

	// find adjacent vertices thru a face
	foreachAdjacent2(VERTEX,FACE,v,e, MAP, myMap)
		std::cout << "vertex of dart "<<e<< " adjacent to vertex of dart " << v.dart<< " by a face" << std::endl;

	return 0;
}
