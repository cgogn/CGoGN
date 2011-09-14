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

#ifndef __GENERIC_MAP__
#define __GENERIC_MAP__

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <list>
#include <vector>
#include <map>

#include "Container/attributeContainer.h"

#include "Topology/generic/dart.h"
#include "Topology/generic/marker.h"
#include "Topology/generic/functor.h"

namespace CGoGN
{

/*
const unsigned int EMBNULL = 0xffffffff;

const unsigned int NB_ORBITS = 6;
const unsigned int NB_THREAD = 16;

const unsigned int VERTEX	= 0;
const unsigned int EDGE		= 1;
const unsigned int ORIENTED_FACE= 2;
const unsigned int FACE		= 3;
const unsigned int VOLUME	= 4;
const unsigned int DART		= 5;
*/

/**
 * Class that allows to browse a map
 * This is a pure virtual class that can be overloaded in:
 * - a map (generic and so on)
 * - a MapBrowserSelector
 * - a MapBrowserLinked (Attr or Auto)
 */
class MapBrowser
{
public:
	virtual Dart begin() = 0;
	virtual Dart end() = 0;
	virtual void next(Dart& d) = 0;
};

class AttributeHandlerGen ;

class GenericMap : public MapBrowser
{

	template<typename T> friend class AttributeHandler ;
	template<typename T> friend class AutoAttributeHandler ;

protected:
	/**
	 * Attributes Containers
	 */
	AttributeContainer m_attribs[NB_ORBITS] ;

	static std::map<std::string, RegisteredBaseAttribute*>* m_attributes_registry_map ;

	/**
	 * Direct access to the Dart attributes that store the orbits embeddings
	 * (only initialized when necessary)
	 */
	AttributeMultiVector<unsigned int>* m_embeddings[NB_ORBITS] ;

	/**
	 * Marks manager
	 */
	MarkSet m_marksets[NB_ORBITS][NB_THREAD] ;

	/**
	 * Direct access to the attributes that store Marks
	 */
	AttributeMultiVector<Mark>* m_markTables[NB_ORBITS][NB_THREAD] ;

	unsigned int m_nbThreads ;

	std::multimap<AttributeMultiVectorGen*, AttributeHandlerGen*> attributeHandlers ;

public:
	static const unsigned int UNKNOWN_ATTRIB = AttributeContainer::UNKNOWN ;

	GenericMap() ;

	~GenericMap() ;

	virtual std::string mapTypeName() = 0 ;

	/**
	 * Clear the map
	 * @param removeAttrib
	 *   if false -> data is deleted but all attributes remain (all AttributeHandlers are still valid)
	 *   if true -> data and attributes are deleted (AttributeHandlers are invalid)
	 */
	virtual void clear(bool removeAttrib) ;

	/**
	 * get the marker_set of an orbit and thread (used for Cell & Dart Marker)
	 */
	MarkSet& getMarkerSet(unsigned int orbit, unsigned int thread) { return m_marksets[orbit][thread]; }

	/****************************************
	 *           DARTS MANAGEMENT           *
	 ****************************************/
protected:
	/**
	 * Add a dart to the map
	 */
	virtual Dart newDart() ;

	/**
	 * Erase a dart of the map
	 */
	void deleteDart(Dart d) ;

public:
	/**
	 * return true if the dart d refers to a valid index
	 */
	bool isDartValid(Dart d) ;

	/**
	 * @return the number of darts in the map
	 */
	unsigned int getNbDarts() ;

	/****************************************
	 *         EMBEDDING MANAGEMENT         *
	 ****************************************/

	/**
	 * tell if an orbit is embedded or not
	 */
	bool isOrbitEmbedded(unsigned int orbit) const ;

	/**
	 * return the number of embedded orbits (including DART)
	 */
	unsigned int nbEmbeddings() const ;

	/**
	 * get the cell index of the given dimension associated to dart d
	 * (can go through the whole orbit due to lazy embedding)
	 * @return EMBNULL if the orbit of d is not attached to any cell
	 */
	unsigned int getEmbedding(unsigned int orbit, Dart d) ;

	/**
	 * Set the cell index of the given dimension associated to dart d
	 */
	void setDartEmbedding(unsigned int orbit, Dart d, unsigned int emb) ;

	/**
	 * Copy the index of the cell associated to a dart over an other dart
	 * @param d the dart to overwrite (dest)
	 * @param e the dart to copy (src)
	 * @param orbit the id of orbit embedding
	 */
	void copyDartEmbedding(unsigned int orbit, Dart d, Dart e) ;

	/**
	 * Allocation of some place in attrib table
	 * @param orbit the orbit of embedding
	 * @return the index to use as embedding
	 */
	unsigned int newCell(unsigned int orbit) ;

	/**
	* Set the index of the associated cell to all the darts of an orbit
	* @param orbit orbit to embed
	* @param d a dart of the topological vertex
	* @param em index of attribute to store as embedding
	*/
	void embedOrbit(unsigned int orbit, Dart d, unsigned int em) ;

	/**
	* Associate an new embedding to all darts of a vertex
	* @param orbit orbit to embed
	* @param d a dart of the topological cell
	* @return index of the attribute in table
	*/
	unsigned int embedNewCell(unsigned int orbit, Dart d) ;

	/**
	* Copy the cell associated to a dart over an other dart
	 * @param orbit attribute orbit to use
	* @param d the dart to overwrite (dest)
	* @param e the dart to copy (src)
	 */
	void copyCell(unsigned int orbit, Dart d, Dart e) ;

	/**
	 * Line of attributes i is overwritten with line j
	 * @param orbit attribute orbit to use
	 * @param i line destination of copy
	 * @param j line source of copy
	 */
	void copyCell(unsigned int orbit, unsigned int i, unsigned int j) ;

	/**
	 * Line of attributes i is initialized
	 * @param orbit attribute orbit to use
	 * @param i line to init
	 */
	void initCell(unsigned int orbit, unsigned int i) ;

	/****************************************
	 *        ATTRIBUTES MANAGEMENT         *
	 ****************************************/

	/**
	 * get the attrib container of a given orbit
	 * @param orbit the orbit !!! (bilbo the orbit !)
	 */
	AttributeContainer& getAttributeContainer(unsigned int orbit) ;

	/**
	 * get a multi vector of mark attribute (direct access with [i])
	 * @param orbit code
	 */
	AttributeMultiVector<Mark>* getMarkVector(unsigned int orbit, unsigned int thread = 0) ;

	/**
	 * return a pointer to the Dart attribute vector that store the embedding of the given orbit
	 * (may be NULL if the orbit is not embedded)
	 */
	AttributeMultiVector<unsigned int>* getEmbeddingAttributeVector(unsigned int orbit) ;

	/**
	 * swap two attribute containers
	 */
	void swapEmbeddingContainers(unsigned int orbit1, unsigned int orbit2) ;

	/**
	 * static function for type registration
	 */
	template <typename R>
	static bool registerAttribute(const std::string &nameType) ;

	/**
	 * Traverse the map and embed all orbits of the given dimension with a new cell
	 * @param realloc if true -> all the orbits are embedded on new cells, if false -> already embedded orbits are not impacted
	 */
	void initOrbitEmbedding(unsigned int orbit, bool realloc = false) ;

	/****************************************
	 *          THREAD MANAGEMENT           *
	 ****************************************/
public:
	/**
	 * add  threads (a table of Marker per orbit for each thread)
	 * to allow MT
	 * @param nb thread to add
	 */
	void addThreadMarker(unsigned int nb) ;

	/**
	 * return allowed threads
	 * @return the number of threads (including principal)
	 */
	unsigned int getNbThreadMarkers() ;

	/**
	 * Remove some added threads
	 * @return remaining number of threads (including principal)
	 */
	void removeThreadMarker(unsigned int nb) ;

	/****************************************
	 *             SAVE & LOAD              *
	 ****************************************/
public:
	/**
	 * check if an xml node has a given name
	 * @param node the xml node
	 * @param name the name
	 * @ return true if node has the good name
	 */
	bool chechXmlNode(xmlNodePtr node, const std::string& name) ;

	/**
	 * update the pointer of embedding vector after loading
	 */
	void update_m_emb_afterLoad();

	/**
	 * Save map in a XML file
	 * @param filename the file name
	 * @return true if OK
	 */
	bool saveMapXml(const std::string& filename, bool compress = false) ;

	 /**
	 * Load map from a XML file
	 * @param filename the file name
	 * @return true if OK
	 */
	bool loadMapXml(const std::string& filename, bool compress = false) ;

	/**
	 * Save map in a binary file
	 * @param filename the file name
	 * @return true if OK
	 */
	bool saveMapBin(const std::string& filename) ;

	/**
	 * Load map from a binary file
	 * @param filename the file name
	 * @return true if OK
	 */
	bool loadMapBin(const std::string& filename) ;

	/****************************************
	 *           DARTS TRAVERSALS           *
	 ****************************************/

	/**
	 * Begin of map
	 * @return the first dart of the map
	 */
	Dart begin() ;

	/**
	 * End of map
	 * @return the end iterator (next of last) of the map
	 */
	Dart end() ;

	/**
	 * allow to go from a dart to the next
	 * in the order of storage
	 * @param d reference to the dart to be modified
	 */
	void next(Dart& d) ;

	/**
	 * Apply a functor on each dart of the map
	 * @param f a ref to the functor obj
	 */
	bool foreach_dart(FunctorType& f, const FunctorSelect& good = SelectorTrue()) ;

	//! Apply a functor on every dart of an orbit
	/*! @param dim dimension of orbit
	 *  @param d a dart of the orbit
	 *  @param f a functor obj
	 */
	bool foreach_dart_of_orbit(unsigned int orbit, Dart d, FunctorType& f, unsigned int thread = 0) ;

	virtual bool foreach_dart_of_vertex(Dart d, FunctorType& f, unsigned int thread = 0) = 0 ;
	virtual bool foreach_dart_of_edge(Dart d, FunctorType& f, unsigned int thread = 0) = 0 ;
	virtual bool foreach_dart_of_oriented_face(Dart d, FunctorType& f, unsigned int thread = 0) = 0 ;
	virtual bool foreach_dart_of_face(Dart d, FunctorType& f, unsigned int thread = 0) = 0 ;
	virtual bool foreach_dart_of_volume(Dart d, FunctorType& f, unsigned int thread = 0) = 0 ;
	virtual bool foreach_dart_of_cc(Dart d, FunctorType& f, unsigned int thread = 0) = 0 ;

	/**
	* execute functor for each orbit
	* @param dim the dimension of the orbit
	* @param f the functor
	* @param good the selector of darts
	*/
	bool foreach_orbit(unsigned int orbit, FunctorType& f, const FunctorSelect& good = SelectorTrue(), unsigned int thread = 0) ;

	//! Count the number of orbits of dimension dim in the map
	/*! @param dim the dimension of the orbit
	 *	@param good the selector of darts
	 * 	@return the number of orbits
	 */
	unsigned int getNbOrbits(unsigned int orbit, const FunctorSelect& good = SelectorTrue()) ;

	/**
	 * print attributes name of map in std::cout (for debugging)
	 */
	void viewAttributesTables();
} ;


template <typename MAP>
bool foreach_dart_of_orbit_in_parent(MAP* ptrMap, unsigned int orbit, Dart d, FunctorType& f, unsigned int thread = 0)
{
	switch(orbit)
	{
		case  DART: return f(d);
		case  VERTEX: return ptrMap->MAP::ParentMap::foreach_dart_of_vertex(d, f, thread) ;
		case  EDGE: return ptrMap->MAP::ParentMap::foreach_dart_of_edge(d, f, thread) ;
		case  ORIENTED_FACE: return ptrMap->MAP::ParentMap::foreach_dart_of_oriented_face(d, f, thread) ;
		case  FACE: return ptrMap->MAP::ParentMap::foreach_dart_of_face(d, f, thread) ;
		case  VOLUME: return ptrMap->MAP::ParentMap::foreach_dart_of_volume(d, f, thread) ;
		default: assert(!"Cells of this dimension are not handled") ;
	}
	return false ;
}

template <typename MAP>
bool foreach_dart_of_orbit_in_parent2(MAP* ptrMap, unsigned int orbit, Dart d, FunctorType& f, unsigned int thread = 0)
{
	switch(orbit)
	{
		case  DART: return f(d);
		case  VERTEX: return ptrMap->MAP::ParentMap::ParentMap::foreach_dart_of_vertex(d, f,thread) ;
		case  EDGE: return ptrMap->MAP::ParentMap::ParentMap::foreach_dart_of_edge(d, f, thread) ;
		case  ORIENTED_FACE: return ptrMap->MAP::ParentMap::ParentMap::foreach_dart_of_oriented_face(d, f, thread) ;
		case  FACE: return ptrMap->MAP::ParentMap::ParentMap::foreach_dart_of_face(d, f, thread) ;
		case  VOLUME: return ptrMap->MAP::ParentMap::ParentMap::foreach_dart_of_volume(d, f, thread) ;
		default: assert(!"Cells of this dimension are not handled") ;
	}
	return false ;
}

} //namespace CGoGN

#include "Topology/generic/genericmap.hpp"

#endif
