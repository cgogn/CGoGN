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

enum
{
	EMPTY = 0,
	SEGMENT = 1,
	BARY = 2
};

typedef struct { Dart d ; float w ; } e0point ;
typedef struct { e0point p1 ; e0point p2 ; unsigned char type ; } e0segment ;
typedef NoMathIONameAttribute<e0segment> ridgeSegment ;

template <typename PFP>
void featureEdgeDetection(
	typename PFP::MAP& map,
	const typename PFP::TVEC3& position,
	CellMarker& featureEdge) ;

template <typename PFP>
void computeFaceGradient(
	typename PFP::MAP& map,
	const typename PFP::TVEC3& position,
	const typename PFP::TVEC3& face_normal,
	const typename PFP::TREAL& kmax,
	const typename PFP::TREAL& face_area,
	typename PFP::TVEC3& face_gradient,
	const FunctorSelect& select = allDarts,
	unsigned int thread = 0) ;

template <typename PFP>
typename PFP::VEC3 faceGradient(
	typename PFP::MAP& map,
	Dart d,
	const typename PFP::TVEC3& position,
	const typename PFP::TVEC3& face_normal,
	const typename PFP::TREAL& kmax,
	const typename PFP::TREAL& area) ;

template <typename PFP>
void computeVertexGradient(
	typename PFP::MAP& map,
	const typename PFP::TVEC3& face_gradient,
	const typename PFP::TREAL& face_area,
	typename PFP::TVEC3& vertex_gradient,
	const FunctorSelect& select = allDarts,
	unsigned int thread = 0) ;

template <typename PFP>
typename PFP::VEC3 vertexGradient(
	typename PFP::MAP& map,
	Dart d,
	const typename PFP::TVEC3& face_gradient,
	const typename PFP::TREAL& area) ;

//template <typename PFP>
//typename PFP::REAL extremality(
//	typename PFP::MAP& map,
//	Dart d,
//	const typename PFP::VEC3& K,
//	const typename PFP::TVEC3& face_gradient,
//	const typename PFP::TREAL& face_area) ;

template <typename PFP>
void computeTriangleType(
	typename PFP::MAP& map,
	const typename PFP::TVEC3& Kmax,
	CellMarker& regularMarker,
	const FunctorSelect& select = allDarts,
	unsigned int thread = 0) ;

template <typename PFP>
bool isTriangleRegular(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& Kmax) ;

template <typename PFP>
void initRidgeSegments(
	typename PFP::MAP& map,
	AttributeHandler<ridgeSegment>& ridge_segments,
	const FunctorSelect& select = allDarts,
	unsigned int thread = 0) ;

template <typename PFP>
void computeRidgeLines(
	typename PFP::MAP& map,
	CellMarker& regularMarker,
	const typename PFP::TVEC3& vertex_gradient,
	const typename PFP::TVEC3& K,
	AttributeHandler<ridgeSegment>& ridge_segments,
	const FunctorSelect& select = allDarts,
	unsigned int thread = 0) ;

template <typename PFP>
void ridgeLines(
	typename PFP::MAP& map,
	Dart d,
	const typename PFP::TVEC3& K,
	const typename PFP::TVEC3& vertex_gradient,
	AttributeHandler<ridgeSegment>& ridge_segments) ;

template <typename PFP>
void computeExtremalities() ;

template <typename PFP>
void computeSingularTriangle(
	typename PFP::MAP& map,
	CellMarker& regularMarker,
	AttributeHandler<ridgeSegment>& ridge_segments,
	const FunctorSelect& select = allDarts,
	unsigned int thread = 0) ;

template <typename PFP>
void singularTriangle(
	typename PFP::MAP& map,
	Dart d,
	CellMarker& regularMarker,
	AttributeHandler<ridgeSegment>& ridge_segments) ;

template <typename PFP>
bool isEdgeInTriangle(typename PFP::MAP& map, Dart edge, Dart triangle) ;

template <typename PFP>
std::vector<typename PFP::VEC3> occludingContoursDetection(
	typename PFP::MAP& map,
	const typename PFP::VEC3& cameraPosition,
	const typename PFP::TVEC3& position,
	const typename PFP::TVEC3& normal) ;

} // namespace Geometry

} // namespace Algo

} // namespace CGoGN

#include "Algo/Geometry/feature.hpp"

#endif
