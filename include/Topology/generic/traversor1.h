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

#ifndef __TRAVERSOR1_H__
#define __TRAVERSOR1_H__

#include "Topology/generic/dart.h"

namespace CGoGN
{

// Traverse the edges incident to a given vertex
template <typename MAP>
class Traversor1VE
{
private:
	MAP& m ;
	Dart start ;
	Dart current ;

public:
	Traversor1VE(MAP& map, Dart dart) : m(map), start(dart)
	{}

	Dart begin() { current = start ; return current ; }
	Dart end() { return NIL ; }
	Dart next()
	{
		current = current == start ? m.phi_1(start) : NIL ;
		return current ;
	}
} ;

// Traverse the vertices adjacent to a given vertex through sharing a common edge
class Traversor1VVaE
{
private:
	MAP& m ;
	Dart start ;
	Dart current ;

public:
	Traversor1VVaE(MAP& map, Dart dart) : m(map)
	{
		start = m.phi_1(dart) ;
	}

	Dart begin() { current = start ; return current ; }
	Dart end() { return NIL ; }
	Dart next()
	{
		current = current == start ? m.phi1(m.phi1(start)) : NIL ;
		return current ;
	}
} ;

// Traverse the vertices incident to a given edge
template <typename MAP>
class Traversor1EV
{
private:
	MAP& m ;
	Dart start ;
	Dart current ;

public:
	Traversor1EV(MAP& map, Dart dart) : m(map), start(dart)
	{}

	Dart begin() { current = start ; return current ; }
	Dart end() { return NIL ; }
	Dart next()
	{
		current = current == start ? m.phi1(start) : NIL ;
		return current ;
	}
} ;

// Traverse the edges adjacent to a given edge through sharing a common vertex
class Traversor1EEaV
{
private:
	MAP& m ;
	Dart start ;
	Dart current ;

public:
	Traversor1EEaV(MAP& map, Dart dart) : m(map)
	{
		start = m.phi_1(dart) ;
	}

	Dart begin() { current = start ; return current ; }
	Dart end() { return NIL ; }
	Dart next()
	{
		current = current == start ? m.phi1(m.phi1(start)) : NIL ;
		return current ;
	}
} ;

} // namespace CGoGN

#endif
