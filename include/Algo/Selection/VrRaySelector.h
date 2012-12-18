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

#ifndef __VRRAYSELECTOR_H_
#define __VRRAYSELECTOR_H_

#include <algorithm>
#include <vector>
#include <cmath>
#include "Algo/Parallel/parallel_foreach.h"



namespace CGoGN
{

namespace Algo
{

namespace Selection
{

/**
 * Function that does the selection of vertices in a cone, returned darts are sorted from closest to farthest
 * @param map the map we want to test
 * @param position the position attribute
 * @param rayA first point of  ray (user side)
 * @param rayAB vector of ray (directed ot the scene)
 * @param angle angle of the code in degree.
 * @param vecVertices (out) vector to store dart of intersected vertices
 * @param good the selector
 */
template<typename PFP>
void verticesConeSelection(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position,
		const typename PFP::VEC3& rayA, const typename PFP::VEC3& rayAB, float angle, std::vector<Dart>& vecVertices,
		const FunctorSelect& good= allDarts);

/**
 * Function that does the selection of edges, returned darts are sorted from closest to farthest
 * @param map the map we want to test
 * @param position the position attribute
 * @param rayA first point of  ray (user side)
 * @param rayAB vector of ray (directed ot the scene)
 * @param angle radius of the cylinder of selection
 * @param vecEdges (out) vector to store dart of intersected edges
 * @param good the selector
 */
template<typename PFP>
void edgesConeSelection(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position,
				const typename PFP::VEC3& rayA, const typename PFP::VEC3& rayAB, float angle, std::vector<Dart>& vecEdges,
				const FunctorSelect& good=allDarts);


/**
 * Function that select the closest vertex in the bubble
 * @param map the map we want to test
 * @param position the position attribute
 * @param cursor the cursor position (center of bubble)
 * @param radiusMax max radius of selection
 * @param good the selector
 */
template<typename PFP>
Dart verticesBubbleSelection(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position,
		const typename PFP::VEC3& cursor, PFP::REAL radiusMax,
		const FunctorSelect& good=allDarts);


/**
 * Function that select the closest edge in the bubble
 * @param map the map we want to test
 * @param position the position attribute
 * @param cursor the cursor position (center of bubble)
 * @param radiusMax max radius of selection
 * @param good the selector
 */
template<typename PFP>
Dart edgesBubbleSelection(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position,
		const typename PFP::VEC3& cursor, PFP::REAL radiusMax,
		const FunctorSelect& good=allDarts);





}
}
}



#endif
