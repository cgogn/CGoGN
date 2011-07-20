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

#ifndef __CELL_MARKER__
#define __CELL_MARKER__

#include "Topology/generic/marker.h"
#include "Topology/generic/attribmap.h"

#include "Topology/generic/functor.h"
#include "Utils/static_assert.h"

namespace CGoGN
{

/**
 * generic class that allows the marking of cells
 * \warning no default constructor
 */
class CellMarkerGen
{
protected:
	Mark m_mark ;
	AttribMap& m_map ;
	unsigned int m_cell ;
	unsigned int m_thread ;

public:
	/**
	 * constructor
	 * @param map the map on which we work (not stored use to get table of markers and new marker.
	 * @param cell the type of cell we want to mark VERTEX, EDGE,...
	 */
	CellMarkerGen(AttribMap& map, unsigned int cell, unsigned int thread = 0) : m_map(map), m_cell(cell), m_thread(thread)
	{
		if(!map.isOrbitEmbedded(cell))
			map.addEmbedding(cell) ;
		m_mark = map.getNewMark(cell, thread) ;
	}

	virtual ~CellMarkerGen()
	{
		m_map.releaseMark(m_mark, m_cell, m_thread);
	}

protected:
	// protected copy constructor to forbid its usage
	CellMarkerGen(const CellMarkerGen& cm) : m_map(cm.m_map)
	{}

public:
	/**
	 * mark the cell of dart
	 */
	virtual void mark(Dart d)
	{
		unsigned int a = m_map.getEmbedding(m_cell, d) ;
		if (a == EMBNULL)
			a = m_map.embedNewCell(m_cell, d) ;

		m_map.getMarkVector(m_cell, m_thread)->operator[](a).setMark(m_mark) ;
	}

	/**
	 * unmark the cell of dart
	 */
	virtual void unmark(Dart d)
	{
		unsigned int a = m_map.getEmbedding(m_cell, d) ;
		if (a == EMBNULL)
			a = m_map.embedNewCell(m_cell, d) ;

		m_map.getMarkVector(m_cell, m_thread)->operator[](a).unsetMark(m_mark) ;
	}

	/**
	 * test if cell of dart is marked
	 */
	virtual bool isMarked(Dart d)
	{
		unsigned int a = m_map.getEmbedding(m_cell, d) ;
		if (a == EMBNULL)
			return false ;

		return m_map.getMarkVector(m_cell, m_thread)->operator[](a).testMark(m_mark) ;
	}

	/**
	 * mark the cell
	 */
	virtual void mark(unsigned int em)
	{
		m_map.getMarkVector(m_cell, m_thread)->operator[](em).setMark(m_mark) ;
	}

	/**
	 * unmark the cell
	 */
	virtual void unmark(unsigned int em)
	{
		m_map.getMarkVector(m_cell, m_thread)->operator[](em).unsetMark(m_mark) ;
	}

	/**
	 * test if cell is marked
	 */
	virtual bool isMarked(unsigned int em)
	{
		return m_map.getMarkVector(m_cell, m_thread)->operator[](em).testMark(m_mark) ;
	}

	/**
	 * mark all the cells
	 */
	virtual void markAll()
	{
		AttributeContainer& cont = m_map.getAttributeContainer(m_cell) ;
		for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
			m_map.getMarkVector(m_cell, m_thread)->operator[](i).setMark(m_mark) ;
	}

	/**
	 * unmark all the cells
	 */
	virtual void unmarkAll() = 0 ;

	bool isAllUnmarked()
	{
		AttributeContainer& cont = m_map.getAttributeContainer(m_cell) ;
		for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
			if(m_map.getMarkVector(m_cell, m_thread)->operator[](i).testMark(m_mark))
				return false ;
		return true ;
	}
};

/**
 * class that allows the marking of cells
 * \warning no default constructor
 */
class CellMarker : public CellMarkerGen
{
public:
	CellMarker(AttribMap& map, unsigned int cell, unsigned int thread = 0) : CellMarkerGen(map, cell, thread)
	{}

	virtual ~CellMarker()
	{
		unmarkAll() ;
	}

protected:
	CellMarker(const CellMarker& cm) : CellMarkerGen(cm)
	{}

public:
	virtual void unmarkAll()
	{
		AttributeContainer& cont = m_map.getAttributeContainer(m_cell) ;
		for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
			m_map.getMarkVector(m_cell, m_thread)->operator[](i).unsetMark(m_mark) ;
	}
};

/**
 * class that allows the marking of cells
 * the marked cells are stored to optimize the unmarking task at destruction
 * \warning no default constructor
 */
class CellMarkerStore: public CellMarkerGen
{
protected:
	std::vector<unsigned int> m_markedCells ;

public:
	CellMarkerStore(AttribMap& map, unsigned int cell, unsigned int thread = 0) : CellMarkerGen(map, cell, thread)
	{}


	virtual ~CellMarkerStore()
	{
		unmarkAll() ;
//		assert(isAllUnmarked);
		CGoGN_ASSERT(isAllUnmarked())
	}

protected:
	CellMarkerStore(const CellMarkerStore& cm) : CellMarkerGen(cm)
	{}

public:
	void mark(Dart d)
	{
		CellMarkerGen::mark(d) ;
		m_markedCells.push_back(m_map.getEmbedding(m_cell, d)) ;
	}

	void mark(unsigned int em)
	{
		CellMarkerGen::mark(em) ;
		m_markedCells.push_back(em) ;
	}

	void unmarkAll()
	{
		for (std::vector<unsigned int>::iterator it = m_markedCells.begin(); it != m_markedCells.end(); ++it)
			m_map.getMarkVector(m_cell, m_thread)->operator[](*it).unsetMark(m_mark) ;
	}
};

/**
 * class that allows the marking of cells
 * the markers are not unmarked at destruction
 * \warning no default constructor
 */
class CellMarkerNoUnmark: public CellMarkerGen
{
public:
	CellMarkerNoUnmark(AttribMap& map, unsigned int cell, unsigned int thread = 0) : CellMarkerGen(map, cell, thread)
	{}

	virtual ~CellMarkerNoUnmark()
	{
//		assert(isAllUnmarked()) ;
		CGoGN_ASSERT(isAllUnmarked())
	}

protected:
	CellMarkerNoUnmark(const CellMarkerNoUnmark& cm) : CellMarkerGen(cm)
	{}

public:
	void unmarkAll()
	{
		AttributeContainer& cont = m_map.getAttributeContainer(m_cell) ;
		for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
			m_map.getMarkVector(m_cell, m_thread)->operator[](i).unsetMark(m_mark) ;
	}
};


/**
 * selector that say if a dart has its cell marked
 */
class SelectorCellMarked : public FunctorSelect
{
protected:
	CellMarkerGen& m_cmarker ;
public:
	SelectorCellMarked(CellMarkerGen& cm) : m_cmarker(cm) {}
	bool operator()(Dart d) const
	{
		if (m_cmarker.isMarked(d))
			return true ;
		return false ;
	}
};

class SelectorCellUnmarked : public FunctorSelect
{
protected:
	CellMarkerGen& m_cmarker ;
public:
	SelectorCellUnmarked(CellMarkerGen& cm) : m_cmarker(cm) {}
	bool operator()(Dart d) const
	{
		if (!m_cmarker.isMarked(d))
			return true ;
		return false ;
	}
};

} // namespace CGoGN

#endif
