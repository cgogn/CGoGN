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

#ifndef __DART_MARKER__
#define __DART_MARKER__

#include "Topology/generic/marker.h"
#include "Topology/generic/genericmap.h"
#include "Topology/generic/functor.h"

namespace CGoGN
{

// Marker Functors
/********************************************************/

template <typename MAP>
class FunctorMarker : public FunctorMap<MAP>
{
protected:
	Marker m_marker;
	AttributeMultiVector<Mark>* m_markerTable;
public:
	FunctorMarker(MAP& map, Marker m, AttributeMultiVector<Mark>* mTable) : FunctorMap<MAP>(map), m_marker(m), m_markerTable(mTable) {}
	Marker getMarker() { return m_marker; }
};

template <typename MAP>
class FunctorMark: public FunctorMarker<MAP>
{
public:
	FunctorMark(MAP& map, Marker m, AttributeMultiVector<Mark>* mTable) : FunctorMarker<MAP>(map, m, mTable) {}
	bool operator()(Dart d)
	{
		this->m_markerTable->operator[](d.index).setMark(this->m_marker);
		return false;
	}
};

template <typename MAP>
class FunctorUnmark: public FunctorMarker<MAP>
{
public:
	FunctorUnmark(MAP& map, Marker m, AttributeMultiVector<Mark>* mTable) : FunctorMarker<MAP>(map, m, mTable) {}
	bool operator()(Dart d)
	{
		this->m_markerTable->operator[](d.index).unsetMark(this->m_marker);
		return false;
	}
};


/**
 * class that allows the marking of darts
 * \warning no default constructor
 */
class DartMarker
{
protected:
	Marker m_marker;
	GenericMap& m_map;
	unsigned int m_thread;
public:
	/**
	 * constructor
	 * @param map the map on which we work
	 */
	DartMarker(GenericMap& map): m_map(map), m_thread(0)
	{
		m_marker = map.getNewMarker(DART_ORBIT,0);
	}

	DartMarker(GenericMap& map, unsigned int thread): m_map(map), m_thread(thread)
	{
		m_marker = map.getNewMarker(DART_ORBIT,thread);
	}

	virtual ~DartMarker()
	{
		unmarkAll();
		m_map.releaseMarker(m_marker,m_thread);
	}

protected:
	// protected copy constructor to forbid its usage
	DartMarker(const DartMarker& dm) : m_map(dm.m_map)
	{}


public:
	/**
	 * mark the cell of dart
	 */
	virtual void mark(Dart d)
	{
		m_map.getMarkerVector(DART_ORBIT,m_thread)->operator[](d.index).setMark(m_marker);
	}

	/**
	 * unmark the cell of dart
	 */
	virtual void unmark(Dart d)
	{
		m_map.getMarkerVector(DART_ORBIT,m_thread)->operator[](d.index).unsetMark(m_marker);
	}

	/**
	 * test if cell of dart is marked
	 */
	virtual bool isMarked(Dart d)
	{
		return m_map.getMarkerVector(DART_ORBIT,m_thread)->operator[](d.index).testMark(m_marker);
	}

	/**
	 * mark the cell
	 */
	virtual void mark(unsigned int em)
	{
		m_map.getMarkerVector(DART_ORBIT,m_thread)->operator[](em).setMark(m_marker);
	}

	/**
	 * unmark the cell
	 */
	virtual void unmark(unsigned int em)
	{
		m_map.getMarkerVector(DART_ORBIT,m_thread)->operator[](em).unsetMark(m_marker);
	}

	/**
	 * test if cell is marked
	 */
	virtual bool isMarked(unsigned int em)
	{
		return m_map.getMarkerVector(DART_ORBIT,m_thread)->operator[](em).testMark(m_marker);
	}

	virtual void markAll()
	{
		AttributeContainer& cont = m_map.getAttributeContainer(DART_ORBIT) ;
		for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
			m_map.getMarkerVector(DART_ORBIT,m_thread)->operator[](i).setMark(m_marker);
	}

	virtual void unmarkAll()
	{
		AttributeContainer& cont = m_map.getAttributeContainer(DART_ORBIT) ;
		for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
			m_map.getMarkerVector(DART_ORBIT,m_thread)->operator[](i).unsetMark(m_marker);
	}

	virtual void markOrbit(unsigned int orbit, Dart d)
	{
		FunctorMark<GenericMap> fm(m_map, m_marker, m_map.getMarkerVector(DART_ORBIT,m_thread)) ;
		m_map.foreach_dart_of_orbit(orbit, d, fm, m_thread) ;
	}

	virtual void unmarkOrbit(unsigned int orbit, Dart d)
	{
		FunctorUnmark<GenericMap> fm(m_map, m_marker, m_map.getMarkerVector(DART_ORBIT, m_thread)) ;
		m_map.foreach_dart_of_orbit(orbit, d, fm, m_thread) ;
	}

	template <typename MAP>
	void markOrbitInParent(unsigned int orbit, Dart d)
	{
		FunctorMark<GenericMap> fm(m_map, m_marker, m_map.getMarkerVector(DART_ORBIT,m_thread)) ;
		foreach_dart_of_orbit_in_parent<MAP>(dynamic_cast<MAP*>(&m_map), orbit, d, fm, m_thread);
	}

	template <typename MAP>
	void unmarkOrbit(unsigned int orbit, Dart d)
	{
		FunctorUnmark<GenericMap> fm(m_map, m_marker, m_map.getMarkerVector(DART_ORBIT, m_thread)) ;
		foreach_dart_of_orbit_in_parent<MAP>(m_map, orbit, d, fm, m_thread);
	}
};

/**
 * class that allows the marking of darts
 * the marked darts are stored to optimize the unmarking task at destruction
 * \warning no default constructor
 */
class DartMarkerStore: public DartMarker
{
protected:
	std::vector<unsigned int> m_markedDarts ;

public:
	/**
	 * constructor
	 * @param map the map on which we work
	 */
	DartMarkerStore(GenericMap& map): DartMarker(map)
	{}

	DartMarkerStore(GenericMap& map, unsigned int th): DartMarker(map,th)
	{}

protected:
	// protected copy constructor to forbid its usage
	DartMarkerStore(const DartMarkerStore& dm) : DartMarker(dm)
	{}


public:
	/**
	 * mark the cell of dart
	 */
	void mark(Dart d)
	{
		DartMarker::mark(d) ;
		m_markedDarts.push_back(d.index) ;
	}

	/**
	 * mark the cell
	 */
	void mark(unsigned int em)
	{
		DartMarker::mark(em) ;
		m_markedDarts.push_back(em);
	}

	void unmarkAll()
	{
		for (std::vector<unsigned int>::iterator it = m_markedDarts.begin(); it != m_markedDarts.end(); ++it)
			m_map.getMarkerVector(DART_ORBIT,m_thread)->operator[](*it).unsetMark(m_marker);
	}
};

/**
 * class that allows the marking of darts
 * the markers are not unmarked at destruction
 * \warning no default constructor
 */
class DartMarkerNoUnmark: public DartMarker
{
public:
	/**
	 * constructor
	 * @param map the map on which we work
	 */
	DartMarkerNoUnmark(GenericMap& map): DartMarker(map)
	{}

	DartMarkerNoUnmark(GenericMap& map, unsigned int th): DartMarker(map,th)
	{}

	~DartMarkerNoUnmark()
	{
		m_map.releaseMarker(m_marker,m_thread);
	}

protected:
	// protected copy constructor to forbid its usage
	DartMarkerNoUnmark(const DartMarkerNoUnmark& dm) : DartMarker(dm)
	{}
};

// Selector and count functors testing for marker existence
/********************************************************/

class SelectorMarked : public FunctorSelect
{
protected:
	DartMarker& m_marker;
public:
	SelectorMarked( DartMarker& m) : m_marker(m) {}
	bool operator()(Dart d) const
	{
		return m_marker.isMarked(d);
	}
};

class SelectorUnmarked : public FunctorSelect
{
protected:
	DartMarker& m_marker;
public:
	SelectorUnmarked( DartMarker& m) : m_marker(m) {}
	bool operator()(Dart d) const
	{
		return !m_marker.isMarked(d);
	}
};

// Functor version (needed for use with foreach_xxx)

class FunctorIsMarked : public FunctorType
{
protected:
	DartMarker& m_marker;
public:
	FunctorIsMarked(DartMarker& dm) : m_marker(dm) {}
	bool operator()(Dart d)
	{
		return m_marker.isMarked(d);
	}
};


class FunctorIsUnmarked : public FunctorType
{
protected:
	DartMarker& m_marker;
public:
	FunctorIsUnmarked(DartMarker& dm) : m_marker(dm) {}
	bool operator()(Dart d)
	{
		return !m_marker.isMarked(d);
	}
};


} // namespace CGoGN

#endif
