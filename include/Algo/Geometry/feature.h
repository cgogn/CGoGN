/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2011, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.u-strasbg.fr/                                         *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#ifndef __ALGO_GEOMETRY_FEATURE_H__
#define __ALGO_GEOMETRY_FEATURE_H__

namespace CGoGN
{

namespace Algo
{

namespace Geometry
{

template <typename PFP>
void featureEdgeDetection(typename PFP::MAP& map, const typename PFP::TVEC3& position, CellMarker& featureEdge) ;

template <typename PFP>
void computeArea(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TREAL& area, const FunctorSelect& select = allDarts, unsigned int thread = 0) ;

template <typename PFP>
void computeFaceGradient(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TVEC3& face_gradient, const typename PFP::TVEC3& face_normal, const typename PFP::TREAL& kmax, const typename PFP::TREAL& area, const FunctorSelect& select = allDarts, unsigned int thread = 0) ;

template <typename PFP>
void computeGradient(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TVEC3& gradient, typename PFP::TVEC3& face_gradient, const typename PFP::TREAL& area, const FunctorSelect& select = allDarts, unsigned int thread = 0) ;

template <typename PFP>
void computeTriangleType(typename PFP::MAP& map, const typename PFP::TVEC3& position, CellMarker& regularMarker, const FunctorSelect& select = allDarts, unsigned int thread = 0) ;

template <typename PFP>
void computeCurvatureSign(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TVEC3& triangle_type, typename PFP::TVEC3& k, const FunctorSelect& select = allDarts, unsigned int thread = 0) ;

template <typename PFP>
std::vector<typename PFP::VEC3> occludingContoursDetection(typename PFP::MAP& map, const typename PFP::VEC3& cameraPosition, const typename PFP::TVEC3& position, const typename PFP::TVEC3& normal) ;

template <typename PFP>
typename PFP::TREAL faceArea(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position) ;

template <typename PFP>
typename PFP::VEC3 faceGradient(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position, const typename PFP::TVEC3& face_normal, const typename PFP::TREAL& kmax, const typename PFP::TREAL& area) ;

template <typename PFP>
bool triangleType(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position) ;

template <typename PFP>
bool isInSameOctant(const typename PFP::VEC3& pos1, const typename PFP::VEC3& pos2) ;

template <typename PFP>
void curvatureSign(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position, typename PFP::TVEC3& triangle_type, typename PFP::TVEC3& k) ;

template <typename PFP>
typename PFP::TVEC3 vertexGradient(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position, const typename PFP::TVEC3& face_gradient, const typename PFP::TREAL& area) ;

} // namespace Geometry

} // namespace Algo

} // namespace CGoGN

#include "Algo/Geometry/feature.hpp"

#endif
