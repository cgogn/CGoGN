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

#ifndef __FUNCTOR_H__
#define __FUNCTOR_H__

#include "Topology/generic/marker.h"
#include <vector>

namespace CGoGN
{

// Base Class for Functors: object function that is applied to darts
/********************************************************/
class FunctorType
{
public:
	FunctorType() {}
	virtual ~FunctorType() {}
	virtual bool operator()(Dart d) = 0;
};

// Base Class for Functors that need access to the map
/********************************************************/
template <typename MAP>
class FunctorMap: public virtual FunctorType
{
protected:
	MAP& m_map ;
public:
	FunctorMap(MAP& m): m_map(m) {}
};

// Selector functors : return true to select or false to not select a dart
/********************************************************/
class FunctorSelect
{
public:
	FunctorSelect() {}
	virtual ~FunctorSelect() {}
	virtual bool operator()(Dart d) const = 0 ;
};

class SelectorTrue : public FunctorSelect
{
public:
	bool operator()(Dart) const {
		return true;
	}
};

class SelectorFalse : public FunctorSelect
{
public:
	bool operator()(Dart) const {
		return false;
	}
};

// Counting Functors : increment its value every time it is applied
/********************************************************/

class FunctorCount: public virtual FunctorType
{
private:
	unsigned m_count;
public:
	FunctorCount(): m_count(0) {}
	bool operator()(Dart) {
		m_count++;
		return false;
	}
	unsigned getNb() const {
		return m_count;
	}
	void init() {
		m_count=0;
	}
	void increment() {
		++m_count;
	}
};

// Embedding Functors
/********************************************************/

template <typename MAP>
class FunctorSetEmb: public FunctorMap<MAP>
{
protected:
	unsigned int orbit;
	unsigned int emb;
public:
	FunctorSetEmb(MAP& map, unsigned int orb, unsigned int e): FunctorMap<MAP>(map), orbit(orb), emb(e) {}
	bool operator()(Dart d)
	{
		this->m_map.setDartEmbedding(orbit, d, emb);
		return false;
	}
};

// Search Functor: look for a given dart when applied
/********************************************************/

template <typename MAP>
class FunctorSearch: public FunctorType
{
protected:
	bool m_found;
	Dart dart;
public:
	FunctorSearch(Dart d): m_found(false), dart(d) {}
	void setDart(Dart d) {
		dart = d;
		m_found = false;
	}
	bool operator()(Dart d) {
		if (d==dart) {
			m_found = true;
			return true;
		}
		return false;
	}
	bool found() {
		return m_found;
	}
};

// Multiple Functor: to apply several Functors in turn to a dart
/********************************************************/

class FunctorDoubleFunctor: public FunctorType
{
protected:
	FunctorType& m_fonct1;
	FunctorType& m_fonct2;
public:
	FunctorDoubleFunctor(FunctorType& f1, FunctorType& f2): m_fonct1(f1), m_fonct2(f2) {}
	bool operator()(Dart d)
	{
		if (m_fonct1(d)) return true;
		return m_fonct2(d);
	}
};

// Functor Store: to store the traversed darts in a given vector
/********************************************************/

class FunctorStore: public FunctorType
{
protected:
	std::vector<Dart>& m_vec;
public:
	FunctorStore(std::vector<Dart>& vec): m_vec(vec) {}
	bool operator()(Dart d) {
		m_vec.push_back(d);
		return false;
	}
};

} //namespace CGoGN

#endif
