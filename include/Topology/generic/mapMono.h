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

#include "attribmap.h"

namespace CGoGN
{

class MapMono : public AttribMap
{
	template<typename MAP> friend class DartMarkerTmpl ;

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

	inline unsigned int dartIndex(Dart d) const
	{
		return d.index;
	}

	inline Dart indexDart(unsigned int index) const
	{
		return Dart(index);
	}

	/**
	 * @return the number of darts in the map
	 */
	inline virtual unsigned int getNbDarts();

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
	 *         EMBEDDING MANAGEMENT         *
	 ****************************************/

	/**
	 * get the cell index of the given dimension associated to dart d
	 * @return EMBNULL if the orbit of d is not attached to any cell
	 */
	template<unsigned int ORBIT>
	inline unsigned int getEmbedding(Dart d) const;

	/**
	 * Set the cell index of the given dimension associated to dart d
	 */
	template <unsigned int ORBIT>
	void setDartEmbedding(Dart d, unsigned int emb) ;

	/**
	 * Set the cell index of the given dimension associated to dart d
	 * !!! WARNING !!! use only on freshly inserted darts (no unref is done on old embedding) !!! WARNING !!!
	 */
	template <unsigned int ORBIT>
	void initDartEmbedding(Dart d, unsigned int emb) ;

	/**
	 * Copy the index of the cell associated to a dart over an other dart
	 * @param orbit the id of orbit embedding
	 * @param dest the dart to overwrite
	 * @param src the dart to copy
	 */
	template <unsigned int ORBIT>
	inline void copyDartEmbedding(Dart dest, Dart src) ;

	/**
	* Set the index of the associated cell to all the darts of an orbit
	* @param orbit orbit to embed
	* @param d a dart of the topological vertex
	* @param em index of attribute to store as embedding
	*/
	template <unsigned int ORBIT>
	void setOrbitEmbedding(Dart d, unsigned int em) ;

	/**
	 * Set the index of the associated cell to all the darts of an orbit
	 * !!! WARNING !!! use only on freshly inserted darts (no unref is done on old embedding)!!! WARNING !!!
	 */
	template <unsigned int ORBIT>
	void initOrbitEmbedding(Dart d, unsigned int em) ;

	/**
	* Associate an new cell to all darts of an orbit
	* @param orbit orbit to embed
	* @param d a dart of the topological cell
	* @return index of the attribute in table
	*/
	template <unsigned int ORBIT>
	unsigned int setOrbitEmbeddingOnNewCell(Dart d) ;

	/**
	 * Associate an new cell to all darts of an orbit
	 * !!! WARNING !!! use only on freshly inserted darts (no unref is done on old embedding)!!! WARNING !!!
	 */
	template <unsigned int ORBIT>
	unsigned int initOrbitEmbeddingOnNewCell(Dart d) ;

	/**
	 * Copy the cell associated to a dart over an other dart
	 * @param orbit attribute orbit to use
	 * @param d the dart to overwrite (dest)
	 * @param e the dart to copy (src)
	 */
	template <unsigned int ORBIT>
	void copyCell(Dart d, Dart e) ;

	/**
	 * Traverse the map and embed all orbits of the given dimension with a new cell
	 * @param realloc if true -> all the orbits are embedded on new cells, if false -> already embedded orbits are not impacted
	 */
	template <unsigned int ORBIT>
	void initAllOrbitsEmbedding(bool realloc = false) ;

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
