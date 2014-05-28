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

#ifndef __CELL_MARKER__
#define __CELL_MARKER__

#include "Topology/generic/marker.h"
#include "Topology/generic/genericmap.h"
#include "Topology/generic/functor.h"
#include "Algo/Topo/embedding.h"

#include "Utils/static_assert.h"

namespace CGoGN
{
/**
 * @brief The CellMarkerGen class
 * @warning CellMarkerGen is no polymorphic version of CellMarker
 */
class CellMarkerGen
{
	friend class GenericMap ;

protected:
	Mark m_mark ;
	AttributeMultiVector<Mark>* m_markVector ;
	unsigned int m_thread ;
	unsigned int m_cell ;

public:
	CellMarkerGen(unsigned int cell, unsigned int thread = 0) :
		m_thread(thread),
		m_cell(cell)
	{}

	virtual ~CellMarkerGen()
	{}

	unsigned int getThread() { return m_thread ; }
	unsigned int getCell() { return m_cell ; }

	void updateMarkVector(AttributeMultiVector<Mark>* amv) { m_markVector = amv ; }

protected:
	// protected copy constructor to forbid its usage
	CellMarkerGen(const CellMarkerGen& /*cm*/)
	{}


//	virtual void mark(Dart d) = 0 ;
//	virtual void unmark(Dart d) = 0 ;
//	virtual bool isMarked(Dart d) const = 0 ;
//	virtual void mark(unsigned int em) = 0 ;
//	virtual void unmark(unsigned int em) = 0 ;
//	virtual bool isMarked(unsigned int em) const = 0 ;
//	virtual void markAll() = 0 ;
//	virtual void unmarkAll() = 0 ;
//	virtual bool isAllUnmarked() = 0 ;
};

/**
 * generic class that allows the marking of cells
 * \warning no default constructor
 */
template <typename MAP, unsigned int CELL>
class CellMarkerBase : public CellMarkerGen
{
protected:
	MAP& m_map ;

public:
	/**
	 * constructor
	 * @param map the map on which we work
	 */
	CellMarkerBase(MAP& map, unsigned int thread = 0) :
		CellMarkerGen(CELL, thread),
		m_map(map)
	{
		if(!m_map.template isOrbitEmbedded<CELL>())
			m_map.template addEmbedding<CELL>() ;
		m_mark = m_map.template getMarkerSet<CELL>(m_thread).getNewMark() ;
		m_markVector = m_map.template getMarkVector<CELL>(m_thread) ;
	}

	CellMarkerBase(const MAP& map, unsigned int thread = 0) :
		CellMarkerGen(CELL, thread),
		m_map(const_cast<MAP&>(map))
	{
		if(!m_map.template isOrbitEmbedded<CELL>())
			m_map.template addEmbedding<CELL>() ;
		m_mark = m_map.template getMarkerSet<CELL>(m_thread).getNewMark() ;
		m_markVector = m_map.template getMarkVector<CELL>(m_thread) ;
	}

	virtual ~CellMarkerBase()
	{
		if (GenericMap::alive(&m_map))
			m_map.template getMarkerSet<CELL>(m_thread).releaseMark(m_mark) ;
	}

protected:
	// protected copy constructor to forbid its usage
	CellMarkerBase(const CellMarkerBase<MAP, CELL>& cm) :
		m_map(cm.m_map),
		CellMarkerGen(CELL)
	{}

public:
	/**
	 * mark the cell of dart
	 */
	inline void mark(Cell<CELL> c)
	{
		assert(m_map.template getMarkerSet<CELL>(m_thread).testMark(m_mark));
		assert(m_markVector != NULL);

		unsigned int a = m_map.getEmbedding(c) ;

		if (a == EMBNULL)
			a = Algo::Topo::setOrbitEmbeddingOnNewCell(m_map, c) ;

		(*m_markVector)[a].setMark(m_mark) ;
	}

	/**
	 * unmark the cell of dart
	 */
	inline void unmark(Cell<CELL> c)
	{
		assert(m_map.template getMarkerSet<CELL>(m_thread).testMark(m_mark));
		assert(m_markVector != NULL);

		unsigned int a = m_map.getEmbedding(c) ;

		if (a == EMBNULL)
			a = Algo::Topo::setOrbitEmbeddingOnNewCell(m_map, c) ;

		(*m_markVector)[a].unsetMark(m_mark) ;
	}

	/**
	 * test if cell of dart is marked
	 */
	inline bool isMarked(Cell<CELL> c) const
	{
		assert(m_map.template getMarkerSet<CELL>(m_thread).testMark(m_mark));
		assert(m_markVector != NULL);

		unsigned int a = m_map.getEmbedding(c) ;

		if (a == EMBNULL)
			return false ;

		return (*m_markVector)[a].testMark(m_mark) ;
	}

	/**
	 * mark the cell
	 */
	inline void mark(unsigned int em)
	{
		assert(m_map.template getMarkerSet<CELL>(m_thread).testMark(m_mark));
		assert(m_markVector != NULL);

		(*m_markVector)[em].setMark(m_mark) ;
	}

	/**
	 * unmark the cell
	 */
	inline void unmark(unsigned int em)
	{
		assert(m_map.template getMarkerSet<CELL>(m_thread).testMark(m_mark));
		assert(m_markVector != NULL);

		(*m_markVector)[em].unsetMark(m_mark) ;
	}

	/**
	 * test if cell is marked
	 */
	inline bool isMarked(unsigned int em) const
	{
		assert(m_map.template getMarkerSet<CELL>(m_thread).testMark(m_mark));
		assert(m_markVector != NULL);

		if (em == EMBNULL)
			return false ;
		return (*m_markVector)[em].testMark(m_mark) ;
	}

	/**
	 * mark all the cells
	 */
	inline void markAll()
	{
		assert(m_map.template getMarkerSet<CELL>(m_thread).testMark(m_mark));
		assert(m_markVector != NULL);

		AttributeContainer& cont = m_map.template getAttributeContainer<CELL>() ;
		for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
			(*m_markVector)[i].setMark(m_mark) ;
	}

	inline bool isAllUnmarked()
	{
		assert(m_map.template getMarkerSet<CELL>(m_thread).testMark(m_mark));
		assert(m_markVector != NULL);

		AttributeContainer& cont = m_map.template getAttributeContainer<CELL>() ;
		for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
			if((*m_markVector)[i].testMark(m_mark))
				return false ;
		return true ;
	}
};

/**
 * class that allows the marking of cells
 * \warning no default constructor
 */
template <typename MAP, unsigned int CELL>
class CellMarker : public CellMarkerBase<MAP, CELL>
{
public:
	CellMarker(MAP& map, unsigned int thread = 0) : CellMarkerBase<MAP, CELL>(map, thread)
	{}

	CellMarker(const MAP& map, unsigned int thread = 0) :
		CellMarkerBase<MAP, CELL>(map, thread)
	{}

	virtual ~CellMarker()
	{
		unmarkAll() ;
	}

protected:
	CellMarker(const CellMarker& cm) :
		CellMarkerBase<MAP, CELL>(cm)
	{}

public:
	inline void unmarkAll()
	{
		assert(this->m_map.template getMarkerSet<CELL>(this->m_thread).testMark(this->m_mark));
		assert(this->m_markVector != NULL);

		AttributeContainer& cont = this->m_map.template getAttributeContainer<CELL>() ;
		for (unsigned int i = cont.realBegin(); i != cont.realEnd(); cont.realNext(i))
			this->m_markVector->operator[](i).unsetMark(this->m_mark) ;
	}
};

/**
 * class that allows the marking of cells
 * the marked cells are stored to optimize the unmarking task at destruction
 * \warning no default constructor
 */
template <typename MAP, unsigned int CELL>
class CellMarkerStore: public CellMarkerBase<MAP, CELL>
{
protected:
	std::vector<unsigned int> m_markedCells ;

public:
	CellMarkerStore(MAP& map, unsigned int thread = 0) :
		CellMarkerBase<MAP, CELL>(map, thread)
	{
		m_markedCells.reserve(128);
	}

	CellMarkerStore(const MAP& map, unsigned int thread = 0) :
		CellMarkerBase<MAP, CELL>(map, thread)
	{
		m_markedCells.reserve(128);
	}

	virtual ~CellMarkerStore()
	{
		unmarkAll() ;
//		assert(isAllUnmarked);
//		CGoGN_ASSERT(this->isAllUnmarked())
	}

protected:
	CellMarkerStore(const CellMarkerStore& cm) :
		CellMarkerBase<MAP, CELL>(cm)
	{}

public:
	inline void mark(Cell<CELL> d)
	{
		CellMarkerBase<MAP, CELL>::mark(d) ;
		m_markedCells.push_back(this->m_map.template getEmbedding<CELL>(d)) ;
	}

	inline void mark(unsigned int em)
	{
		CellMarkerBase<MAP, CELL>::mark(em) ;
		m_markedCells.push_back(em) ;
	}

	inline void unmarkAll()
	{
		assert(this->m_map.template getMarkerSet<CELL>(this->m_thread).testMark(this->m_mark));
		assert(this->m_markVector != NULL);

		for (std::vector<unsigned int>::iterator it = m_markedCells.begin(); it != m_markedCells.end(); ++it)
			this->m_markVector->operator[](*it).unsetMark(this->m_mark) ;
	}
};

/**
 * class that allows the marking of Darts
 * the marked Darts are stored to optimize the unmarking task at destruction
 * \warning no default constructor
 */
template <typename MAP, unsigned int CELL>
class CellMarkerMemo: public CellMarkerBase<MAP, CELL>
{
protected:
	std::vector<Dart> m_markedDarts ;

public:
	CellMarkerMemo(MAP& map, unsigned int thread = 0) :
		CellMarkerBase<MAP, CELL>(map, thread)
	{
		m_markedDarts.reserve(128);
	}

	CellMarkerMemo(const MAP& map, unsigned int thread = 0) :
		CellMarkerBase<MAP, CELL>(map, thread)
	{
		m_markedDarts.reserve(128);
	}

	virtual ~CellMarkerMemo()
	{
		unmarkAll() ;
//		assert(isAllUnmarked);
//		CGoGN_ASSERT(this->isAllUnmarked())
	}

protected:
	CellMarkerMemo(const CellMarkerMemo& cm) :
		CellMarkerBase<MAP, CELL>(cm)
	{}

public:
	inline void mark(Cell<CELL> c)
	{
		if(!this->isMarked(c))
		{
			CellMarkerBase<MAP, CELL>::mark(c) ;
			m_markedDarts.push_back(c.dart) ;
		}
	}

	inline void unmarkAll()
	{
		assert(this->m_map.template getMarkerSet<CELL>(this->m_thread).testMark(this->m_mark));
		assert(this->m_markVector != NULL);
		for (std::vector<Dart>::iterator it = m_markedDarts.begin(); it != m_markedDarts.end(); ++it)
		{
			this->unmark(*it) ;
		}
		m_markedDarts.clear();

	}

	inline const std::vector<Dart>& get_markedCells()
	{
		return m_markedDarts;
	}
};

/**
 * class that allows the marking of cells
 * the markers are not unmarked at destruction
 * \warning no default constructor
 */
template <typename MAP, unsigned int CELL>
class CellMarkerNoUnmark: public CellMarkerBase<MAP, CELL>
{
public:
	CellMarkerNoUnmark(MAP& map, unsigned int thread = 0) :
		CellMarkerBase<MAP, CELL>(map, thread)
	{}

	CellMarkerNoUnmark(const MAP& map, unsigned int thread = 0) :
		CellMarkerBase<MAP, CELL>(map, thread)
	{}

	virtual ~CellMarkerNoUnmark()
	{
//		assert(isAllUnmarked()) ;
//		CGoGN_ASSERT(this->isAllUnmarked())
	}

protected:
	CellMarkerNoUnmark(const CellMarkerNoUnmark& cm) :
		CellMarkerBase<MAP, CELL>(cm)
	{}

public:
	inline void unmarkAll()
	{
		assert(this->m_map.template getMarkerSet<CELL>(this->m_thread).testMark(this->m_mark));
		assert(this->m_markVector != NULL);

		AttributeContainer& cont = this->m_map.template getAttributeContainer<CELL>() ;
		for (unsigned int i = cont.realBegin(); i != cont.realEnd(); cont.realNext(i))
			this->m_markVector->operator[](i).unsetMark(this->m_mark) ;
	}
};

// Selector and count functors testing for marker existence
/********************************************************/

/**
 * selector that say if a dart has its cell marked
 */
template <typename MAP, unsigned int CELL>
class SelectorCellMarked : public FunctorSelect
{
protected:
	const CellMarkerBase<MAP, CELL>& m_cmarker ;

public:
	SelectorCellMarked(const CellMarkerBase<MAP, CELL>& cm) :
		m_cmarker(cm)
	{}

	inline bool operator()(Cell<CELL> d) const
	{
		if (m_cmarker.isMarked(d))
			return true ;
		return false ;
	}

	inline FunctorSelect* copy() const { return new SelectorCellMarked(m_cmarker); }
};

template <typename MAP, unsigned int CELL>
class SelectorCellUnmarked : public FunctorSelect
{
protected:
	const CellMarkerBase<MAP, CELL>& m_cmarker ;

public:
	SelectorCellUnmarked(const CellMarkerBase<MAP, CELL>& cm) :
		m_cmarker(cm)
	{}

	inline bool operator()(Cell<CELL> d) const
	{
		if (!m_cmarker.isMarked(d))
			return true ;
		return false ;
	}

	inline FunctorSelect* copy() const
	{
		return new SelectorCellUnmarked(m_cmarker);
	}
};

// Functor version (needed for use with foreach_xxx)

template <typename MAP, unsigned int CELL>
class FunctorCellIsMarked : public FunctorType
{
protected:
	CellMarkerBase<MAP, CELL>& m_marker;

public:
	FunctorCellIsMarked(CellMarkerBase<MAP, CELL>& cm) :
		m_marker(cm)
	{}

	inline bool operator()(Cell<CELL> d)
	{
		return m_marker.isMarked(d);
	}
};

template <typename MAP, unsigned int CELL>
class FunctorCellIsUnmarked : public FunctorType
{
protected:
	CellMarkerBase<MAP, CELL>& m_marker;
public:
	FunctorCellIsUnmarked(CellMarkerBase<MAP, CELL>& cm) :
		m_marker(cm)
	{}

	inline bool operator()(Cell<CELL> d)
	{
		return !m_marker.isMarked(d);
	}
};

} // namespace CGoGN

#endif
