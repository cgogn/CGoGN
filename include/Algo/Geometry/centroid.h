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

#ifndef __ALGO_GEOMETRY_CENTROID_H__
#define __ALGO_GEOMETRY_CENTROID_H__

#include "Geometry/basic.h"
#include "Topology/generic/attribops.h"

namespace CGoGN
{

namespace Algo
{

namespace Geometry
{

/**
* Compute volume centroid (generic version)
*  Pre: closed volume & embedded vertices
* Template param:
*  PFP:  as usual
*  EMBV: attributes vector type  or cell type (VertexCell, FaceCell, ...)
*  EMB:  type of attribute (Geom::Vec3f) or cell type (VertexCell, FaceCell, ...)
* @param map the map
* @param d a dart of the face
* @param attributs the vector of attribute or cell
*/
template <typename PFP, typename EMBV, typename EMB>
EMB volumeCentroidGen(typename PFP::MAP& map, Dart d, const EMBV& attributs, unsigned int thread = 0);

/**
* Compute volume centroid
* Pre: closed volume & embedded vertices
 * @param map the map
 * @param d a dart of the face
 * @param position the vector of attribute
 */
template <typename PFP>
typename PFP::VEC3 volumeCentroid(typename PFP::MAP& map, Dart d, const VertexAttribute<typename PFP::VEC3>& position, unsigned int thread = 0)
{
	return volumeCentroidGen<PFP, VertexAttribute<typename PFP::VEC3>, typename PFP::VEC3>(map, d, position, thread);
}

/**
 * Compute face centroid (generic version)
 * Template param:
 *  PFP:  as usual
 *  EMBV: attributes vector type  or cell type (VertexCell, FaceCell, ...)
 *  EMB:  type of attribute (Geom::Vec3f) or cell type (VertexCell, FaceCell, ...)
 * @param map the map
 * @param d a dart of the face
 * @param attributs the vector of attribute or cell
 */
template <typename PFP, typename EMBV, typename EMB>
EMB faceCentroidGen(typename PFP::MAP& map, Dart d, const EMBV& attributs);

/**
* Compute face centroid
* Pre: closed face & embedded vertices
 * @param map the map
 * @param d a dart of the face
 * @param position the vector of attribute
 */
template <typename PFP>
typename PFP::VEC3 faceCentroid(typename PFP::MAP& map, Dart d, const VertexAttribute<typename PFP::VEC3>& position)
{
	return faceCentroidGen<PFP, VertexAttribute<typename PFP::VEC3>, typename PFP::VEC3>(map, d, position);
}

/**
 * Compute vertex neighbours centroid (generic version)
 * Template param:
 *  PFP:  as usual
 *  EMBV: attributes vector type  or cell type (VertexCell, FaceCell, ...)
 *  EMB:  type of attribute (Geom::Vec3f) or cell type (VertexCell, FaceCell, ...)
 * @param map the map
 * @param d a dart of the face
 * @param position the vector of attribute or cell
 */
template <typename PFP, typename EMBV, typename EMB>
EMB vertexNeighborhoodCentroidGen(typename PFP::MAP& map, Dart d, const EMBV& attributs);

/**
 * Compute  vertex neighbours centroid
 * @param map the map
 * @param d a dart of the face
 * @param position the vector of attribute
 */
template <typename PFP>
typename PFP::VEC3 vertexNeighborhoodCentroid(typename PFP::MAP& map, Dart d, const VertexAttribute<typename PFP::VEC3>& position)
{
	return vertexNeighborhoodCentroidGen<PFP, VertexAttribute<typename PFP::VEC3>, typename PFP::VEC3>(map, d, position);
}

template <typename PFP>
void computeCentroidVolumes(typename PFP::MAP& map,
		const VertexAttribute<typename PFP::VEC3>& position, VolumeAttribute<typename PFP::VEC3>& vol_centroid,
		const FunctorSelect& select = allDarts, unsigned int thread = 0) ;

template <typename PFP>
void computeCentroidFaces(typename PFP::MAP& map,
		const VertexAttribute<typename PFP::VEC3>& position, FaceAttribute<typename PFP::VEC3>& face_centroid,
		const FunctorSelect& select = allDarts, unsigned int thread = 0) ;

template <typename PFP>
void computeNeighborhoodCentroidVertices(typename PFP::MAP& map,
		const VertexAttribute<typename PFP::VEC3>& position, VertexAttribute<typename PFP::VEC3>& vertex_centroid,
		const FunctorSelect& select = allDarts, unsigned int thread = 0) ;


namespace Parallel
{
template <typename PFP>
void computeCentroidVolumes(typename PFP::MAP& map,
		const VertexAttribute<typename PFP::VEC3>& position, VolumeAttribute<typename PFP::VEC3>& vol_centroid,
		const FunctorSelect& select = allDarts, unsigned int nbth = 0, unsigned int current_thread = 0) ;

template <typename PFP>
void computeCentroidFaces(typename PFP::MAP& map,
		const VertexAttribute<typename PFP::VEC3>& position, FaceAttribute<typename PFP::VEC3>& face_centroid,
		const FunctorSelect& select = allDarts, unsigned int nbth = 0, unsigned int current_thread = 0) ;

template <typename PFP>
void computeNeighborhoodCentroidVertices(typename PFP::MAP& map,
		const VertexAttribute<typename PFP::VEC3>& position, VertexAttribute<typename PFP::VEC3>& vertex_centroid,
		const FunctorSelect& select = allDarts, unsigned int nbth = 0, unsigned int current_thread = 0) ;
}

} // namespace Geometry

} // namespace Algo

} // namespace CGoGN

#include "Algo/Geometry/centroid.hpp"

#endif
