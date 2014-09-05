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

/**
 * @brief test if map has a Vertex Attribute of VEC3 named name
 * @param map  the map
 * @param name name of attribute
 */
void testVAbyNames(MAP& map, const std::string& name)
{
	VertexAttribute<VEC3, MAP> testPos = map.getAttribute<VEC3, VERTEX, MAP>(name);
	if (testPos.isValid())
		std::cout << "Attribute "<< name <<" valid"<< std::endl;
	else
		std::cout << "Attribute "<< name <<"invalid"<< std::endl;
}

/**
 * @brief computeLengthEdges
 * Demonstrate usage of 2 attributes on 2 differents orbits.
 * @param map the map
 * @param pos attribute handler of position of vertices
 * @param len attribute handler of length of edges
 */
void computeLengthEdges(MAP& map,const VertexAttribute<VEC3, MAP>& pos, EdgeAttribute<float, MAP> len)
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

/**
 * @brief computeNewPositions Demonstrate  the usage of AutoAttributes
 */
void computeNewPositions(MAP& map, VertexAttribute<VEC3, MAP>& pos)
{
	// here we need new and old positions simultaneously so create temporary attribute position

	VertexAutoAttribute<VEC3, MAP> pos2(map);

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
	// only possible with same type and same orbit attribute.
	map.swapAttributes(pos,pos2);

	// destruction of VertexAutoAttribute handller remove the attribute from the map.
}

/**
* @brief dump any attribute
* @param attr attribute
*/
template<typename ATTRIB>
void dumpAttribute(const ATTRIB& attr)
{
	CHECK_ATTRIBUTEHANDLER(ATTRIB);

	std::cout << "Attribute " << attr.name() << " of orbit " << orbitName(attr.getOrbit()) << " of type " << attr.typeName() << std::endl;

	// NEVER USE 0 AND ++ IN FOR LOOP ON ATTRIBUTE !
	// USE ONLY BEGIN / END / NEXT (for hole jumping over)
	for (unsigned int i = attr.begin(); i != attr.end(); attr.next(i))
	{
		std::cout << attr.name() << "[" << i << "] = " << attr[i] << std::endl;
	}
}


//function that apply on vertice with templated attribute type
template<typename T>
void VertexTyped(MAP& map, T& va)
{

//	foreach_cell<VERTEX>(map,[&](Vertex v)  // for all vertices
//	{
//		va[v] = 1.1 * va[v];
//	});

	// other syntax for traversal
	 for (Vertex v : allVerticesOf(map))
	 {
		 va[v] = 1.1 * va[v];
		 std::cout << "V:" << v << " -> "<<va[v]<< std::endl;
	 }
}

// version that take a VertexAttribute, check type at runtime and call instancied template version
void VertexGeneric(MAP& map, VertexAttributeGen& vg)
{
	auto va3 = dynamic_cast<VertexAttribute<VEC3, MAP>*>(&vg);
	if (va3 != NULL)
		return VertexTyped(map,*va3);

	auto vaf = dynamic_cast<VertexAttribute<float, MAP>*>(&vg);
	if (vaf != NULL)
		return VertexTyped(map,*vaf);
}



int main()
{
	// declare a map to handle the mesh
	MAP myMap;

	// add position attribute on vertices and get handler on it
	VertexAttribute<VEC3, MAP> positionAtt = myMap.addAttribute<VEC3, VERTEX, MAP>("position");
	if (!positionAtt.isValid())
		std::cerr << "impossible to create an attribute with name position (already used ?)"<< std::endl;


	// create a topo grid of 2x2 squares
	Algo::Surface::Tilings::Square::Grid<PFP> grid(myMap, 2, 2, true);
	// and embed it using position attribute
	grid.embedIntoGrid(positionAtt, 1.,1.,0.);


	VertexGeneric(myMap,positionAtt);

	// ATTRIBUTE DECLARATION

	// add an attribute of type float on orbit EDGE
	EdgeAttribute<float, MAP> lengthAtt = myMap.addAttribute<float, EDGE, MAP>("length");
	if (!lengthAtt.isValid())
		std::cerr << "impossible to create the attribute"<< std::endl;

	computeLengthEdges(myMap,positionAtt,lengthAtt);

	// add an attribute of type std::string on orbit FACE
	FaceAttribute<std::string, MAP> nameAtt = myMap.addAttribute<std::string, FACE, MAP>("name");
	if (!nameAtt.isValid())
		std::cerr << "impossible to create the attribute"<< std::endl;

	// for complex type use following template (function nameOfType not applicable)
	EdgeAttribute< NoTypeNameAttribute< std::vector<int> >, MAP> vectAtt = myMap.addAttribute< NoTypeNameAttribute< std::vector<int> >, EDGE, MAP>("vector_of_int");
	if (!vectAtt.isValid())
		std::cerr << "impossible to create the attribute"<< std::endl;

	Dart d = myMap.begin();
	// define a vertex from a dart
	Vertex v(d);
	// define a face from a dart
	Face f(d);


	// ATTRIBUTE ACCESS

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

	// access with unsigned int is dangerous, index must be obtain with begin/end/next (see dumpAttribute)

	// COPY, REMOVE, SWAP

	// possible to have any number of attribute a same ORBIT
	VertexAttribute<VEC3, MAP> position2Att = myMap.addAttribute<VEC3, VERTEX, MAP>("other_position");

	// copy of attribute of same type (linear complexity)
	myMap.copyAttribute(position2Att,positionAtt);

	positionAtt[v] += VEC3(0,0,1);

	computeNewPositions(myMap,positionAtt);
	dumpAttribute(positionAtt);

	//check if there is a Vertex Attribute of VEC3 named position => yes
	testVAbyNames(myMap,"position");

	// remove the attribute
	myMap.removeAttribute(positionAtt);

	//check if there is a Vertex Attribute of VEC3 named position => no
	testVAbyNames(myMap,"position");

	return 0;
}
