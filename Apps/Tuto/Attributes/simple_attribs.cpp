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

/**
 * @brief get attribute
 * @param map
 */
void byNames(MAP& map, const std::string& name)
{
	VertexAttribute<VEC3, MAP_IMPL> testPos = map.getAttribute<VEC3, VERTEX>(name);
	if (testPos.isValid())
		std::cout << "Attribute "<< name <<" valid"<< std::endl;
	else
		std::cout << "Attribute "<< name <<"invalid"<< std::endl;
}

/**
 * @brief computeLengthEdges
 * @param map the map
 * @param pos attribute handler of position of vertices
 * @param len attribute handler of length of edges
 */
void computeLengthEdges(MAP& map,const VertexAttribute<VEC3, MAP_IMPL>& pos, EdgeAttribute<float, MAP_IMPL> len)
{
	// warning c++11 lambda syntax
	foreach_cell<EDGE>(map,[&](Edge e) // for all edge e of map do
	{
		VEC3 P1 = pos[e.dart]; // access with dart because of access to VertexAttribute with an edge
		VEC3 P2 = pos[map.phi1(e)]; // phi1 return a dart so no problem (and e can auto-cast in dart)
		VEC3 V = P2 - P1;
		len[e] = V.norm();
	});
}


void computeNewPositions(MAP& map, VertexAttribute<VEC3, MAP_IMPL>& pos)
{
	// here we need new and old positions simultaneously so create temporary position

	VertexAutoAttribute<VEC3, MAP_IMPL> pos2(map);

	foreach_cell<VERTEX>(map,[&](Vertex v)  // for all vertices
	{
		int nb=0;
		pos2[v] = VEC3(0,0,0);// init with 0,0,0,
		foreach_adjacent2<EDGE>(map,v,[&](Vertex x) // for all its neighbours (by edges)
		{
			pos2[v] += pos[x];
			nb++;
		});
		pos2[v] /= nb;
	});

	// swap attribute position with temporary (constant complexity !)
	map.swapAttributes(pos,pos2);
}

/**
* @brief dump any attribute
* @param attr attribute
*/
template<typename ATTRIB>
void dumpAttribute(const ATTRIB& attr)
{
	std::cout << "Attribute " <<attr.name() << " of orbit "<< orbitName(attr.getOrbit())<< " of type "<< attr.typeName()<< std::endl;

	// NEVER USE 0 AND ++ IN FOR LOOP ON ATTRIBUTE !
	// USE ONLY BEGIN / END / NEXT (for hole jumping over)
	for (unsigned int i=attr.begin(); i!=attr.end(); attr.next(i))
	{
		std::cout << attr.name() << "["<< i << "] = "<<attr[i]<< std::endl;
	}
}


int main()
{
	// declare a map to handle the mesh
	MAP myMap;

	// add position attribute on vertices and get handler on it
	VertexAttribute<VEC3, MAP_IMPL> positionAtt = myMap.addAttribute<VEC3, VERTEX>("position");
	if (!positionAtt.isValid())
		std::cerr << "impossible to create an attribute with name position (already used ?)"<< std::endl;


	// create a topo grid of 2x2 squares
	Algo::Surface::Tilings::Square::Grid<PFP> grid(myMap, 2, 2, true);
	// and embed it using position attribute
	grid.embedIntoGrid(positionAtt, 1.,1.,0.);


	// add an attribute of type float on orbit EDGE
	EdgeAttribute<float, MAP_IMPL> lengthAtt = myMap.addAttribute<float, EDGE>("length");
	if (!lengthAtt.isValid())
		std::cerr << "impossible to create the attribute"<< std::endl;

	computeLengthEdges(myMap,positionAtt,lengthAtt);

	// add an attribute of type std::string on orbit FACE
	FaceAttribute<std::string, MAP_IMPL> nameAtt = myMap.addAttribute<std::string, FACE>("name");
	if (!nameAtt.isValid())
		std::cerr << "impossible to create the attribute"<< std::endl;

	// for complex type use following template (function nameOfType not applicable)
	EdgeAttribute< NoTypeNameAttribute< std::vector<int> >, MAP_IMPL> vectAtt = myMap.addAttribute< NoTypeNameAttribute< std::vector<int> >, EDGE>("vector_of_int");
	if (!vectAtt.isValid())
		std::cerr << "impossible to create the attribute"<< std::endl;

	Dart d = myMap.begin();
	// define a vertex from a dart
	Vertex v(d);
	// define a face from a dart
	Face f(d);

	// [] operator can take a dart, a cell (only same off attribute), or an unsigned inf
	// access to any attributes with darts
	std::cout << positionAtt[d]<< std::endl;
	nameAtt[d] = "Hello";
	lengthAtt[myMap.phi1(d)] = 54.0f;

	std::vector<int> vi = {3,5,7,9,11};
	vectAtt[d]= vi;
	vectAtt[d].push_back(11);

	// access to VertexAttribute with a Vertex
	std::cout << positionAtt[v]<< std::endl;

	// access to FaceAttribute with a Face
	std::cout << nameAtt[f]<< std::endl;

// following line does not compile because of wrong cell type
//	std::cout << positionAtt[f]<< std::endl;
//  possible to bypass using dart access
	std::cout << positionAtt[f.dart]<< std::endl;

	// possible to have any number of attribute a same ORBIT
	VertexAttribute<VEC3, MAP_IMPL> position2Att = myMap.addAttribute<VEC3, VERTEX>("other_position");

	// copy of attribute of same type (linear complexity)
	myMap.copyAttribute(position2Att,positionAtt);

	positionAtt[v] += VEC3(0,0,1);

	computeNewPositions(myMap,positionAtt);

	dumpAttribute(positionAtt);

	byNames(myMap,"position");

	myMap.removeAttribute<VEC3, VERTEX>(positionAtt);

	byNames(myMap,"position");


	return 0;
}
