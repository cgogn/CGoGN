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

#include "Algo/ImplicitHierarchicalMesh/ihm3.h"
#include <math.h>

namespace CGoGN
{

namespace Algo
{

namespace IHM
{

//OK
ImplicitHierarchicalMap3::ImplicitHierarchicalMap3() : m_curLevel(0), m_maxLevel(0), m_edgeIdCount(0), m_faceIdCount(0)
{
	m_dartLevel = Map3::addAttribute<unsigned int>(DART_ORBIT, "dartLevel") ;
	m_edgeId = Map3::addAttribute<unsigned int>(DART_ORBIT, "edgeId") ;
	m_faceId = Map3::addAttribute<unsigned int>(DART_ORBIT, "faceId") ;

	for(unsigned int i = 0; i < NB_ORBITS; ++i)
		m_nextLevelCell[i] = NULL ;
}

//OK
ImplicitHierarchicalMap3::~ImplicitHierarchicalMap3()
{
	Map3::removeAttribute(m_edgeId) ;
	Map3::removeAttribute(m_faceId) ;
	Map3::removeAttribute(m_dartLevel) ;
}

//OK
void ImplicitHierarchicalMap3::init()
{
	initEdgeId() ;
	initFaceId();

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

//OK
void ImplicitHierarchicalMap3::initEdgeId()
{
	m_edgeIdCount = 0 ;
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

//OK
void ImplicitHierarchicalMap3::initFaceId()
{
	m_faceIdCount = 0 ;
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

				if(Map3::phi3(e) != e)
				{
					m_faceId[Map3::phi3(e)] = m_faceIdCount ;
					faceMark.mark(Map3::phi3(e));
				}

				e = Map3::phi1(e);
			} while(e != d);

			m_faceIdCount++;
		}
	}
}

//OK
unsigned int ImplicitHierarchicalMap3::faceLevel(Dart d)
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
		if(m_dartLevel[it] < m_dartLevel[old])  	// in a first time, the level of a face
			old = it ;                      		// is the minimum of the levels
		unsigned int l = edgeLevel(it) ;        	// of its edges
		fLevel = l < fLevel ? l : fLevel ;
	} while(it != d) ;

	unsigned int cur = m_curLevel ;
	m_curLevel = fLevel ;

	unsigned int nbSubd = 0 ;
	it = old ;
	unsigned int eId = m_edgeId[old] ;              // the particular case of a face
	do                                              // with all neighboring faces regularly subdivided
	{                                               // but not the face itself
		++nbSubd ;                              	// is treated here
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

//TODO
unsigned int ImplicitHierarchicalMap3::volumeLevel(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;

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
	unsigned int vLevel = faceLevel(d);

	//parcours les faces du volume au niveau courant
	//on cherche le brin de niveau le plus bas de la hierarchie
	//on note le niveau le plus bas de la hierarchie
	mark.markOrbit(FACE_ORBIT, d) ;
	for(face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;

		// in a first time, the level of a face
		//the level of the volume is the minimum of the
		//levels of its faces
		unsigned int fLevel = faceLevel(e) ;
		vLevel = fLevel < vLevel ? fLevel : vLevel ;

		Dart old = faceOldestDart(e);
		if(m_dartLevel[old] < m_dartLevel[oldest])
			oldest = old ;

		do	// add all face neighbours to the table
		{
			Dart ee = phi2(e) ;
			if(!mark.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mark.markOrbit(FACE_ORBIT, ee) ;
			}
			e = phi1(e) ;
		} while(e != *face) ;
	}

	std::cout << "vLevel avant=" << vLevel << std::endl;

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

	std::cout << "vLevel apres=" << vLevel << std::endl;

	return vLevel;
}

//OK
Dart ImplicitHierarchicalMap3::faceOldestDart(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;

	Dart it = d;
	Dart oldest = it;

	do
	{
		it = phi1(it);

		if(m_dartLevel[it] < m_dartLevel[oldest])
			oldest = it;
	}while(it != d);

	return oldest;
}

//TODO
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
	mark.markOrbit(FACE_ORBIT, d) ;

	for(face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;

		//for every dart in this face
		Dart old = faceOldestDart(*face);
		if(m_dartLevel[old] < m_dartLevel[oldest])
			oldest = old;

		do	// add all face neighbours to the table
		{
			Dart ee = phi2(e) ;
			if(!mark.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mark.markOrbit(FACE_ORBIT, ee) ;
			}
			e = phi1(e) ;
		} while(e != *face) ;
	}

	return oldest;
}

//OK
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

//OK
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

//OK
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
	mark.markOrbit(FACE_ORBIT, d) ;
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
				mark.markOrbit(FACE_ORBIT, ee) ;
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

} //namespace IHM

} //namespace Algo

} //namespace CGoGN

