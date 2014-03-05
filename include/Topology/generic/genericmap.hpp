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

#include "Topology/generic/dartmarker.h"
#include "Topology/generic/traversorCell.h"
#include "Topology/generic/traversorFactory.h"

namespace CGoGN
{

/****************************************
 *           DARTS MANAGEMENT           *
 ****************************************/

inline Dart GenericMap::newDart()
{
	unsigned int di = m_attribs[DART].insertLine();		// insert a new dart line
	for(unsigned int i = 0; i < NB_ORBITS; ++i)
	{
		if (m_embeddings[i])							// set all its embeddings
			(*m_embeddings[i])[di] = EMBNULL ;			// to EMBNULL
	}

	return Dart::create(di) ;
}

inline void GenericMap::deleteDartLine(unsigned int index)
{
	m_attribs[DART].removeLine(index) ;	// free the dart line

	for (unsigned int t = 0; t < m_nbThreads; ++t)	// clear markers of
		(*m_markTables[DART][t])[index].clear() ;	// the removed dart

	for(unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		if (m_embeddings[orbit])									// for each embedded orbit
		{
			unsigned int emb = (*m_embeddings[orbit])[index] ;		// get the embedding of the dart
			if(emb != EMBNULL)
			{
				if(m_attribs[orbit].unrefLine(emb))					// unref the pointed embedding line
				{
					for (unsigned int t = 0; t < m_nbThreads; ++t)	// and clear its markers if it was
						(*m_markTables[orbit][t])[emb].clear() ;	// its last unref (and was thus freed)
				}
			}
		}
	}
}

inline unsigned int GenericMap::copyDartLine(unsigned int index)
{
	unsigned int newindex = m_attribs[DART].insertLine() ;	// create a new dart line
	m_attribs[DART].copyLine(newindex, index) ;				// copy the given dart line
	for(unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		if (m_embeddings[orbit])
		{
			unsigned int emb = (*m_embeddings[orbit])[newindex] ;	// add a ref to the cells pointed
			if(emb != EMBNULL)										// by the new dart line
				m_attribs[orbit].refLine(emb) ;
		}
	}
	return newindex ;
}

//inline bool GenericMap::isDartValid(Dart d)
//{
//	return !d.isNil() && m_attribs[DART].used(dartIndex(d)) ;
//}

/****************************************
 *         EMBEDDING MANAGEMENT         *
 ****************************************/

template <unsigned int ORBIT>
inline bool GenericMap::isOrbitEmbedded() const
{
	return (ORBIT == DART) || (m_embeddings[ORBIT] != NULL) ;
}

inline bool GenericMap::isOrbitEmbedded(unsigned int orbit) const
{
	return (orbit == DART) || (m_embeddings[orbit] != NULL) ;
}

template <unsigned int ORBIT>
inline unsigned int GenericMap::newCell()
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");
	return m_attribs[ORBIT].insertLine();
}

template <unsigned int ORBIT>
inline void GenericMap::copyCell(unsigned int i, unsigned int j)
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");
	m_attribs[ORBIT].copyLine(i, j) ;
}

template <unsigned int ORBIT>
inline void GenericMap::initCell(unsigned int i)
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");
	m_attribs[ORBIT].initLine(i) ;
}

/****************************************
 *   ATTRIBUTES CONTAINERS MANAGEMENT   *
 ****************************************/

inline unsigned int GenericMap::getNbCells(unsigned int orbit)
{
	return m_attribs[orbit].size() ;
}

template <unsigned int ORBIT>
inline AttributeContainer& GenericMap::getAttributeContainer()
{
	return m_attribs[ORBIT] ;
}

template <unsigned int ORBIT>
inline const AttributeContainer& GenericMap::getAttributeContainer() const
{
	return m_attribs[ORBIT] ;
}

inline AttributeContainer& GenericMap::getAttributeContainer(unsigned int orbit)
{
	return m_attribs[orbit] ;
}

inline const AttributeContainer& GenericMap::getAttributeContainer(unsigned int orbit) const
{
	return m_attribs[orbit] ;
}

template <unsigned int ORBIT>
inline AttributeMultiVector<Mark>* GenericMap::getMarkVector(unsigned int thread)
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded") ;
	return m_markTables[ORBIT][thread] ;
}

template <unsigned int ORBIT>
inline AttributeMultiVector<unsigned int>* GenericMap::getEmbeddingAttributeVector()
{
	return m_embeddings[ORBIT] ;
}

template <typename R>
bool GenericMap::registerAttribute(const std::string &nameType)
{
	RegisteredBaseAttribute* ra = new RegisteredAttribute<R>;
	if (ra == NULL)
	{
		CGoGNerr << "Erreur enregistrement attribut" << CGoGNendl;
		return false;
	}

	ra->setTypeName(nameType);

	m_attributes_registry_map->insert(std::pair<std::string, RegisteredBaseAttribute*>(nameType,ra));
	return true;
}

/****************************************
 *   EMBEDDING ATTRIBUTES MANAGEMENT    *
 ****************************************/

template <unsigned int ORBIT>
void GenericMap::addEmbedding()
{
	assert(!isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit already embedded") ;

	std::ostringstream oss;
	oss << "EMB_" << ORBIT;

	AttributeContainer& dartCont = m_attribs[DART] ;
	AttributeMultiVector<unsigned int>* amv = dartCont.addAttribute<unsigned int>(oss.str()) ;
	m_embeddings[ORBIT] = amv ;

	// set new embedding to EMBNULL for all the darts of the map
	for(unsigned int i = dartCont.begin(); i < dartCont.end(); dartCont.next(i))
		(*amv)[i] = EMBNULL ;
}

/****************************************
 *          ORBITS TRAVERSALS           *
 ****************************************/

template <unsigned int ORBIT>
bool GenericMap::foreach_dart_of_orbit(Dart d, FunctorType& f, unsigned int thread) const
{
	switch(ORBIT)
	{
		case DART:		return f(d);
		case VERTEX: 	return foreach_dart_of_vertex(d, f, thread);
		case EDGE: 		return foreach_dart_of_edge(d, f, thread);
		case FACE: 		return foreach_dart_of_face(d, f, thread);
		case VOLUME: 	return foreach_dart_of_volume(d, f, thread);
		case VERTEX1: 	return foreach_dart_of_vertex1(d, f, thread);
		case EDGE1: 	return foreach_dart_of_edge1(d, f, thread);
		case VERTEX2: 	return foreach_dart_of_vertex2(d, f, thread);
		case EDGE2:		return foreach_dart_of_edge2(d, f, thread);
		case FACE2:		return foreach_dart_of_face2(d, f, thread);
		default: 		assert(!"Cells of this dimension are not handled"); break;
	}
	return false;
}

/****************************************
 *  TOPOLOGICAL ATTRIBUTES MANAGEMENT   *
 ****************************************/

inline AttributeMultiVector<Dart>* GenericMap::addRelation(const std::string& name)
{
	AttributeContainer& cont = m_attribs[DART] ;
	AttributeMultiVector<Dart>* amv = cont.addAttribute<Dart>(name) ;

	// set new relation to fix point for all the darts of the map
	for(unsigned int i = cont.begin(); i < cont.end(); cont.next(i))
		(*amv)[i] = i ;

	return amv ;
}

inline AttributeMultiVector<Dart>* GenericMap::getRelation(const std::string& name)
{
	AttributeContainer& cont = m_attribs[DART] ;
	AttributeMultiVector<Dart>* amv = cont.getDataVector<Dart>(cont.getAttributeIndex(name)) ;
	return amv ;
}

} //namespace CGoGN
