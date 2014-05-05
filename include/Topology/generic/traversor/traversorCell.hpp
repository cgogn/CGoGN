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

template <typename MAP, unsigned int ORBIT>
TraversorCell<MAP, ORBIT>::TraversorCell(const MAP& map, bool forceDartMarker, unsigned int thread) :
	m(map), dmark(NULL), cmark(NULL), quickTraversal(NULL), current(NIL), firstTraversal(true)
{
	dimension = map.dimension();

	if(forceDartMarker)
		dmark = new DartMarker<MAP>(map, thread) ;
	else
	{
		quickTraversal = map.template getQuickTraversal<ORBIT>() ;
		if(quickTraversal != NULL)
		{
			cont = &(map.template getAttributeContainer<ORBIT>()) ;

		}
		else
		{
			if(map.template isOrbitEmbedded<ORBIT>())
				cmark = new CellMarker<MAP, ORBIT>(map, thread) ;
			else
				dmark = new DartMarker<MAP>(map, thread) ;
		}
	}
}

template <typename MAP, unsigned int ORBIT>
TraversorCell<MAP, ORBIT>::~TraversorCell()
{
	if(dmark)
		delete dmark ;
	else if(cmark)
		delete cmark ;
}

template <typename MAP, unsigned int ORBIT>
Cell<ORBIT> TraversorCell<MAP, ORBIT>::begin()
{
	if(quickTraversal != NULL)
	{
		qCurrent = cont->begin() ;
		current.dart = (*quickTraversal)[qCurrent] ;
	}
	else
	{
		if(!firstTraversal)
		{
			if(dmark)
				dmark->unmarkAll() ;
			else
				cmark->unmarkAll() ;
		}

		current.dart = m.begin() ;
		while(current.dart != m.end() && (m.isBoundaryMarked(dimension, current.dart) ))
			m.next(current.dart) ;

		if(current.dart == m.end())
			current.dart = NIL ;
		else
		{
			if(dmark)
				dmark->template markOrbit<ORBIT>(current.dart) ;
			else
				cmark->mark(current) ;
		}

		firstTraversal = false ;
	}

	return current ;
}

template <typename MAP, unsigned int ORBIT>
Cell<ORBIT> TraversorCell<MAP, ORBIT>::end()
{
	return Cell<ORBIT>(NIL) ;
}

template <typename MAP, unsigned int ORBIT>
Cell<ORBIT> TraversorCell<MAP, ORBIT>::next()
{
	assert(current.dart != NIL);
//	if(current.dart != NIL)
//	{
	if(quickTraversal != NULL)
	{
		cont->next(qCurrent) ;
		if (qCurrent != cont->end())
			current.dart = (*quickTraversal)[qCurrent] ;
		else current.dart = NIL;
	}
	else
	{
		if(dmark)
		{
			bool ismarked = dmark->isMarked(current.dart) ;
			while(current.dart != NIL && (ismarked || m.isBoundaryMarked(dimension, current.dart)))
			{
				m.next(current.dart) ;
				if(current.dart == m.end())
					current.dart = NIL ;
				else
					ismarked = dmark->isMarked(current.dart) ;
			}
			if(current.dart != NIL)
				dmark->template markOrbit<ORBIT>(current.dart) ;
		}
		else
		{
			bool ismarked = cmark->isMarked(current) ;
			while(current.dart != NIL && (ismarked || m.isBoundaryMarked(dimension, current.dart) ))
			{
				m.next(current.dart) ;
				if(current.dart == m.end())
					current.dart = NIL ;
				else
					ismarked = cmark->isMarked(current) ;
			}
			if(current.dart != NIL)
				cmark->mark(current) ;
		}
	}
//	}
	return current ;
}

template <typename MAP, unsigned int ORBIT>
void TraversorCell<MAP, ORBIT>::skip(Cell<ORBIT> c)
{
	if(dmark)
		dmark->template markOrbit<ORBIT>(c.dart) ;
	else
		cmark->mark(c) ;
}



namespace Parallel
{

/// internal functor for boost call
template <unsigned int ORBIT, typename FUNC>
class ThreadFunction
{
protected:
	typedef Cell<ORBIT> CELL;
	std::vector<CELL>& m_cells;
	boost::barrier& m_sync1;
	boost::barrier& m_sync2;
	bool& m_finished;
	unsigned int m_id;
	FUNC m_lambda;
public:
	ThreadFunction(FUNC func, std::vector<CELL>& vd, boost::barrier& s1, boost::barrier& s2, bool& finished, unsigned int id):
		m_cells(vd), m_sync1(s1), m_sync2(s2), m_finished(finished), m_id(id), m_lambda(func)
	{
	}

	ThreadFunction(const ThreadFunction<ORBIT, FUNC>& tf):
		m_cells(tf.m_cells), m_sync1(tf.m_sync1), m_sync2(tf.m_sync2), m_finished(tf.m_finished), m_id(tf.m_id), m_lambda(tf.m_lambda){}

	void operator()()
	{
		while (!m_finished)
		{
			for (typename std::vector<CELL>::const_iterator it = m_cells.begin(); it != m_cells.end(); ++it)
				m_lambda(*it,m_id);
			m_sync1.wait(); // wait every body has finished
			m_sync2.wait(); // wait vectors has been refilled
		}
	}
};


template <unsigned int ORBIT, typename MAP, typename FUNC>
void foreach_cell(MAP& map, FUNC func, unsigned int nbth, bool needMarkers)
{
	if (nbth==0)
		nbth = boost::thread::hardware_concurrency();

	std::vector< Cell<ORBIT> >* vd = new std::vector< Cell<ORBIT> >[nbth];

	for (unsigned int i = 0; i < nbth; ++i)
		vd[i].reserve(SIZE_BUFFER_THREAD);

	//    TraversorCell<MAP, ORBIT> trav(map,false,th_orig);
	unsigned int nb = 0;
	TraversorCell<MAP, ORBIT> trav(map);
	Cell<ORBIT> cell = trav.begin();
	Cell<ORBIT> c_end = trav.end();
	while ((cell.dart != c_end.dart) && (nb < nbth*SIZE_BUFFER_THREAD) )
	{
		vd[nb%nbth].push_back(cell);
		nb++;
		cell = trav.next();
	}
	boost::barrier sync1(nbth+1);
	boost::barrier sync2(nbth+1);
	bool finished=false;
	// lauch threads
	if (needMarkers)
	{
		unsigned int nbth_prec = map.getNbThreadMarkers();
		if (nbth_prec < nbth+1)
			map.addThreadMarker(nbth+1-nbth_prec);
	}

	boost::thread** threads = new boost::thread*[nbth];
	ThreadFunction<ORBIT,FUNC>** tfs = new ThreadFunction<ORBIT,FUNC>*[nbth];

	for (unsigned int i = 0; i < nbth; ++i)
	{
		tfs[i] = new ThreadFunction<ORBIT,FUNC>(func, vd[i],sync1,sync2, finished,1+i);
		threads[i] = new boost::thread( boost::ref( *(tfs[i]) ) );
	}

	// and continue to traverse the map
	std::vector< Cell<ORBIT> >* tempo = new std::vector< Cell<ORBIT> >[nbth];
	for (unsigned int i = 0; i < nbth; ++i)
		tempo[i].reserve(SIZE_BUFFER_THREAD);

	while (cell.dart != c_end.dart)
	{
		for (unsigned int i = 0; i < nbth; ++i)
			tempo[i].clear();
		unsigned int nb = 0;

		while ((cell.dart != c_end.dart) && (nb < nbth*SIZE_BUFFER_THREAD) )
		{
			tempo[nb%nbth].push_back(cell);
			nb++;
			cell = trav.next();
		}
		sync1.wait();// wait for all thread to finish its vector
		for (unsigned int i = 0; i < nbth; ++i)
			vd[i].swap(tempo[i]);
		sync2.wait();// everybody refilled then go
	}

	sync1.wait();// wait for all thread to finish its vector
	finished = true;// say finsih to everyone
	sync2.wait(); // just wait for last barrier wait !


	//wait for all theads to be finished
	for (unsigned int i = 0; i < nbth; ++i)
	{
		threads[i]->join();
		delete threads[i];
		delete tfs[i];
	}
	delete[] tfs;
	delete[] threads;
	delete[] vd;
	delete[] tempo;
}

}


} // namespace CGoGN
