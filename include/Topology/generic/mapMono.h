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

#ifndef __MAP_MONO__
#define __MAP_MONO__

#include "Topology/generic/genericmap.h"

namespace CGoGN
{

class MapMono : public GenericMap
{
	template<typename MAP> friend class DartMarkerTmpl ;
	template<typename MAP> friend class DartMarkerStore ;

public:
	MapMono()
	{}

protected:
	std::vector<AttributeMultiVector<Dart>*> m_permutation;
	std::vector<AttributeMultiVector<Dart>*> m_permutation_inv;
	std::vector<AttributeMultiVector<Dart>*> m_involution;

	/****************************************
	 *          DARTS MANAGEMENT            *
	 ****************************************/

	inline virtual Dart newDart();

	inline virtual void deleteDart(Dart d);

public:
	inline unsigned int dartIndex(Dart d) const;

	inline Dart indexDart(unsigned int index) const;

	inline virtual unsigned int getNbDarts();

protected:
	inline AttributeContainer& getDartContainer();

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
	 *           DARTS TRAVERSALS           *
	 ****************************************/
public:
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

	/****************************************
	 *             SAVE & LOAD              *
	 ****************************************/

	bool saveMapBin(const std::string& filename) const;

	bool loadMapBin(const std::string& filename);

	bool copyFrom(const GenericMap& map);
} ;

} //namespace CGoGN

#include "Topology/generic/mapMono.hpp"

#endif
