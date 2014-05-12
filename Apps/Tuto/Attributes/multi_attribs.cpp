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
#include "Topology/generic/multiAttribs.h"
#include "Algo/Geometry/centroid.h"

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
typedef PFP::VEC4 VEC4;



/**
 * Example of function that work with any kind of Vertex Attribute (normal/2/3/4)
 *
 * operators on multi-attribute are limited to:
 *   affectation
 *   +, -, +=, -=
 *   *(double) /(double) *=(double) /=(double)
 *
 */
template <typename MAP, typename ATT>
typename ATT::DATA_TYPE smooth(MAP& map, Vertex v, const ATT& attributs)
{
	typename ATT::DATA_TYPE res(0);
	int count=0;

	foreach_adjacent2<EDGE>(map,v,[&](Vertex x) // for all its neighbours (by edges)
	{
		res += attributs[x];
		count ++;
	});
	res /= typename PFP::REAL(count);
	return res;
}

/**
 * Example of function that work with any kind of Vertex Attribute
 */
template <typename MAP, typename ATT>
void applySmooth(MAP& map, const ATT& att_in, ATT& att_out)
{
	foreach_cell<VERTEX>(map,[&](Vertex v) // for all edge e of map do
	{
		att_out[v] = smooth(map,v,att_in);
	});
}





int main()
{
	// declare a map to handle the mesh
	MAP myMap;

	// add position attribute on vertices and get handler on it
	VertexAttribute<VEC3, MAP_IMPL> position = myMap.addAttribute<VEC3, VERTEX>("position");

	// create a topo grid of 2x2 squares
	Algo::Surface::Tilings::Square::Grid<PFP> grid(myMap, 2, 2, true);
	// and embed it using position attribute
	grid.embedIntoGrid(position, 2.,2.,0.);

	// second vertex attribute (for out)
	VertexAttribute<VEC3, MAP_IMPL> pos2 = myMap.addAttribute<VEC3, VERTEX>("pos2");

	// first color attribute on vertices
	VertexAttribute<VEC4, MAP_IMPL> vc = myMap.addAttribute<VEC4, VERTEX>("vertexColor");
	// second color attribute on vertices
	VertexAttribute<VEC4, MAP_IMPL> vc2 = myMap.addAttribute<VEC4, VERTEX>("vertexColor2");

	// add some random on position and compute a color
	for (unsigned int i= position.begin(); i != position.end(); position.next(i))
	{
		position[i] += VEC3(0.02f*i,0.01f*i,0.03f*i);
		vc[i] = VEC4(position[i][0]*0.3f,position[i][1]*0.3f,position[i][2]*0.3f,1.0f);
	}


	std::cout << "========== initial values =========="<<std::endl;
	// just print attributes values
	foreach_cell<VERTEX>(myMap,[&](Vertex v) // for all edge e of map do
	{
		std::cout << position[v]<< " / "<< vc[v]<< std::endl;
	});

	// define two multi attributes (one for in, one for out)
	Vertex2Attributes<VEC3, VEC4, MAP_IMPL> pv_in(position, vc);
	Vertex2Attributes<VEC3, VEC4, MAP_IMPL> pv_out(pos2, vc2);

	// and call algo once just like with simple attributes
	applySmooth(myMap, pv_in, pv_out);

	std::cout << "========== after smooth on position and color =========="<<std::endl;
	foreach_cell<VERTEX>(myMap,[&](Vertex v) // for all edge e of map do
	{
		std::cout << pos2[v]<< " / "<< vc2[v]<< std::endl;
	});


	return 0;
}
