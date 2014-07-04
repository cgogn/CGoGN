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

namespace CGoGN
{

/*******************************************************************************
					VERTEX CENTERED TRAVERSALS
*******************************************************************************/

// VTraversor1VE

template <typename MAP>
VTraversor1VE<MAP>::VTraversor1VE(const MAP& map, Dart dart) : m(map), start(dart)
{
	d2 = m.phi_1(start) ;
	if(d2 == start) // 1 edge loop case
		d2 = NIL ;
}

template <typename MAP>
Dart VTraversor1VE<MAP>::begin()
{
	current = start ;
	return current ;
}

template <typename MAP>
Dart VTraversor1VE<MAP>::end()
{
	return NIL ;
}

template <typename MAP>
Dart VTraversor1VE<MAP>::next()
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

// VTraversor1VVaE

template <typename MAP>
VTraversor1VVaE<MAP>::VTraversor1VVaE(const MAP& map, Dart dart) : m(map)
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

template <typename MAP>
Dart VTraversor1VVaE<MAP>::begin()
{
	current = start ;
	return current ;
}

template <typename MAP>
Dart VTraversor1VVaE<MAP>::end()
{
	return NIL ;
}

template <typename MAP>
Dart VTraversor1VVaE<MAP>::next()
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

/*******************************************************************************
					EDGE CENTERED TRAVERSALS
*******************************************************************************/

// VTraversor1EV

template <typename MAP>
VTraversor1EV<MAP>::VTraversor1EV(const MAP& map, Dart dart) : m(map), start(dart)
{
	d2 = m.phi1(start) ;
	if(d2 == start) // 1 edge loop case
		d2 = NIL ;
}

template <typename MAP>
Dart VTraversor1EV<MAP>::begin()
{
	current = start ;
	return current ;
}

template <typename MAP>
Dart VTraversor1EV<MAP>::end()
{
	return NIL ;
}

template <typename MAP>
Dart VTraversor1EV<MAP>::next()
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

// VTraversor1EEaV

template <typename MAP>
VTraversor1EEaV<MAP>::VTraversor1EEaV(const MAP& map, Dart dart) : m(map)
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

template <typename MAP>
Dart VTraversor1EEaV<MAP>::begin()
{
	current = start ;
	return current ;
}

template <typename MAP>
Dart VTraversor1EEaV<MAP>::end()
{
	return NIL ;
}

template <typename MAP>
Dart VTraversor1EEaV<MAP>::next()
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

} // namespace CGoGN
