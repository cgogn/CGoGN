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
#include "Utils/static_assert.h"

namespace CGoGN
{

//**********************
// Marker for traversor
//**********************

template <typename MAP>
MarkerForTraversor<MAP>::MarkerForTraversor(MAP& map, unsigned int orbit, bool forceDartMarker, unsigned int thread) :
	m_map(map),
	m_dmark(NULL),
	m_cmark(NULL),
	m_orbit(orbit)
{
	if(!forceDartMarker && map.isOrbitEmbedded(m_orbit))
		m_cmark = new CellMarkerStore(map, m_orbit, thread) ;
	else
		m_dmark = new DartMarkerStore(map, thread) ;
}


template <typename MAP>
MarkerForTraversor<MAP>::~MarkerForTraversor()
{
	if (m_cmark)
		delete m_cmark;
	if (m_dmark)
		delete m_dmark;
}

template <typename MAP>
void MarkerForTraversor<MAP>::mark(Dart d)
{
	if (m_cmark)
		m_cmark->mark(d);
	else
		m_dmark->markOrbit(m_orbit,d);
}


template <typename MAP>
void MarkerForTraversor<MAP>::unmark(Dart d)
{
	if (m_cmark)
		m_cmark->unmark(d);
	else
		m_dmark->unmarkOrbit(m_orbit,d);
}

template <typename MAP>
bool MarkerForTraversor<MAP>::isMarked(Dart d)
{
	if (m_cmark)
		return m_cmark->isMarked(d);
	return m_dmark->isMarked(d);
}

template <typename MAP>
CellMarkerStore* MarkerForTraversor<MAP>::cmark()
{
	return m_cmark;
}

template <typename MAP>
DartMarkerStore* MarkerForTraversor<MAP>::dmark()
{
	return m_dmark;
}



//**************************************
// Traversor cellX Y incident to cell X
//**************************************


template <typename MAP>
Traversor3XY<MAP>::Traversor3XY(MAP& map, Dart dart, unsigned int orbX, unsigned int orbY, bool forceDartMarker, unsigned int thread) :
//	Traversor3MarkOnly<MAP,ORBIT_Y>(map, forceDartMarker, thread, true),
	m_map(map),
	m_dmark(NULL),
	m_cmark(NULL),
	m_tradoo(map,orbX,dart,thread),
	m_orbx(orbX),
	m_orby(orbY),
	m_allocated(true),
	m_first(true)
{
	if(!forceDartMarker && map.isOrbitEmbedded(orbY))
		m_cmark = new CellMarkerStore(map, orbY, thread) ;
	else
		m_dmark = new DartMarkerStore(map, thread) ;
}

template <typename MAP>
Traversor3XY<MAP>::Traversor3XY(MAP& map, Dart dart, unsigned int orbX, unsigned int orbY, MarkerForTraversor<MAP>& tmo, bool forceDartMarker, unsigned int thread) :
	m_map(map),
	m_tradoo(map,orbX,dart,thread),
	m_orbx(orbX),
	m_orby(orbY),
	m_allocated(false),
	m_first(true)
{
	m_cmark = tmo.cmark();
	m_dmark = tmo.dmark();
}

template <typename MAP>
Traversor3XY<MAP>::~Traversor3XY()
{
	if (m_allocated)
	{
		if (m_cmark)
			delete m_cmark;
		if (m_dmark)
			delete m_dmark;
	}
}

template <typename MAP>
Dart Traversor3XY<MAP>::begin()
{
	if (!m_first)
	{
		if (m_cmark)
			m_cmark->unmarkAll();
		else
			m_dmark->unmarkAll();
	}
	m_first=false;

	m_current = m_tradoo.begin() ;
	// for the case of beginning with a given MarkerForTraversor
	if (!m_allocated)
	{
		if (m_cmark)
		{
			while ((m_current != NIL) && m_cmark->isMarked(m_current))
				m_current = m_tradoo.next();
		}
		else
		{
			while ((m_current != NIL) && m_dmark->isMarked(m_current))
				m_current = m_tradoo.next();
		}
	}
	return m_current;
}

template <typename MAP>
Dart Traversor3XY<MAP>::end()
{
	return NIL ;
}

template <typename MAP>
Dart Traversor3XY<MAP>::next()
{
	if(m_current != NIL)
	{
		if (m_cmark)
		{
			m_cmark->mark(m_current);
			m_current = m_tradoo.next();
			while ((m_current != NIL) && m_cmark->isMarked(m_current))
				m_current = m_tradoo.next();
		}
		else
		{
			if (m_orbx==VOLUME)
			{
				// if allocated we are in a local traversal of volume so we can mark only darts of volume
				if (m_allocated)
					m_dmark->markOrbit(MAP::ORBIT_IN_PARENT(m_orby), m_current);
				else
					m_dmark->markOrbit(m_orby, m_current); // here we need to mark all the darts
			}
			else
				m_dmark->markOrbit(m_orby, m_current);
			m_current = m_tradoo.next();
			while ((m_current != NIL) && m_dmark->isMarked(m_current))
				m_current = m_tradoo.next();
		}
	}
	return m_current ;
}



//*********************************************
// Traversor cellX to cellX adjacent by cell Y
//*********************************************


template <typename MAP>
Traversor3XXaY<MAP>::Traversor3XXaY(MAP& map, Dart dart, unsigned int orbX, unsigned int orbY, bool forceDartMarker, unsigned int thread):
	m_map(map)
{
	MarkerForTraversor<MAP> mk(map,orbX,forceDartMarker,thread);
	mk.mark(dart);

	Traversor3XY<MAP> traAdj(map, dart, orbX, orbY, forceDartMarker, thread);
	for (Dart d = traAdj.begin(); d!=traAdj.end(); d=traAdj.next())
	{
		Traversor3XY<MAP> traInci(map, d, orbY, orbX, mk, forceDartMarker, thread);
		for (Dart e = traInci.begin(); e!=traInci.end(); e=traInci.next())
			m_vecDarts.push_back(e);
	}
	m_vecDarts.push_back(NIL);
}

template <typename MAP>
Dart Traversor3XXaY<MAP>::begin()
{
	m_iter = m_vecDarts.begin();
	return *m_iter;
}

template <typename MAP>
Dart Traversor3XXaY<MAP>::end()
{
	return NIL;
}


template <typename MAP>
Dart Traversor3XXaY<MAP>::next()
{
	if (*m_iter != NIL)
		m_iter++;
	return *m_iter ;
}


} // namespace CGoGN
