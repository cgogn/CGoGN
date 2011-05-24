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

#ifndef __CELL_MARKER__
#define __CELL_MARKER__

#include "Topology/generic/marker.h"
#include "Topology/generic/attribmap.h"

#include "Topology/generic/functor.h"

#define VERTEX_CELL	VERTEX_ORBIT
#define EDGE_CELL	EDGE_ORBIT
#define ORIENTED_FACE_CELL	ORIENTED_FACE_ORBIT
#define FACE_CELL	FACE_ORBIT
#define VOLUME_CELL	VOLUME_ORBIT
#define DART_CELL	DART_ORBIT	// ??

namespace CGoGN
{

/**
 * class that allows the marking of cells
 * \warning no default constructor
 */
class CellMarker
{
protected:
	Marker m_marker;
	AttribMap& m_map;
	unsigned int m_thread;

public:
	/**
	 * constructor
	 * @param map the map on which we work (not stored use to get table of markers and new marker.
	 * @param cell the type of cell we want to mark VERTEX_CELL, EDGE_CELL,...
	 * \pre the cell is embedded in the map
	 */

	CellMarker(AttribMap& map, unsigned int cell): m_map(map), m_thread(0)
	{
		if(!map.isOrbitEmbedded(cell))
			map.addEmbedding(cell) ;
		m_marker = map.getNewMarker(cell);
	}

	CellMarker(AttribMap& map, unsigned int cell, unsigned int thread): m_map(map), m_thread(thread)
	{
		m_marker = map.getNewMarker(cell,thread);
	}

	virtual ~CellMarker()
	{
		unmarkAll();
		m_map.releaseMarker(m_marker,m_thread);
	}

protected:
	// protected copy constructor to forbid its usage
	CellMarker(const CellMarker& cm) : m_map(cm.m_map)
	{}

public:
	/**
	 * mark the cell of dart
	 */
	virtual void mark(Dart d)
	{
		unsigned int cell = m_marker.getCell() ;
		unsigned int a = m_map.getEmbedding(cell, d);
		if (a == EMBNULL)
			a = m_map.embedNewCell(cell, d);

		m_map.getMarkerVector(cell,m_thread)->operator[](a).setMark(m_marker);
	}

	/**
	 * unmark the cell of dart
	 */
	virtual void unmark(Dart d)
	{
		unsigned int cell = m_marker.getCell() ;
		unsigned int a = m_map.getEmbedding(cell, d);
		if (a == EMBNULL)
			a = m_map.embedNewCell(cell, d);

		m_map.getMarkerVector(cell,m_thread)->operator[](a).unsetMark(m_marker);
	}

	/**
	 * test if cell of dart is marked
	 */
	virtual bool isMarked(Dart d)
	{
		unsigned int cell = m_marker.getCell() ;
		unsigned int a = m_map.getEmbedding(cell, d);
		if (a == EMBNULL)
			return false;

		return m_map.getMarkerVector(cell,m_thread)->operator[](a).testMark(m_marker);
	}

	/**
	 * mark the cell
	 */
	virtual void mark(unsigned int em)
	{
		m_map.getMarkerVector(m_marker.getCell(),m_thread)->operator[](em).setMark(m_marker);
	}

	/**
	 * unmark the cell
	 */
	virtual void unmark(unsigned int em)
	{
		m_map.getMarkerVector(m_marker.getCell(),m_thread)->operator[](em).unsetMark(m_marker);
	}

	/**
	 * test if cell is marked
	 */
	virtual bool isMarked(unsigned int em)
	{
		return m_map.getMarkerVector(m_marker.getCell(),m_thread)->operator[](em).testMark(m_marker);
	}

	virtual void markAll()
	{
		unsigned int cell = m_marker.getCell() ;
		AttributeContainer& cont = m_map.getAttributeContainer(cell) ;
		for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
			m_map.getMarkerVector(cell,m_thread)->operator[](i).setMark(m_marker);
	}

	virtual void unmarkAll()
	{
		unsigned int cell = m_marker.getCell() ;
		AttributeContainer& cont = m_map.getAttributeContainer(cell) ;
		for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
			m_map.getMarkerVector(cell,m_thread)->operator[](i).unsetMark(m_marker);
	}
};

/**
 * class that allows the marking of cells
 * the marked cells are stored to optimize the unmarking task at destruction
 * \warning no default constructor
 */
class CellMarkerStore: public CellMarker
{
protected:
	std::vector<unsigned int> m_markedCells ;

public:
	/**
	 * constructor
	 * @param map the map on which we work (not stored use to get table of markers and new marker.
	 * @param cell the type of cell we want to mark VERTEX_CELL, EDGE_CELL,...
	 * \pre the cell is embedded in the map
	 */
	CellMarkerStore(AttribMap& map, unsigned int cell): CellMarker(map, cell)
	{}

	CellMarkerStore(AttribMap& map, unsigned int cell, unsigned int thread): CellMarker(map, cell,thread)
	{}

protected:
	// protected copy constructor to forbid its usage
	CellMarkerStore(const CellMarkerStore& cm) : CellMarker(cm)
	{}


public:
	/**
	 * mark the cell of dart
	 */
	void mark(Dart d)
	{
		CellMarker::mark(d) ;
		m_markedCells.push_back(m_map.getEmbedding(m_marker.getCell(), d)) ;
	}

	/**
	 * mark the cell
	 */
	void mark(unsigned int em)
	{
		CellMarker::mark(em) ;
		m_markedCells.push_back(em);
	}

	void unmarkAll()
	{
		unsigned int cell = m_marker.getCell() ;
		for (std::vector<unsigned int>::iterator it = m_markedCells.begin(); it != m_markedCells.end(); ++it)
			m_map.getMarkerVector(cell,m_thread)->operator[](*it).unsetMark(m_marker);
	}
};

/**
 * class that allows the marking of cells
 * the markers are not unmarked at destruction
 * \warning no default constructor
 */
class CellMarkerNoUnmark: public CellMarker
{
public:
	/**
	 * constructor
	 * @param map the map on which we work (not stored use to get table of markers and new marker.
	 * @param cell the type of cell we want to mark VERTEX_CELL, EDGE_CELL,...
	 * \pre the cell is embedded in the map
	 */
	CellMarkerNoUnmark(AttribMap& map, unsigned int cell): CellMarker(map, cell)
	{}


	CellMarkerNoUnmark(AttribMap& map, unsigned int cell, unsigned int thread): CellMarker(map, cell,thread)
	{}

	~CellMarkerNoUnmark()
	{
		m_map.releaseMarker(m_marker,m_thread);
	}

protected:
	// protected copy constructor to forbid its usage
	CellMarkerNoUnmark(const CellMarkerNoUnmark& cm) : CellMarker(cm)
	{}
};


/**
 * selector that say if a dart has it cell marked
 */
class SelectorCellMarked: public FunctorSelect
{
protected:
	CellMarker& m_cmarker;
public:
	SelectorCellMarked(CellMarker& cm): m_cmarker(cm) {}
	bool operator()(Dart d) const
	{
		if (m_cmarker.isMarked(d))
			return true;
		return false;
	}
};



} // namespace CGoGN

#endif
