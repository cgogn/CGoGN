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
#include "Topology/generic/attribmap.h"
#include "Topology/generic/attributeHandler.h"
#include "Topology/generic/autoAttributeHandler.h"

namespace CGoGN
{

/**
 * Browser that traverses all darts and jumps over
 * those not selected by the selector
 */
class MapBrowserSelector : public MapBrowser
{
protected:
	AttribMap& m_map ;
	const FunctorSelect& m_selector ;

public:
	MapBrowserSelector(AttribMap& m, const FunctorSelect& fs);

	Dart begin() const;

	Dart end() const;

	void next(Dart& d) const;
} ;

/**
 * Browser that traverses a "submap" stored in a
 * linked list (an attribute of dart orbit)
 * It inherits from FunctorType to allow the use in
 * a foreach_cell which adds darts in the list
 */
class MapBrowserLinked : public MapBrowser, public FunctorType
{
protected:
	// The browsed map
	AttribMap& m_map ;

	// The table attributes of links storing the linking
	// The boolean autoAttribute is set if this attribute is managed by the browser
	bool autoAttribute ;
	DartAttribute<Dart> m_links ;

	Dart m_first ;
	Dart m_end ;

public:
	MapBrowserLinked(AttribMap& m);

	MapBrowserLinked(AttribMap& m, DartAttribute<Dart>& links);
	
	MapBrowserLinked(AttribMap& m, DartAttribute<Dart>& links, Dart first, Dart end);

	~MapBrowserLinked();
	DartAttribute<Dart>& getLinkAttr();
	
	void clear();

	Dart begin() const;

	Dart end() const;

	void next(Dart& d) const;

	void pushFront(Dart d);

	void pushBack(Dart d);

	void popFront();


	void addSelected(const FunctorSelect& fs);

	// operator() for use of foreach_cell
	bool operator()(Dart d);

	void append(MapBrowserLinked& mbl);
} ;

} // namespace CGoGN

#include "mapBrowser.hpp"

#endif /* MAPBROWSER_H_ */
