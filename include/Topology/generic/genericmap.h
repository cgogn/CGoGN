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

#ifndef __GENERIC_MAP__
#define __GENERIC_MAP__

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <list>
#include <vector>
#include <map>
#include <boost/thread/mutex.hpp>

#include "Container/attributeContainer.h"
#include "Container/fakeAttribute.h"

#include "Topology/generic/dart.h"
#include "Topology/generic/cells.h"
#include "Topology/generic/marker.h"
#include "Topology/generic/functor.h"

#include <boost/thread.hpp>

namespace CGoGN
{

namespace Parallel
{
/**
 * @brief Number of threads used for // traversal foreach
 */
extern int NumberOfThreads;

/**
 * @brief get number of cores of computer (threads, /2 if hyperthreading)
 * @param hyperthreading
 * @return nb of core
 */
inline int getSystemNumberOfCores(bool hyperthreading=false)
{
	if (hyperthreading)
		return boost::thread::hardware_concurrency()/2;
	return boost::thread::hardware_concurrency();
}

}


class AttributeHandlerGen ;
class DartMarkerGen ;
class CellMarkerGen ;

class GenericMap
{
	template<typename T, unsigned int ORBIT, typename MAP> friend class AttributeHandler ;
	template<typename T, typename MAP> friend class DartAutoAttribute ;
	template<typename T, typename MAP> friend class VertexAutoAttribute ;
	template<typename T, typename MAP> friend class EdgeAutoAttribute ;
	template<typename T, typename MAP> friend class FaceAutoAttribute ;
	template<typename T, typename MAP> friend class VolumeAutoAttribute ;
	friend class DartMarkerGen ;
	friend class CellMarkerGen ;
	template<typename MAP, unsigned int CELL> friend class CellMarkerBase ;

protected:
	// protected copy constructor to prevent the copy of map
	GenericMap(const GenericMap& ) {}

	/**
	 * Attributes Containers
	 */
	AttributeContainer m_attribs[NB_ORBITS] ;

	static std::map<std::string, RegisteredBaseAttribute*>* m_attributes_registry_map;
	static int m_nbInstances;

	/**
	 * Direct access to the Dart attributes that store the orbits embeddings
	 * (only initialized when necessary, i.e. addEmbedding function)
	 */
	AttributeMultiVector<unsigned int>* m_embeddings[NB_ORBITS] ;

	/**
	 * Direct access to quick traversal attributes
	 * (initialized by enableQuickTraversal function)
	 */
	AttributeMultiVector<Dart>* m_quickTraversal[NB_ORBITS] ;
	AttributeMultiVector<NoTypeNameAttribute<std::vector<Dart> > >* m_quickLocalIncidentTraversal[NB_ORBITS][NB_ORBITS] ;
	AttributeMultiVector<NoTypeNameAttribute<std::vector<Dart> > >* m_quickLocalAdjacentTraversal[NB_ORBITS][NB_ORBITS] ;

	/**
	 * Marks manager
	 */
	MarkSet m_marksets[NB_ORBITS][NB_THREAD] ;

	/**
	 * Direct access to the attributes that store Marks
	 */
	AttributeMultiVector<Mark>* m_markTables[NB_ORBITS][NB_THREAD] ;

	/**
	 * Reserved boundary markers
	 */
	Mark m_boundaryMarkers[2] ; // 0 for dim 2 / 1 for dim 3

	unsigned int m_nbThreadMarkers ;

	/**
	 * Store links to created AttributeHandlers, DartMarkers and CellMarkers
	 */
	std::multimap<AttributeMultiVectorGen*, AttributeHandlerGen*> attributeHandlers ; // TODO think of MT (AttributeHandler creation & release are not thread safe!)
	boost::mutex attributeHandlersMutex;

	std::vector<DartMarkerGen*> dartMarkers[NB_THREAD] ;
	std::vector<CellMarkerGen*> cellMarkers[NB_THREAD] ;

// table of instancied maps for Dart/CellMarker release
	static std::vector<GenericMap*> s_instances;

public:
	static const unsigned int UNKNOWN_ATTRIB = AttributeContainer::UNKNOWN ;

	GenericMap() ;

	virtual ~GenericMap() ;

	static inline bool alive(GenericMap* map)
	{
		for (auto it=s_instances.begin(); it != s_instances.end(); ++it)
			if (*it == map)
				return true;
		return false;
	}

protected:
	void init();

public:
	virtual std::string mapTypeName() const = 0 ;

	virtual unsigned int dimension() const = 0 ;

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
	template <unsigned int ORBIT>
	MarkSet& getMarkerSet(unsigned int thread = 0) { return m_marksets[ORBIT][thread]; }

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
	virtual void deleteDart(Dart d) = 0 ;

	/**
	 * create a copy of a dart (based on its index in m_attribs[DART]) and returns its index
	 */
	unsigned int copyDartLine(unsigned int index) ;

	/**
	 * Properly deletes a dart in m_attribs[DART]
	 */
	void deleteDartLine(unsigned int index) ;

public:
	/****************************************
	 *          ORBITS TRAVERSALS           *
	 ****************************************/

//	virtual void foreach_dart_of_vertex(Dart /*d*/, std::function<void (Dart)>& /*f*/, unsigned int /*thread = 0*/) const { std::cerr << "Not implemented" << std::endl; }
//	virtual void foreach_dart_of_edge(Dart /*d*/, std::function<void (Dart)>& /*f*/, unsigned int /*thread = 0*/) const { std::cerr << "Not implemented" << std::endl; }
//	virtual void foreach_dart_of_face(Dart /*d*/, std::function<void (Dart)>& /*f*/, unsigned int /*thread = 0*/) const { std::cerr << "Not implemented" << std::endl; }
//	virtual void foreach_dart_of_volume(Dart /*d*/, std::function<void (Dart)>& /*f*/, unsigned int /*thread = 0*/) const { std::cerr << "Not implemented" << std::endl; }
//	virtual void foreach_dart_of_cc(Dart /*d*/, std::function<void (Dart)>& /*f*/, unsigned int /*thread = 0*/) const { std::cerr << "Not implemented" << std::endl; }

//	virtual void foreach_dart_of_vertex1(Dart /*d*/, std::function<void (Dart)>& /*f*/, unsigned int /*thread = 0*/) const { std::cerr << "Not implemented" << std::endl; }
//	virtual void foreach_dart_of_edge1(Dart /*d*/, std::function<void (Dart)>& /*f*/, unsigned int /*thread = 0*/) const { std::cerr << "Not implemented" << std::endl; }

//	virtual void foreach_dart_of_vertex2(Dart /*d*/, std::function<void (Dart)>& /*f*/, unsigned int /*thread = 0*/) const { std::cerr << "Not implemented" << std::endl; }
//	virtual void foreach_dart_of_edge2(Dart /*d*/, std::function<void (Dart)>& /*f*/, unsigned int /*thread = 0*/) const { std::cerr << "Not implemented" << std::endl; }
//	virtual void foreach_dart_of_face2(Dart /*d*/, std::function<void (Dart)>& /*f*/, unsigned int /*thread = 0*/) const { std::cerr << "Not implemented" << std::endl; }

	/****************************************
	 *         EMBEDDING MANAGEMENT         *
	 ****************************************/

	/**
	 * tell if an orbit is embedded or not
	 */
	template <unsigned int ORBIT>
	bool isOrbitEmbedded() const ;

	bool isOrbitEmbedded(unsigned int orbit) const ;

	/**
	 * Allocation of some place in attrib table
	 * @param orbit the orbit of embedding
	 * @return the index to use as embedding
	 */
	template <unsigned int ORBIT>
	unsigned int newCell() ;

	/**
	 * Line of attributes i is overwritten with line j
	 * @param orbit attribute orbit to use
	 * @param i line destination of copy
	 * @param j line source of copy
	 */
	template <unsigned int ORBIT>
	void copyCell(unsigned int i, unsigned int j) ;

	/**
	 * Line of attributes i is initialized
	 * @param orbit attribute orbit to use
	 * @param i line to init
	 */
	template <unsigned int ORBIT>
	void initCell(unsigned int i) ;

	/****************************************
	 *   ATTRIBUTES CONTAINERS MANAGEMENT   *
	 ****************************************/

	/**
	 * get the number of cell in the attribute container of an orbit
	 * @param orb the orbit to get number of cells
	 */
	unsigned int getNbCells(unsigned int orbit);

	/**
	 * get the attrib container of a given orbit
	 * @param orbit the orbit !!! (bilbo the orbit !)
	 */
	template <unsigned int ORBIT>
	AttributeContainer& getAttributeContainer() ;

	template <unsigned int ORBIT>
	const AttributeContainer& getAttributeContainer() const;

	AttributeContainer& getAttributeContainer(unsigned int orbit) ;

	const AttributeContainer& getAttributeContainer(unsigned int orbit) const;

	/**
	 * @brief get a generic pointer to an existing attribute multi vector
	 * @param orbit the concerned orbit
	 * @param nameAttr attribute name
	 * @return a pointer to an AttributeMultiVectorGen
	 */
	inline AttributeMultiVectorGen* getAttributeVectorGen(unsigned int orbit, const std::string& nameAttr) ;

	/**
	 * get a multi vector of mark attribute (direct access with [i])
	 */
	template <unsigned int ORBIT>
	AttributeMultiVector<Mark>* getMarkVector(unsigned int thread = 0) ;

	/**
	 * return a pointer to the Dart attribute vector that store the embedding of the given orbit
	 * (may be NULL if the orbit is not embedded)
	 */
	template <unsigned int ORBIT>
	AttributeMultiVector<unsigned int>* getEmbeddingAttributeVector() ;

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
	 * print attributes name of map in std::cout (for debugging)
	 */
	void viewAttributesTables() ;

	void printDartsTable();

	/****************************************
	 *   EMBEDDING ATTRIBUTES MANAGEMENT    *
	 ****************************************/
	/**
	 * Create the dart attribute to store the embedding of this orbit (for internal use only)
	 */
	template <unsigned int ORBIT>
	void addEmbedding() ;

protected:
	/****************************************
	 *  TOPOLOGICAL ATTRIBUTES MANAGEMENT   *
	 ****************************************/

	/**
	 * Add a topological relation in the map
	 * @param name name of relation
	 */
	AttributeMultiVector<Dart>* addRelation(const std::string& name) ;

	/**
	 * Get AttributeMultivector pointer of a relation attribute
	 * @param name name of the relation
	 * @return the attribute multi-vector pointer
	 */
	AttributeMultiVector<Dart>* getRelation(const std::string& name) ;

	/****************************************
	 *          THREAD MANAGEMENT           *
	 ****************************************/
public:
	/**
	 * add threads (a table of Marker per orbit for each thread)
	 * to allow MT
	 * @param nb thread to add
	 */
	void addThreadMarker(unsigned int nb) ;

	/**
	 * return allowed threads
	 * @return the number of threads (including principal)
	 */
	unsigned int getNbThreadMarkers() const;

	/**
	 * Remove some added threads
	 * @return remaining number of threads (including principal)
	 */
	void removeThreadMarker(unsigned int nb) ;

	/****************************************
	 *             SAVE & LOAD              *
	 ****************************************/
protected:
	/**
	 * restore embedding / markers / quick traversal shortcuts
	 */
	void restore_shortcuts();

public:
	/**
	 * Save map in a binary file
	 * @param filename the file name
	 * @return true if OK
	 */
	virtual bool saveMapBin(const std::string& filename) const = 0;

	/**
	 * Load map from a binary file
	 * @param filename the file name
	 * @return true if OK
	 */
	virtual bool loadMapBin(const std::string& filename) = 0 ;

	/**
	 * copy from another map (of same type)
	 */
	virtual bool copyFrom(const GenericMap& map) = 0 ;

	/**
	 * Dump attributes types and names per orbit
	 */
	void dumpAttributesAndMarkers() ;

protected:
	/**
	 * compact topo relations
	 */
	virtual void compactTopo() = 0 ;

public:
	/**
	 * compact the map
	 */
	void compact() ;

	/**
	 * @brief dump all attributes of map in CSV format  (; separated columns)
	 */
	void dumpCSV() const;
} ;



//
//template <typename MAP>
//bool foreach_dart_of_orbit_in_parent(MAP* ptrMap, unsigned int orbit, Dart d, FunctorType& f, unsigned int thread = 0)
//{
//	switch(orbit)
//	{
//		case  DART: return f(d);
//		case  VERTEX: return ptrMap->MAP::ParentMap::foreach_dart_of_vertex(d, f, thread) ;
//		case  EDGE: return ptrMap->MAP::ParentMap::foreach_dart_of_edge(d, f, thread) ;
//		case  ORIENTED_FACE: return ptrMap->MAP::ParentMap::foreach_dart_of_oriented_face(d, f, thread) ;
//		case  FACE: return ptrMap->MAP::ParentMap::foreach_dart_of_face(d, f, thread) ;
//		case  VOLUME: return ptrMap->MAP::ParentMap::foreach_dart_of_volume(d, f, thread) ;
//		default: assert(!"Cells of this dimension are not handled") ;
//	}
//	return false ;
//}
//
//template <typename MAP>
//bool foreach_dart_of_orbit_in_parent2(MAP* ptrMap, unsigned int orbit, Dart d, FunctorType& f, unsigned int thread = 0)
//{
//	switch(orbit)
//	{
//		case  DART: return f(d);
//		case  VERTEX: return ptrMap->MAP::ParentMap::ParentMap::foreach_dart_of_vertex(d, f,thread) ;
//		case  EDGE: return ptrMap->MAP::ParentMap::ParentMap::foreach_dart_of_edge(d, f, thread) ;
//		case  ORIENTED_FACE: return ptrMap->MAP::ParentMap::ParentMap::foreach_dart_of_oriented_face(d, f, thread) ;
//		case  FACE: return ptrMap->MAP::ParentMap::ParentMap::foreach_dart_of_face(d, f, thread) ;
//		case  VOLUME: return ptrMap->MAP::ParentMap::ParentMap::foreach_dart_of_volume(d, f, thread) ;
//		default: assert(!"Cells of this dimension are not handled") ;
//	}
//	return false ;
//}

} //namespace CGoGN

#include "Topology/generic/genericmap.hpp"

#endif
