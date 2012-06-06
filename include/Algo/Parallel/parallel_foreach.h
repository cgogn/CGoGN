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

// TODO a deplacer dans functor.h
/**
 * Functor class for parallel::foreach_xxx
 */
template<typename MAP>
class FunctorMapThreaded: public FunctorMap<MAP>
{
protected:
	// current thread id
	unsigned int m_threadId;
public:
	/**
	 * constructor
	 * @param map traversed map
	 * @param th current thread id
	 */
	FunctorMapThreaded(MAP& m, unsigned int th = 0): FunctorMap<MAP>(m), m_threadId(th) {}

	/**
	 * Method that duplicate the fonctor for each thread, typically:
	 *    FunctorXXXX<MAP>* copy = new FunctorXXXX<MAP>(this->m_map,pos,norm,id);
	 *    return reinterpret_cast<FunctorMapThreaded<MAP>*>(copy);
	 *  @param id thread id assigned to functor
	 */
	virtual FunctorMapThreaded<MAP>* duplicate(unsigned int id)=0;
};

/**
 * Functor class for parallel::foreach_xxx_res
 */
template<typename MAP,typename T>
class FunctorMapThreadedResult: public FunctorMapThreaded<MAP>
{
public:
	FunctorMapThreadedResult(MAP& m, unsigned int th = 0): FunctorMapThreaded<MAP>(m,th) {}

	/**
	 * get back result (used by foreach_xxx_res for filling vector of results
	 */
	virtual T getResult() = 0;
};

/**
 * Traverse orbits of a map in parallel. Use topological marker
 * Functor application must be independant
 * @param map the map
 * @param func the functor to apply
 * @param nbth number of thread to use
 * @param szbuff size of buffers to store darts in each thread (default is 8192, use less for lower memory consumsion)
 * @param needMarkers set to yes if you want that each thread use different markers (markers are allocated if necessary)
 * @param good a selector
 */
template <typename PFP, unsigned int ORBIT>
void foreach_orbit(typename PFP::MAP& map, FunctorMapThreaded<typename PFP::MAP>& func, unsigned int nbth, unsigned int szbuff = 8192, bool needMarkers = false, const FunctorSelect& good = allDarts);

/**
 * Traverse cells of a map in parallel. Use embedding marker
 * Functor application must be independant
 * @param map the map
 * @param orbit the cell (VERTEX/EDGE/FACE/..
 * @param func the functor to apply
 * @param nbth number of threads
 * @param szbuff size of buffers to store darts in each thread (default is 8192, use less for lower memory consumsion)
 * @param needMarkers set to yes if you want that each thread use different markers (markers are allocated if necessary)
 * @param good a selector
 */
template <typename PFP, unsigned int CELL>
void foreach_cell(typename PFP::MAP& map, FunctorMapThreaded<typename PFP::MAP>& func, unsigned int nbth, unsigned int szbuff = 8192, bool needMarkers = false, const FunctorSelect& good = allDarts);

/**
 * Traverse darts of a map in parallel
 * Functor application must be independant
 * @param map the map
 * @param func the functor to apply
 * @param nbth number of thread to use
 * @param szbuff size of buffers to store darts in each thread (default is 8192, use less for lower memory consumsion)
 * @param needMarkers set to yes if you want that each thread use different markers (markers are allocated if necessary)
 * @param good a selector
 */
template <typename PFP>
void foreach_dart(typename PFP::MAP& map, FunctorMapThreaded<typename PFP::MAP>& func,  unsigned int nbth, unsigned int szbuff = 8192, bool needMarkers = false, const FunctorSelect& good = allDarts);

/**
 * Traverse orbits of a map in parallel. Use topo marker
 * Use this version if you want to store a result for traversal (sum, average, max, etc..)
 * @param map the map
 * @param orbit the cell (VERTEX/EDGE/FACE/..
 * @param func the functor to apply
 * @param nbth number of threads
 * @param szbuff size of buffers to store darts in each thread (default is 8192, use less for lower memory consumsion)
 * @param results output of the FunctorMapThreadedResult of each thread
 * @param needMarkers set to yes if you want that each thread use different markers (markers are allocated if necessary)
 * @param good a selector
 */
template <typename PFP, unsigned int ORBIT, typename T>
void foreach_orbit_res(typename PFP::MAP& map, FunctorMapThreadedResult<typename PFP::MAP, T>& func, unsigned int nbth, unsigned int szbuff, std::vector<T>& results, bool needMarkers = false, const FunctorSelect& good = allDarts);

/**
 * Traverse cells of a map in parallel. Use embedding marker
 * Use this version if you want to store a result for traversal (sum, average, max, etc..)
 * @param map the map
 * @param orbit the cell (VERTEX/EDGE/FACE/..
 * @param func the functor to apply
 * @param nbth number of threads
 * @param szbuff size of buffers to store darts in each thread (default is 8192, use less for lower memory consumsion)
 * @param results output of the FunctorMapThreadedResult of each thread
 * @param needMarkers set to yes if you want that each thread use different markers (markers are allocated if necessary)
 * @param good a selector
 */
template <typename PFP, unsigned int CELL, typename T>
void foreach_cell_res(typename PFP::MAP& map, FunctorMapThreadedResult<typename PFP::MAP, T>& func, unsigned int nbth, unsigned int szbuff, std::vector<T>& results, bool needMarkers = false, const FunctorSelect& good = allDarts);

/**
 * Traverse cells of a map in parallel. Use embedding marker
 * Use this version if you want to store a result for traversal (sum, average, max, etc..)
 * @param map the map
 * @param orbit the cell (VERTEX/EDGE/FACE/..
 * @param func the functor to apply
 * @param nbth number of threads
 * @param szbuff size of buffers to store darts in each thread (default is 8192, use less for lower memory consumsion)
 * @param results output of the FunctorMapThreadedResult of each thread
 * @param needMarkers set to yes if you want that each thread use different markers (markers are allocated if necessary)
 * @param good a selector
 */
template <typename PFP, typename T>
void foreach_dart_res(typename PFP::MAP& map, FunctorMapThreadedResult<typename PFP::MAP, T>& func, unsigned int nbth, unsigned int szbuff, std::vector<T>& results, bool needMarkers = false, const FunctorSelect& good = allDarts);

/**
 * easy sum of returned result of foreach_xxx_res
 */
template <typename T>
T sumResult(const std::vector<T>& res);

/**
 *  easy sum of returned result of foreach_xxx_res
 */
template <typename T1, typename T2>
std::pair<T1,T2> sumPairResult(const std::vector< std::pair<T1,T2> >& res);

/**
 *  easy max of returned result of foreach_xxx_res
 */
template <typename T>
T maxResult(const std::vector<T>& res);

/**
 *  easy min of returned result of foreach_xxx_res
 */
template <typename T>
T minResult(const std::vector<T>& res);

/**
 * Class to encapsulate algorithm in a boost thread
 * Usage:
 * - Define a class MyCGoGNThread that inherit from CGoGNThread
 * - call with boost::thread cgt1(MyCGoGNThread(map,1, ...);
 * - wait to finish: cgt1.join();
 */
template<typename MAP>
class CGoGNThread
{
protected:
	MAP& m_map;
	unsigned int m_threadId;

public:
	CGoGNThread(MAP& map, unsigned int th):
		m_map(map), m_threadId(th) {}

	/**
	 * to implement with algo to execute (use m_threadId
	 */
	virtual void operator()() =0;
};

} // namespace Parallel

} // namespace Algo

} // namespace CGoGN

#include "Algo/Parallel/parallel_foreach.hpp"

#endif
