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

#ifndef __MAP_MULTI__
#define __MAP_MULTI__

#include "attribmap.h"

namespace CGoGN
{

class MapMulti : public AttribMap
{
public:
	MapMulti()
	{
		initMR();
	}

	virtual void clear(bool removeAttrib)
	{
		AttribMap::clear(removeAttrib);
		initMR();
	}

protected:
	std::vector<AttributeMultiVector<Dart>*> m_permutation;
	std::vector<AttributeMultiVector<Dart>*> m_permutation_inv;
	std::vector<AttributeMultiVector<Dart>*> m_involution;

	/**
	 * container for multiresolution darts
	 */
	AttributeContainer m_mrattribs ;

	/**
	 * pointers to attributes of m_mrattribs that store indices of m_attribs[DART]
	 * (one for each level)
	 */
	std::vector< AttributeMultiVector<unsigned int>* > m_mrDarts ;

	/**
	 * pointer to attribute of m_mrattribs that stores darts insertion levels
	 */
	AttributeMultiVector<unsigned int>* m_mrLevels ;

	/**
	 * vector that stores the number of darts inserted on each resolution level
	 */
	std::vector<unsigned int> m_mrNbDarts ;

	/**
	 * current level in multiresolution map
	 */
	unsigned int m_mrCurrentLevel ;

	/**
	 * stack for current level temporary storage
	 */
	std::vector<unsigned int> m_mrLevelStack ;

	/****************************************
	 *          DARTS MANAGEMENT            *
	 ****************************************/

	inline virtual Dart newDart();

	inline virtual void deleteDart(Dart d);

	inline virtual unsigned int dartIndex(Dart d) const;

	/**
	 * get the number of darts inserted in the given leveldart
	 */
	unsigned int getNbInsertedDarts(unsigned int level) ;

	/**
	 * get the number of darts that define the map of the given leveldart
	 */
	virtual unsigned int getNbDarts(unsigned int level) ;

	/**
	 * @return the number of darts in the map
	 */
	virtual unsigned int getNbDarts() ;

	/**
	 * get the insertion level of a dart
	 */
	unsigned int getDartLevel(Dart d) const ;

	/**
	 *
	 */
	void incDartLevel(Dart d) const ;

	/**
	 * duplicate a dart starting from current level
	 */
	void duplicateDart(Dart d) ;

	void duplicateDartAtOneLevel(Dart d, unsigned int level) ;

	/****************************************
	 *        RELATIONS MANAGEMENT          *
	 ****************************************/

	inline void addInvolution();

	inline void addPermutation();

	template <int I>
	inline Dart getInvolution(Dart d) const;

	template <int I>
	inline Dart getPermutation(Dart d) const;

	template <int I>
	inline Dart getPermutationInv(Dart d) const;

	template <int I>
	inline void permutationSew(Dart d, Dart e);

	template <int I>
	inline void permutationUnsew(Dart d);

	template <int I>
	inline void involutionSew(Dart d, Dart e);

	template <int I>
	inline void involutionUnsew(Dart d);

	inline virtual void compactTopoRelations(const std::vector<unsigned int>& oldnew);

	/****************************************
	 *      MR CONTAINER MANAGEMENT         *
	 ****************************************/

	/**
	 * get the MR attribute container
	 */
	AttributeContainer& getMRAttributeContainer() ;

	/**
	 * get the MR attribute container
	 */
	AttributeMultiVector<unsigned int>* getMRDartAttributeVector(unsigned int level) ;
	AttributeMultiVector<unsigned int>* getMRLevelAttributeVector();

	/****************************************
	 *     RESOLUTION LEVELS MANAGEMENT     *
	 ****************************************/

	void printMR() ;

	/**
	 * initialize the multiresolution attribute container
	 */
	void initMR() ;

	/**
	 * get the current resolution level
	 */
	unsigned int getCurrentLevel() ;

	/**
	 * set the current resolution level
	 */
	void setCurrentLevel(unsigned int l) ;

	/**
	 * increment the current resolution level
	 */
	void incCurrentLevel() ;

	/**
	 * decrement the current resolution level
	 */
	void decCurrentLevel() ;

	/**
	 * store current resolution level on a stack
	 */
	void pushLevel() ;

	/**
	 * set as current the resolution level of the top of the stack
	 */
	void popLevel() ;

	/**
	 * get the maximum resolution level
	 */
	unsigned int getMaxLevel() ;

	/**
	 * add a resolution level in the back of the level table
	 */
	void addLevelBack() ;

	/**
	 * add a resolution level in the front of the level table
	 */
	void addLevelFront();

	/**
	 * remove last resolution level
	 */
	void removeLevelBack() ;

	/**
	 * remove first resolution level
	 */
	void removeLevelFront();

	/**
	 * copy MRDarts from level-1 to level
	 */
	void copyLevel(unsigned int level);

	/**
	 * duplicate darts from level-1 to level
	 */
	void duplicateDarts(unsigned int newlevel);

	/****************************************
	 *           DARTS TRAVERSALS           *
	 ****************************************/

	/**
	 * Begin of map
	 * @return the first dart of the map
	 */
	inline virtual Dart begin() const;

	/**
	 * End of map
	 * @return the end iterator (next of last) of the map
	 */
	inline virtual Dart end() const;

	/**
	 * allow to go from a dart to the next
	 * in the order of storage
	 * @param d reference to the dart to be modified
	 */
	inline virtual void next(Dart& d) const;
} ;

} //namespace CGoGN

#include "Topology/generic/mapMulti.hpp"

#endif
