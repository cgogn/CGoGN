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

#ifndef __DECIMATION_H__
#define __DECIMATION_H__

#include "Algo/Decimation/edgeSelector.h"
#include "Algo/Decimation/halfEdgeSelector.h"
#include "Algo/Decimation/geometryApproximator.h"
#include "Algo/Decimation/colorPerVertexApproximator.h"
#include "Algo/Decimation/lightfieldApproximator.h"

namespace CGoGN
{

namespace Algo
{

namespace Decimation
{

/**
 * Decimate the mesh through edge contraction
 * by using a selector and approximator scheme.
 *
 * \param map the map to decimate
 * \param s the SelectorType
 * \param a the ApproximatorType
 * \param position the vertex position embeddings
 * \param nbWantedVertices the aimed amount of vertices after decimation
 * \param selected the selector stipulating which darts are eligible for contraction
 * \param edgeErrors will (if not null) contain the edge errors computed by the approximator/selector (default NULL)
 * \param callback_wrapper a callback function for progress monitoring (default NULL)
 * \param callback_object the object to call the callback on (default NULL)
 */
template <typename PFP>
void decimate(
	typename PFP::MAP& map,
	SelectorType s,
	ApproximatorType a,
	std::vector<VertexAttribute<typename PFP::VEC3> *>& position,
	unsigned int nbWantedVertices,
	const FunctorSelect& selected = allDarts,
	VertexAttribute<typename PFP::VEC3> *edgeErrors = NULL,
	void (*callback_wrapper)(void*, const void*) = NULL, void *callback_object = NULL
) ;

} //namespace Decimation

} //namespace Algo

} //namespace CGoGN

#include "Algo/Decimation/decimation.hpp"

#endif
