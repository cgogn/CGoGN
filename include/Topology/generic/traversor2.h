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

#ifndef __TRAVERSOR2_H__
#define __TRAVERSOR2_H__

#include "Topology/generic/dart.h"

namespace CGoGN
{

/*******************************************************************************
					VERTEX CENTERED TRAVERSALS
*******************************************************************************/

// Traverse the edges incident to a given vertex
template <typename MAP>
class Traversor2VE
{
private:
	MAP& m ;
	Dart start ;
	Dart current ;

public:
	Traversor2VE(MAP& map, Dart dart) : m(map), start(dart)
	{}

	Dart begin() { current = start ; return current ; }
	Dart end() { return NIL ; }
	Dart next()
	{
		if(current != NIL)
		{
			current = m.alpha1(current) ;
			if(current == start)
				current = NIL ;
		}
		return current ;
	}
} ;

// Traverse the faces incident to a given vertex
template <typename MAP>
class Traversor2VF
{
private:
	MAP& m ;
	Dart start ;
	Dart current ;

public:
	Traversor2VF(MAP& map, Dart dart) : m(map), start(dart)
	{}

	Dart begin() { current = start ; return current ; }
	Dart end() { return NIL ; }
	Dart next()
	{
		if(current != NIL)
		{
			current = m.alpha1(current) ;
			if(m.isBoundarymarked(current)) // jump over a boundary face
				current = m.alpha1(current) ;
			if(current == start)
				current = NIL ;
		}
		return current ;
	}
} ;

// Traverse the vertices adjacent to a given vertex through sharing a common edge
template <typename MAP>
class Traversor2VVaE
{
private:
	MAP& m ;
	Dart start ;
	Dart current ;

public:
	Traversor2VVaE(MAP& map, Dart dart) : m(map)
	{
		start = m.phi2(dart) ;
	}

	Dart begin() { current = start ; return current ; }
	Dart end() { return NIL ; }
	Dart next()
	{
		if(current != NIL)
		{
			current = m.phi_1(m.phi2(current)) ;
			if(current == start)
				current = NIL ;
		}
		return current ;
	}
} ;

// Traverse the vertices adjacent to a given vertex through sharing a common face
template <typename MAP>
class Traversor2VVaF
{
private:
	MAP& m ;
	Dart start ;
	Dart current ;

	Dart stop ;

public:
	Traversor2VVaF(MAP& map, Dart dart) : m(map)
	{
		if(m.isBoundaryMarked(dart))
			dart = m.alpha1(dart) ;
		start = m.phi1(m.phi1(dart)) ;
		if(start == dart)
			start = m.phi1(dart) ;
		stop = dart ;
	}

	Dart begin() { current = start ; return current ; }
	Dart end() { return NIL ; }
	Dart next()
	{
		if(current != NIL)
		{
			current = m.phi1(current) ;
			if(current == stop)
			{
				Dart d = m.alpha1(current) ;
				if(m.isBoundaryMarked(d)) // jump over a boundary face
					d = m.alpha1(d) ;
				current = m.phi1(m.phi1(d)) ;
				if(current == d)
					current = m.phi1(d) ;
				stop = d ;
			}
			if(current == start)
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
class Traversor2EV
{
private:
	MAP& m ;
	Dart start ;
	Dart current ;

public:
	Traversor2EV(MAP& map, Dart dart) : m(map), start(dart)
	{}

	Dart begin() { current = start ; return current ; }
	Dart end() { return NIL ; }
	Dart next()
	{
		if(current != NIL)
		{
			current = m.phi2(current) ;
			if(current == start)
				current = NIL ;
		}
		return current ;
	}
} ;

// Traverse the faces incident to a given edge
template <typename MAP>
class Traversor2EF
{
private:
	MAP& m ;
	Dart start ;
	Dart current ;

public:
	Traversor2EF(MAP& map, Dart dart) : m(map), start(dart)
	{}

	Dart begin() { current = start ; return current ; }
	Dart end() { return NIL ; }
	Dart next()
	{
		if(current != NIL)
		{
			current = m.phi2(current) ;
			if(current == start || m.isBoundaryMarked(current)) // do not consider a boundary face
				current = NIL ;
		}
		return current ;
	}
} ;

// Traverse the edges adjacent to a given edge through sharing a common vertex
template <typename MAP>
class Traversor2EEaV
{
private:
	MAP& m ;
	Dart start ;
	Dart current ;

	Dart stop1, stop2 ;

public:
	Traversor2EEaV(MAP& map, Dart dart) : m(map)
	{
		start = m.alpha1(dart) ;
		stop1 = dart ;
		stop2 = m.phi2(dart) ;
	}

	Dart begin() { current = start ; return current ; }
	Dart end() { return NIL ; }
	Dart next()
	{
		if(current != NIL)
		{
			current = m.alpha1(current) ;
			if(current == stop1)
				current = m.alpha1(stop2) ;
			else if(current == stop2)
				current = NIL ;
		}
		return current ;
	}
} ;

// Traverse the edges adjacent to a given edge through sharing a common face
template <typename MAP>
class Traversor2EEaF
{
private:
	MAP& m ;
	Dart start ;
	Dart current ;

	Dart stop1, stop2 ;

public:
	Traversor2EEaF(MAP& map, Dart dart) : m(map)
	{
		start = m.phi1(dart) ;
		stop1 = dart ;
		stop2 = m.phi2(dart) ;
	}

	Dart begin() { current = start ; return current ; }
	Dart end() { return NIL ; }
	Dart next()
	{
		if(current != NIL)
		{
			current = m.phi1(current) ;
			if(current == stop1)
				current = m.phi1(stop2) ;
			else if(current == stop2)
				current = NIL ;
		}
		return current ;
	}
} ;

/*******************************************************************************
					FACE CENTERED TRAVERSALS
*******************************************************************************/

// Traverse the vertices incident to a given face
template <typename MAP>
class Traversor2FV
{
private:
	MAP& m ;
	Dart start ;
	Dart current ;

public:
	Traversor2FV(MAP& map, Dart dart) : m(map), start(dart)
	{}

	Dart begin() { current = start ; return current ; }
	Dart end() { return NIL ; }
	Dart next()
	{
		if(current != NIL)
		{
			current = m.phi1(current) ;
			if(current == start)
				current = NIL ;
		}
		return current ;
	}
} ;

// Traverse the edges incident to a given face
template <typename MAP>
class Traversor2FE
{
private:
	MAP& m ;
	Dart start ;
	Dart current ;

public:
	Traversor2FE(MAP& map, Dart dart) : m(map), start(dart)
	{}

	Dart begin() { current = start ; return current ; }
	Dart end() { return NIL ; }
	Dart next()
	{
		if(current != NIL)
		{
			current = m.phi1(current) ;
			if(current == start)
				current = NIL ;
		}
		return current ;
	}
} ;

// Traverse the faces adjacent to a given face through sharing a common vertex
template <typename MAP>
class Traversor2FFaV
{
private:
	MAP& m ;
	Dart start ;
	Dart current ;

	Dart stop ;

public:
	Traversor2FFaV(MAP& map, Dart dart) : m(map)
	{
		start = m.alpha1(m.alpha1(dart)) ;
		current = start ;
		if(start == dart)
		{
			stop = m.alpha1(dart) ;
			start = next() ;
		}
		stop = dart ;
		if(m.isBoundaryMarked(start))
			start = next() ;
	}

	Dart begin() { current = start ; return current ; }
	Dart end() { return NIL ; }
	Dart next()
	{
		if(current != NIL)
		{
			current = m.alpha1(current) ;
			if(current == stop)
			{
				Dart d = m.phi1(current) ;
				current = m.alpha1(m.alpha1(d)) ;
				if(current == d)
				{
					stop = m.alpha1(d) ;
					return next() ;
				}
				stop = d ;
				if(m.isBoundaryMarked(current))
					return next() ;
			}
			if(current == start)
				current = NIL ;
		}
		return current ;
	}
} ;

// Traverse the faces adjacent to a given face through sharing a common edge
template <typename MAP>
class Traversor2FFaE
{
private:
	MAP& m ;
	Dart start ;
	Dart current ;

public:
	Traversor2FFaE(MAP& map, Dart dart) : m(map)
	{
		start = m.phi2(dart) ;
		while(start != NIL && m.isBoundaryMarked(start))
		{
			start = m.phi2(m.phi1(m.phi2(start))) ;
			if(start == m.phi2(dart))
				start = NIL ;
		}
	}

	Dart begin() { current = start ; return current ; }
	Dart end() { return NIL ; }
	Dart next()
	{
		if(current != NIL)
		{
			do
			{
				current = m.phi2(m.phi1(m.phi2(current))) ;
			} while(m.isBoundaryMarked(current)) ;
			if(current == start)
				current = NIL ;
		}
		return current ;
	}
} ;

} // namespace CGoGN

#endif
