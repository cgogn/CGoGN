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

#ifndef __ALGO_GEOMETRY_NORMAL_H__
#define __ALGO_GEOMETRY_NORMAL_H__

#include "Geometry/basic.h"

namespace CGoGN
{

namespace Algo
{

namespace Geometry
{

template <typename PFP>
typename PFP::VEC3 triangleNormal(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position) ;

template <typename PFP>
typename PFP::VEC3 faceNormal(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position) ;

template <typename PFP>
typename PFP::VEC3 vertexNormal(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position) ;

template <typename PFP>
void computeNormalFaces(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TVEC3& face_normal, const FunctorSelect& select = SelectorTrue(), unsigned int thread = 0) ;

/**
 * compute normals of  vertices
 * @param map the map on which we work
 * @param position the position of vertices attribute handler
 * @param normal the normal handler in which the result will be stored
 * @param the selector
 * @ param th the thread number
 */
template <typename PFP>
void computeNormalVertices(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TVEC3& normal, const FunctorSelect& select = SelectorTrue(), unsigned int thread = 0) ;

template <typename PFP>
typename PFP::REAL computeAngleBetweenNormalsOnEdge(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position) ;

template <typename PFP>
void computeAnglesBetweenNormalsOnEdges(typename PFP::MAP& map, typename PFP::TVEC3& position, typename PFP::TREAL& angles, const FunctorSelect& select = SelectorTrue(), unsigned int thread = 0) ;

} // namespace Geometry

} // namespace Algo

} // namespace CGoGN

#include "Algo/Geometry/normal.hpp"

#endif
