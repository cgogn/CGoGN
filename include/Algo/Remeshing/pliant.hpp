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

#include "Algo/Geometry/basic.h"

namespace CGoGN
{

namespace Algo
{

namespace Remeshing
{

template <typename PFP>
void pliantRemeshing(typename PFP::MAP& map, typename PFP::TVEC3& position, typename PFP::TVEC3& normal)
{
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	// compute the mean edge length
	DartMarker m1(map) ;
	REAL meanEdgeLength = 0 ;
	unsigned int nbEdges = 0 ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(!m1.isMarked(d))
		{
			m1.markOrbit(EDGE_ORBIT, d) ;
			meanEdgeLength += Algo::Geometry::edgeLength<PFP>(map, d, position) ;
			++nbEdges ;
		}
	}
	meanEdgeLength /= REAL(nbEdges) ;

	// compute the min and max edge lengths
	REAL edgeLengthInf = REAL(3) / REAL(4) * meanEdgeLength ;
	REAL edgeLengthSup = REAL(4) / REAL(3) * meanEdgeLength ;

	// split long edges
	DartMarker m2(map) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(!m2.isMarked(d))
		{
			m2.markOrbit(EDGE_ORBIT, d) ;
			REAL length = Algo::Geometry::edgeLength<PFP>(map, d, position) ;
			if(length > edgeLengthSup)
			{
				Dart dd = map.phi2(d) ;
				VEC3 p = REAL(0.5) * (position[d] + position[dd]) ;
				map.cutEdge(d) ;
				position[map.phi1(d)] = p ;
				map.splitFace(map.phi1(d), map.phi_1(d)) ;
				map.splitFace(map.phi1(dd), map.phi_1(dd)) ;
			}
		}
	}

	// collapse short edges
	DartMarker m3(map) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(!m3.isMarked(d))
		{
			m3.markOrbit(EDGE_ORBIT, d) ;
			REAL length = Algo::Geometry::edgeLength<PFP>(map, d, position) ;
			if(length < edgeLengthInf)
			{
				bool collapse = true ;
				Dart dd = map.phi2(d) ;
				VEC3 p = position[dd] ;
				Dart vit = map.alpha1(d) ;
				do
				{
					VEC3 vec = position[dd] - position[map.phi1(vit)] ;
					if(vec.norm() > edgeLengthSup)
						collapse = false ;
					vit = map.alpha1(vit) ;
				} while(vit != d && collapse) ;
				if(collapse)
				{
					Dart v = map.collapseEdge(d) ;
					position[v] = p ;
				}
			}
		}
	}

	// equalize valences with edge flips
	DartMarker m4(map) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(!m4.isMarked(d))
		{
			m4.markOrbit(EDGE_ORBIT, d) ;
			Dart e = map.phi2(d) ;
			unsigned int w = map.vertexDegree(d) ;
			unsigned int x = map.vertexDegree(e) ;
			unsigned int y = map.vertexDegree(map.phi1(map.phi1(d))) ;
			unsigned int z = map.vertexDegree(map.phi1(map.phi1(e))) ;
			int flip = 0 ;
			flip += w > 6 ? 1 : (w < 6 ? -1 : 0) ;
			flip += x > 6 ? 1 : (x < 6 ? -1 : 0) ;
			flip += y < 6 ? 1 : (y > 6 ? -1 : 0) ;
			flip += z < 6 ? 1 : (z > 6 ? -1 : 0) ;
			if(flip > 2)
			{
				map.flipEdge(d) ;
				m4.markOrbit(EDGE_ORBIT, map.phi1(d)) ;
				m4.markOrbit(EDGE_ORBIT, map.phi_1(d)) ;
				m4.markOrbit(EDGE_ORBIT, map.phi1(e)) ;
				m4.markOrbit(EDGE_ORBIT, map.phi_1(e)) ;
			}
		}
	}

	// update vertices normals
	Algo::Geometry::computeNormalVertices<PFP>(map, position, normal) ;

	// tangential relaxation
	AttributeHandler<VEC3> newPosition = map.template addAttribute<VEC3>(VERTEX_ORBIT, "newPosition") ;
	Algo::Geometry::computeNeighborhoodCentroidVertices<PFP>(map, position, newPosition) ;

	CellMarker vm(map, VERTEX_CELL) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(!vm.isMarked(d))
		{
			vm.mark(d) ;
			VEC3 l = position[d] - newPosition[d] ;
			newPosition[d] = newPosition[d] + ((normal[d] * l) * normal[d]) ;
		}
	}
	map.template swapAttributes<VEC3>(position, newPosition) ;
	map.template removeAttribute<VEC3>(newPosition) ;
}

} // namespace Remeshing

} // namespace Algo

} // namespace CGoGN
