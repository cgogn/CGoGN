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

namespace CGoGN
{

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
		CGoGNout << "setCurrentLevel : try to access nonexistent resolution level" << CGoGNendl ;
}

inline void GenericMap::incCurrentLevel()
{
	if(m_mrCurrentLevel < m_mrDarts.size() - 1)
		++m_mrCurrentLevel ;
	else
		CGoGNout << "incCurrentLevel : already at maximum resolution level" << CGoGNendl ;
}

inline void GenericMap::decCurrentLevel()
{
	if(m_mrCurrentLevel > 0)
		--m_mrCurrentLevel ;
	else
		CGoGNout << "decCurrentLevel : already at minimum resolution level" << CGoGNendl ;
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

/****************************************
 *           DARTS MANAGEMENT           *
 ****************************************/

inline Dart GenericMap::newDart()
{
	unsigned int di = m_attribs[DART].insertLine();			// insert a new dart line
	for(unsigned int i = 0; i < NB_ORBITS; ++i)
	{
		if (m_embeddings[i])								// set all its embeddings
			(*m_embeddings[i])[di] = EMBNULL ;				// to EMBNULL
	}

	if (m_isMultiRes)
	{
		unsigned int mrdi = m_mrattribs.insertLine() ;		// insert a new MRdart line
		(*m_mrLevels)[mrdi] = m_mrCurrentLevel ;			// set the introduction level of the dart
		m_mrNbDarts[m_mrCurrentLevel]++ ;

		for(unsigned int i = 0; i < m_mrCurrentLevel; ++i)	// for all previous levels
			(*m_mrDarts[i])[mrdi] = MRNULL ;				// this MRdart does not exist

		for(unsigned int i = m_mrCurrentLevel; i < m_mrDarts.size(); ++i)	// for all levels from current to max
			(*m_mrDarts[i])[mrdi] = di ;									// make this MRdart points to the new dart line

		return Dart::create(mrdi) ;
	}

	return Dart::create(di) ;
}

inline void GenericMap::deleteDart(Dart d)
{
	if(m_isMultiRes)
	{
		// a MRdart can only be deleted on its insertion level
		assert(getDartLevel(d) == m_mrCurrentLevel || !"deleteDart : try to delete a dart on a level greater than its insertion level") ;

		for(unsigned int i = m_mrCurrentLevel; i < m_mrDarts.size(); ++i)
		{
			unsigned int index = (*m_mrDarts[i])[d.index] ;
			if(isDartValid(index))
				deleteDartLine(index) ;
		}
		m_mrattribs.removeLine(d.index) ;
		m_mrNbDarts[m_mrCurrentLevel]-- ;
	}
	else
		deleteDartLine(dartIndex(d)) ;
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
//			(*m_embeddings[orbit])[newindex] = EMBNULL ;
		{
			unsigned int emb = (*m_embeddings[orbit])[newindex] ;	// add a ref to the cells pointed
			if(emb != EMBNULL)										// by the new dart line
				m_attribs[orbit].refLine(emb) ;
		}
	}
	return newindex ;
}

inline void GenericMap::duplicateDart(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"duplicateDart : called with a dart inserted after current level") ;

	if(getDartLevel(d) == getCurrentLevel())	// no need to duplicate
		return ;								// a dart from its insertion level

	unsigned int oldindex = dartIndex(d) ;

	if((*m_mrDarts[m_mrCurrentLevel - 1])[d.index] != oldindex)	// no need to duplicate if the dart is already
		return ;												// duplicated with respect to previous level

	unsigned int newindex = copyDartLine(oldindex) ;

	for(unsigned int i = getCurrentLevel(); i <= getMaxLevel(); ++i) // for all levels from current to max
	{
		assert((*m_mrDarts[i])[d.index] == oldindex || !"duplicateDart : dart was already duplicated on a greater level") ;
		(*m_mrDarts[i])[d.index] = newindex ;						// make this MRdart points to the new dart line
	}
}

inline unsigned int GenericMap::dartIndex(Dart d) const
{
	if (m_isMultiRes)
		return (*m_mrDarts[m_mrCurrentLevel])[d.index] ;
	return d.index;
}

inline unsigned int GenericMap::getDartLevel(Dart d) const
{
	return (*m_mrLevels)[d.index] ;
}

inline unsigned int GenericMap::getNbInsertedDarts(unsigned int level)
{
	if(level < m_mrDarts.size())
		return m_mrNbDarts[level] ;
	else
		return 0 ;
}

inline unsigned int GenericMap::getNbDarts(unsigned int level)
{
	if(level < m_mrDarts.size())
	{
		unsigned int nb = 0 ;
		for(unsigned int i = 0; i <= level; ++i)
			nb += m_mrNbDarts[i] ;
		return nb ;
	}
	else
		return 0 ;
}

inline unsigned int GenericMap::getNbDarts()
{
	if(m_isMultiRes)
		return getNbDarts(m_mrCurrentLevel) ;

	return m_attribs[DART].size() ;
}

inline bool GenericMap::isDartValid(Dart d)
{
	return !d.isNil() && m_attribs[DART].used(dartIndex(d)) ;
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

inline void GenericMap::copyDartEmbedding(unsigned int orbit, Dart dest, Dart src)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");
	setDartEmbedding(orbit, dest, getEmbedding(orbit, src));
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

inline Dart GenericMap::begin() const
{
	if (m_isMultiRes)
	{
		unsigned int d = m_mrattribs.begin() ;
		while (getDartLevel(d) > m_mrCurrentLevel)
			m_mrattribs.next(d) ;
		return Dart::create(d) ;
	}

	return Dart::create(m_attribs[DART].begin()) ;
}

inline Dart GenericMap::end() const
{
	if (m_isMultiRes)
		return Dart::create(m_mrattribs.end()) ;

	return Dart::create(m_attribs[DART].end()) ;
}

inline void GenericMap::next(Dart& d) const
{
	if (m_isMultiRes)
	{
		do
		{
			m_mrattribs.next(d.index) ;
		} while (d.index != m_mrattribs.end() && getDartLevel(d) > m_mrCurrentLevel) ;
	}
	else
		m_attribs[DART].next(d.index) ;
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


/**************************
 *  BOUNDARY MANAGEMENT   *
 **************************/

inline void GenericMap::boundaryMark(Dart d)
{
	m_markTables[DART][0]->operator[](dartIndex(d)).setMark(m_boundaryMarker);
}

inline void GenericMap::boundaryUnmark(Dart d)
{
	m_markTables[DART][0]->operator[](dartIndex(d)).unsetMark(m_boundaryMarker);
}

inline bool GenericMap::isBoundaryMarked(Dart d) const
{
	return m_markTables[DART][0]->operator[](dartIndex(d)).testMark(m_boundaryMarker);
}


} //namespace CGoGN
