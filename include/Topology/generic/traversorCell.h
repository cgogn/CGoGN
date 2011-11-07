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

namespace CGoGN
{

template <typename MAP, unsigned int ORBIT>
class TraversorCell
{
private:
	MAP& m ;
	DartMarker mark ;
	Dart current ;

public:
	TraversorCell(MAP& map) : m(map), mark(map)
	{}

	Dart begin()
	{
		mark.unmarkAll() ;	// TODO ajouter test pour ne pas toujours faire le unmark !!
		current = m.begin() ;
		while(m.isBoundaryMarked(current))
			m.next(current) ;
		if(current == m.end())
			current = NIL ;
		else
			mark.markOrbit(ORBIT, current) ;
		return current ;
	}
	Dart end() { return NIL ; }
	Dart next()
	{
		if(current != NIL)
		{
			while(current != NIL && (mark.isMarked(current) || m.isBoundaryMarked(current)))
			{
				m.next(current) ;
				if(current == m.end())
					current = NIL ;
			}
			if(current != NIL)
				mark.markOrbit(ORBIT, current) ;
		}
		return current ;
	}
} ;

template <typename MAP>
class TraversorV : public TraversorCell<MAP, VERTEX>
{
public:
	TraversorV(MAP& m) : TraversorCell<MAP, VERTEX>(m) {}
};

template <typename MAP>
class TraversorE : public TraversorCell<MAP, EDGE>
{
public:
	TraversorE(MAP& m) : TraversorCell<MAP, EDGE>(m) {}
};

template <typename MAP>
class TraversorF : public TraversorCell<MAP, FACE>
{
public:
	TraversorF(MAP& m) : TraversorCell<MAP, FACE>(m) {}
};

template <typename MAP>
class TraversorW : public TraversorCell<MAP, VOLUME>
{
public:
	TraversorW(MAP& m) : TraversorCell<MAP, VOLUME>(m) {}
};

} // namespace CGoGN

#endif
