/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2012, IGG Team, LSIIT, University of Strasbourg           *
*                                                                              *
* This library is free software ; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation ; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY ; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library ; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#ifndef MAPBROWSER_H_
#define MAPBROWSER_H_

#include "Topology/generic/dart.h"
#include "Topology/generic/attributeHandler.h"
#include "Topology/generic/autoAttributeHandler.h"

namespace CGoGN
{

/**
 * Browser that traverses all darts and jumps over
 * those not selected by the selector
 */
template <typename MAP>
class MapBrowserSelector : public MapBrowser
{
protected:
	MAP& m_map ;
	const FunctorSelect& m_selector ;
public:
	MapBrowserSelector(MAP& m, const FunctorSelect& fs) :
		m_map(m), m_selector(fs)
	{}

	Dart begin() const
	{
		return m_map.begin() ;
	}

	Dart end() const
	{
		return m_map.end() ;
	}

	void next(Dart& d) const
	{
		do
		{
			m_map.next(d) ;
		}
		while ( (d != m_map.end()) && !m_selector(d) ) ;
	}
} ;

/**
 * Browser that traverses a "submap" stored in a
 * linked list (an attribute of dart orbit)
 * It inherits from FunctorType to allow the use in
 * a foreach_cell which adds darts in the list
 */
template <typename MAP>
class MapBrowserLinked : public MapBrowser, public FunctorType
{
protected:
	// The browsed map
	MAP& m_map ;

	// The table attributes of links storing the linking
	// The boolean autoAttribute is set if this attribut is managed by the browser
	bool autoAttribute ;
	AttributeHandler<Dart, DART> m_links ;

	Dart m_first ;
	Dart m_end ;

public:
	MapBrowserLinked(MAP& m) :
		m_map(m), autoAttribute(true), m_first(NIL), m_end(NIL)
	{
		m_links = m.template addAttribute<Dart, DART>("") ;
	}

	MapBrowserLinked(MAP& m, AttributeHandler<Dart, DART>& links) :
		m_map(m), autoAttribute(false), m_links(links), m_first(NIL), m_end(NIL)
	{
	}

	~MapBrowserLinked()
	{
		if (autoAttribute)
			m_map.template removeAttribute<Dart>(m_links) ;
	}

	void clear()
	{
		m_first = NIL ;
		m_end = NIL ;
	}

	Dart begin() const
	{
		return m_first ;
	}

	Dart end() const
	{
		return NIL ;
	}

	void next(Dart& d) const
	{
		assert(d != NIL) ;
		d = m_links[d] ;
	}

	void pushFront(Dart d)
	{
		assert(d != NIL) ;
		m_links[d] = m_first ;
		m_first = d ;
		if (m_end == NIL)		// empty list
			m_end = d ;
	}

	void pushBack(Dart d)
	{
		assert(d != NIL) ;
		m_links[d] = NIL ;
		if (m_first == NIL)		// empty list
		{
			m_first = d ;
			m_end = d ;
		}
		else
		{
			m_links[m_end] = d ;
			m_end = d ;
		}
	}

	void popFront()
	{
		if (m_first == m_end)	// one element or empty list
		{
			m_first = NIL ;
			m_end = NIL ;
		}
		else m_first = m_links[m_first] ;
	}


	void addSelected(const FunctorSelect& fs)
	{
		for (Dart d = m_map.begin() ; d != m_map.end() ; m_map.next(d))
		{
			if (fs(d))
				pushFront(d) ;
		}
	}

	// operator() for use of foreach_cell
	bool operator()(Dart d)
	{
		pushFront(d) ;
		return false ;
	}
} ;

} // namespace CGoGN

#endif /* MAPBROWSER_H_ */
