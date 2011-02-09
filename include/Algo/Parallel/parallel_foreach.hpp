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
#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>


namespace CGoGN
{

namespace Algo
{

namespace Parallel
{




class ThreadFunctor
{
protected:
	FunctorType& m_functor;
	const std::vector<Dart>& m_darts;
	boost::barrier& m_sync1;
	boost::barrier& m_sync2;
	bool& m_finished;
public:
	ThreadFunctor(FunctorType& func, const std::vector<Dart>& vd, boost::barrier& s1, boost::barrier& s2, bool& finished):
		m_functor(func), m_darts(vd), m_sync1(s1), m_sync2(s2), m_finished(finished) {}

	void operator()()
	{
		while (!m_finished)
		{
			for (std::vector<Dart>::const_iterator it = m_darts.begin(); it != m_darts.end(); ++it)
				m_functor(*it);
			m_sync1.wait();
			m_sync2.wait();
		}
	}
};

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
void foreach_orbit(typename PFP::MAP& map,  unsigned int orbit, FunctorType& func,  unsigned int nbth, unsigned int szbuff, const FunctorSelect& good)
{
	std::vector<Dart> vd[nbth];
	boost::thread* threads[nbth];

	DartMarker dm(map);
	Dart d=map.begin();

	// nbth new functions, new thread (with good darts !)
	for (unsigned int i=0; i<nbth; ++i)
		vd[i].reserve(szbuff);

	// fill each vd buffers with 4096 darts
	unsigned int nb =0;
	while (( d != map.end()) && (nb < nbth*szbuff) )
	{
		if (good(d) && (!dm.isMarked(d)))
		{
			dm.markOrbit(orbit,d);
			vd[nb%nbth].push_back(d);
			nb++;
		}
		map.next(d);
	}

	boost::barrier sync1(nbth+1);
	boost::barrier sync2(nbth+1);
	bool finished=false;
	// lauch threads
	for (unsigned int i=0; i<nbth; ++i)
		threads[i] = new boost::thread(ThreadFunctor(func, vd[i],sync1,sync2, finished));

	// and continue to traverse the map
	std::vector<Dart> tempo[nbth];
	for (unsigned int i=0; i<nbth; ++i)
		tempo[i].reserve(szbuff);

	while ( d != map.end())
	{
		for (unsigned int i=0; i<nbth; ++i)
			tempo[i].clear();

		unsigned int nb =0;
		while (( d != map.end()) && (nb < nbth*szbuff) )
		{
			if (good(d) && (!dm.isMarked(d)))
			{
				dm.markOrbit(orbit,d);
				tempo[nb%nbth].push_back(d);
				nb++;
			}
			map.next(d);
		}

		sync1.wait();
		for (unsigned int i=0; i<nbth; ++i)
			vd[i].swap(tempo[i]);
		sync2.wait();
	}

	sync1.wait();
	finished = true;
	sync2.wait();

	//wait for all theads to be finished
	for (unsigned int i=0; i< nbth; ++i)
	{
		threads[i]->join();
		delete threads[i];
	}
}

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
void foreach_cell(typename PFP::MAP& map, unsigned int cell, FunctorType& func,  unsigned int nbth, unsigned int szbuff, const FunctorSelect& good)
{
	std::vector<Dart> vd[nbth];
	boost::thread* threads[nbth];

	CellMarker cm(map,cell);
	Dart d=map.begin();

	// nbth new functions, new thread (with good darts !)
	for (unsigned int i=0; i<nbth; ++i)
		vd[i].reserve(szbuff);

	// fill each vd buffers with 4096 darts
	unsigned int nb =0;
	while (( d != map.end()) && (nb < nbth*szbuff) )
	{
		if (good(d) && (!cm.isMarked(d)))
		{
			cm.mark(d);
			vd[nb%nbth].push_back(d);
			nb++;
		}
		map.next(d);
	}

	boost::barrier sync1(nbth+1);
	boost::barrier sync2(nbth+1);
	bool finished=false;
	// lauch threads
	for (unsigned int i=0; i<nbth; ++i)
		threads[i] = new boost::thread(ThreadFunctor(func, vd[i],sync1,sync2, finished));

	// and continue to traverse the map
	std::vector<Dart> tempo[nbth];
	for (unsigned int i=0; i<nbth; ++i)
		tempo[i].reserve(szbuff);

	while ( d != map.end())
	{
		for (unsigned int i=0; i<nbth; ++i)
			tempo[i].clear();

		unsigned int nb =0;
		while (( d != map.end()) && (nb < nbth*szbuff) )
		{
			if (good(d) && (!cm.isMarked(d)))
			{
				cm.mark(d);
				tempo[nb%nbth].push_back(d);
				nb++;
			}
			map.next(d);
		}

		sync1.wait();
		for (unsigned int i=0; i<nbth; ++i)
			vd[i].swap(tempo[i]);
		sync2.wait();
	}

	sync1.wait();
	finished = true;
	sync2.wait();

	//wait for all theads to be finished
	for (unsigned int i=0; i< nbth; ++i)
	{
		threads[i]->join();
		delete threads[i];
	}
}



template <typename PFP>
void foreach_dart(typename PFP::MAP& map, FunctorType& func,  unsigned int nbth, unsigned int szbuff, const FunctorSelect& good)
{
	std::vector<Dart> vd[nbth];
	boost::thread* threads[nbth];

	Dart d=map.begin();

	// nbth new functions, new thread (with good darts !)
	for (unsigned int i=0; i<nbth; ++i)
		vd[i].reserve(szbuff);

	// fill each vd buffers with 4096 darts
	unsigned int nb =0;
	while (( d != map.end()) && (nb < nbth*szbuff) )
	{
		if (good(d))
		{
			vd[nb%nbth].push_back(d);
			nb++;
		}
		map.next(d);
	}

	boost::barrier sync1(nbth+1);
	boost::barrier sync2(nbth+1);
	bool finished=false;
	// lauch threads
	for (unsigned int i=0; i<nbth; ++i)
		threads[i] = new boost::thread(ThreadFunctor(func, vd[i],sync1,sync2, finished));

	// and continue to traverse the map
	std::vector<Dart> tempo[nbth];
	for (unsigned int i=0; i<nbth; ++i)
		tempo[i].reserve(szbuff);

	while ( d != map.end())
	{
		for (unsigned int i=0; i<nbth; ++i)
			tempo[i].clear();

		unsigned int nb =0;
		while (( d != map.end()) && (nb < nbth*szbuff) )
		{
			if (good(d))
			{
				tempo[nb%nbth].push_back(d);
				nb++;
			}
			map.next(d);
		}

		sync1.wait();
		for (unsigned int i=0; i<nbth; ++i)
			vd[i].swap(tempo[i]);
		sync2.wait();
	}

	sync1.wait();
	finished = true;
	sync2.wait();

	//wait for all theads to be finished
	for (unsigned int i=0; i< nbth; ++i)
	{
		threads[i]->join();
		delete threads[i];
	}
}


}
}	// end namespace
}



