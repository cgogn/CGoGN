/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2011, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.u-strasbg.fr/                                         *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#ifndef __TRAVERSOR_CELL_H__
#define __TRAVERSOR_CELL_H__

#include "Topology/generic/dart.h"
#include "Topology/generic/dartmarker.h"
#include "Topology/generic/cellmarker.h"

namespace CGoGN
{

template <typename MAP>
class TraversorCell
{
private:
	MAP& m ;
	DartMarker* dmark ;
	CellMarker* cmark ;
	Dart current ;
	bool firstTraversal ;
	unsigned int m_orbit;

public:
	TraversorCell(MAP& map, unsigned int orbit, unsigned int thread = 0) :
		m(map), dmark(NULL), cmark(NULL), current(NIL), firstTraversal(true), m_orbit(orbit)
	{
		if(map.isOrbitEmbedded(m_orbit))
			cmark = new CellMarker(map, m_orbit, thread) ;
		else
			dmark = new DartMarker(map, thread) ;
	}

	~TraversorCell()
	{
		if(dmark)
			delete dmark ;
		else
			delete cmark ;
	}

	Dart begin()
	{
		if(!firstTraversal)
		{
			if(dmark)
				dmark->unmarkAll() ;
			else
				cmark->unmarkAll() ;
		}

		current = m.begin() ;
		while(m.isBoundaryMarked(current))
			m.next(current) ;

		if(current == m.end())
			current = NIL ;
		else
		{
			if(dmark)
				dmark->markOrbit(m_orbit, current) ;
			else
				cmark->mark(current) ;
		}

		firstTraversal = false ;
		return current ;
	}

	Dart end() { return NIL ; }

	Dart next()
	{
		if(current != NIL)
		{
			bool ismarked ;
			if(dmark)
				ismarked = dmark->isMarked(current) ;
			else
				ismarked = cmark->isMarked(current) ;

			while(current != NIL && (ismarked || m.isBoundaryMarked(current)))
			{
				m.next(current) ;
				if(current == m.end())
					current = NIL ;
				else
				{
					if(dmark)
						ismarked = dmark->isMarked(current) ;
					else
						ismarked = cmark->isMarked(current) ;
				}
			}

			if(current != NIL)
			{
				if(dmark)
					dmark->markOrbit(m_orbit, current) ;
				else
					cmark->mark(current) ;
			}
		}
		return current ;
	}
} ;

template <typename MAP>
class TraversorV : public TraversorCell<MAP>
{
public:
	TraversorV(MAP& m, unsigned int thread = 0) : TraversorCell<MAP>(m, VERTEX, thread)
	{}
};

template <typename MAP>
class TraversorE : public TraversorCell<MAP>
{
public:
	TraversorE(MAP& m, unsigned int thread = 0) : TraversorCell<MAP>(m, EDGE, thread)
	{}
};

template <typename MAP>
class TraversorF : public TraversorCell<MAP>
{
public:
	TraversorF(MAP& m, unsigned int thread = 0) : TraversorCell<MAP>(m, FACE, thread)
	{}
};

template <typename MAP>
class TraversorW : public TraversorCell<MAP>
{
public:
	TraversorW(MAP& m, unsigned int thread = 0) : TraversorCell<MAP>(m, VOLUME, thread)
	{}
};

} // namespace CGoGN

#endif
