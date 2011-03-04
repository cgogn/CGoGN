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

#ifndef __DECIMATION_H__
#define __DECIMATION_H__

#include "Algo/Decimation/edgeSelector.h"
#include "Algo/Decimation/halfEdgeSelector.h"
#include "Algo/Decimation/geometryApproximator.h"
#include "Algo/Decimation/lightfieldApproximator.h"

namespace CGoGN
{

namespace Algo
{

namespace Decimation
{

template <typename PFP>
void decimate(
	typename PFP::MAP& map,
	SelectorType s,
	ApproximatorType a,
	typename PFP::TVEC3& position,
	unsigned int nbWantedVertices,
	const FunctorSelect& selected = SelectorTrue()
) ;

} //namespace Decimation

} //namespace Algo

} //namespace CGoGN

#include "Algo/Decimation/decimation.hpp"

#endif
