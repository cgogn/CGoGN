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
#include "Utils/chrono.h"

using namespace CGoGN ;

/**
 * Struct that contains some informations about the types of the manipulated objects
 * Mainly here to be used by the algorithms that are parameterized by it
 */
struct PFP: public PFP_DOUBLE
{
	// definition of the type of the map
	typedef EmbeddedMap3 MAP;
};

// some typedef shortcuts
typedef PFP::MAP MAP ;				// map type
typedef PFP::VEC3 VEC3 ;			// type of R³ vector 



template <typename PFP, typename V_ATT>
typename V_ATT::DATA_TYPE localvolumeCentroidELW(typename PFP::MAP& map, Vol d, const V_ATT& attributs, unsigned int thread=0)
{
	typedef typename V_ATT::DATA_TYPE EMB;
	EMB center(0.0);

	double count=0.0;

	for (Edge it : edgesIncidentToVolume3(map,d))
	{
		EMB e1 = attributs[it.dart];
		EMB e2 = attributs[map.phi1(it)];
		double l = (e2-e1).norm();
		center += (e1+e2)*l;
		count += 2.0*l ;
	}
	center /= double(count);
	return center ;
}


int main()
{
	// declare a map to handle the mesh
	MAP myMap;
	// add position attribute on vertices and get handler on it
	VertexAttribute<VEC3, MAP> position = myMap.addAttribute<VEC3, VERTEX, MAP>("position");


	// create a topo grid of 4x4x4 squares
	Algo::Volume::Tilings::Cubic::Grid<PFP> cubic(myMap, 20, 20, 20);
	cubic.embedIntoGrid(position, 1.0f, 1.0f, 1.0f);

	// easy way to find the central vertex of the grid	
	Vertex v;
	foreach_cell_until<VERTEX>(myMap, [&] (Vertex it)
	{
		if (position[it] == VEC3(0,0,0))
		{
			v = it;
			std::cout << "Trouve"<< std::endl;
			return false;
		}
		return true;
	});

	// must test of find ok (if not v.dart is NIL)
	if (! v.valid())
		std::cerr << "could not find a vertex with position (0,0,0)" << std::endl;

// WITH TRAVERSORS:
	
	// find incident faces to vertex
	Traversor3VF<MAP> trvf(myMap, v.dart);
	for (Dart e = trvf.begin(); e != trvf.end(); e = trvf.next())
	{
		std::cout << "Face of dart "<<e<< " incident to vertex of dart " << v.dart<< std::endl;
	}

	// find adjacent vertices thru a face
	Traversor3VVaF<MAP> trvvaf(myMap, v.dart);
	for (Dart e = trvvaf.begin(); e != trvvaf.end(); e = trvvaf.next())
	{
		std::cout << "vertex of dart "<<e<< " adjacent to vertex of dart " << v.dart<< " by a face" << std::endl;
	}

// WITH FOREACH FUNCTION (C++11 lambda expression)

	// find faces incident to vertex v
	foreach_incident3<FACE>(myMap, v, [&](Face f)
	{
		std::cout << "Face of dart " << f << " incident to vertex of dart " << v.dart << std::endl;
	});

	// find vertices adjacent to vertex v thru a face
	foreach_adjacent3<FACE>(myMap, v, [&](Vertex x)
	{
		std::cout << "vertex of dart " << x << " adjacent to vertex of dart " << v.dart << " by a face" << std::endl;
	});

// WITH FOR C++11 SYNTAX
	for(Face f : facesIncidentToVertex3(myMap,v))
		std::cout << "Face of dart " << f << " incident to vertex of dart " << v.dart << std::endl;

	for (Vertex x : verticesAdjacentByFace3(myMap,v))
		std::cout << "vertex of dart " << x << " adjacent to vertex of dart " << v.dart << " by a face" << std::endl;



	for(Vertex v1 : allVerticesOf(myMap))
	{
		for (Vertex v2 : verticesAdjacentByEdge3(myMap,v1))
		{
			for (Face f : facesIncidentToVertex3(myMap,v2))
				if ((f.dart.index)%1000 == 1 )
					std::cout << "juste for fun face " << f << std::endl;
		}
	}


	Utils::Chrono ch;

	VertexAttribute<VEC3,MAP> pos2 = myMap.getAttribute<VEC3, VERTEX, MAP>("pos2") ;
	if(!pos2.isValid())
		pos2 = myMap.addAttribute<VEC3, VERTEX, MAP>("pos2") ;

	ch.start();

	for (int i=0; i< 20; ++i)
	{
		VEC3 xx(0,0,0);
		int nb=0;
		for(Vol d : allVolumesOf(myMap))
		{
			VEC3 vCentroid = localvolumeCentroidELW<PFP>(myMap, d, position) ;
			xx += vCentroid;
			nb++;
		}
		xx /= nb;
		std::cout << xx << std::endl;
	}
	std::cout << "for "<< ch.elapsed()<< " ms "<< std::endl;

	ch.start();

	for (int i=0; i< 20; ++i)
	{
		VEC3 xx(0,0,0);
		int nb=0;
		foreach_cell<VOLUME>(myMap, [&] (Vol d)
		{
			VEC3 vCentroid = Algo::Surface::Geometry::volumeCentroidELW<PFP>(myMap, d, position) ;
			xx += vCentroid;
			nb++;
		});
		xx /= nb;
		std::cout << xx << std::endl;
	}
	std::cout << "Lambda "<< ch.elapsed()<< " ms "<< std::endl;




	return 0;
}
