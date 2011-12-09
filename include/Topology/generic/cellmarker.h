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
#include "Topology/generic/genericmap.h"
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
	friend class GenericMap ;

protected:
	GenericMap& m_map ;
	Mark m_mark ;
	unsigned int m_cell ;
	unsigned int m_thread ;
	bool releaseOnDestruct ;

public:
	/**
	 * constructor
	 * @param map the map on which we work
	 * @param cell the type of cell we want to mark VERTEX, EDGE,...
	 */
	CellMarkerGen(GenericMap& map, unsigned int cell, unsigned int thread = 0) : m_map(map), m_cell(cell), m_thread(thread), releaseOnDestruct(true)
	{
		if(!map.isOrbitEmbedded(cell))
			map.addEmbedding(cell) ;
		m_mark = m_map.getMarkerSet(m_cell, m_thread).getNewMark() ;
		m_map.cellMarkers.push_back(this) ;
	}

	virtual ~CellMarkerGen()
	{
		if(releaseOnDestruct)
		{
			m_map.getMarkerSet(m_cell, m_thread).releaseMark(m_mark) ;
			for(std::vector<CellMarkerGen*>::iterator it = m_map.cellMarkers.begin(); it != m_map.cellMarkers.end(); ++it)
			{
				if(*it == this)
				{
					*it = m_map.cellMarkers.back();
					m_map.cellMarkers.pop_back();
					return;
				}
			}
		}
	}

protected:
	// protected copy constructor to forbid its usage
	CellMarkerGen(const CellMarkerGen& cm) : m_map(cm.m_map)
	{}

	/**
	 * set if the mark has to be release on destruction or not
	 */
	void setReleaseOnDestruct(bool b) { releaseOnDestruct = b ; }

public:
	/**
	 * mark the cell of dart
	 */
	virtual void mark(Dart d)
	{
		assert(m_map.getMarkerSet(m_cell, m_thread).testMark(m_mark));
		assert(m_map.getMarkVector(m_cell, m_thread) != NULL);

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
		assert(m_map.getMarkerSet(m_cell, m_thread).testMark(m_mark));
		assert(m_map.getMarkVector(m_cell, m_thread) != NULL);

		unsigned int a = m_map.getEmbedding(m_cell, d) ;
		if (a == EMBNULL)
			a = m_map.embedNewCell(m_cell, d) ;

		m_map.getMarkVector(m_cell, m_thread)->operator[](a).unsetMark(m_mark) ;
	}

	/**
	 * test if cell of dart is marked
	 */
	virtual bool isMarked(Dart d) const
	{
		assert(m_map.getMarkerSet(m_cell, m_thread).testMark(m_mark));
		assert(m_map.getMarkVector(m_cell, m_thread) != NULL);

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
		assert(m_map.getMarkerSet(m_cell, m_thread).testMark(m_mark));
		assert(m_map.getMarkVector(m_cell, m_thread) != NULL);

		m_map.getMarkVector(m_cell, m_thread)->operator[](em).setMark(m_mark) ;
	}

	/**
	 * unmark the cell
	 */
	virtual void unmark(unsigned int em)
	{
		assert(m_map.getMarkerSet(m_cell, m_thread).testMark(m_mark));
		assert(m_map.getMarkVector(m_cell, m_thread) != NULL);

		m_map.getMarkVector(m_cell, m_thread)->operator[](em).unsetMark(m_mark) ;
	}

	/**
	 * test if cell is marked
	 */
	virtual bool isMarked(unsigned int em) const
	{
		assert(m_map.getMarkerSet(m_cell, m_thread).testMark(m_mark));
		assert(m_map.getMarkVector(m_cell, m_thread) != NULL);

		return m_map.getMarkVector(m_cell, m_thread)->operator[](em).testMark(m_mark) ;
	}

	/**
	 * mark all the cells
	 */
	virtual void markAll()
	{
		assert(m_map.getMarkerSet(m_cell, m_thread).testMark(m_mark));
		AttributeMultiVector<Mark>* mark_vect = m_map.getMarkVector(m_cell, m_thread);
		assert(mark_vect != NULL);

		AttributeContainer& cont = m_map.getAttributeContainer(m_cell) ;
		for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
			mark_vect->operator[](i).setMark(m_mark) ;
	}

	/**
	 * unmark all the cells
	 */
	virtual void unmarkAll() = 0 ;

	bool isAllUnmarked()
	{
		assert(m_map.getMarkerSet(m_cell, m_thread).testMark(m_mark));

		AttributeMultiVector<Mark>* mark_vect = m_map.getMarkVector(m_cell, m_thread);
		assert(mark_vect != NULL);

		AttributeContainer& cont = m_map.getAttributeContainer(m_cell) ;
		for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
			if(mark_vect->operator[](i).testMark(m_mark))
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
	CellMarker(GenericMap& map, unsigned int cell, unsigned int thread = 0) : CellMarkerGen(map, cell, thread)
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
		assert(m_map.getMarkerSet(m_cell, m_thread).testMark(m_mark));
		AttributeMultiVector<Mark>* mark_vect = m_map.getMarkVector(m_cell, m_thread);
		assert(mark_vect != NULL);

		AttributeContainer& cont = m_map.getAttributeContainer(m_cell) ;
		for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
			mark_vect->operator[](i).unsetMark(m_mark) ;
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
	CellMarkerStore(GenericMap& map, unsigned int cell, unsigned int thread = 0) : CellMarkerGen(map, cell, thread)
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
		assert(m_map.getMarkerSet(m_cell, m_thread).testMark(m_mark));
		AttributeMultiVector<Mark>* mark_vect = m_map.getMarkVector(m_cell, m_thread);
		assert(mark_vect != NULL);

		for (std::vector<unsigned int>::iterator it = m_markedCells.begin(); it != m_markedCells.end(); ++it)
			mark_vect->operator[](*it).unsetMark(m_mark) ;
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
	CellMarkerNoUnmark(GenericMap& map, unsigned int cell, unsigned int thread = 0) : CellMarkerGen(map, cell, thread)
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
		assert(m_map.getMarkerSet(m_cell, m_thread).testMark(m_mark));
		AttributeMultiVector<Mark>* mark_vect = m_map.getMarkVector(m_cell, m_thread);
		assert(mark_vect != NULL);

		AttributeContainer& cont = m_map.getAttributeContainer(m_cell) ;
		for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
			mark_vect->operator[](i).unsetMark(m_mark) ;
	}
};


/**
 * selector that say if a dart has its cell marked
 */
class SelectorCellMarked : public FunctorSelect
{
protected:
	const CellMarkerGen& m_cmarker ;
public:
	SelectorCellMarked(const CellMarkerGen& cm) : m_cmarker(cm) {}
	bool operator()(Dart d) const
	{
		if (m_cmarker.isMarked(d))
			return true ;
		return false ;
	}
	FunctorSelect* copy() const { return new SelectorCellMarked(m_cmarker);}
};

class SelectorCellUnmarked : public FunctorSelect
{
protected:
	const CellMarkerGen& m_cmarker ;
public:
	SelectorCellUnmarked(const CellMarkerGen& cm) : m_cmarker(cm) {}
	bool operator()(Dart d) const
	{
		if (!m_cmarker.isMarked(d))
			return true ;
		return false ;
	}
	FunctorSelect* copy() const { return new SelectorCellUnmarked(m_cmarker);}
};

// Functor version (needed for use with foreach_xxx)

class FunctorCellIsMarked : public FunctorType
{
protected:
	CellMarkerGen& m_marker;
public:
	FunctorCellIsMarked(CellMarkerGen& cm) : m_marker(cm) {}
	bool operator()(Dart d)
	{
		return m_marker.isMarked(d);
	}
};

class FunctorCellIsUnmarked : public FunctorType
{
protected:
	CellMarkerGen& m_marker;
public:
	FunctorCellIsUnmarked(CellMarkerGen& cm) : m_marker(cm) {}
	bool operator()(Dart d)
	{
		return !m_marker.isMarked(d);
	}
};

} // namespace CGoGN

#endif
