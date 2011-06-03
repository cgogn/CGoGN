/*******************************************************************************
 * CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
 * version 0.1                                                                  *
 * Copyright (C) 2009, IGG Team, LSIIT, University of Strasbourg                *
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
 * Web site: https://iggservis.u-strasbg.fr/CGoGN/                              *
 * Contact information: cgogn@unistra.fr                                        *
 *                                                                              *
 *******************************************************************************/

#include <iostream>
#include "Topology/generic/parameters.h"
#include "Topology/map/map2.h"
#include "Topology/generic/embeddedMap2.h"
#include <Algo/Geometry/inclusion.h>
#include "Geometry/vector_gen.h"

using namespace CGoGN;

struct PFP: public PFP_STANDARD
{
	// definition of the type of the map
	typedef EmbeddedMap2<Map2> MAP;
};

int main()
{
	std::cout << "Check Algo/Geometry/inclusion.h" << std::endl;
	std::cout << "Check Status : PARTIAL" << std::endl;

	PFP::MAP map;
	AttributeHandler<PFP::VEC3> position = map.addAttribute<PFP::VEC3>(VERTEX, "position");
	map.newFace(3);

	PFP::VEC3 ta(0,0,0);
	PFP::VEC3 tb(5,0,0);
	PFP::VEC3 tc(0,5,0);

	Dart d = map.begin();
	position[d] = ta;
	position[map.phi1(d)]= tb;
	position[map.phi1(map.phi1(d))] = tc;

	PFP::VEC3 pIn1(1,1,0);
	PFP::VEC3 pIn2(2,0,0);
	PFP::VEC3 pOut(6,0,0);


	std::cout << "Check isPointInConvexFace2D : Start" << std::endl;

	if(!Algo::Geometry::isPointInConvexFace2D<PFP>(map,d,position,pIn1))
		std::cout << "ERROR : isPointInConvexFace2D : face inclusion" << std::endl;

	if(!Algo::Geometry::isPointInConvexFace2D<PFP>(map,d,position,pIn2))
		std::cout << "ERROR : isPointInConvexFace2D : face inclusion" << std::endl;

	if(Algo::Geometry::isPointInConvexFace2D<PFP>(map,d,position,pOut))
		std::cout << "ERROR : isPointInConvexFace2D : face inclusion" << std::endl;

	if(!Algo::Geometry::isPointInConvexFace2D<PFP>(map,d,position,ta))
		std::cout << "ERROR : isPointInConvexFace2D : vertex inclusion" << std::endl;

	if(!Algo::Geometry::isPointInConvexFace2D<PFP>(map,d,position,tb))
		std::cout << "ERROR : isPointInConvexFace2D : vertex inclusion" << std::endl;

	if(!Algo::Geometry::isPointInConvexFace2D<PFP>(map,d,position,tc))
		std::cout << "ERROR : isPointInConvexFace2D : vertex inclusion" << std::endl;

	std::cout << "Check isPointInConvexFace2D : Done" << std::endl;

//	template <typename PFP>
//	bool isConvex(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& positions, bool CCW);

//	template <typename PFP>
//	bool isPointInVolume(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& positions, const typename PFP::VEC3& point);

//	template <typename PFP>
//	bool isPointInConvexVolume(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& positions, const typename PFP::VEC3& point, bool CCW);

//	template <typename PFP>
//	bool isPointInConvexFace(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& positions, const typename PFP::VEC3& point, bool CCW);

//	template <typename PFP>
//	bool isPointOnEdge(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& positions, const typename PFP::VEC3& point);

//	template <typename PFP>
//	bool isPointOnHalfEdge(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& positions, const typename PFP::VEC3& point);

//	template <typename PFP>
//	bool isPointOnVertex(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& positions, const typename PFP::VEC3& point);

//	template <typename PFP>
//	bool isConvexFaceInOrIntersectingTetrahedron(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& positions, const typename PFP::VEC3 points[4], bool CCW);

	return 0;
}
