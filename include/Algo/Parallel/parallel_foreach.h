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


#include "Topology/generic/functor.h"

#ifndef __PARALLEL_FOREACH__
#define __PARALLEL_FOREACH__

namespace CGoGN
{

namespace Algo
{

namespace Parallel
{


/**
 * Traverse orbits of a map in parallel. Use topological marker
 * Functor application must be independant
 * @param map the map
 * @param orbit the orbit (VERTEX_ORBIT/EDGE_ORBIT/FACE_ORBIT/..
 * @param func the functor to apply
 * @param nbth number of thread to use ((use twice as threads of processor)
 * @param szbuff size of buffers to store darts in each thread (default is 8192, use less for lower memory consumsion)
 * @param good a selector
 */
template <typename PFP>
void foreach_orbit(typename PFP::MAP& map,  unsigned int orbit, FunctorType& func,  unsigned int nbth, unsigned int szbuff=8192, const FunctorSelect& good= SelectorTrue());


/**
 * Traverse cells of a map in parallel. Use embedding marker
 * Functor application must be independant
 * @param map the map
 * @param orbit the cell (VERTEX_CELL/EDGE_CELL/FACE_CELL/..
 * @param func the functor to apply
 * @param nbth number of thread to use (use twice as threads of processor)
 * @param szbuff size of buffers to store darts in each thread (default is 8192, use less for lower memory consumsion)
 * @param good a selector
 */
template <typename PFP>
void foreach_cell(typename PFP::MAP& map, unsigned int cell, FunctorType& func,  unsigned int nbth, unsigned int szbuff=8192, const FunctorSelect& good= SelectorTrue());


}
}	// end namespace
}


#include "Algo/Parallel/parallel_foreach.hpp"


#endif

