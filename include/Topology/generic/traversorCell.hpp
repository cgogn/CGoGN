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

namespace CGoGN
{

template <typename MAP>
TraversorCell<MAP>::TraversorCell(MAP& map, unsigned int orbit, const FunctorSelect& good, bool forceDartMarker, unsigned int thread) :
m(map), m_orbit(orbit), dmark(NULL), cmark(NULL), current(NIL), firstTraversal(true), m_good(good)
{
	if(!forceDartMarker && map.isOrbitEmbedded(m_orbit))
		cmark = new CellMarker(map, m_orbit, thread) ;
	else
		dmark = new DartMarker(map, thread) ;
}

template <typename MAP>
TraversorCell<MAP>::~TraversorCell()
{
	if(dmark)
		delete dmark ;
	else
		delete cmark ;
}

template <typename MAP>
Dart TraversorCell<MAP>::begin()
{
	if(!firstTraversal)
	{
		if(dmark)
			dmark->unmarkAll() ;
		else
			cmark->unmarkAll() ;
	}

	current = m.begin() ;
	while(current != m.end() && (m.isBoundaryMarked(current) || !m_good(current)))
		m.next(current) ;

	if(current == m.end())
		current = NIL ;
	else
	{
		if(dmark)
			dmark->markOrbit(m_orbit, current) ;
		else
			cmark->mark(current) ;
	}

	firstTraversal = false ;
	return current ;
}


template <typename MAP>
Dart TraversorCell<MAP>::end() { return NIL ; }


template <typename MAP>
Dart TraversorCell<MAP>::next()
{
	if(current != NIL)
	{
		bool ismarked ;
		if(dmark)
			ismarked = dmark->isMarked(current) ;
		else
			ismarked = cmark->isMarked(current) ;

		while(current != NIL && (ismarked || m.isBoundaryMarked(current) || !m_good(current)))
		{
			m.next(current) ;
			if(current == m.end())
				current = NIL ;
			else
			{
				if(dmark)
					ismarked = dmark->isMarked(current) ;
				else
					ismarked = cmark->isMarked(current) ;
			}
		}

		if(current != NIL)
		{
			if(dmark)
				dmark->markOrbit(m_orbit, current) ;
			else
				cmark->mark(current) ;
		}
	}
	return current ;
}

template <typename MAP>
void TraversorCell<MAP>::mark(Dart d)
{
	if(dmark)
		dmark->markOrbit(m_orbit, d) ;
	else
		cmark->mark(d) ;
}



template <typename MAP>
TraversorDartsOfOrbit<MAP>::TraversorDartsOfOrbit(MAP& map, unsigned int orbit, Dart d, unsigned int thread)
{
	m_vd.reserve(16);
	FunctorStoreNotBoundary<MAP> fs(map,m_vd);
	map.foreach_dart_of_orbit(orbit, d, fs, thread);
	m_vd.push_back(NIL);
}

template <typename MAP>
Dart TraversorDartsOfOrbit<MAP>::begin()
{
	m_current=m_vd.begin();
	return *m_current;
}

template <typename MAP>
Dart TraversorDartsOfOrbit<MAP>::end()
{
	return NIL;
}

template <typename MAP>
Dart TraversorDartsOfOrbit<MAP>::next()
{
	if (*m_current != NIL)
		m_current++;
	return *m_current;
}

}

