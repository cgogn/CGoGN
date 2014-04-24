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

#ifndef __MAP_COMMON__
#define __MAP_COMMON__

#include "Topology/generic/attributeHandler.h"
#include "Topology/generic/cells.h"

namespace CGoGN
{

template <typename MAP_IMPL>
class MapCommon : public MAP_IMPL
{
	typedef MAP_IMPL IMPL;

	/****************************************
	 *           DARTS TRAVERSALS           *
	 ****************************************/
public:
	/**
	* execute functor for each orbit
	* @param dim the dimension of the orbit
	* @param f the functor
	*/
	template <unsigned int ORBIT>
	bool foreach_orbit(FunctorType& f, unsigned int thread = 0);

	//! Count the number of orbits of dimension dim in the map
	/*! @param dim the dimension of the orbit
	 * 	@return the number of orbits
	 */
	template <unsigned int ORBIT>
	unsigned int getNbOrbits() const;

	unsigned int getNbOrbits(unsigned int orbit) const;

	//! For an orbit of a given dimension, return the number of incident cells of an other given dimension
	/*! @param d a dart
	 */
	template <unsigned int ORBIT, unsigned int INCIDENT>
	unsigned int degree(Dart d) const;

	template <unsigned int ORBIT>
	bool sameOrbit(Cell<ORBIT> c1, Cell<ORBIT> c2, unsigned int thread=0) const;


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
	 *         BOUNDARY MANAGEMENT          *
	 ****************************************/
protected:
	/**
	 * mark a dart as  belonging to boundary
	 */
	template <unsigned int DIM>
	void boundaryMark(Dart d) ;

	/**
	 * unmark a dart from the boundary
	 */
	template <unsigned int DIM>
	void boundaryUnmark(Dart d) ;

	/**
	 * mark an orbit of dart as belonging to boundary
	 */
	template <unsigned int ORBIT, unsigned int DIM>
	void boundaryMarkOrbit(Dart d) ;

	/**
	 * unmark an orbit of dart from the boundary
	 */
	template <unsigned int ORBIT, unsigned int DIM>
	void boundaryUnmarkOrbit(Dart d) ;

	/**
	 * clear all boundary markers
	 */
	template<unsigned int DIM>
	void boundaryUnmarkAll() ;

public:
	/**
	 * test if a dart belong to the boundary
	 */
	template <unsigned int DIM>
	inline bool isBoundaryMarked(Dart d) const ;

	inline bool isBoundaryMarkedCurrent(Dart d) const ;

	inline bool isBoundaryMarked(unsigned int dim, Dart d) const ;

	/****************************************
	 *        ATTRIBUTES MANAGEMENT         *
	 ****************************************/

	/**
	* Create an attribute for a given orbit
	* @param nameAttr attribute name
	* @return an AttributeHandler
	*/
	template <typename T, unsigned int ORBIT>
	inline AttributeHandler<T, ORBIT, MAP_IMPL> addAttribute(const std::string& nameAttr = "") ;

	/**
	 * remove an attribute
	 * @param attr a handler to the attribute to remove
	 * @return true if remove succeed else false
	 */
	template <typename T, unsigned int ORBIT>
	inline bool removeAttribute(AttributeHandler<T, ORBIT, MAP_IMPL>& attr) ;

	/**
	* search an attribute for a given orbit
	* @param nameAttr attribute name
	* @return an AttributeHandler
	*/
	template <typename T, unsigned int ORBIT>
	inline AttributeHandler<T, ORBIT, MAP_IMPL> getAttribute(const std::string& nameAttr) ;

	/**
	* check if an attribute exist ( get, test if valid and add if necessary)
	* @param nameAttr attribute name
	* @return an AttributeHandler
	*/
	template <typename T, unsigned int ORBIT>
	AttributeHandler<T, ORBIT, MAP_IMPL> checkAttribute(const std::string& nameAttr) ;

	/**
	 * swap the content of two attributes (efficient, only swap pointers)
	 */
	template <typename T, unsigned int ORBIT>
	bool swapAttributes(AttributeHandler<T, ORBIT, MAP_IMPL>& attr1, AttributeHandler<T, ORBIT, MAP_IMPL>& attr2) ;

	/**
	 * copy the content of src attribute to dst attribute
	 */
	template <typename T, unsigned int ORBIT>
	bool copyAttribute(AttributeHandler<T, ORBIT, MAP_IMPL>& dst, AttributeHandler<T, ORBIT, MAP_IMPL>& src) ;

	/**
	 * get a DartAttribute to an involution of the map
	 */
	inline DartAttribute<Dart, MAP_IMPL> getInvolution(unsigned int i);

	/**
	 * get a DartAttribute to a permutation of the map
	 */
	inline DartAttribute<Dart, MAP_IMPL> getPermutation(unsigned int i);

	/**
	 * get a DartAttribute to a permutation of the map
	 */
	inline DartAttribute<Dart, MAP_IMPL> getPermutationInv(unsigned int i);

	/****************************************
	 *     QUICK TRAVERSAL MANAGEMENT       *
	 ****************************************/

	template <unsigned int ORBIT>
	void enableQuickTraversal() ;

	template <unsigned int ORBIT>
	void updateQuickTraversal() ;

	template <unsigned int ORBIT>
	const AttributeMultiVector<Dart>* getQuickTraversal() const;

	template <unsigned int ORBIT>
	void disableQuickTraversal() ;

	template <unsigned int ORBIT, unsigned int INCI>
	void enableQuickIncidentTraversal();

	template <unsigned int ORBIT, unsigned int INCI>
	void updateQuickIncidentTraversal();

	template <unsigned int ORBIT, unsigned int INCI>
	const AttributeMultiVector<NoTypeNameAttribute<std::vector<Dart> > >* getQuickIncidentTraversal() const;

	template <unsigned int ORBIT, unsigned int INCI>
	void disableQuickIncidentTraversal();

	template <unsigned int ORBIT, unsigned int ADJ>
	void enableQuickAdjacentTraversal();

	template <unsigned int ORBIT, unsigned int ADJ>
	void updateQuickAdjacentTraversal();

	template <unsigned int ORBIT, unsigned int INCI>
	const AttributeMultiVector<NoTypeNameAttribute<std::vector<Dart> > >* getQuickAdjacentTraversal() const;

	template <unsigned int ORBIT, unsigned int ADJ>
	void disableQuickAdjacentTraversal();

	/****************************************
	 *               UTILITIES              *
	 ****************************************/

	/**
	 * use the given attribute to store the indices of the cells of the corresponding orbit
	 * @return the number of cells of the orbit
	 */
	template <unsigned int ORBIT>
	unsigned int computeIndexCells(AttributeHandler<unsigned int, ORBIT, MAP_IMPL>& idx) ;

	/**
	 * ensure that each orbit as one embedding and that each embedding is handle by only one orbit
	 */
	template <unsigned int ORBIT>
	void bijectiveOrbitEmbedding();
};

} //namespace CGoGN

#include "Topology/generic/mapCommon.hpp"

#endif
