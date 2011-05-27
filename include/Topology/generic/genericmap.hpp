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

inline bool GenericMap::chechXmlNode(xmlNodePtr node, const std::string& name)
{
	return (strcmp((char*)(node->name),(char*)(name.c_str())) == 0);
}

/****************************************
 *           DARTS MANAGEMENT           *
 ****************************************/

inline Dart GenericMap::newDart()
{
	Dart d = Dart::create(m_attribs[DART_ORBIT].insertLine());
	for(unsigned int i = 0; i < NB_ORBITS; ++i)
		if (m_embeddings[i])
			(*m_embeddings[i])[d.index] = EMBNULL ;
	return d ;
}

inline void GenericMap::deleteDart(Dart d)
{
	m_attribs[DART_ORBIT].removeLine(d.index) ;
	for (unsigned int t = 0; t < m_nbThreads; ++t)
		m_markerTables[DART_ORBIT][t]->operator[](d.index).clear() ;

	for(unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		if (m_embeddings[orbit])
		{
			unsigned int emb = (*m_embeddings[orbit])[d.index] ;
			if(emb != EMBNULL)
			{
				if(m_attribs[orbit].unrefLine(emb))
				{
					for (unsigned int t = 0; t < m_nbThreads; ++t)
						m_markerTables[orbit][t]->operator[](emb).clear() ;
				}
			}
		}
	}
}

inline bool GenericMap::isDartValid(Dart d)
{
	return !d.isNil() && m_attribs[DART_ORBIT].used(d.index) ;
}

inline unsigned int GenericMap::getNbDarts()
{
	return m_attribs[DART_ORBIT].size() ;
}

/****************************************
 *         EMBEDDING MANAGEMENT         *
 ****************************************/

inline bool GenericMap::isOrbitEmbedded(unsigned int orbit) const
{
	return (orbit == DART_ORBIT) || (m_embeddings[orbit] != NULL) ;
}

inline unsigned int GenericMap::nbEmbeddings() const
{
	unsigned int nb = 0;
	for(unsigned int i = 0; i < NB_ORBITS; ++i)
		if (isOrbitEmbedded(i))
			++nb;
	return nb ;
}

inline unsigned int GenericMap::getEmbedding(unsigned int orbit, Dart d)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");

	if (orbit == DART_ORBIT)
		return d.index;

	return (*m_embeddings[orbit])[d.index] ;
}

//inline unsigned int GenericMap::getDartEmbedding(unsigned int orbit, Dart d)
//{
//	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");
//
//	if (orbit == DART_ORBIT)
//		return d.index;
//
//	return (*m_embeddings[orbit])[d.index] ;
//}

inline void GenericMap::copyDartEmbedding(unsigned int orbit, Dart d, Dart e)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");
	setDartEmbedding(orbit, d, getEmbedding(orbit, e));
}

inline unsigned int GenericMap::newCell(unsigned int orbit)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");
	return m_attribs[orbit].insertLine();
}

inline void GenericMap::embedOrbit(unsigned int orbit, Dart d, unsigned int em)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");
	FunctorSetEmb<GenericMap> fsetemb(*this, orbit, em);
	foreach_dart_of_orbit(orbit, d, fsetemb);
}

inline unsigned int GenericMap::embedNewCell(unsigned int orbit, Dart d)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");
	unsigned int em = newCell(orbit);
	embedOrbit(orbit, d, em);
	return em;
}

inline void GenericMap::copyCell(unsigned int orbit, Dart d, Dart e)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");
	unsigned int dE = getEmbedding(orbit, d) ;
	unsigned int eE = getEmbedding(orbit, e) ;
	if(eE != EMBNULL)	// if the source is NULL, nothing to copy
	{
		if(dE == EMBNULL)	// if the dest is NULL, create a new cell
			dE = embedNewCell(orbit, d) ;
		m_attribs[orbit].copyLine(dE, eE) ;	// copy the data
	}
}

inline void GenericMap::copyCell(unsigned int orbit, unsigned int i, unsigned int j)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");
	m_attribs[orbit].copyLine(i, j) ;
}

inline void GenericMap::initCell(unsigned int orbit, unsigned int i)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");
	m_attribs[orbit].initLine(i) ;
}

/****************************************
 *        ATTRIBUTES MANAGEMENT         *
 ****************************************/

inline AttributeContainer& GenericMap::getAttributeContainer(unsigned int orbit)
{
	return m_attribs[orbit] ;
}

inline AttributeMultiVector<Mark>* GenericMap::getMarkerVector(unsigned int orbit, unsigned int thread)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded") ;
	return m_markerTables[orbit][thread] ;
}

inline AttributeMultiVector<unsigned int>* GenericMap::getEmbeddingAttributeVector(unsigned int orbit)
{
	return m_embeddings[orbit] ;
}

inline void GenericMap::swapEmbeddingContainers(unsigned int orbit1, unsigned int orbit2)
{
	assert(orbit1 != orbit2 || !"Cannot swap a container with itself") ;
	assert((orbit1 != DART_ORBIT && orbit2 != DART_ORBIT) || !"Cannot swap the darts container") ;

	m_attribs[orbit1].swap(m_attribs[orbit2]) ;
	m_attribs[orbit1].setOrbit(orbit1) ;	// to update the orbit information
	m_attribs[orbit2].setOrbit(orbit2) ;	// in the contained AttributeMultiVectors

	m_embeddings[orbit1]->swap(m_embeddings[orbit2]) ;

	for(unsigned int t = 0; t < m_nbThreads; ++t)
	{
		AttributeMultiVector<Mark>* m = m_markerTables[orbit1][t] ;
		m_markerTables[orbit1][t] = m_markerTables[orbit2][t] ;
		m_markerTables[orbit2][t] = m ;

		MarkerSet ms = m_orbMarker[orbit1][t] ;
		m_orbMarker[orbit1][t] = m_orbMarker[orbit2][t] ;
		m_orbMarker[orbit2][t] = ms ;
	}
}

/****************************************
 *          MARKERS MANAGEMENT          *
 ****************************************/

inline Marker GenericMap::getNewMarker(unsigned int cell, unsigned int thread)
{
	return m_orbMarker[cell][thread].getNewMarker(cell) ;
}

inline void GenericMap::releaseMarker(Marker m, unsigned int thread)
{
	unsigned int cell = m.getCell();
	assert(isOrbitEmbedded(cell) || !"Try to release a marker on non embedded cell! ") ;
	m_orbMarker[cell][thread].releaseMarker(m) ;
}

/****************************************
 *           DARTS TRAVERSALS           *
 ****************************************/

inline Dart GenericMap::begin()
{
	return Dart::create(m_attribs[DART_ORBIT].begin());
}

inline Dart GenericMap::end()
{
	return Dart::create(m_attribs[DART_ORBIT].end()) ;
}

inline void GenericMap::next(Dart& d)
{
	m_attribs[DART_ORBIT].next(d.index) ;
}

} //namespace CGoGN
