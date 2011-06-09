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

#include "Algo/ImplicitHierarchicalMesh/ihm3.h"
#include <math.h>
#include <limits>

namespace CGoGN
{

namespace Algo
{

namespace IHM
{

ImplicitHierarchicalMap3::ImplicitHierarchicalMap3() : m_curLevel(0), m_maxLevel(0), m_edgeIdCount(0), m_faceIdCount(0)
{
	m_dartLevel = Map3::addAttribute<unsigned int>(DART, "dartLevel") ;
	m_faceId = Map3::addAttribute<unsigned int>(DART, "faceId") ;
	m_edgeId = Map3::addAttribute<unsigned int>(DART, "edgeId") ;

	for(unsigned int i = 0; i < NB_ORBITS; ++i)
		m_nextLevelCell[i] = NULL ;
}

ImplicitHierarchicalMap3::~ImplicitHierarchicalMap3()
{
	Map3::removeAttribute(m_edgeId) ;
	Map3::removeAttribute(m_faceId) ;
	Map3::removeAttribute(m_dartLevel) ;
}

void ImplicitHierarchicalMap3::init()
{
	initEdgeId() ;
	initFaceId();

	for(unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		if(m_nextLevelCell[orbit] != NULL)
		{
			AttributeContainer& cellCont = m_attribs[orbit] ;
			for(unsigned int i = cellCont.begin(); i < cellCont.end(); cellCont.next(i))
				m_nextLevelCell[orbit]->operator[](i) = EMBNULL ;
		}
	}
}

void ImplicitHierarchicalMap3::initEdgeId()
{
	DartMarker edgeMark(*this) ;
	for(Dart d = Map3::begin(); d != Map3::end(); Map3::next(d))
	{
		if(!edgeMark.isMarked(d))
		{
			Dart e = d;
			do
			{
				m_edgeId[e] = m_edgeIdCount;
				edgeMark.mark(e);

				m_edgeId[Map3::phi2(e)] = m_edgeIdCount ;
				edgeMark.mark(Map3::phi2(e));

				e = Map3::alpha2(e);
			} while(e != d);

			m_edgeIdCount++;
		}
	}
}

void ImplicitHierarchicalMap3::initFaceId()
{
	DartMarker faceMark(*this) ;
	for(Dart d = Map3::begin(); d != Map3::end(); Map3::next(d))
	{
		if(!faceMark.isMarked(d))
		{
			Dart e = d;
			do
			{
				m_faceId[e] = m_faceIdCount ;
				faceMark.mark(e);

				Dart e3 = Map3::phi3(e);
				if(e3 != e)
				{
					m_faceId[e3] = m_faceIdCount ;
					faceMark.mark(e3);
				}

				e = Map3::phi1(e);
			} while(e != d);

			m_faceIdCount++;
		}
	}
}

unsigned int ImplicitHierarchicalMap3::faceLevel(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;

	if(m_curLevel == 0)
		return 0 ;

	Dart it = d ;
	Dart old = it ;
	unsigned int l_old = m_dartLevel[old] ;
	unsigned int fLevel = edgeLevel(it) ;
	do
	{
		it = phi1(it) ;
		unsigned int dl = m_dartLevel[it] ;
		if(dl < l_old)							// compute the oldest dart of the face
		{										// in the same time
			old = it ;
			l_old = dl ;
		}										// in a first time, the level of a face
		unsigned int l = edgeLevel(it) ;		// is the minimum of the levels
		fLevel = l < fLevel ? l : fLevel ;		// of its edges
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

	while(nbSubd > 1)
	{
		nbSubd /= 2 ;
		--fLevel ;
	}

	m_curLevel = cur ;

	return fLevel ;
}
//	Dart it = d ;
//	Dart old = it ;
//	unsigned int fLevel = edgeLevel(it) ;
//	do
//	{
//		it = phi1(it) ;
//		if(m_dartLevel[it] < m_dartLevel[old])  	// in a first time, the level of a face
//			old = it ;                      		// is the minimum of the levels
//		unsigned int l = edgeLevel(it) ;        	// of its edges
//		fLevel = l < fLevel ? l : fLevel ;
//	} while(it != d) ;
//
//	unsigned int cur = m_curLevel ;
//	m_curLevel = fLevel ;
//
//	unsigned int nbSubd = 0 ;
//	it = old ;
//	unsigned int eId = m_edgeId[old] ;              // the particular case of a face
//	do                                              // with all neighboring faces regularly subdivided
//	{                                               // but not the face itself
//		++nbSubd ;                              	// is treated here
//		it = phi1(it) ;
//	} while(m_edgeId[it] == eId) ;
//
//	while(nbSubd > 1)
//	{
//		nbSubd /= 2 ;
//		--fLevel ;
//	}
//
//	m_curLevel = cur ;
//
//	return fLevel ;


unsigned int ImplicitHierarchicalMap3::volumeLevel(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;

//std::cout << "volumeLevel" << std::endl;

	if(m_curLevel == 0)
		return 0 ;

	//First : the level of a volume is the
	//minimum of the levels of its faces

	DartMarkerStore mark(*this);		// Lock a marker

	std::vector<Dart> visitedFaces;		// Faces that are traversed
	visitedFaces.reserve(16);
	visitedFaces.push_back(d);			// Start with the face of d
	std::vector<Dart>::iterator face;

	Dart oldest = d ;
	unsigned int vLevel = std::numeric_limits<unsigned int>::max() ; //hook de ouf

	//parcours les faces du volume au niveau courant
	//on cherche le brin de niveau le plus bas de la hierarchie
	//on note le niveau le plus bas de la hierarchie
	mark.markOrbit(FACE, d) ;
	for(face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;

//std::cout << "dart from face = " << e << std::endl;

		// in a first time, the level of a face
		//the level of the volume is the minimum of the
		//levels of its faces

		//
		// Compute the level of this face
		// and the oldest dart
		//
		Dart it = e ;
		Dart old = it ;
		unsigned int l_old = m_dartLevel[old] ;
		unsigned int fLevel = edgeLevel(it) ;
		do
		{
			it = phi1(it) ;
			unsigned int dl = m_dartLevel[it] ;
			if(dl < l_old)							// compute the oldest dart of the face
			{										// in the same time
				old = it ;
				l_old = dl ;
			}										// in a first time, the level of a face
			unsigned int l = edgeLevel(it) ;		// is the minimum of the levels
			fLevel = l < fLevel ? l : fLevel ;		// of its edges
		} while(it != e) ;

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

		while(nbSubd > 1)
		{
			nbSubd /= 2 ;
			--fLevel ;
		}

		m_curLevel = cur ;

		//
		// compute the minimum level of the volume
		// if the level of this face is lower than the saved volume level
		//
		vLevel = fLevel < vLevel ? fLevel : vLevel ;

		//
		// compute the oldest dart from the volume
		// if the oldest dart from this face is oldest than the oldest saved dart
		//
		if(m_dartLevel[old] < m_dartLevel[oldest])
				oldest = old ;


		//
		// add all face neighbours to the table
		//
		do
		{
			Dart ee = phi2(e) ;
//std::cout << "\t face a cote =" << ee << std::endl;
			if(!mark.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mark.markOrbit(FACE, ee) ;
			}
			e = phi1(e) ;
		} while(e != *face) ;
	}

//std::cout << "fin" << std::endl << std::endl;

	//Second : the case of all faces regularly subdivided but not the volume itself
	unsigned int cur = m_curLevel ;
	m_curLevel = vLevel ;

	unsigned int nbSubd = 0 ;
	Dart it = oldest ;
	unsigned int eId = m_edgeId[oldest] ;

	do
	{
		++nbSubd ;
		it = phi1(it) ;
	} while(m_edgeId[it] == eId) ;


	while(nbSubd > 1)
	{
		nbSubd /= 2 ;
	    --vLevel ;
	}

	m_curLevel = cur ;

	return vLevel;
}

//		unsigned int fLevel = faceLevel(e) ;
//		vLevel = fLevel < vLevel ? fLevel : vLevel ;
//
//		Dart old = faceOldestDart(e);
//		if(m_dartLevel[old] < m_dartLevel[oldest])
//			oldest = old ;





Dart ImplicitHierarchicalMap3::faceOldestDart(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;
	Dart it = d ;
	Dart oldest = it ;
	unsigned int l_old = m_dartLevel[oldest] ;
	do
	{
		unsigned int l = m_dartLevel[it] ;
		if(l < l_old || (l == l_old && it < oldest))
		{
			oldest = it ;
			l_old = l ;
		}
		it = phi1(it) ;
	} while(it != d) ;
	return oldest ;
}

Dart ImplicitHierarchicalMap3::volumeOldestDart(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;

	Dart oldest = d;
	DartMarkerStore mark(*this);	// Lock a marker

	std::list<Dart> visitedFaces;	// Faces that are traversed
	visitedFaces.push_back(d);		// Start with the face of d
	std::list<Dart>::iterator face;

	// For every face added to the list
	//the oldest dart from a volume is the oldest dart from all faces of this volume
	mark.markOrbit(FACE, d) ;

	for(face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;

		//for every dart in this face
		Dart old = faceOldestDart(e);
		if(m_dartLevel[old] < m_dartLevel[oldest])
			oldest = old;

		do	// add all face neighbours to the table
		{
			Dart ee = phi2(e) ;
			if(!mark.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mark.markOrbit(FACE, ee) ;
			}
			e = phi1(e) ;
		} while(e != *face) ;
	}

	return oldest;
}

bool ImplicitHierarchicalMap3::edgeIsSubdivided(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;

	Dart d2 = phi2(d) ;
	++m_curLevel ;
	Dart d2_l = phi2(d) ;
	--m_curLevel ;
	if(d2 != d2_l)
		return true ;
	else
		return false ;
}

bool ImplicitHierarchicalMap3::faceIsSubdivided(Dart d)
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

bool ImplicitHierarchicalMap3::volumeIsSubdivided(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;
	unsigned int vLevel = volumeLevel(d);
	if(vLevel < m_curLevel)
		return false;

	//test si toutes les faces sont subdivisee
	DartMarkerStore mark(*this);		// Lock a marker

	std::vector<Dart> visitedFaces;		// Faces that are traversed
	visitedFaces.reserve(16);
	visitedFaces.push_back(d);			// Start with the face of d
	std::vector<Dart>::iterator face;


	Dart old = d ;
	bool facesAreSubdivided = false;

	//parcours les faces du volume au niveau courant
	//on cherche le brin de niveau le plus bas de la hierarchie
	//on note le niveau le plus bas de la hierarchie
	mark.markOrbit(FACE, d) ;
	for(face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;

		// in a first time, the level of a face
		//the level of the volume is the minimum of the
		//levels of its faces

		facesAreSubdivided &= faceIsSubdivided(e) ;


		do	// add all face neighbours to the table
		{
			Dart ee = phi2(e) ;
			if(!mark.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mark.markOrbit(FACE, ee) ;
			}
			e = phi1(e) ;
		} while(e != *face) ;
	}

	//mais pas le volume lui-meme
	bool subd = false;
	++m_curLevel;
	if(facesAreSubdivided && m_dartLevel[phi2(phi1(phi1(d)))] == m_curLevel && m_faceId[phi2(phi1(phi1(d)))] != m_faceId[d])
		subd = true;
	--m_curLevel;
	return subd;
}


bool  ImplicitHierarchicalMap3::edgeCanBeCoarsened(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;

	bool subd = false ;
	bool subdOnce = true ;
	bool degree2 = false ;

	if(edgeIsSubdivided(d))
	{
		subd = true ;
		Dart d2 = phi2(d) ;
		++m_curLevel ;

		if(vertexDegree(phi1(d)) == 2)
		{
			degree2 = true ;
			if(edgeIsSubdivided(d) || edgeIsSubdivided(d2))
				subdOnce = false ;
		}
		--m_curLevel ;
	}
	return subd && degree2 && subdOnce ;
}

bool ImplicitHierarchicalMap3::faceIsSubdividedOnce(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;
	unsigned int fLevel = faceLevel(d) ;
	if(fLevel < m_curLevel)		// a face whose level in the current level map is lower than
		return false ;			// the current level can not be subdivided to higher levels

	unsigned int degree = 0 ;
	bool subd = false ;
	bool subdOnce = true ;
	Dart fit = d ;
	do
	{
		++m_curLevel ;
		if(m_dartLevel[phi1(fit)] == m_curLevel && m_edgeId[phi1(fit)] != m_edgeId[fit])
		{
			subd = true ;
			++m_curLevel ;
			if(m_dartLevel[phi1(fit)] == m_curLevel && m_edgeId[phi1(fit)] != m_edgeId[fit])
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
		if(m_dartLevel[phi1(cf)] == m_curLevel && m_edgeId[phi1(cf)] != m_edgeId[cf])
			subdOnce = false ;
		--m_curLevel ;
		--m_curLevel ;
	}

	return subd && subdOnce ;
}

bool ImplicitHierarchicalMap3:: volumeIsSubdividedOnce(Dart d)
{

	return true;
}

} //namespace IHM

} //namespace Algo

} //namespace CGoGN

