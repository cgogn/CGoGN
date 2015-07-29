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

#include "Container/attributeContainer.h"
#include "Container/fakeAttribute.h"

#include "Topology/generic/dart.h"
#include "Topology/generic/cells.h"
#include "Topology/generic/marker.h"
#include "Topology/generic/functor.h"

#include <thread>
#include <mutex>

#include "Topology/dll.h"

namespace CGoGN
{

namespace Parallel
{
/**
 * @brief Number of threads used for // traversal foreach
 */
CGoGN_TOPO_API extern int NumberOfThreads;

/**
 * @brief get number of cores of computer (threads, /2 if hyperthreading)
 * @param hyperthreading
 * @return nb of core
 */
inline int getSystemNumberOfCores(bool hyperthreading=false)
{
	if (hyperthreading)
		return std::thread::hardware_concurrency()/2;
	return std::thread::hardware_concurrency();
}

}

// forward
class GenericMap;

// classs that store static pointer for copy in schnapps plugins
struct StaticPointers
{
	std::map<std::string, RegisteredBaseAttribute*>* att_registry;
	std::vector<GenericMap*>* instances;
	std::vector< std::vector<Dart>* >* vdartsBuffers;
	std::vector< std::vector<unsigned int>* >* vintsBuffers;
};

class AttributeHandlerGen ;
class DartMarkerGen ;
class CellMarkerGen ;
class MapManipulator;

class CGoGN_TOPO_API GenericMap
{
	template<typename T, unsigned int ORBIT, typename MAP> friend class AttributeHandler ;
	template<typename T, typename MAP> friend class DartAutoAttribute ;
	template<typename T, typename MAP> friend class VertexAutoAttribute ;
	template<typename T, typename MAP> friend class EdgeAutoAttribute ;
	template<typename T, typename MAP> friend class FaceAutoAttribute ;
	template<typename T, typename MAP> friend class VolumeAutoAttribute ;
//	friend class DartMarkerGen ;
//	friend class CellMarkerGen ;
//	template<typename MAP, unsigned int CELL> friend class CellMarkerBase ;

protected:
	// protected copy constructor to prevent the copy of map
	GenericMap(const GenericMap& ) {}

	/**
	 * @brief m_thread_ids
	 * vector of known thread ids, i.e. threads for which a mark vector,
	 * a Dart buffer or a uint buffer will be given when asked
	 */
	mutable std::vector<std::thread::id> m_thread_ids;

	/**
	 * @brief m_authorizeExternalThreads
	 * if true, getCurrentThreadIndex will give an index to an unknown thread
	 */
	bool m_authorizeExternalThreads;

public:
	/// compute thread index in the table of thread
	inline unsigned int getCurrentThreadIndex() const;

//	/// register the given thread ID for access to resources on this map
//	inline void addThreadId(const std::thread::id id);

	/// unregister the given thread to access resources on this map
	inline void removeThreadId(const std::thread::id id);

	/// add room for a new thread ID (will be set on thread start)
	inline std::thread::id& addEmptyThreadId();

	/// get a threadId based on its index
	inline std::thread::id getThreadId(unsigned int index) const;

	/**
	 * @brief setExternalThreadsAuthorization
	 * @param b
	 * if true, threads that did not created the map will be able to traverse it
	 * if false, traversal of the map by other threads will fail
	 */
	inline void setExternalThreadsAuthorization(bool b);

protected:
	/**
	 * Attributes Containers
	 */
	AttributeContainer m_attribs[NB_ORBITS] ;

	static std::map<std::string, RegisteredBaseAttribute*>* m_attributes_registry_map;

	/// buffer for less memory allocation
	mutable std::vector< std::vector<Dart>* > s_vdartsBuffers[NB_THREADS];
	mutable std::vector< std::vector<unsigned int>* > s_vintsBuffers[NB_THREADS];

public:
	/// table of instancied maps for Dart/CellMarker release
	static std::vector<GenericMap*>* s_instances;

protected:
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

	std::vector< AttributeMultiVector<MarkerBool>* > m_markVectors_free[NB_ORBITS][NB_THREADS] ;
	std::mutex m_MarkerStorageMutex[NB_ORBITS];

	unsigned int m_nextMarkerId;

	/**
	 * Reserved boundary markers
	 */
	AttributeMultiVector<MarkerBool>* m_boundaryMarkers[2];

	/**
	 * Store links to created AttributeHandlers
	 */
	std::multimap<AttributeMultiVectorGen*, AttributeHandlerGen*> attributeHandlers ;
	std::mutex attributeHandlersMutex;

public:
	static const unsigned int UNKNOWN_ATTRIB = AttributeContainer::UNKNOWN ;

	/// copy all static pointers: use in SCHNApps only
	static void copyAllStatics(const StaticPointers& sp);

	/// init all static and store in sp (if not null) : use in SCHNApps only
	static void initAllStatics(StaticPointers* sp);

	GenericMap() ;

	virtual ~GenericMap() ;

	static inline bool alive(GenericMap* map)
	{
		for (auto it=s_instances->begin(); it != s_instances->end(); ++it)
		{
			if (*it == map)
				return true;
		}
		return false;
	}

	inline std::vector<Dart>* askDartBuffer() const;
	inline void releaseDartBuffer(std::vector<Dart>* vd) const;

	inline std::vector<unsigned int>* askUIntBuffer() const;
	inline void releaseUIntBuffer(std::vector<unsigned int>* vd) const;

protected:
	void init(bool addBoundaryMarkers=true);

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


	/****************************************
	 *     MANIPULATOR MANAGEMENT           *
	 ****************************************/
protected:
	/// manipulator pointer to Manipulator object that currently work on map
	MapManipulator* m_manipulator;

public:
	/**
	 * @brief ask for associating manipulator to the map
	 * @param ptr Manipulator ptr
	 * @return ok or not
	 */
	bool askManipulate(MapManipulator* ptr);

	/**
	 * @brief release the map from manipulator
	 * @param ptr manipulator asking for release
	 * @return ok or not
	 */
	bool releaseManipulate(MapManipulator* ptr);

	/**
	 * @brief get the manipulator
	 * @return manipulator ptr
	 */
	MapManipulator* getManipulator();

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
	 * @brief ask for a marker attribute
	 */
	template <unsigned int ORBIT>
	AttributeMultiVector<MarkerBool>* askMarkVector() ;

	/**
	 * @brief release allocated marker attribute
	 */
	template <unsigned int ORBIT>
	void releaseMarkVector(AttributeMultiVector<MarkerBool>* amv);

protected:
	/**
	 * @brief scan attributes for MarkerBool, clean them and store as free in thread 0
	 */
	void garbageMarkVectors();

	/**
	 * @brief scan attributes for MarkerBool and remove them
	 */
	void removeMarkVectors();
public:

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
	 * @warning the quickTraversals needs to be updated
	 * @param topoOnly compact only the topo ?
	 */
	void compact(bool topoOnly = false) ;


	/**
	 * compact a container (and update embedding attribute of topo)
	 * @param orbit orbit of container to compact
	 * @param frag minimum fragmentation value for compacting (default value 1.0 mean always compact)s
	 */
	void compactOrbitContainer(unsigned int orbit, float frag=1.0);

	/**
	 * @brief compact if containers are fragmented.
	 * @warning the quickTraversals needs to be updated
	 * @param frag if fragmentation (filling) of containers inferior to frag then compact
	 * @param topoOnly compact only the topo ?
	 */
	void compactIfNeeded(float frag, bool topoOnly = false) ;

	/**
	 * test if containers are fragmented
	 *  ~1.0 (full filled) no need to compact
	 *  ~0.0 (lots of holes) need to compact
	 */
	inline float fragmentation(unsigned int orbit);

	/**
	 * @brief dump all attributes of map in CSV format  (; separated columns)
	 */
	virtual void dumpCSV() const;

public:
	/**
	 * @brief move data (container && shortcuts from a map to this map
	 * @param mapf map from which data are moved);
	 */
	void moveData(GenericMap &mapf);
} ;


// DartBufferThread class that hide the usage of askDartBuffer & releaseDartBuffer
// scope of DartBuffer declaration use for auto release;
// typical usage:
//    DartBuffer buff(thread)
//    std::vector<DART>& vd=buff.vector();
//    vd.push_back(..)
//    ....

//class DartsBufferThread
//{
//protected:
//	std::vector<Dart>* m_vd;
//public:
//	inline DartsBufferThread()
//	{
//		m_vd = GenericMap::askDartBuffer();
//	}

//	inline ~DartsBufferThread()
//	{
//		GenericMap::releaseDartBuffer(m_vd);
//	}

//	inline std::vector<Dart>& vector() { return *m_vd;}
//};



} //namespace CGoGN

#include "Topology/generic/genericmap.hpp"

#endif
