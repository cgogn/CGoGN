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
 *          DARTS MANAGEMENT            *
 ****************************************/

inline virtual Dart MapMulti::newDart()
{
	Dart d = GenericMap::newDart() ;

	unsigned int mrdi = m_mrattribs.insertLine() ;	// insert a new MRdart line
	(*m_mrLevels)[mrdi] = m_mrCurrentLevel ;		// set the introduction level of the dart
	m_mrNbDarts[m_mrCurrentLevel]++ ;

	for(unsigned int i = 0; i < m_mrCurrentLevel; ++i)	// for all previous levels
		(*m_mrDarts[i])[mrdi] = MRNULL ;				// this MRdart does not exist

	for(unsigned int i = m_mrCurrentLevel; i < m_mrDarts.size(); ++i)	// for all levels from current to max
		(*m_mrDarts[i])[mrdi] = d.index ;								// make this MRdart point to the new dart line

	pushLevel() ;
	for(unsigned int i = m_mrCurrentLevel;  i < m_mrDarts.size(); ++i)
	{
		setCurrentLevel(i) ;

		unsigned int d_index = dartIndex(d) ;

		for (unsigned int i = 0; i < m_permutation.size(); ++i)
			(*m_permutation[i])[d_index] = d ;
		for (unsigned int i = 0; i < m_permutation_inv.size(); ++i)
			(*m_permutation_inv[i])[d_index] = d ;
		for (unsigned int i = 0; i < m_involution.size(); ++i)
			(*m_involution[i])[d_index] = d ;
	}
	popLevel() ;

	return Dart::create(mrdi) ;
}

inline virtual void MapMulti::deleteDart(Dart d)
{
	unsigned int index = (*m_mrDarts[m_mrCurrentLevel])[d.index] ;

	if(getDartLevel(d) > m_mrCurrentLevel)
	{
		unsigned int di = (*m_mrDarts[m_mrCurrentLevel + 1])[d.index];
		// si le brin de niveau i pointe sur le meme brin que le niveau i-1
		if(di != index)
		{
			if(isDartValid(d))//index))
				deleteDartLine(index) ;
		}

		(*m_mrDarts[m_mrCurrentLevel])[d.index] = MRNULL ;
		return;
	}

	// a MRdart can only be deleted on its insertion level
	if(getDartLevel(d) == m_mrCurrentLevel)
	{
		if(isDartValid(d))
		{
			deleteDartLine(index) ;
			m_mrattribs.removeLine(d.index);
			m_mrNbDarts[m_mrCurrentLevel]--;
		}
	}
	else
	{
		unsigned int di = (*m_mrDarts[m_mrCurrentLevel - 1])[d.index];
		// si le brin de niveau i pointe sur le meme brin que le niveau i-1
		if(di != index)
		{
			if(isDartValid(d))//index))
				deleteDartLine(index) ;
		}

		for(unsigned int i = m_mrCurrentLevel; i <= getMaxLevel(); ++i) // for all levels from current to max
			(*m_mrDarts[i])[d.index] = di ; //copy the index from previous level
	}
}

inline virtual unsigned int MapMulti::dartIndex(Dart d) const
{
	return (*m_mrDarts[m_mrCurrentLevel])[d.index] ;
}

inline unsigned int MapMulti::getNbInsertedDarts(unsigned int level)
{
	if(level < m_mrDarts.size())
		return m_mrNbDarts[level] ;
	else
		return 0 ;
}

inline unsigned int MapMulti::getNbDarts(unsigned int level)
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

inline unsigned int MapMulti::getNbDarts()
{
	return getNbDarts(m_mrCurrentLevel) ;
}

inline unsigned int MapMulti::getDartLevel(Dart d) const
{
	return (*m_mrLevels)[d.index] ;
}

inline void MapMulti::incDartLevel(Dart d) const
{
	++((*m_mrLevels)[d.index]) ;
}

inline void MapMulti::duplicateDart(Dart d)
{
	assert(getDartLevel(d) <= m_mrCurrentLevel || !"duplicateDart : called with a dart inserted after current level") ;

	if(getDartLevel(d) == m_mrCurrentLevel)	// no need to duplicate
		return ;							// a dart from its insertion level

	unsigned int oldindex = dartIndex(d) ;

	if(m_mrCurrentLevel > 0)
	{
		if((*m_mrDarts[m_mrCurrentLevel - 1])[d.index] != oldindex)	// no need to duplicate if the dart is already
			return ;												// duplicated with respect to previous level
	}

	unsigned int newindex = copyDartLine(oldindex) ;

	for(unsigned int i = m_mrCurrentLevel; i <= getMaxLevel(); ++i) // for all levels from current to max
	{
		assert((*m_mrDarts[i])[d.index] == oldindex || !"duplicateDart : dart was already duplicated on a greater level") ;
		(*m_mrDarts[i])[d.index] = newindex ;	// make this MRdart points to the new dart line
	}
}

inline void MapMulti::duplicateDartAtOneLevel(Dart d, unsigned int level)
{
	(*m_mrDarts[level])[d.index] = copyDartLine(dartIndex(d)) ;
}

/****************************************
 *        RELATIONS MANAGEMENT          *
 ****************************************/

inline void MapMulti::addInvolution()
{
	std::stringstream sstm;
	sstm << "involution_" << m_involution.size();
	m_involution.push_back(addRelation(sstm.str()));
}

inline void MapMulti::addPermutation()
{
	std::stringstream sstm;
	sstm << "permutation_" << m_permutation.size();
	m_permutation.push_back(addRelation(sstm.str()));
	std::stringstream sstm2;
	sstm2 << "permutation_inv_" << m_permutation_inv.size();
	m_permutation_inv.push_back(addRelation(sstm2.str()));
}

template <int I>
inline Dart MapMulti::getInvolution(Dart d)
{
	return (*m_involution[I])[dartIndex(d)];
}

template <int I>
inline Dart MapMulti::getPermutation(Dart d)
{
	return (*m_permutation[I])[dartIndex(d)];
}

template <int I>
inline Dart MapMulti::getPermutationInv(Dart d)
{
	return (*m_permutation_inv[I])[dartIndex(d)];
}

template <int I>
inline void MapMulti::permutationSew(Dart d, Dart e)
{
	unsigned int d_index = dartIndex(d);
	unsigned int e_index = dartIndex(e);
	Dart f = (*m_permutation[I])[d_index] ;
	Dart g = (*m_permutation[I])[e_index] ;
	(*m_permutation[I])[d_index] = g ;
	(*m_permutation[I])[e_index] = f ;
	(*m_permutation_inv[I])[dartIndex(g)] = d ;
	(*m_permutation_inv[I])[dartIndex(f)] = e ;
}

template <int I>
inline void MapMulti::permutationUnsew(Dart d)
{
	unsigned int d_index = dartIndex(d);
	Dart e = (*m_permutation[I])[d_index] ;
	unsigned int e_index = dartIndex(e);
	Dart f = (*m_permutation[I])[e_index] ;
	(*m_permutation[I])[d_index] = f ;
	(*m_permutation[I])[e_index] = e ;
	(*m_permutation_inv[I])[dartIndex(f)] = d ;
	(*m_permutation_inv[I])[e_index] = e ;
}

template <int I>
inline void MapMulti::involutionSew(Dart d, Dart e)
{
	assert((*m_phi2)[dartIndex(d)] == d) ;
	assert((*m_phi2)[dartIndex(e)] == e) ;
	(*m_involution[I])[dartIndex(d)] = e ;
	(*m_involution[I])[dartIndex(e)] = d ;
}

template <int I>
inline void MapMulti::involutionUnsew(Dart d)
{
	unsigned int d_index = dartIndex(d);
	Dart e = (*m_involution[I])[d_index] ;
	(*m_involution[I])[d_index] = d ;
	(*m_involution[I])[dartIndex(e)] = e ;
}

/****************************************
 *      MR CONTAINER MANAGEMENT         *
 ****************************************/

inline AttributeContainer& MapMulti::getMRAttributeContainer()
{
	return m_mrattribs ;
}

inline AttributeMultiVector<unsigned int>* MapMulti::getMRDartAttributeVector(unsigned int level)
{
	assert(level <= getMaxLevel() || !"Invalid parameter: level does not exist");
	return m_mrDarts[level] ;
}

inline AttributeMultiVector<unsigned int>* MapMulti::getMRLevelAttributeVector()
{
	return m_mrLevels ;
}

/****************************************
 *     RESOLUTION LEVELS MANAGEMENT     *
 ****************************************/

inline unsigned int MapMulti::getCurrentLevel()
{
	return m_mrCurrentLevel ;
}

inline void MapMulti::setCurrentLevel(unsigned int l)
{
	if(l < m_mrDarts.size())
		m_mrCurrentLevel = l ;
	else
		CGoGNout << "setCurrentLevel : try to access nonexistent resolution level" << CGoGNendl ;
}

inline void MapMulti::incCurrentLevel()
{
	if(m_mrCurrentLevel < m_mrDarts.size() - 1)
		++m_mrCurrentLevel ;
	else
		CGoGNout << "incCurrentLevel : already at maximum resolution level" << CGoGNendl ;
}

inline void MapMulti::decCurrentLevel()
{
	if(m_mrCurrentLevel > 0)
		--m_mrCurrentLevel ;
	else
		CGoGNout << "decCurrentLevel : already at minimum resolution level" << CGoGNendl ;
}

inline void MapMulti::pushLevel()
{
	m_mrLevelStack.push_back(m_mrCurrentLevel) ;
}

inline void MapMulti::popLevel()
{
	m_mrCurrentLevel = m_mrLevelStack.back() ;
	m_mrLevelStack.pop_back() ;
}

inline unsigned int MapMulti::getMaxLevel()
{
	return m_mrDarts.size() - 1 ;
}

/****************************************
 *           DARTS TRAVERSALS           *
 ****************************************/

inline virtual Dart begin() const
{
	unsigned int d = m_mrattribs.begin() ;
	if(d != m_mrattribs.end())
	{
		while (getDartLevel(d) > m_mrCurrentLevel)
			m_mrattribs.next(d) ;
	}
	return Dart::create(d) ;
}

inline virtual Dart end() const
{
	return Dart::create(m_mrattribs.end()) ;
}

inline virtual void next(Dart& d) const
{
	do
	{
		m_mrattribs.next(d.index) ;
	} while (d.index != m_mrattribs.end() && getDartLevel(d) > m_mrCurrentLevel) ;
}

} // namespace CGoGN
