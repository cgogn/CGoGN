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

#ifndef __ALGO_GEOMETRY_DISTANCE_H__
#define __ALGO_GEOMETRY_DISTANCE_H__

namespace CGoGN
{

namespace Algo
{

namespace Geometry
{

/**
* compute squared distance from point to the plane of a planar face
* @param map the map
* @param f a face
* @param position the vertex attribute storing positions
* @param P the point
* @return the squared distance to the plane
*/
template <typename PFP>
typename PFP::REAL squaredDistancePoint2FacePlane(typename PFP::MAP& map, Face f, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position, const typename PFP::VEC3& P) ;

/**
* compute squared distance from point to face (assuming face is convex)
* Algo: min  distance of each subtriangle of face (not optimum ?)
* @param map the map
* @param f a face
* @param position the vertex attribute storing positions
* @param P the point
* @return the squared distance
*/
template <typename PFP>
typename PFP::REAL squaredDistancePoint2Face(typename PFP::MAP& map, Face f, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position, const typename PFP::VEC3& P) ;

/**
* compute the barycentric coordinates of the point in the triangle f that is closest to point P
* @param map the map
* @param f a triangle face
* @param position the vertex attribute storing positions
* @param P the point
* @param u barycentric coordinate 1 of closest point
* @param v barycentric coordinate 2 of closest point
* @param w barycentric coordinate 3 of closest point
*/
template <typename PFP>
void closestPointInTriangle(typename PFP::MAP& map, Face f, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position, const typename PFP::VEC3& P, double& u, double& v, double& w) ;

/**
* compute squared distance from point to an edge
* @param map the map
* @param e an edge
* @param position the vertex attribute storing positions
* @param P the point
* @return the squared distance
*/
template <typename PFP>
typename PFP::REAL squaredDistancePoint2Edge(typename PFP::MAP& map, Edge e, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position, const typename PFP::VEC3& P) ;

//template <typename PFP>
//void computeDistance(typename PFP::MAP& map1, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position1, VertexAttribute<typename PFP::REAL, typename PFP::MAP>& distance1,
//					 typename PFP::MAP& map2, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position2) ;

} // namespace Geometry

} // namespace Algo

} // namespace CGoGN

#include "Algo/Geometry/distances.hpp"

#endif
