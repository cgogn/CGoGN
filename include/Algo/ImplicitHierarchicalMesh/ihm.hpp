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

namespace CGoGN
{

namespace Algo
{

namespace IHM
{

/***************************************************
 *             ATTRIBUTES MANAGEMENT               *
 ***************************************************/

template <typename T>
AttributeHandler_IHM<T> ImplicitHierarchicalMap::addAttribute(unsigned int orbit, const std::string& nameAttr)
{
	bool addNextLevelCell = false ;
	if(!isOrbitEmbedded(orbit))
		addNextLevelCell = true ;

	AttributeHandler<T> h = Map2::addAttribute<T>(orbit, nameAttr) ;

	if(addNextLevelCell)
	{
		AttributeContainer& cellCont = m_attribs[orbit] ;
		unsigned int index = cellCont.addAttribute<unsigned int>("nextLevelCell") ;
		AttributeMultiVector<unsigned int>& amv = cellCont.getDataVector<unsigned int>(index) ;
		m_nextLevelCell[orbit] = &amv ;
		for(unsigned int i = cellCont.begin(); i < cellCont.end(); cellCont.next(i))
			amv[i] = EMBNULL ;
	}

	return AttributeHandler_IHM<T>(this, h.getOrbit(), h.getIndex()) ;
}

template <typename T>
AttributeHandler_IHM<T> ImplicitHierarchicalMap::getAttribute(unsigned int orbit, const std::string& nameAttr)
{
	AttributeHandler<T> h = Map2::getAttribute<T>(orbit, nameAttr) ;
	return AttributeHandler_IHM<T>(this, h.getOrbit(), h.getIndex()) ;
}

/***************************************************
 *                 MAP TRAVERSAL                   *
 ***************************************************/

inline Dart ImplicitHierarchicalMap::newDart()
{
	Dart d = Map2::newDart() ;
	m_dartLevel[d] = m_curLevel ;
	if(m_curLevel > m_maxLevel)			// update max level
		m_maxLevel = m_curLevel ;		// if needed
	return d ;
}

inline Dart ImplicitHierarchicalMap::phi1(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;
	bool finished = false ;
	unsigned int edgeId = m_edgeId[d] ;
	Dart it = d ;
	do
	{
		it = Map2::phi1(it) ;
		if(m_dartLevel[it] <= m_curLevel)
			finished = true ;
		else
		{
			while(m_edgeId[it] != edgeId)
				it = Map2::alpha_1(it) ;
		}
	} while(!finished) ;
	return it ;
}

inline Dart ImplicitHierarchicalMap::phi_1(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;
	bool finished = false ;
	Dart it = Map2::phi_1(d) ;
	unsigned int edgeId = m_edgeId[it] ;
	do
	{
		if(m_dartLevel[it] <= m_curLevel)
			finished = true ;
		else
		{
			it = Map2::phi_1(it) ;
			while(m_edgeId[it] != edgeId)
				it = Map2::phi_1(Map2::phi2(it)) ;
		}
	} while(!finished) ;
	return it ;
}

inline Dart ImplicitHierarchicalMap::phi2(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;
	if(Map2::phi2(d) == d)
		return d ;
	return Map2::alpha1(phi1(d)) ;
}

inline Dart ImplicitHierarchicalMap::alpha0(Dart d)
{
	return phi2(d) ;
}

inline Dart ImplicitHierarchicalMap::alpha1(Dart d)
{
	return Map2::alpha1(d) ;
}

inline Dart ImplicitHierarchicalMap::alpha_1(Dart d)
{
	return Map2::alpha_1(d) ;
}

inline Dart ImplicitHierarchicalMap::begin()
{
	Dart d = Map2::begin() ;
	while(m_dartLevel[d] > m_curLevel)
		Map2::next(d) ;
	return d ;
}

inline Dart ImplicitHierarchicalMap::end()
{
	return Map2::end() ;
}

inline void ImplicitHierarchicalMap::next(Dart& d)
{
	do
	{
		Map2::next(d) ;
	} while(d != Map2::end() && m_dartLevel[d] > m_curLevel) ;
}

inline bool ImplicitHierarchicalMap::foreach_dart_of_vertex(Dart d, FunctorType& f)
{
	Dart dNext = d;
	do
	{
		if (f(dNext))
			return true;
		dNext = alpha1(dNext);
 	} while (dNext != d);
 	return false;
}

inline bool ImplicitHierarchicalMap::foreach_dart_of_edge(Dart d, FunctorType& f)
{
	if (f(d))
		return true;

	Dart d2 = phi2(d);
	if (d2 != d)
		return f(d2);
	else
		return false;
}

inline bool ImplicitHierarchicalMap::foreach_dart_of_oriented_face(Dart d, FunctorType& f)
{
	Dart dNext = d ;
	do
	{
		if (f(dNext))
			return true ;
		dNext = phi1(dNext) ;
	} while (dNext != d) ;
	return false ;
}

inline bool ImplicitHierarchicalMap::foreach_dart_of_face(Dart d, FunctorType& f)
{
	return foreach_dart_of_oriented_face(d, f) ;
}

inline bool ImplicitHierarchicalMap::foreach_dart_of_oriented_volume(Dart d, FunctorType& f)
{
	DartMarkerStore mark(*this);	// Lock a marker
	bool found = false;				// Last functor return value

	std::list<Dart> visitedFaces;	// Faces that are traversed
	visitedFaces.push_back(d);		// Start with the face of d
	std::list<Dart>::iterator face;

	// For every face added to the list
	for (face = visitedFaces.begin(); !found && face != visitedFaces.end(); ++face)
	{
		if (!mark.isMarked(*face))		// Face has not been visited yet
		{
			// Apply functor to the darts of the face
			found = foreach_dart_of_oriented_face(*face, f);

			// If functor returns false then mark visited darts (current face)
			// and add non visited adjacent faces to the list of face
			if (!found)
			{
				Dart dNext = *face ;
				do
				{
					mark.mark(dNext);					// Mark
					Dart adj = phi2(dNext);				// Get adjacent face
					if (adj != dNext && !mark.isMarked(adj))
						visitedFaces.push_back(adj);	// Add it
					dNext = phi1(dNext);
				} while(dNext != *face);
			}
		}
	}
	return found;
}

inline bool ImplicitHierarchicalMap::foreach_dart_of_volume(Dart d, FunctorType& f)
{
	return foreach_dart_of_oriented_volume(d, f) ;
}

inline bool ImplicitHierarchicalMap::foreach_dart_of_cc(Dart d, FunctorType& f)
{
	return foreach_dart_of_oriented_volume(d, f) ;
}

/***************************************************
 *               MAP MANIPULATION                  *
 ***************************************************/

inline void ImplicitHierarchicalMap::splitFace(Dart d, Dart e)
{
	EmbeddedMap2<Map2>::splitFace(d, e) ;
	if(isOrbitEmbedded(FACE_ORBIT))
	{
		unsigned int cur = m_curLevel ;
		m_curLevel = m_maxLevel ;
		this->embedOrbit(FACE_ORBIT, d, this->getDartEmbedding(FACE_ORBIT, d)) ;
		this->embedOrbit(FACE_ORBIT, e, this->getDartEmbedding(FACE_ORBIT, e)) ;
		m_curLevel = cur ;
	}
}

/***************************************************
 *              LEVELS MANAGEMENT                  *
 ***************************************************/

inline unsigned int ImplicitHierarchicalMap::getCurrentLevel()
{
	return m_curLevel ;
}

inline void ImplicitHierarchicalMap::setCurrentLevel(unsigned int l)
{
	assert(l >= 0 || !"Trying to set current level to a negative value") ;
	m_curLevel = l ;
}

inline unsigned int ImplicitHierarchicalMap::getMaxLevel()
{
	return m_maxLevel ;
}

inline unsigned int ImplicitHierarchicalMap::getDartLevel(Dart d)
{
	return m_dartLevel[d] ;
}

inline void ImplicitHierarchicalMap::setDartLevel(Dart d, unsigned int l)
{
	m_dartLevel[d] = l ;
}

/***************************************************
 *             EDGE ID MANAGEMENT                  *
 ***************************************************/

inline unsigned int ImplicitHierarchicalMap::getNewEdgeId()
{
	return m_idCount++ ;
}

inline unsigned int ImplicitHierarchicalMap::getEdgeId(Dart d)
{
	return m_edgeId[d] ;
}

inline void ImplicitHierarchicalMap::setEdgeId(Dart d, unsigned int i)
{
	m_edgeId[d] = i ;
}

/***************************************************
 *               CELLS INFORMATION                 *
 ***************************************************/

inline unsigned int ImplicitHierarchicalMap::vertexInsertionLevel(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;
	return m_dartLevel[d] ;
}

inline unsigned int ImplicitHierarchicalMap::edgeLevel(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;
	unsigned int ld = m_dartLevel[d] ;
//	unsigned int ldd = m_dartLevel[phi2(d)] ;	// the level of an edge is the maximum of the
	unsigned int ldd = m_dartLevel[phi1(d)] ;
	return ld < ldd ? ldd : ld ;				// insertion levels of its two darts
}

/***************************************************
 *               ATTRIBUTE HANDLER                 *
 ***************************************************/

template <typename T>
T& AttributeHandler_IHM<T>::operator[](Dart d)
{
	ImplicitHierarchicalMap* m = reinterpret_cast<ImplicitHierarchicalMap*>(this->m_map) ;
	assert(m->m_dartLevel[d] <= m->m_curLevel || !"Access to a dart introduced after current level") ;
	assert(m->vertexInsertionLevel(d) <= m->m_curLevel || !"Access to the embedding of a vertex inserted after current level") ;

	unsigned int orbit = this->m_orbit ;
	unsigned int nbSteps = m->m_curLevel - m->vertexInsertionLevel(d) ;
	unsigned int index = m->getEmbedding(d, orbit) ;

	if(index == EMBNULL && nbSteps == 0)
	{
		index = m->embedNewCell(orbit, d) ;
		m->m_nextLevelCell[orbit]->operator[](index) = EMBNULL ;
		return this->m_attrib->operator[](index);
	}

	AttributeContainer& cont = m->getAttributeContainer(orbit) ;
	unsigned int step = 0 ;
	while(step < nbSteps)
	{
		step++ ;
		unsigned int nextIdx = m->m_nextLevelCell[orbit]->operator[](index) ;
		if (nextIdx == EMBNULL)
		{
			nextIdx = m->newCell(orbit) ;
			m->copyCell(orbit, nextIdx, index) ;
			m->m_nextLevelCell[orbit]->operator[](index) = nextIdx ;
			m->m_nextLevelCell[orbit]->operator[](nextIdx) = EMBNULL ;
			cont.refLine(index) ;
		}
		index = nextIdx ;
	}
	return this->m_attrib->operator[](index);
}

template <typename T>
const T& AttributeHandler_IHM<T>::operator[](Dart d) const
{
	ImplicitHierarchicalMap* m = reinterpret_cast<ImplicitHierarchicalMap*>(this->m_map) ;
	assert(m->m_dartLevel[d] <= m->m_curLevel || !"Access to a dart introduced after current level") ;
	assert(m->vertexInsertionLevel(d) <= m->m_curLevel || !"Access to the embedding of a vertex inserted after current level") ;

	unsigned int orbit = this->m_orbit ;
	unsigned int nbSteps = m->m_curLevel - m->vertexInsertionLevel(d) ;
	unsigned int index = m->getEmbedding(d, orbit) ;

	unsigned int step = 0 ;
	while(step < nbSteps)
	{
		step++ ;
		unsigned int next = m->m_nextLevelCell[orbit]->operator[](index) ;
		if(next != EMBNULL) index = next ;
		else break ;
	}
	return this->m_attrib->operator[](index);
}

} //namespace IHM

} //namespace Algo

} //namespace CGoGN
