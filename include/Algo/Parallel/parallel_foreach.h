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

#include "Topology/generic/functor.h"

#ifndef __PARALLEL_FOREACH__
#define __PARALLEL_FOREACH__

namespace CGoGN
{

namespace Algo
{

namespace Parallel
{

/// size of buffers to store darts or indexes in each threads
const unsigned int SIZE_BUFFER_THREAD = 8192;	// seems to be the best compromise

/**
 * How much threads has you computer
 */
inline unsigned int nbThreads();

inline unsigned int optimalNbThreads();

/**
 * Traverse orbits of a map in parallel. Use DartMarker
 * Use this version if you need to have acces to each functors after the traversal (to compute a sum or an average for example)
 * @param map the map
 * @param funcs the functors (one for each thread) to apply (size of vector must be equal to nbth)
 * @param nbth number of thread to use
 * @param needMarkers set to yes if you want that each thread use different markers. Warning if set to false do not use thread id in algo !!
 * @param good a selector
 */
//template <typename MAP, unsigned int ORBIT>
//void foreach_orbit(MAP& map, std::vector<FunctorMapThreaded<MAP>*>& funcs, unsigned int nbth, bool needMarkers = false, const FunctorSelect& good = allDarts);

/**
 * Traverse orbits of a map in parallel. Use DartMarker
 * Use this version if you do not need to keep functors
 * @param map the map
 * @param func the functor
 * @param shared set to true if functors can be shared over the different thread (if false a copy will be used)
 * @param nbth number of thread to use
 * @param needMarkers set to yes if you want that each thread use different markers. Warning if set to false do not use thread id in algo !!
 * @param good a selector
 */
//template <typename MAP, unsigned int ORBIT>
//void foreach_orbit(MAP& map, FunctorMapThreaded<MAP>& func, bool shared, unsigned int nbth, bool needMarkers = false, const FunctorSelect& good = allDarts);

/**
 * Traverse cells of a map in parallel. Use quick traversal, cell markers or dart markers if available !
 * Use this version if you need to have acces to each functors after the traversal (to compute a sum or an average for example)
 * @param map the map
 * @param funcs the functors to apply (size of vector must be equal to nbth)
 * @param nbth number of threads
 * @param needMarkers set to yes if you want that each thread use different markers. Warning if set to false (default) do not use algo with thread id or markers !!
 * @param good a selector
 */
template <typename MAP, unsigned int ORBIT>
void foreach_cell(MAP& map, std::vector<FunctorMapThreaded<MAP>*>& funcs, unsigned int nbth, bool needMarkers = false, const FunctorSelect& good = allDarts, unsigned int currentThread = 0);

/**
 * Traverse cells of a map in parallel. Use quick traversal, cell markers or dart markers if available !
 * Use this version if you do not need to keep functors
 * @param map the map
 * @param func the functor to apply
 * @param shared set to true if functors can be shared over the different thread (if false a copy will be used)
 * @param nbth number of threads 0 for let the system choose
 * @param needMarkers set to yes if you want that each thread use different markers. Warning if set to false (default) do not use algo with thread id or markers !!
 * @param good a selector
 */
template <typename MAP, unsigned int ORBIT>
void foreach_cell(MAP& map, FunctorMapThreaded<MAP>& func, bool shared, unsigned int nbth = 0, bool needMarkers = false, const FunctorSelect& good = allDarts, unsigned int currentThread = 0);

/**
 * Traverse darts of a map in parallel
 * Use this version if you need to have acces to each functors after the traversal (to compute a sum or an average for example)
 * @param map the map
 * @param funcs the functors to apply
 * @param nbth number of thread to use
 * @param needMarkers set to yes if you want that each thread use different markers.Warning if set to false (default) do not use algo with thread id or markers !!
 * @param good a selector
 */
template <typename MAP>
void foreach_dart(MAP& map, std::vector<FunctorMapThreaded<MAP>*>& funcs,  unsigned int nbth, bool needMarkers = false, const FunctorSelect& good = allDarts);


/**
 * Traverse darts of a map in parallel
 * @param map the map
 * @param funcs the functor
 * @param shared set to true if functors can be shared over the different thread (if false a copy will be used)
 * @param nbth number of thread to use, 0 for let the system choose
 * @param needMarkers set to yes if you want that each thread use different markers. Warning if set to false (default) do not use algo with thread id or markers !!
 * @param good a selector
 */
template <typename MAP>
void foreach_dart(MAP& map, FunctorMapThreaded<MAP>& func, bool shared, unsigned int nbth = 0, bool needMarkers = false, const FunctorSelect& good = allDarts);


/**
 * Traverse all elements of an attribute container (attribute handler is placed in FunctorAttribThreaded)
 * @param attr_cont the attribute container to traverse
 * @param func the fonctors to use
 * @param nbth number of thread to use for computation
 */
void foreach_attrib(AttributeContainer& attr_cont, std::vector<FunctorAttribThreaded*> funcs, unsigned int nbth);

/**
 * Traverse all elements of an attribute container (attribute handler is placed in FunctorAttribThreaded
 * @param attr_cont the attribute container to traverse
 * @param func the functor to use
 * @param shared if true the internal functors pointers are used as address of functor param, else copies are done with duplicate
 * @param nbth number of thread to use for computation 0 for let the system choose
 */
void foreach_attrib(AttributeContainer& attr_cont, FunctorAttribThreaded& func, bool shared, unsigned int nbth = 0);


/**
 * Optimized version for // foreach with to pass (2 functors), with several loops
 * Use this version if you need to keep functors
 * @param map the map
 * @param funcsFrontnBack nbth front pass functors followed by nbth back pass functors
 * @param nbLoops number of loops to execute
 * @param nbth number of threads to use
 * @param needMarkers set to yes if you want that each thread use different markers (markers are allocated if necessary)
 * @param good a selector
 */
template <typename MAP, unsigned int CELL>
void foreach_cell2Pass(MAP& map, std::vector<FunctorMapThreaded<MAP>*>& funcsFrontnBack, unsigned int nbLoops, unsigned int nbth, bool needMarkers = false, const FunctorSelect& good = allDarts);

/**
 * Optimized version for // foreach with to pass (2 functors), with several loops
 * Use this version if you do not need to keep functors
 * @param map the map
 * @param funcFront front pass functor
 * @param funcBack back pass functor
 * @param nbLoops number of loops to execute
 * @param nbth number of threads to use
 * @param needMarkers set to yes if you want that each thread use different markers (markers are allocated if necessary)
 * @param good a selector
 */
template <typename MAP, unsigned int CELL>
void foreach_cell2Pass(MAP& map, FunctorMapThreaded<MAP>& funcFront, FunctorMapThreaded<MAP>& funcBack, bool shared, unsigned int nbLoops, unsigned int nbth, bool needMarkers = false, const FunctorSelect& good = allDarts);


} // namespace Parallel

} // namespace Algo

} // namespace CGoGN

#include "Algo/Parallel/parallel_foreach.hpp"

#endif
