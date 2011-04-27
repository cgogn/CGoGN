/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009, IGG Team, LSIIT, University of Strasbourg                *
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
* Web site: https://iggservis.u-strasbg.fr/CGoGN/                              *
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
class MapBrowserSelector: public MapBrowser
{
protected:
	MAP& m_map;
	const FunctorSelect& m_selector;
public:
	MapBrowserSelector(MAP& m, const FunctorSelect& fs):
		m_map(m), m_selector(fs)
	{}

	Dart begin() { return m_map.begin(); }

	Dart end() { return m_map.end(); }

	void next(Dart& d)
	{
		do
		{
			m_map.next(d);
		}
		while ( (d != m_map.end()) && !m_selector(d) );
	}
};

/**
 * Browser that traverses a "submap" stored in a
 * linked list (an attribute of dart orbit)
 * It inherits from FunctorType to allow the use in
 * a foreach_cell which adds darts in the list
 */
template <typename MAP>
class MapBrowserLinked: public MapBrowser, public FunctorType
{
protected:
	Dart m_first;
public:
	MapBrowserLinked():	m_first(NIL)
	{}

	MapBrowserLinked(MAP& map, const FunctorSelect& fs): m_first(NIL) { addSelected(map, fs); }

	Dart begin() { return m_first;}

	Dart end() { return NIL;}

	// init the list
	void init() { m_first = NIL;}

	// add a dart to the list
	virtual void add(Dart d) = 0;

	virtual void popFront() = 0;

	void addSelected(MAP& map, const FunctorSelect& fs)
	{
		for (Dart d = map.begin(); d != map.end(); map.next(d))
		{
			if (fs(d))
				add(d);
		}
	}

	// operator() for use of foreach_cell
	bool operator()(Dart d)
	{
		add(d);
		return false;
	}
};

/**
 * MapBrowserLinked with AttributeHandler<Dart> given
 * as parameter of construction (ref). the AttributeHandler
 * can be shared by many MapBrowser
 */
template <typename MAP>
class MapBrowserLinkedAttr: public MapBrowserLinked<MAP>
{
protected:
	/// ref to the table attributes of links
	AttributeHandler<Dart>& m_links;
public:
	MapBrowserLinkedAttr(AttributeHandler<Dart>& links):
		MapBrowserLinked<MAP>(), m_links(links)
	{}

	void next(Dart& d) { d = m_links[d];}

	/// add a dart to linked list
	void add(Dart d) { m_links[d.index] = this->m_first; this->m_first = d; }

	//pop the first
	void popFront() { this->m_first = m_links[this->m_first.index]; }
};

/**
 * MapBrowser with local attribute of linked darts
 * Attribute is added and removed automatically
 * with the scope of object
 */
template <typename MAP>
class MapBrowserLinkedAuto: public MapBrowserLinked<MAP>
{
protected:
	/// auto table attributes of links (added to construction and removed at destruction)
	AutoAttributeHandler<Dart> m_links;
public:
	/**
	 * Constructor
	 * @param m the map (used to add the attribute)
	 * @param nameAtt attributeName
	 */
	MapBrowserLinkedAuto(MAP& m):
		MapBrowserLinked<MAP>(), m_links(m, DART_ORBIT)
	{}

	void next(Dart& d) { d = m_links[d]; }

	/// add a dart to linked list
	void add(Dart d) { m_links[d.index] = this->m_first; this->m_first = d; }

	//pop the first
	void popFront() { this->m_first = m_links[this->m_first.index]; }
};

} // end namespace CGoGN

#endif /* MAPBROWSER_H_ */
