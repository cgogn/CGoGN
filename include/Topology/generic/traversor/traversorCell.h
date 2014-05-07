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

#ifndef __TRAVERSOR_CELL_H__
#define __TRAVERSOR_CELL_H__

#include "Topology/generic/dart.h"
#include "Topology/generic/cells.h"
#include "Topology/generic/dartmarker.h"
#include "Topology/generic/cellmarker.h"
#include "Topology/generic/traversor/traversorGen.h"

#include <functional>

namespace CGoGN
{
/**
 * Travsersor class
 * template params:
 *  - MAP type of map
 *  - ORBIT orbit of the cell
 *  - OPT type of optimization
 */

enum TraversalOptim {AUTO=0, FORCE_DART_MARKING, FORCE_CELL_MARKING, FORCE_QUICK_TRAVERSAL};

template <typename MAP, unsigned int ORBIT, TraversalOptim OPT=AUTO>
class TraversorCell
{
protected:
	const MAP& m ;
	unsigned int dimension ;

	const AttributeContainer* cont ;
	unsigned int qCurrent ;

	DartMarker<MAP>* dmark ;
	CellMarker<MAP, ORBIT>* cmark ;
	const AttributeMultiVector<Dart>* quickTraversal ;

	Cell<ORBIT> current ;
	bool firstTraversal ;

	// just for odd/even versions
	TraversorCell(const TraversorCell<MAP, ORBIT,OPT>& tc);

public:
	TraversorCell(const MAP& map, bool forceDartMarker = false, unsigned int thread = 0) ;

	~TraversorCell() ;

	inline Cell<ORBIT> begin() ;

	inline Cell<ORBIT> end() ;

	inline Cell<ORBIT> next() ;

	inline void skip(Cell<ORBIT> c);

	inline unsigned int nbCells();
} ;




template <typename MAP, unsigned int ORBIT, TraversalOptim OPT=AUTO>
class TraversorCellEven : public TraversorCell<MAP,ORBIT,OPT>
{
public:
	TraversorCellEven(const TraversorCell<MAP,ORBIT,OPT>& tra):
		TraversorCell<MAP,ORBIT,OPT>(tra) {}
	~TraversorCellEven() { this->cmark = NULL; this->dmark=NULL; }
	inline Cell<ORBIT> begin() ;
} ;


template <typename MAP, unsigned int ORBIT, TraversalOptim OPT=AUTO>
class TraversorCellOdd : public TraversorCell<MAP,ORBIT,OPT>
{
public:
	TraversorCellOdd(const TraversorCell<MAP,ORBIT,OPT>& tra):
		TraversorCell<MAP,ORBIT,OPT>(tra) {}
	~TraversorCellOdd() {this->cmark = NULL; this->dmark=NULL; }
	inline Cell<ORBIT> begin() ;
	inline Cell<ORBIT> next() ;
} ;






/*
 * Executes function f on each ORBIT
 */
//template <unsigned int ORBIT, typename MAP, typename FUNC>
//inline void foreach_cell(const MAP& map, FUNC f, bool forceDartMarker = false, unsigned int thread = 0)
//{
//	TraversorCell<MAP, ORBIT> trav(map, forceDartMarker, thread);
//	for (Cell<ORBIT> c = trav.begin(), e = trav.end(); c.dart != e.dart; c = trav.next())
//		f(c);
//}

template <unsigned int ORBIT, typename MAP, typename FUNC>
inline void foreach_cell(const MAP& map, FUNC f, TraversalOptim opt = AUTO, unsigned int thread = 0)
{
	switch(opt)
	{
	case FORCE_DART_MARKING:
	{
		TraversorCell<MAP, ORBIT,FORCE_DART_MARKING> trav(map, false, thread);
		for (Cell<ORBIT> c = trav.begin(), e = trav.end(); c.dart != e.dart; c = trav.next())
			f(c);
	}
	break;
	case FORCE_CELL_MARKING:
	{
		TraversorCell<MAP, ORBIT,FORCE_CELL_MARKING> trav(map, false, thread);
		for (Cell<ORBIT> c = trav.begin(), e = trav.end(); c.dart != e.dart; c = trav.next())
			f(c);
	}
	break;
	case FORCE_QUICK_TRAVERSAL:
	{
		TraversorCell<MAP, ORBIT,FORCE_QUICK_TRAVERSAL> trav(map, false, thread);
		for (Cell<ORBIT> c = trav.begin(), e = trav.end(); c.dart != e.dart; c = trav.next())
			f(c);
	}
	break;
	case AUTO:
	default:
	{
		TraversorCell<MAP, ORBIT,AUTO> trav(map, false, thread);
		for (Cell<ORBIT> c = trav.begin(), e = trav.end(); c.dart != e.dart; c = trav.next())
			f(c);
	}
	break;

	}
}

/*
 * Executes function f on each ORBIT until f returns false
 */
//template <unsigned int ORBIT, typename MAP, typename FUNC>
//inline void foreach_cell_until(const MAP& map, FUNC f, bool forceDartMarker = false, unsigned int thread = 0)
//{
//	TraversorCell<MAP, ORBIT> trav(map, forceDartMarker, thread);
//	for (Cell<ORBIT> c = trav.begin(), e = trav.end(); c.dart != e.dart; c = trav.next())
//		if (!f(c))
//			break;
//}

template <unsigned int ORBIT, typename MAP, typename FUNC>
inline void foreach_cell_until(const MAP& map, FUNC f, TraversalOptim opt = AUTO, unsigned int thread = 0)
{
	switch(opt)
	{
	case FORCE_DART_MARKING:
	{
		TraversorCell<MAP, ORBIT,FORCE_DART_MARKING> trav(map, false, thread);
		for (Cell<ORBIT> c = trav.begin(), e = trav.end(); c.dart != e.dart; c = trav.next())
			if (!f(c))
				break;
	}
	break;
	case FORCE_CELL_MARKING:
	{
		TraversorCell<MAP, ORBIT,FORCE_CELL_MARKING> trav(map, false, thread);
		for (Cell<ORBIT> c = trav.begin(), e = trav.end(); c.dart != e.dart; c = trav.next())
			if (!f(c))
				break;
	}
	break;
	case FORCE_QUICK_TRAVERSAL:
	{
		TraversorCell<MAP, ORBIT,FORCE_QUICK_TRAVERSAL> trav(map, false, thread);
		for (Cell<ORBIT> c = trav.begin(), e = trav.end(); c.dart != e.dart; c = trav.next())
			if (!f(c))
				break;
	}
	break;
	case AUTO:
	default:
	{
		TraversorCell<MAP, ORBIT,AUTO> trav(map, false, thread);
		for (Cell<ORBIT> c = trav.begin(), e = trav.end(); c.dart != e.dart; c = trav.next())
			if (!f(c))
				break;
	}
	break;
	}
}


template <unsigned int ORBIT, typename MAP, typename FUNC, typename FUNC2>
inline void foreach_cell_EvenOddd(const MAP& map, FUNC f, FUNC2 g, unsigned int nbpasses=1, TraversalOptim opt = AUTO, unsigned int thread = 0)
{
	switch(opt)
	{
	case FORCE_DART_MARKING:
	{
		TraversorCell<MAP,ORBIT,FORCE_DART_MARKING> trav(map, false, thread);
		TraversorCellEven<MAP,ORBIT,FORCE_DART_MARKING> tr1(trav);
		TraversorCellOdd<MAP,ORBIT,FORCE_DART_MARKING> tr2(trav);

		for (unsigned int i=0; i<nbpasses; ++i)
		{
			for (Cell<ORBIT> c = trav.begin(), e = trav.end(); c.dart != e.dart; c = trav.next())
				f(c);
			for (Cell<ORBIT> c = trav.begin(), e = trav.end(); c.dart != e.dart; c = trav.next())
				g(c);
		}
	}
	break;
	case FORCE_CELL_MARKING:
	{
		TraversorCell<MAP,ORBIT,FORCE_CELL_MARKING> trav(map, false, thread);
		TraversorCellEven<MAP,ORBIT,FORCE_CELL_MARKING> tr1(trav);
		TraversorCellOdd<MAP,ORBIT, FORCE_CELL_MARKING> tr2(trav);

		for (unsigned int i=0; i<nbpasses; ++i)
		{
			for (Cell<ORBIT> c = trav.begin(), e = trav.end(); c.dart != e.dart; c = trav.next())
				f(c);
			for (Cell<ORBIT> c = trav.begin(), e = trav.end(); c.dart != e.dart; c = trav.next())
				g(c);
		}
	}
	break;
	case FORCE_QUICK_TRAVERSAL:
	{
		TraversorCell<MAP,ORBIT,FORCE_QUICK_TRAVERSAL> trav(map, false, thread);
		TraversorCellEven<MAP,ORBIT,FORCE_QUICK_TRAVERSAL> tr1(trav);
		TraversorCellOdd<MAP,ORBIT,FORCE_QUICK_TRAVERSAL> tr2(trav);

		for (unsigned int i=0; i<nbpasses; ++i)
		{
			for (Cell<ORBIT> c = trav.begin(), e = trav.end(); c.dart != e.dart; c = trav.next())
				f(c);
			for (Cell<ORBIT> c = trav.begin(), e = trav.end(); c.dart != e.dart; c = trav.next())
				g(c);
		}
	}
	break;
	case AUTO:
	default:
	{
		TraversorCell<MAP,ORBIT,AUTO> trav(map, false, thread);
		TraversorCellEven<MAP,ORBIT,AUTO> tr1(trav);
		TraversorCellOdd<MAP,ORBIT,AUTO> tr2(trav);

		for (unsigned int i=0; i<nbpasses; ++i)
		{
			for (Cell<ORBIT> c = trav.begin(), e = trav.end(); c.dart != e.dart; c = trav.next())
				f(c);
			for (Cell<ORBIT> c = trav.begin(), e = trav.end(); c.dart != e.dart; c = trav.next())
				g(c);
		}
	}
	break;

	}

}



namespace Parallel
{
template <unsigned int ORBIT, typename MAP, typename FUNC>
void foreach_cell(MAP& map, FUNC func, unsigned int nbth=0, bool needMarkers=true);

template <unsigned int ORBIT, typename MAP, typename FUNC_E, typename FUNC_O>
void foreach_cell_EO(MAP& map, FUNC_E funcEven, FUNC_O funcOdd,unsigned int nbth=0, bool needMarkers=true);


}

template <typename MAP>
class TraversorV : public TraversorCell<MAP, VERTEX>
{
public:
	TraversorV(const MAP& m, unsigned int thread = 0) : TraversorCell<MAP, VERTEX>(m, false, thread)
	{}
};

template <typename MAP>
class TraversorE : public TraversorCell<MAP, EDGE>
{
public:
	TraversorE(const MAP& m, unsigned int thread = 0) : TraversorCell<MAP, EDGE>(m, false, thread)
	{}
};

template <typename MAP>
class TraversorF : public TraversorCell<MAP, FACE>
{
public:
	TraversorF(const MAP& m, unsigned int thread = 0) : TraversorCell<MAP, FACE>(m, false, thread)
	{}
};

template <typename MAP>
class TraversorW : public TraversorCell<MAP, VOLUME>
{
public:
	TraversorW(const MAP& m, unsigned int thread = 0) : TraversorCell<MAP, VOLUME>(m, false, thread)
	{}
};




} // namespace CGoGN

#include "Topology/generic/traversor/traversorCell.hpp"

#endif
