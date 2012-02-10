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

#ifndef __ALGO_GEOMETRY_AREA_H__
#define __ALGO_GEOMETRY_AREA_H__

namespace CGoGN
{

namespace Algo
{

namespace Geometry
{

template <typename PFP>
typename PFP::REAL triangleArea(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position) ;

template <typename PFP>
typename PFP::REAL convexFaceArea(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position) ;

template <typename PFP>
typename PFP::REAL totalArea(typename PFP::MAP& map, const typename PFP::TVEC3& position, const FunctorSelect& select = allDarts, unsigned int thread = 0) ;

template <typename PFP>
typename PFP::REAL vertexOneRingArea(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position) ;

template <typename PFP>
typename PFP::REAL vertexBarycentricArea(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position) ;

template <typename PFP>
typename PFP::REAL vertexVoronoiArea(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position) ;

template <typename PFP>
void computeAreaFaces(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TREAL& face_area, const FunctorSelect& select = allDarts) ;

template <typename PFP>
void computeOneRingAreaVertices(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TREAL& vertex_area, const FunctorSelect& select = allDarts) ;

template <typename PFP>
void computeBarycentricAreaVertices(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TREAL& vertex_area, const FunctorSelect& select = allDarts) ;

template <typename PFP>
void computeVoronoiAreaVertices(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TREAL& vertex_area, const FunctorSelect& select = allDarts) ;

} // namespace Geometry

} // namespace Algo

} // namespace CGoGN

#include "Algo/Geometry/area.hpp"

#endif
