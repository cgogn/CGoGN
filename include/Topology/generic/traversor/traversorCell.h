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

template <typename MAP, unsigned int ORBIT>
class TraversorCell //: public Traversor<MAP>
{
private:
	const MAP& m ;
	unsigned int dimension ;

	const AttributeContainer* cont ;
	unsigned int qCurrent ;

	DartMarker<MAP>* dmark ;
	CellMarker<MAP, ORBIT>* cmark ;
	const AttributeMultiVector<Dart>* quickTraversal ;

	Cell<ORBIT> current ;
	bool firstTraversal ;

public:
	TraversorCell(const MAP& map, bool forceDartMarker = false, unsigned int thread = 0) ;

	~TraversorCell() ;

	inline Cell<ORBIT> begin() ;

	inline Cell<ORBIT> end() ;

	inline Cell<ORBIT> next() ;

	inline void skip(Cell<ORBIT> c);
} ;



template <unsigned int ORBIT, typename MAP, typename FUNC>
inline void foreach_cell(const MAP& map, FUNC f, bool forceDartMarker = false, unsigned int thread = 0)
{
	TraversorCell<MAP, ORBIT> trav(map, forceDartMarker, thread);
	for (Cell<ORBIT> c = trav.begin(), e = trav.end(); c.dart != e.dart; c = trav.next())
		f(c);
}


template <unsigned int ORBIT, typename MAP, typename FUNC>
inline void foreach_cell_until(const MAP& map, FUNC f, bool forceDartMarker = false, unsigned int thread = 0)
{
	TraversorCell<MAP, ORBIT> trav(map, forceDartMarker, thread);
	for (Cell<ORBIT> c = trav.begin(), e = trav.end(); c.dart != e.dart; c = trav.next())
		if (!f(c))
			break;
}

namespace Parallel
{
template <unsigned int ORBIT, typename MAP, typename FUNC>
void foreach_cell(MAP& map, FUNC func, unsigned int nbth=0, bool needMarkers=true);
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
