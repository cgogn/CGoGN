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

template <typename MAP, unsigned int ORBIT>
TraversorCell<MAP, ORBIT>::TraversorCell(const MAP& map, bool forceDartMarker, unsigned int thread) :
	m(map), dmark(NULL), cmark(NULL), quickTraversal(NULL), current(NIL), firstTraversal(true)
{
	dimension = map.dimension();

	if(forceDartMarker)
		dmark = new DartMarker<MAP>(map, thread) ;
	else
	{
		quickTraversal = map.template getQuickTraversal<ORBIT>() ;
		if(quickTraversal != NULL)
		{
			cont = &(map.template getAttributeContainer<ORBIT>()) ;

		}
		else
		{
			if(map.template isOrbitEmbedded<ORBIT>())
				cmark = new CellMarker<MAP, ORBIT>(map, thread) ;
			else
				dmark = new DartMarker<MAP>(map, thread) ;
		}
	}
}

template <typename MAP, unsigned int ORBIT>
TraversorCell<MAP, ORBIT>::~TraversorCell()
{
	if(dmark)
		delete dmark ;
	else if(cmark)
		delete cmark ;
}

template <typename MAP, unsigned int ORBIT>
Cell<ORBIT> TraversorCell<MAP, ORBIT>::begin()
{
	if(quickTraversal != NULL)
	{
		qCurrent = cont->begin() ;
		current.dart = (*quickTraversal)[qCurrent] ;
	}
	else
	{
		if(!firstTraversal)
		{
			if(dmark)
				dmark->unmarkAll() ;
			else
				cmark->unmarkAll() ;
		}

		current.dart = m.begin() ;
		while(current.dart != m.end() && (m.isBoundaryMarked(dimension, current.dart) ))
			m.next(current.dart) ;

		if(current.dart == m.end())
			current.dart = NIL ;
		else
		{
			if(dmark)
				dmark->template markOrbit<ORBIT>(current.dart) ;
			else
				cmark->mark(current) ;
		}

		firstTraversal = false ;
	}

	return current ;
}

template <typename MAP, unsigned int ORBIT>
Cell<ORBIT> TraversorCell<MAP, ORBIT>::end()
{
	return Cell<ORBIT>(NIL) ;
}

template <typename MAP, unsigned int ORBIT>
Cell<ORBIT> TraversorCell<MAP, ORBIT>::next()
{
	assert(current.dart != NIL);
//	if(current.dart != NIL)
//	{
	if(quickTraversal != NULL)
	{
		cont->next(qCurrent) ;
		if (qCurrent != cont->end())
			current.dart = (*quickTraversal)[qCurrent] ;
		else current.dart = NIL;
	}
	else
	{
		if(dmark)
		{
			bool ismarked = dmark->isMarked(current.dart) ;
			while(current.dart != NIL && (ismarked || m.isBoundaryMarked(dimension, current.dart)))
			{
				m.next(current.dart) ;
				if(current.dart == m.end())
					current.dart = NIL ;
				else
					ismarked = dmark->isMarked(current.dart) ;
			}
			if(current.dart != NIL)
				dmark->template markOrbit<ORBIT>(current.dart) ;
		}
		else
		{
			bool ismarked = cmark->isMarked(current) ;
			while(current.dart != NIL && (ismarked || m.isBoundaryMarked(dimension, current.dart) ))
			{
				m.next(current.dart) ;
				if(current.dart == m.end())
					current.dart = NIL ;
				else
					ismarked = cmark->isMarked(current) ;
			}
			if(current.dart != NIL)
				cmark->mark(current) ;
		}
	}
//	}
	return current ;
}

template <typename MAP, unsigned int ORBIT>
void TraversorCell<MAP, ORBIT>::skip(Cell<ORBIT> c)
{
	if(dmark)
		dmark->template markOrbit<ORBIT>(c.dart) ;
	else
		cmark->mark(c) ;
}

} // namespace CGoGN
