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
#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>
#include <vector>

namespace CGoGN
{

namespace Algo
{

namespace Parallel
{


/**
 *
 */
template<typename MAP>
class ThreadFunction
{
protected:
	std::vector<Dart>& m_darts;
	boost::barrier& m_sync1;
	boost::barrier& m_sync2;
	bool& m_finished;
	FunctorMapThreaded<MAP>* m_functor;
public:
	ThreadFunction(FunctorMapThreaded<MAP>& func, std::vector<Dart>& vd, boost::barrier& s1, boost::barrier& s2, bool& finished, unsigned int id):
		m_darts(vd), m_sync1(s1), m_sync2(s2), m_finished(finished)
	{
		m_functor = func.duplicate(id);
	}

	ThreadFunction(const ThreadFunction<MAP>& tf):
		m_darts(tf.m_darts), m_sync1(tf.m_sync1), m_sync2(tf.m_sync2), m_finished(tf.m_finished), m_functor(tf.m_functor){}

	void operator()()
	{
		while (!m_finished)
		{
			for (std::vector<Dart>::const_iterator it = m_darts.begin(); it != m_darts.end(); ++it)
				m_functor->operator()(*it);
			m_sync1.wait();
			m_sync2.wait();
		}
	}

	void clean()
	{
		delete m_functor;
	}

};

template<typename MAP, typename T>
class ThreadFunctionResult
{
protected:
	std::vector<Dart>& m_darts;
	boost::barrier& m_sync1;
	boost::barrier& m_sync2;
	bool& m_finished;
	FunctorMapThreadedResult<MAP,T>* m_functor;
	T& m_result;
public:
	ThreadFunctionResult(FunctorMapThreadedResult<MAP,T>& func, std::vector<Dart>& vd, boost::barrier& s1, boost::barrier& s2, bool& finished, unsigned int id, T& result):
		m_darts(vd), m_sync1(s1), m_sync2(s2), m_finished(finished), m_result(result)
	{
		m_functor = reinterpret_cast< FunctorMapThreadedResult<MAP,T>* >(func.duplicate(id));
	}

	ThreadFunctionResult(const ThreadFunctionResult<MAP,T>& tf):
		m_darts(tf.m_darts),
		m_sync1(tf.m_sync1),
		m_sync2(tf.m_sync2),
		m_finished(tf.m_finished),
		m_functor(tf.m_functor),
		m_result(tf.m_result){}

	void operator()()
	{

		while (!m_finished)
		{
			for (std::vector<Dart>::const_iterator it = m_darts.begin(); it != m_darts.end(); ++it)
				m_functor->operator()(*it);
			m_sync1.wait();
			m_sync2.wait();
		}
		m_result = this->m_functor->getResult();
	}

	void clean()
	{
		delete m_functor;
	}
};



template <typename PFP, unsigned int ORBIT>
void foreach_orbit(typename PFP::MAP& map, FunctorMapThreaded<typename PFP::MAP>& func, unsigned int nbth, unsigned int szbuff, bool needMarkers, const FunctorSelect& good)
{
	std::vector<Dart>* vd = new std::vector<Dart>[nbth];
	boost::thread** threads = new boost::thread*[nbth];

	DartMarker dm(map);
	Dart d = map.begin();

	// nbth new functions, new thread (with good darts !)
	for (unsigned int i=0; i<nbth; ++i)
		vd[i].reserve(szbuff);

	// fill each vd buffers with 4096 darts
	unsigned int nb = 0;
	while ((d != map.end()) && (nb < nbth*szbuff) )
	{
		if (good(d) && (!dm.isMarked(d)))
		{
			dm.markOrbit<ORBIT>(d);
			vd[nb%nbth].push_back(d);
			nb++;
		}
		map.next(d);
	}

	if (needMarkers)
	{
		// ensure that there is enough threads
		unsigned int nbth_prec = map.getNbThreadMarkers();
		if (nbth_prec < nbth+1)
			map.addThreadMarker(nbth+1-nbth_prec);
	}

	boost::barrier sync1(nbth+1);
	boost::barrier sync2(nbth+1);
	bool finished = false;
	// lauch threads
	if (needMarkers)
	{
		for (unsigned int i = 0; i < nbth; ++i)
			threads[i] = new boost::thread(ThreadFunction<typename PFP::MAP>(func, vd[i],sync1,sync2, finished,1+i));
	}
	else
	{
		for (unsigned int i = 0; i < nbth; ++i)
			threads[i] = new boost::thread(ThreadFunction<typename PFP::MAP>(func, vd[i],sync1,sync2, finished,0));
	}

	// and continue to traverse the map
	std::vector<Dart>* tempo = new std::vector<Dart>[nbth];

	for (unsigned int i = 0; i < nbth; ++i)
		tempo[i].reserve(szbuff);

	while (d != map.end())
	{
		for (unsigned int i = 0; i < nbth; ++i)
			tempo[i].clear();

		unsigned int nb = 0;
		while ((d != map.end()) && (nb < nbth*szbuff) )
		{
			if (good(d) && (!dm.isMarked(d)))
			{
				dm.markOrbit<ORBIT>(d);
				tempo[nb%nbth].push_back(d);
				nb++;
			}
			map.next(d);
		}

		// sync and swap the two vectors
		sync1.wait();
		for (unsigned int i = 0; i < nbth; ++i)
			vd[i].swap(tempo[i]);
		sync2.wait();
	}

	sync1.wait();
	finished = true;
	sync2.wait();

	//wait for all theads to be finished
	for (unsigned int i = 0; i < nbth; ++i)
	{
		threads[i]->join();
		delete threads[i];
	}

	// and release memory
	delete[] threads;
	delete[] vd;
	delete[] tempo;
}

/**
 * Traverse cells of a map in parallel. Use embedding marker
 * Functor application must be independant
 * @param map the map
 * @param func the functor to apply
 * @param nbth number of thread to use (use twice as threads of processor)
 * @param szbuff size of buffers to store darts in each thread (default is 8192, use less for lower memory consumsion)
 * @param good a selector
 */
template <typename PFP, unsigned int CELL>
void foreach_cell(typename PFP::MAP& map, FunctorMapThreaded<typename PFP::MAP>& func, unsigned int nbth, unsigned int szbuff, bool needMarkers, const FunctorSelect& good)
{
	std::vector<Dart>* vd = new std::vector<Dart>[nbth];
	boost::thread** threads = new boost::thread*[nbth];

	CellMarker<CELL> cm(map);
	Dart d = map.begin();

	// nbth new functions, new thread (with good darts !)
	for (unsigned int i = 0; i < nbth; ++i)
		vd[i].reserve(szbuff);

	// fill each vd buffers with 4096 darts
	unsigned int nb = 0;
	while ((d != map.end()) && (nb < nbth*szbuff) )
	{
		if (good(d) && (!cm.isMarked(d)))
		{
			cm.mark(d);
			vd[nb%nbth].push_back(d);
			nb++;
		}
		map.next(d);
	}

	if (needMarkers)
	{
		// ensure that there is enough threads
		unsigned int nbth_prec = map.getNbThreadMarkers();
		if (nbth_prec < nbth+1)
			map.addThreadMarker(nbth+1-nbth_prec);
	}

	boost::barrier sync1(nbth+1);
	boost::barrier sync2(nbth+1);
	bool finished=false;
	// lauch threads
	if (needMarkers)
	{
		for (unsigned int i = 0; i < nbth; ++i)
			threads[i] = new boost::thread(ThreadFunction<typename PFP::MAP>(func, vd[i],sync1,sync2, finished,1+i));
	}
	else
	{
		for (unsigned int i = 0; i < nbth; ++i)
			threads[i] = new boost::thread(ThreadFunction<typename PFP::MAP>(func, vd[i],sync1,sync2, finished,0));
	}
	// and continue to traverse the map
	std::vector<Dart>* tempo = new std::vector<Dart>[nbth];

	for (unsigned int i = 0; i < nbth; ++i)
		tempo[i].reserve(szbuff);

	while (d != map.end())
	{
		for (unsigned int i = 0; i < nbth; ++i)
			tempo[i].clear();

		unsigned int nb = 0;
		while ((d != map.end()) && (nb < nbth*szbuff) )
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
		for (unsigned int i = 0; i < nbth; ++i)
			vd[i].swap(tempo[i]);
		sync2.wait();
	}

	sync1.wait();
	finished = true;
	sync2.wait();

	//wait for all theads to be finished
	for (unsigned int i = 0; i < nbth; ++i)
	{
		threads[i]->join();
		delete threads[i];
	}
	delete[] threads;
	delete[] vd;
	delete[] tempo;
}

template <typename PFP>
void foreach_dart(typename PFP::MAP& map, FunctorMapThreaded<typename PFP::MAP>& func, unsigned int nbth, unsigned int szbuff, bool needMarkers, const FunctorSelect& good)
{
	std::vector<Dart>* vd = new std::vector<Dart>[nbth];
	boost::thread** threads = new boost::thread*[nbth];

	Dart d = map.begin();

	// nbth new functions, new thread (with good darts !)
	for (unsigned int i = 0; i < nbth; ++i)
		vd[i].reserve(szbuff);

	// fill each vd buffers with 4096 darts
	unsigned int nb = 0;
	while ((d != map.end()) && (nb < nbth*szbuff) )
	{
		if (good(d))
		{
			vd[nb%nbth].push_back(d);
			nb++;
		}
		map.next(d);
	}

	if (needMarkers)
	{
		// ensure that there is enough threads
		unsigned int nbth_prec = map.getNbThreadMarkers();
		if (nbth_prec < nbth+1)
			map.addThreadMarker(nbth+1-nbth_prec);
	}

	boost::barrier sync1(nbth+1);
	boost::barrier sync2(nbth+1);
	bool finished = false;
	// lauch threads
	if (needMarkers)
	{
		for (unsigned int i = 0; i < nbth; ++i)
			threads[i] = new boost::thread(ThreadFunction<typename PFP::MAP>(func, vd[i],sync1,sync2, finished,1+i));
	}
	else
	{
		for (unsigned int i = 0; i < nbth; ++i)
			threads[i] = new boost::thread(ThreadFunction<typename PFP::MAP>(func, vd[i],sync1,sync2, finished,0));
	}
	// and continue to traverse the map
	std::vector<Dart>* tempo = new std::vector<Dart>[nbth];
	for (unsigned int i = 0; i < nbth; ++i)
		tempo[i].reserve(szbuff);

	while (d != map.end())
	{
		for (unsigned int i = 0; i < nbth; ++i)
			tempo[i].clear();

		unsigned int nb =0;
		while ((d != map.end()) && (nb < nbth*szbuff) )
		{
			if (good(d))
			{
				tempo[nb%nbth].push_back(d);
				nb++;
			}
			map.next(d);
		}

		sync1.wait();
		for (unsigned int i = 0; i < nbth; ++i)
			vd[i].swap(tempo[i]);
		sync2.wait();
	}

	sync1.wait();
	finished = true;
	sync2.wait();

	//wait for all theads to be finished
	for (unsigned int i = 0; i < nbth; ++i)
	{
		threads[i]->join();
		delete threads[i];
	}
	
	delete vd;
	delete threads;
	delete tempo;
}

template <typename PFP, unsigned int ORBIT, typename T>
void foreach_orbit_res(typename PFP::MAP& map, FunctorMapThreadedResult<typename PFP::MAP, T>& func, unsigned int nbth, unsigned int szbuff, std::vector<T>& results, bool needMarkers, const FunctorSelect& good)
{
	std::vector<Dart>* vd = new std::vector<Dart>[nbth];
	boost::thread** threads = new boost::thread*[nbth];
	ThreadFunctionResult<typename PFP::MAP,T>** th_funcs= new ThreadFunctionResult<typename PFP::MAP,T>*[nbth];

	DartMarker dm(map);
	Dart d = map.begin();

	// nbth new functions, new thread (with good darts !)
	for (unsigned int i = 0; i < nbth; ++i)
		vd[i].reserve(szbuff);

	// fill each vd buffers with 4096 darts
	unsigned int nb = 0;
	while ((d != map.end()) && (nb < nbth*szbuff) )
	{
		if (good(d) && (!dm.isMarked(d)))
		{
			dm.markOrbit<ORBIT>(d);
			vd[nb%nbth].push_back(d);
			nb++;
		}
		map.next(d);
	}

	if (needMarkers)
	{
		// ensure that there is enough threads
		unsigned int nbth_prec = map.getNbThreadMarkers();
		if (nbth_prec < nbth+1)
			map.addThreadMarker(nbth+1-nbth_prec);
	}

	// prepare some space pour results
	results.resize(nbth);

	boost::barrier sync1(nbth+1);
	boost::barrier sync2(nbth+1);
	bool finished=false;
	// lauch threads
	if (needMarkers)
	{
		for (unsigned int i = 0; i < nbth; ++i)
		{
			// here dynamic allocation to allow the freeing of m_functor (clean) at the end
			th_funcs[i] = new ThreadFunctionResult<typename PFP::MAP,T>(func, vd[i],sync1,sync2, finished,1+i,results[i]);
			threads[i] = new boost::thread(*(th_funcs[i]));
		}
	}
	else
	{
		for (unsigned int i = 0; i < nbth; ++i)
		{
			th_funcs[i] = new ThreadFunctionResult<typename PFP::MAP,T>(func, vd[i],sync1,sync2, finished,0,results[i]);
			threads[i] = new boost::thread(*(th_funcs[i]));
		}
	}

	// and continue to traverse the map
	std::vector<Dart>* tempo = new std::vector<Dart>[nbth];

	for (unsigned int i = 0; i < nbth; ++i)
		tempo[i].reserve(szbuff);

	while (d != map.end())
	{
		for (unsigned int i = 0; i < nbth; ++i)
			tempo[i].clear();

		unsigned int nb = 0;
		while ((d != map.end()) && (nb < nbth*szbuff) )
		{
			if (good(d) && (!dm.isMarked(d)))
			{
				dm.markOrbit<ORBIT>(d);
				tempo[nb%nbth].push_back(d);
				nb++;
			}
			map.next(d);
		}

		// sync and swap the two vectors
		sync1.wait();
		for (unsigned int i=0; i<nbth; ++i)
			vd[i].swap(tempo[i]);
		sync2.wait();
	}

	sync1.wait();
	finished = true;
	sync2.wait();

	std::vector<T> res;
	//wait for all theads to be finished and get results
	for (unsigned int i = 0; i < nbth; ++i)
	{
		threads[i]->join();
		delete threads[i];
		th_funcs[i]->clean();
	}

	//release dynamic allocation
	delete[] threads;
	delete[] vd;
	delete[] th_funcs;
	delete[] tempo;
}

template <typename PFP, unsigned int CELL, typename T>
void foreach_cell_res(typename PFP::MAP& map, FunctorMapThreadedResult<typename PFP::MAP, T>& func,  unsigned int nbth, unsigned int szbuff, std::vector<T>& results, bool needMarkers, const FunctorSelect& good)
{
	std::vector<Dart>* vd = new std::vector<Dart>[nbth];
	boost::thread** threads = new boost::thread*[nbth];
	ThreadFunctionResult<typename PFP::MAP,T>**  th_funcs= new ThreadFunctionResult<typename PFP::MAP,T>*[nbth];

	CellMarker<CELL> cm(map);
	Dart d = map.begin();

	// nbth new functions, new thread (with good darts !)
	for (unsigned int i = 0; i < nbth; ++i)
		vd[i].reserve(szbuff);

	// fill each vd buffers with 4096 darts
	unsigned int nb = 0;
	while ((d != map.end()) && (nb < nbth*szbuff) )
	{
		if (good(d) && (!cm.isMarked(d)))
		{
			cm.mark(d);
			vd[nb%nbth].push_back(d);
			nb++;
		}
		map.next(d);
	}

	if (needMarkers)
	{
		// ensure that there is enough threads
		unsigned int nbth_prec = map.getNbThreadMarkers();
		if (nbth_prec < nbth+1)
			map.addThreadMarker(nbth+1-nbth_prec);
	}

	// prepare some space pour results
	results.resize(nbth);

	boost::barrier sync1(nbth+1);
	boost::barrier sync2(nbth+1);
	bool finished=false;
	// lauch threads
	if (needMarkers)
	{
		for (unsigned int i = 0; i < nbth; ++i)
		{
			th_funcs[i] = new ThreadFunctionResult<typename PFP::MAP,T>(func, vd[i],sync1,sync2, finished,1+i,results[i]);
			threads[i] = new boost::thread(*(th_funcs[i]));
		}
	}
	else
	{
		for (unsigned int i = 0; i < nbth; ++i)
		{
			th_funcs[i] = new ThreadFunctionResult<typename PFP::MAP,T>(func, vd[i],sync1,sync2, finished,0,results[i]);
			threads[i] = new boost::thread(*(th_funcs[i]));
		}
	}

	// and continue to traverse the map
	std::vector<Dart>* tempo = new std::vector<Dart>[nbth];

	for (unsigned int i = 0; i < nbth; ++i)
		tempo[i].reserve(szbuff);

	while ( d != map.end())
	{
		for (unsigned int i = 0; i < nbth; ++i)
			tempo[i].clear();

		unsigned int nb = 0;
		while ((d != map.end()) && (nb < nbth*szbuff) )
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

	std::vector<T> res;
	//wait for all theads to be finished and get results
	for (unsigned int i = 0; i < nbth; ++i)
	{
		threads[i]->join();
		delete threads[i];
		th_funcs[i]->clean();
	}

	delete[] threads;
	delete[] vd;
	delete[] th_funcs;
	delete[] tempo;
}

template <typename PFP, typename T>
void foreach_dart_res(typename PFP::MAP& map, FunctorMapThreadedResult<typename PFP::MAP, T>& func, unsigned int nbth, unsigned int szbuff, std::vector<T>& results, bool needMarkers, const FunctorSelect& good)
{
	std::vector<Dart>* vd = new std::vector<Dart>[nbth];
	boost::thread** threads = new boost::thread*[nbth];
	ThreadFunctionResult<typename PFP::MAP,T>** th_funcs= new ThreadFunctionResult<typename PFP::MAP,T>*[nbth];

	Dart d = map.begin();

	// nbth new functions, new thread (with good darts !)
	for (unsigned int i=0; i<nbth; ++i)
		vd[i].reserve(szbuff);

	// fill each vd buffers with szbuff darts
	unsigned int nb = 0;
	while ((d != map.end()) && (nb < nbth*szbuff) )
	{
		if (good(d))
		{
			vd[nb%nbth].push_back(d);
			nb++;
		}
		map.next(d);
	}

	if (needMarkers)
	{
		// ensure that there is enough threads
		unsigned int nbth_prec = map.getNbThreadMarkers();
		if (nbth_prec < nbth+1)
			map.addThreadMarker(nbth+1-nbth_prec);
	}

	// prepare some space pour results
	results.resize(nbth);

	boost::barrier sync1(nbth+1);
	boost::barrier sync2(nbth+1);
	bool finished = false;
	// lauch threads
	if (needMarkers)
	{
		for (unsigned int i = 0; i < nbth; ++i)
		{
			th_funcs[i] = new ThreadFunctionResult<typename PFP::MAP,T>(func, vd[i],sync1,sync2, finished,1+i,results[i]);
			threads[i] = new boost::thread(*(th_funcs[i]));
		}
	}
	else
	{
		for (unsigned int i = 0; i < nbth; ++i)
		{
			th_funcs[i] = new ThreadFunctionResult<typename PFP::MAP,T>(func, vd[i],sync1,sync2, finished,0,results[i]);
			threads[i] = new boost::thread(*(th_funcs[i]));
		}
	}

	// and continue to traverse the map
	std::vector<Dart>* tempo = new std::vector<Dart>[nbth];

	for (unsigned int i = 0; i < nbth; ++i)
		tempo[i].reserve(szbuff);

	while (d != map.end())
	{
		for (unsigned int i=0; i<nbth; ++i)
			tempo[i].clear();

		unsigned int nb = 0;
		while ((d != map.end()) && (nb < nbth*szbuff) )
		{
			if (good(d))
			{
				tempo[nb%nbth].push_back(d);
				nb++;
			}
			map.next(d);
		}

		sync1.wait();
		for (unsigned int i = 0; i < nbth; ++i)
			vd[i].swap(tempo[i]);
		sync2.wait();
	}

	sync1.wait();
	finished = true;
	sync2.wait();

	std::vector<T> res;
	//wait for all theads to be finished and get results
	for (unsigned int i = 0; i < nbth; ++i)
	{
		threads[i]->join();
		delete threads[i];
		th_funcs[i]->clean();
	}

	delete[] threads;
	delete[] vd;
	delete[] th_funcs;
	delete[] tempo;
}

template <typename T>
T sumResult(const std::vector<T>& res)
{
	T sum(res.front());
	typename std::vector<T>::const_iterator it = res.begin();
	it++;
	while (it != res.end())
	{
		sum += *it;
		++it;
	}
	return sum;
}

template <typename T1, typename T2>
std::pair<T1,T2> sumPairResult(const std::vector< std::pair<T1,T2> >& res)
{
	T1 sum1(res.front().first);
	T2 sum2(res.front().second);
	typename std::vector< std::pair<T1,T2> >::const_iterator it = res.begin();
	it++;
	while (it != res.end())
	{
		sum1 += it->first;
		sum2 += it->second;
		++it;
	}
	return std::pair<T1,T2>(sum1,sum2);
}

template <typename T>
T maxResult(const std::vector<T>& res)
{
	T maxr(res.front());
	typename std::vector<T>::const_iterator it = res.begin();
	it++;
	while (it != res.end())
	{
		if (*it > maxr)
		maxr = *it;
		++it;
	}
	return maxr;
}

template <typename T>
T minResult(const std::vector<T>& res)
{
	T minr(res.front());
	typename std::vector<T>::const_iterator it = res.begin();
	it++;
	while (it != res.end())
	{
		if (*it < minr)
		minr = *it;
		++it;
	}
	return minr;
}

} // namespace Parallel

} // namespace Algo

} // namespace CGoGN
