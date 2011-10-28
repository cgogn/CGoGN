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

/*******************************************************************************
					VERTEX CENTERED TRAVERSALS
*******************************************************************************/

// Traverse the edges incident to a given vertex
template <typename MAP>
class Traversor1VE
{
private:
	MAP& m ;
	Dart start ;
	Dart current ;

	Dart d2 ;

public:
	Traversor1VE(MAP& map, Dart dart) : m(map), start(dart)
	{
		d2 = m.phi_1(start) ;
		if(d2 == start) // 1 edge loop case
			d2 = NIL ;
	}

	Dart begin() { current = start ; return current ; }
	Dart end() { return NIL ; }
	Dart next()
	{
		if(current != NIL)
		{
			if(current == start)
				current = d2 ;
			else
				current = NIL ;
		}
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

	Dart d2 ;

public:
	Traversor1VVaE(MAP& map, Dart dart) : m(map)
	{
		start = m.phi_1(dart) ;
		if(start == dart) // 1 edge loop case
			start = NIL ;
		else
		{
			d2 = m.phi1(dart) ;
			if(d2 == start) // 2 edges loop case
				d2 = NIL ;
		}
	}

	Dart begin() { current = start ; return current ; }
	Dart end() { return NIL ; }
	Dart next()
	{
		if(current != NIL)
		{
			if(current == start)
				current = d2 ;
			else
				current = NIL ;
		}
		return current ;
	}
} ;

/*******************************************************************************
					EDGE CENTERED TRAVERSALS
*******************************************************************************/

// Traverse the vertices incident to a given edge
template <typename MAP>
class Traversor1EV
{
private:
	MAP& m ;
	Dart start ;
	Dart current ;

	Dart d2 ;

public:
	Traversor1EV(MAP& map, Dart dart) : m(map), start(dart)
	{
		d2 = m.phi1(start) ;
		if(d2 == start) // 1 edge loop case
			d2 = NIL ;
	}

	Dart begin() { current = start ; return current ; }
	Dart end() { return NIL ; }
	Dart next()
	{
		if(current != NIL)
		{
			if(current == start)
				current = d2 ;
			else
				current = NIL ;
		}
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

	Dart d2 ;

public:
	Traversor1EEaV(MAP& map, Dart dart) : m(map)
	{
		start = m.phi_1(dart) ;
		if(start == dart) // 1 edge loop case
			start = NIL ;
		else
		{
			d2 = m.phi1(dart) ;
			if(d2 == start) // 2 edges loop case
				d2 = NIL ;
		}
	}

	Dart begin() { current = start ; return current ; }
	Dart end() { return NIL ; }
	Dart next()
	{
		if(current != NIL)
		{
			if(current == start)
				current = d2 ;
			else
				current = NIL ;
		}
		return current ;
	}
} ;

} // namespace CGoGN

#endif
