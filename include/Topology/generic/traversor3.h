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

#ifndef __TRAVERSOR3_H__
#define __TRAVERSOR3_H__

#include "Topology/generic/dart.h"

namespace CGoGN
{

/*******************************************************************************
					VERTEX CENTERED TRAVERSALS
*******************************************************************************/

// Traverse the edges incident to a given vertex
template <typename MAP>
class Traversor3VE
{
private:
	MAP& m ;
	Dart start ;
	Dart current ;

public:
	Traversor3VE(MAP& map, Dart dart) ;

	Dart begin() ;
	Dart end() ;
	Dart next() ;
} ;

// Traverse the faces incident to a given vertex
template <typename MAP>
class Traversor3VF
{
private:
	MAP& m ;
	Dart start ;
	Dart current ;

public:
	Traversor3VF(MAP& map, Dart dart) ;

	Dart begin() ;
	Dart end() ;
	Dart next() ;
} ;

// Traverse the volumes incident to a given vertex
template <typename MAP>
class Traversor3VW
{
private:
	MAP& m ;
	Dart start ;
	Dart current ;

public:
	Traversor3VW(MAP& map, Dart dart) ;

	Dart begin() ;
	Dart end() ;
	Dart next() ;
} ;

/*******************************************************************************
					EDGE CENTERED TRAVERSALS
*******************************************************************************/



/*******************************************************************************
					FACE CENTERED TRAVERSALS
*******************************************************************************/



/*******************************************************************************
					VOLUME CENTERED TRAVERSALS
*******************************************************************************/
// Traverse the vertices incident to a given volume
template <typename MAP, unsigned int ORBIT>
class Traversor3WX
{
private:
	MAP& m ;
	Dart start ;
	Dart current ;
	DartMarkerStore* dmark ;
	CellMarkerStore* cmark ;
	TraversorDartsOfOrbit<MAP> m_tradoo;

public:
	Traversor3WX(MAP& map, Dart dart, bool forceDartMarker = false, unsigned int thread = 0) ;
	~Traversor3WX();

	Dart begin() ;
	Dart end() ;
	Dart next() ;
} ;


template <typename MAP>
class Traversor3WV: public Traversor3WX<MAP,VERTEX>
{
public:
	Traversor3WV(MAP& m, Dart dart, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3WX<MAP,VERTEX>(m, dart, forceDartMarker, thread)	{}
};

template <typename MAP>
class Traversor3WE: public Traversor3WX<MAP,EDGE>
{
public:
	Traversor3WE(MAP& m, Dart dart, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3WX<MAP,EDGE>(m, dart, forceDartMarker, thread)	{}
};

template <typename MAP>
class Traversor3WF: public Traversor3WX<MAP,FACE>
{
public:
	Traversor3WF(MAP& m, Dart dart, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3WX<MAP,FACE>(m, dart, forceDartMarker, thread) {}
};


} // namespace CGoGN

#include "Topology/generic/traversor3.hpp"

#endif
