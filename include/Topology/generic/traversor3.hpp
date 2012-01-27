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

/*******************************************************************************
					VERTEX CENTERED TRAVERSALS
*******************************************************************************/

// Traversor3VE

template <typename MAP>
Traversor3VE<MAP>::Traversor3VE(MAP& map, Dart dart) : m(map), start(dart)
{}

template <typename MAP>
Dart Traversor3VE<MAP>::begin()
{
	current = start ;
	return current ;
}

template <typename MAP>
Dart Traversor3VE<MAP>::end()
{
	return NIL ;
}

template <typename MAP>
Dart Traversor3VE<MAP>::next()
{
	if(current != NIL)
	{
		current = m.alpha1(current) ;
		if(current == start)
			current = NIL ;
	}
	return current ;
}


template <typename MAP, unsigned int ORBIT>
Traversor3WX<MAP,ORBIT>::Traversor3WX(MAP& map, Dart dart, bool forceDartMarker, unsigned int thread) :
	m(map), start(dart),dmark(NULL), cmark(NULL), m_tradoo(map,VOLUME,dart,thread)
{
	if(!forceDartMarker && map.isOrbitEmbedded(ORBIT))
		cmark = new CellMarkerStore(map, ORBIT, thread) ;
	else
		dmark = new DartMarkerStore(map, thread) ;
}

template <typename MAP, unsigned int ORBIT>
Traversor3WX<MAP,ORBIT>::~Traversor3WX()
{
	if (cmark)
		delete cmark;
	if (dmark)
		delete dmark;

}

template <typename MAP, unsigned int ORBIT>
Dart Traversor3WX<MAP,ORBIT>::begin()
{
	current = m_tradoo.begin() ;
	return current;
}

template <typename MAP, unsigned int ORBIT>
Dart Traversor3WX<MAP,ORBIT>::end()
{
	return NIL ;
}

template <typename MAP, unsigned int ORBIT>
Dart Traversor3WX<MAP,ORBIT>::next()
{
	if(current != NIL)
	{
		if (cmark)
		{
			cmark->mark(current);
			current = m_tradoo.next();
			while ((current != NIL) && cmark->isMarked(current))
				current = m_tradoo.next();
		}
		else
		{
			dmark->markOrbit(ORBIT, current);
			current = m_tradoo.next();
			while ((current != NIL) && dmark->isMarked(current))
				current = m_tradoo.next();
		}
	}
	return current ;
}



} // namespace CGoGN
