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
#ifndef __PARALLEL_THREAD__
#define __PARALLEL_THREAD__

namespace CGoGN
{

namespace Algo
{

namespace Parallel
{

/**
 * Class to encapsulate algorithm in a boost thread
 * Usage:
 * - Define a class MyCGoGNThread that inherit from CGoGNThread
 * - call with boost::thread cgt1(MyCGoGNThread(map,1, ...);
 * - wait to finish: cgt1.join();
 *
 * TODO: write a CGoGNThread version of "all" algorithm
 */
template<typename MAP>
class CGoGNThread
{
protected:
	MAP& m_map;
	unsigned int m_threadId;

	unsigned int tid()
	{
		return m_threadId;
	}

public:
	CGoGNThread(MAP& map, unsigned int th):
		m_map(map), m_threadId(th) {}

	virtual ~CGoGNThread() {}

	/**
	 * to implement with algo to execute (use m_threadId)
	 */
	virtual void operator()()=0;


};


} // namespace Parallel

} // namespace Algo

} // namespace CGoGN

#endif
