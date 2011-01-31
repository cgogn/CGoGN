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

#include "Algo/ImplicitHierarchicalMesh/ihm.h"
#include <math.h>

namespace CGoGN
{

namespace Algo
{

namespace IHM
{

ImplicitHierarchicalMap::ImplicitHierarchicalMap() : m_curLevel(0), m_maxLevel(0), m_idCount(0)
{
	m_dartLevel = Map2::addAttribute<unsigned int>(DART_ORBIT, "dartLevel") ;
	m_edgeId = Map2::addAttribute<unsigned int>(DART_ORBIT, "edgeId") ;
	for(unsigned int i = 0; i < NB_ORBITS; ++i)
		m_nextLevelCell[i] = NULL ;
}

ImplicitHierarchicalMap::~ImplicitHierarchicalMap()
{
	Map2::removeAttribute(m_edgeId) ;
	Map2::removeAttribute(m_dartLevel) ;
}

void ImplicitHierarchicalMap::init()
{
	initEdgeId() ;

	for(unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		if(m_nextLevelCell[orbit] != NULL)
		{
			AttribContainer& cellCont = m_attribs[orbit] ;
			for(unsigned int i = cellCont.begin(); i < cellCont.end(); cellCont.next(i))
				m_nextLevelCell[orbit]->operator[](i) = EMBNULL ;
		}
	}
}

void ImplicitHierarchicalMap::initEdgeId()
{
	m_idCount = 0 ;
	DartMarker edgeMark(*this) ;
	for(Dart d = Map2::begin(); d != Map2::end(); Map2::next(d))
	{
		if(!edgeMark.isMarked(d))
		{
			edgeMark.markOrbit(EDGE_ORBIT, d) ;
			m_edgeId[d] = m_idCount ;
			m_edgeId[Map2::phi2(d)] = m_idCount++ ;
		}
	}
}

unsigned int ImplicitHierarchicalMap::faceLevel(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;
	if(m_curLevel == 0)
		return 0 ;

	Dart it = d ;
	Dart old = it ;
	unsigned int fLevel = edgeLevel(it) ;
	do
	{
		it = phi1(it) ;
		if(m_dartLevel[it] < m_dartLevel[old])	// in a first time, the level of a face
			old = it ;							// is the minimum of the levels
		unsigned int l = edgeLevel(it) ;		// of its edges
		fLevel = l < fLevel ? l : fLevel ;
	} while(it != d) ;

	unsigned int cur = m_curLevel ;
	m_curLevel = fLevel ;

	unsigned int nbSubd = 0 ;
	it = old ;
	unsigned int eId = m_edgeId[old] ;			// the particular case of a face
	do											// with all neighboring faces regularly subdivided
	{											// but not the face itself
		++nbSubd ;								// is treated here
		it = phi1(it) ;
	} while(m_edgeId[it] == eId) ;
	//fLevel -= (unsigned int)(log2(nbSubd)) ;	PB WINDOWS log2 n'existe pas sous Visual 2010 !!
	fLevel -= (unsigned int)(log((double)nbSubd)/log(2.0)) ;
	m_curLevel = cur ;

	return fLevel ;
}

Dart ImplicitHierarchicalMap::faceOldestDart(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;
	Dart it = d ;
	Dart oldest = it ;
	do
	{
		it = phi1(it) ;
		if(m_dartLevel[it] < m_dartLevel[oldest])
			oldest = it ;
	} while(it != d) ;
	return oldest ;
}

bool ImplicitHierarchicalMap::edgeIsSubdivided(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;

//	Dart d2 = phi2(d) ;
//	++m_curLevel ;
//	Dart d2_l = phi2(d) ;
//	--m_curLevel ;
//	if(d2 != d2_l)
//		return true ;
//	else
//		return false ;

	Dart d1 = phi1(d) ;
	++m_curLevel ;
	Dart d1_l = phi1(d) ;
	--m_curLevel ;
	if(d1 != d1_l)
		return true ;
	else
		return false ;
}

bool ImplicitHierarchicalMap::faceIsSubdivided(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;
	unsigned int fLevel = faceLevel(d) ;
	if(fLevel < m_curLevel)
		return false ;

	bool subd = false ;
	++m_curLevel ;
	if(m_dartLevel[phi1(d)] == m_curLevel && m_edgeId[phi1(d)] != m_edgeId[d])
		subd = true ;
	--m_curLevel ;
	return subd ;
}

bool ImplicitHierarchicalMap::faceIsSubdividedOnce(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;
	unsigned int fLevel = faceLevel(d) ;
	if(fLevel < m_curLevel)
		return false ;

	unsigned int degree = 0 ;
	bool subd = false ;
	bool subdOnce = true ;
	Dart fit = d ;
	do
	{
		++m_curLevel ;
		if(m_dartLevel[phi1(d)] == m_curLevel && m_edgeId[phi1(d)] != m_edgeId[d])
		{
			subd = true ;
			++m_curLevel ;
			if(m_dartLevel[phi1(d)] == m_curLevel && m_edgeId[phi1(d)] != m_edgeId[d])
				subdOnce = false ;
			--m_curLevel ;
		}
		--m_curLevel ;
		++degree ;
		fit = phi1(fit) ;
	} while(subd && subdOnce && fit != d) ;

	if(degree == 3 && subd)
	{
		++m_curLevel ;
		Dart cf = phi2(phi1(d)) ;
		++m_curLevel ;
		if(m_dartLevel[phi1(d)] == m_curLevel && m_edgeId[phi1(d)] != m_edgeId[d])
			subdOnce = false ;
		--m_curLevel ;
		--m_curLevel ;
	}

	return subd && subdOnce ;
}

} //namespace IHM

} //namespace Algo

} //namespace CGoGN
