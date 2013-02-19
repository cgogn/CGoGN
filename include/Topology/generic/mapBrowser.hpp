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

namespace CGoGN
{


inline MapBrowserSelector::MapBrowserSelector(AttribMap& m, const FunctorSelect& fs) :
	m_map(m), m_selector(fs)
{}

inline Dart MapBrowserSelector::begin() const
{
	Dart d = m_map.realBegin() ;
	while ( (d != m_map.realEnd()) && !m_selector(d) )
		m_map.realNext(d);

	return d;
}

inline Dart MapBrowserSelector::end() const
{
	return m_map.realEnd() ;
}

inline void MapBrowserSelector::next(Dart& d) const
{
	do
	{
		m_map.realNext(d) ;
	}
	while ( (d != m_map.realEnd()) && !m_selector(d) ) ;
}


inline MapBrowserLinked::MapBrowserLinked(AttribMap& m) :
	m_map(m), autoAttribute(true), m_first(NIL), m_end(NIL)
{
	m_links = m.addAttribute<Dart, DART>("") ;
}

inline MapBrowserLinked::MapBrowserLinked(AttribMap& m, DartAttribute<Dart>& links) :
	m_map(m), autoAttribute(false), m_links(links), m_first(NIL), m_end(NIL)
{
}

inline MapBrowserLinked::MapBrowserLinked(AttribMap& m, DartAttribute<Dart>& links, Dart first, Dart end) :
	m_map(m), autoAttribute(false), m_links(links), m_first(first), m_end(end)
{
}

inline MapBrowserLinked::~MapBrowserLinked()
{
	if (autoAttribute)
		m_map.removeAttribute(m_links) ;
}

inline DartAttribute<Dart>& MapBrowserLinked::getLinkAttr()
{
	return m_links;
}

inline void MapBrowserLinked::clear()
{
	m_first = NIL ;
	m_end = NIL ;
}

inline Dart MapBrowserLinked::begin() const
{
	return m_first ;
}

inline Dart MapBrowserLinked::end() const
{
	return NIL ;
}

inline void MapBrowserLinked::next(Dart& d) const
{
	assert(d != NIL) ;
	d = m_links[d] ;
}

inline void MapBrowserLinked::pushFront(Dart d)
{
	assert(d != NIL) ;
	m_links[d] = m_first ;
	m_first = d ;
	if (m_end == NIL)		// empty list
		m_end = d ;
}

inline void MapBrowserLinked::pushBack(Dart d)
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

inline void MapBrowserLinked::popFront()
{
	if (m_first == m_end)	// one element or empty list
	{
		m_first = NIL ;
		m_end = NIL ;
	}
	else m_first = m_links[m_first] ;
}


inline void MapBrowserLinked::addSelected(const FunctorSelect& fs)
{
	for (Dart d = m_map.begin() ; d != m_map.end() ; m_map.next(d))
	{
		if (fs(d))
			//			pushFront(d) ;
			pushBack(d); // better tot use push_back for memory access
	}
}

// operator() for use of foreach_cell
inline bool MapBrowserLinked::operator()(Dart d)
{
	pushFront(d) ;
	return false ;
}

inline void MapBrowserLinked::append(MapBrowserLinked& mbl)
{
	// nothing to append
	if (mbl.m_end == NIL)
		return;
	if (m_end == NIL)
	{
		m_first = mbl.m_first;
		m_end = mbl.m_end;
		return;
	}

	m_links[m_end] = mbl.m_first;
	m_end = mbl.m_end;
}


} // namespace CGoGN
