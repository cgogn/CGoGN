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

#ifndef __TRAVERSOR3_H__
#define __TRAVERSOR3_H__

#include "Topology/generic/dart.h"
#include "Topology/generic/traversorCell.h"

namespace CGoGN
{
/**
 * class Marker for Traversor usefull to combine
 * several TraversorXY
 */
template <typename MAP>
class MarkerForTraversor
{
private:
	MAP& m_map ;
	DartMarkerStore* m_dmark ;
	CellMarkerStore* m_cmark ;
	unsigned int m_orbit;
public:
	MarkerForTraversor(MAP& map, unsigned int orbit, bool forceDartMarker = false, unsigned int thread = 0) ;
	~MarkerForTraversor();
	DartMarkerStore* dmark();
	CellMarkerStore* cmark();
	void mark(Dart d);
	void unmark(Dart d);
	bool isMarked(Dart d);
} ;

/**
 * Generic class Traversor (do not use directly)
 * Traverse all Y incident to X
 */
template <typename MAP>
class Traversor3XY
{
private:
	MAP& m_map ;
	DartMarkerStore* m_dmark ;
	CellMarkerStore* m_cmark ;
	Dart m_current ;
	TraversorDartsOfOrbit<MAP> m_tradoo;
	unsigned int m_orbx;
	unsigned int m_orby;
	bool m_allocated;
	bool m_first;
public:
	Traversor3XY(MAP& map, Dart dart, unsigned int orbX, unsigned int orbY, bool forceDartMarker = false, unsigned int thread = 0) ;
	Traversor3XY(MAP& map, Dart dart, unsigned int orbX, unsigned int orbY, MarkerForTraversor<MAP>& tmo, bool forceDartMarker = false, unsigned int thread = 0) ;
	~Traversor3XY();
	Dart begin() ;
	Dart end() ;
	Dart next() ;
} ;


/**
 * Generic class Traversor (do not use directly)
 * Traverse all X adjacent to X by an Y
 */
template <typename MAP>
class Traversor3XXaY
{
private:
	MAP& m_map ;
	std::vector<Dart> m_vecDarts;
	std::vector<Dart>::iterator m_iter;
public:
	Traversor3XXaY(MAP& map, Dart dart, unsigned int orbX, unsigned int orbY, bool forceDartMarker = false, unsigned int thread = 0);

	Dart begin();

	Dart end();

	Dart next();
};




/**
 * Traverse vertices incident to volume
 */
template <typename MAP>
class Traversor3WV: public Traversor3XY<MAP>
{
public:
	Traversor3WV(MAP& m, Dart dart, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XY<MAP>(m, dart,VOLUME,VERTEX,forceDartMarker, thread)	{}
};

/**
 * Traverse edges incident to volume
 */
template <typename MAP>
class Traversor3WE: public Traversor3XY<MAP>
{
public:
	Traversor3WE(MAP& m, Dart dart, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XY<MAP>(m, dart,VOLUME,EDGE, forceDartMarker, thread)	{}
};

/**
 * Traverse faces incident to volume
 */
template <typename MAP>
class Traversor3WF: public Traversor3XY<MAP>
{
public:
	Traversor3WF(MAP& m, Dart dart, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XY<MAP>(m, dart,VOLUME,FACE, forceDartMarker, thread) {}
};


/**
 * Traverse vertices incident to face
 */
template <typename MAP>
class Traversor3FV: public Traversor3XY<MAP>
{
public:
	Traversor3FV(MAP& m, Dart dart, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XY<MAP>(m, dart,FACE,VERTEX, forceDartMarker, thread)	{}
};

/**
 * Traverse edges incident to face
 */
template <typename MAP>
class Traversor3FE: public Traversor3XY<MAP>
{
public:
	Traversor3FE(MAP& m, Dart dart, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XY<MAP>(m, dart,FACE,EDGE, forceDartMarker, thread)	{}
};

/**
 * Traverse volumes incident to face
 */
template <typename MAP>
class Traversor3FW: public Traversor3XY<MAP>
{
public:
	Traversor3FW(MAP& m, Dart dart, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XY<MAP>(m, dart,FACE,VOLUME, forceDartMarker, thread)	{}
};


/**
 * Traverse vertices incident to edge
 */
template <typename MAP>
class Traversor3EV: public Traversor3XY<MAP>
{
public:
	Traversor3EV(MAP& m, Dart dart, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XY<MAP>(m, dart,EDGE,VERTEX, forceDartMarker, thread)	{}
};

/**
 * Traverse faces incident to edge
 */
template <typename MAP>
class Traversor3EF: public Traversor3XY<MAP>
{
public:
	Traversor3EF(MAP& m, Dart dart, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XY<MAP>(m, dart,EDGE,FACE, forceDartMarker, thread)	{}
};

/**
 * Traverse volumes incident to edge
 */
template <typename MAP>
class Traversor3EW: public Traversor3XY<MAP>
{
public:
	Traversor3EW(MAP& m, Dart dart, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XY<MAP>(m, dart,EDGE,VOLUME, forceDartMarker, thread)	{}
};



/**
 * Traverse edges incident to vertex
 */
template <typename MAP>
class Traversor3VE: public Traversor3XY<MAP>
{
public:
	Traversor3VE(MAP& m, Dart dart, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XY<MAP>(m, dart,VERTEX,EDGE, forceDartMarker, thread)	{}
};

/**
 * Traverse faces incident to vertex
 */
template <typename MAP>
class Traversor3VF: public Traversor3XY<MAP>
{
public:
	Traversor3VF(MAP& m, Dart dart, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XY<MAP>(m, dart,VERTEX,FACE, forceDartMarker, thread)	{}
};

/**
 * Traverse volumes incident to vertex
 */
template <typename MAP>
class Traversor3VW: public Traversor3XY<MAP>
{
public:
	Traversor3VW(MAP& m, Dart dart, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XY<MAP>(m, dart,VERTEX,VOLUME, forceDartMarker, thread)	{}
};



/**
 * Traverse vertices adjacent to a vertex by an edge
 */
template <typename MAP>
class Traversor3VVaE: public Traversor3XXaY<MAP>
{
public:
	Traversor3VVaE(MAP& m, Dart d, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XXaY<MAP>(m, d,VERTEX,EDGE, forceDartMarker, thread)	{}
};

/**
 * Traverse vertices adjacent to a vertex by a face
 */
template <typename MAP>
class Traversor3VVaF: public Traversor3XXaY<MAP>
{
public:
	Traversor3VVaF(MAP& m, Dart d, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XXaY<MAP>(m, d,VERTEX,FACE, forceDartMarker, thread)	{}
};

/**
 * Traverse vertices adjacent to a vertex by a volume
 */
template <typename MAP>
class Traversor3VVaW: public Traversor3XXaY<MAP>
{
public:
	Traversor3VVaW(MAP& m, Dart d, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XXaY<MAP>(m, d,VERTEX,VOLUME, forceDartMarker, thread)	{}
};


/**
 * Traverse edges adjacent to an egde by a vertex
 */
template <typename MAP>
class Traversor3EEaV: public Traversor3XXaY<MAP>
{
public:
	Traversor3EEaV(MAP& m, Dart d, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XXaY<MAP>(m, d,EDGE,VERTEX, forceDartMarker, thread)	{}
};

/**
 * Traverse edges adjacent to an egde by a face
 */
template <typename MAP>
class Traversor3EEaF: public Traversor3XXaY<MAP>
{
public:
	Traversor3EEaF(MAP& m, Dart d, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XXaY<MAP>(m, d,EDGE,FACE, forceDartMarker, thread)	{}
};

/**
 * Traverse edges adjacent to an egde by a volume
 */
template <typename MAP>
class Traversor3EEaW: public Traversor3XXaY<MAP>
{
public:
	Traversor3EEaW(MAP& m, Dart d, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XXaY<MAP>(m, d,EDGE,VOLUME, forceDartMarker, thread)	{}
};


/**
 * Traverse faces adjacent to a face by a vertex
 */
template <typename MAP>
class Traversor3FFaV: public Traversor3XXaY<MAP>
{
public:
	Traversor3FFaV(MAP& m, Dart d, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XXaY<MAP>(m, d,FACE,VERTEX, forceDartMarker, thread)	{}
};

/**
 * Traverse faces adjacent to a face by an edge
 */
template <typename MAP>
class Traversor3FFaE: public Traversor3XXaY<MAP>
{
public:
	Traversor3FFaE(MAP& m, Dart d, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XXaY<MAP>(m, d,FACE,EDGE, forceDartMarker, thread)	{}
};

/**
 * Traverse faces adjacent to a face by a volume
 */
template <typename MAP>
class Traversor3FFaW: public Traversor3XXaY<MAP>
{
public:
	Traversor3FFaW(MAP& m, Dart d, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XXaY<MAP>(m, d,FACE,VOLUME, forceDartMarker, thread)	{}
};


/**
 * Traverse volumes adjacent to a volume by a vertex
 */
template <typename MAP>
class Traversor3WWaV: public Traversor3XXaY<MAP>
{
public:
	Traversor3WWaV(MAP& m, Dart d, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XXaY<MAP>(m, d,VOLUME,VERTEX, forceDartMarker, thread)	{}
};

/**
 * Traverse volumes adjacent to a volume by an edge
 */
template <typename MAP>
class Traversor3WWaE: public Traversor3XXaY<MAP>
{
public:
	Traversor3WWaE(MAP& m, Dart d, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XXaY<MAP>(m, d,VOLUME,EDGE, forceDartMarker, thread)	{}
};

/**
 * Traverse volumes adjacent to a volume by a face
 */
template <typename MAP>
class Traversor3WWaF: public Traversor3XXaY<MAP>
{
public:
	Traversor3WWaF(MAP& m, Dart d, bool forceDartMarker = false, unsigned int thread = 0) : Traversor3XXaY<MAP>(m, d,VOLUME,FACE, forceDartMarker, thread)	{}
};


} // namespace CGoGN

#include "Topology/generic/traversor3.hpp"

#endif
