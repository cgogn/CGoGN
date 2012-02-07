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

namespace CGoGN
{

inline unsigned int GenericMap::dartIndex(Dart d)
{
	if (m_isMultiRes)
		return m_mrDarts[m_mrCurrentLevel]->operator[](d.index) ;
	return d.index;
}

/****************************************
 *           MULTIRES                   *
 ****************************************/

inline unsigned int GenericMap::getCurrentLevel()
{
	return m_mrCurrentLevel ;
}

inline void GenericMap::setCurrentLevel(unsigned int l)
{
	if(l < m_mrDarts.size())
		m_mrCurrentLevel = l ;
	else
		CGoGNout << "try to access inexisting resolution level" << CGoGNendl ;
}

inline void GenericMap::pushLevel()
{
	m_mrLevelStack.push_back(m_mrCurrentLevel) ;
}

inline void GenericMap::popLevel()
{
	m_mrCurrentLevel = m_mrLevelStack.back() ;
	m_mrLevelStack.pop_back() ;
}

inline unsigned int GenericMap::getMaxLevel()
{
	return m_mrDarts.size() - 1 ;
}

inline unsigned int GenericMap::getDartLevel(Dart d)
{
	return m_mrLevels->operator [](d.index) ;
}

/****************************************
 *           DARTS MANAGEMENT           *
 ****************************************/

inline Dart GenericMap::newDart()
{
	unsigned int di = m_attribs[DART].insertLine();
	for(unsigned int i = 0; i < NB_ORBITS; ++i)
	{
		if (m_embeddings[i])
			(*m_embeddings[i])[di] = EMBNULL ;
	}
	if (m_isMultiRes)
	{
		unsigned int mrdi = m_mrattribs.insertLine() ;
		m_mrLevels->operator[](mrdi) = m_mrCurrentLevel ;
		for(unsigned int i = 0; i < m_mrCurrentLevel; ++i)
			m_mrDarts[i]->operator[](mrdi) = MRNULL ;
		for(unsigned int i = m_mrCurrentLevel; i < m_mrDarts.size(); ++i)
			m_mrDarts[i]->operator[](mrdi) = di ;
		return Dart::create(mrdi) ;
	}
	return Dart::create(di) ;
}

inline void GenericMap::deleteDart(Dart d)
{
	unsigned int d_index = dartIndex(d);
	m_attribs[DART].removeLine(d_index) ;
	for (unsigned int t = 0; t < m_nbThreads; ++t)
		m_markTables[DART][t]->operator[](d_index).clear() ;

	for(unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		if (m_embeddings[orbit])
		{
			unsigned int emb = (*m_embeddings[orbit])[d_index] ;
			if(emb != EMBNULL)
			{
				if(m_attribs[orbit].unrefLine(emb))
				{
					for (unsigned int t = 0; t < m_nbThreads; ++t)
						m_markTables[orbit][t]->operator[](emb).clear() ;
				}
			}
		}
	}

	// hypothese : le brin MR pointe vers le même brin pour tous les niveaux >= au courant
	if(m_isMultiRes)
	{
		if(m_mrCurrentLevel == 0)
			m_mrattribs.removeLine(d.index) ;
		else
		{
			unsigned int di = m_mrDarts[m_mrCurrentLevel - 1]->operator[](d.index) ;
			for(unsigned int i = m_mrCurrentLevel; i < m_mrDarts.size(); ++i)
				m_mrDarts[i]->operator[](d.index) = di ;
		}
	}
}

inline bool GenericMap::isDartValid(Dart d)
{
	return !d.isNil() && m_attribs[DART].used(dartIndex(d)) ;
}

inline unsigned int GenericMap::getNbDarts()
{
	if(m_isMultiRes)
	{
		unsigned int nbDarts = 0 ;
		for(unsigned int i = m_mrattribs.begin(); i != m_mrattribs.end(); m_mrattribs.next(i))
			if(m_mrLevels->operator[](i) <= m_mrCurrentLevel)
				++nbDarts ;
		return nbDarts ;
	}
	return m_attribs[DART].size() ;
}

/****************************************
 *         EMBEDDING MANAGEMENT         *
 ****************************************/

inline bool GenericMap::isOrbitEmbedded(unsigned int orbit) const
{
	return (orbit == DART) || (m_embeddings[orbit] != NULL) ;
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

	unsigned int d_index = dartIndex(d);

	if (orbit == DART)
		return d_index;

	return (*m_embeddings[orbit])[d_index] ;
}

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

inline AttributeMultiVector<Mark>* GenericMap::getMarkVector(unsigned int orbit, unsigned int thread)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded") ;
	return m_markTables[orbit][thread] ;
}

inline AttributeMultiVector<unsigned int>* GenericMap::getEmbeddingAttributeVector(unsigned int orbit)
{
	return m_embeddings[orbit] ;
}

/****************************************
 *           DARTS TRAVERSALS           *
 ****************************************/

inline Dart GenericMap::begin()
{
	return Dart::create(m_attribs[DART].begin());
}

inline Dart GenericMap::end()
{
	return Dart::create(m_attribs[DART].end()) ;
}

inline void GenericMap::next(Dart& d)
{
	unsigned int d_index = dartIndex(d);
	m_attribs[DART].next(d_index) ;

	d = Dart::create(d_index);
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
		amv->operator[](i) = i ;

	return amv ;
}

inline AttributeMultiVector<Dart>* GenericMap::getRelation(const std::string& name)
{
	AttributeContainer& cont = m_attribs[DART] ;
	AttributeMultiVector<Dart>* amv = cont.getDataVector<Dart>(cont.getAttributeIndex(name)) ;
	return amv ;
}


/******************************************
 *  TOPOLOGICAL MULTIRESOLUTION MANAGEMENT*
 ******************************************/

inline unsigned int GenericMap::getCurrentLevel()
{
	return m_mrCurrentLevel ;
}


inline void GenericMap::setCurrentLevel(unsigned int l)
{
	if (l < m_mrDarts.size())
		m_mrCurrentLevel = l ;
	else
		CGoGNout << "try to access inexisting resolution level" << CGoGNendl ;
}

inline void GenericMap::pushLevel()
{
	m_mrLevelStack.push_back(m_mrCurrentLevel) ;
}


inline void GenericMap::popLevel()
{
	m_mrCurrentLevel = m_mrLevelStack.back() ;
	m_mrLevelStack.pop_back() ;
}


inline unsigned int GenericMap::getMaxLevel()
{
	return m_mrDarts.size() - 1 ;
}


} //namespace CGoGN
