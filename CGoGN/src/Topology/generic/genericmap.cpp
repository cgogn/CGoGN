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

#define CGoGN_TOPO_DLL_EXPORT 1

#include "Topology/generic/genericmap.h"
#include "Topology/generic/attributeHandler.h"

#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"
#include "Container/registered.h"

#include <algorithm>

namespace CGoGN
{

namespace Parallel
{
//int NumberOfThreads=1;
CGoGN_TOPO_API int NumberOfThreads = getSystemNumberOfCores();
}

std::map<std::string, RegisteredBaseAttribute*>* GenericMap::m_attributes_registry_map = NULL;

//std::mutex* GenericMap::s_DartBufferMutex = NULL;
//std::mutex* GenericMap::s_IntBufferMutex = NULL;

std::vector<GenericMap*>*  GenericMap::s_instances = NULL;

GenericMap::GenericMap():
	m_nextMarkerId(0),
	m_authorizeExternalThreads(false),
	m_manipulator(NULL)
{
	if(m_attributes_registry_map == NULL)
		initAllStatics(NULL); // no need here to store the pointers

	s_instances->push_back(this);

	m_thread_ids.reserve(NB_THREADS + 1);
	m_thread_ids.push_back(std::this_thread::get_id());

	for(unsigned int i = 0; i < NB_ORBITS; ++i)
	{
		m_attribs[i].setOrbit(i) ;
		m_attribs[i].setRegistry(m_attributes_registry_map) ;
	}

	for(unsigned int i = 0; i < NB_THREADS; ++i)
	{
		s_vdartsBuffers[i].reserve(8);
		s_vintsBuffers[i].reserve(8);

//		for(unsigned int j = 0; j < NB_ORBITS; ++j)
//			m_markVectors_free[i][j].reserve(8);
	}

	init();
}

void GenericMap::copyAllStatics(const StaticPointers& sp)
{
	m_attributes_registry_map = sp.att_registry;
	s_instances = sp.instances;
}

void GenericMap::initAllStatics(StaticPointers* sp)
{
	if(m_attributes_registry_map == NULL)
	{
		m_attributes_registry_map = new std::map<std::string, RegisteredBaseAttribute*>;

		// register all known types
		registerAttribute<Dart>("Dart");
		registerAttribute<Mark>("Mark");

		registerAttribute<char>("char");
		registerAttribute<short>("short");
		registerAttribute<int>("int");
		registerAttribute<long>("long");

		registerAttribute<unsigned char>("unsigned char");
		registerAttribute<unsigned short>("unsigned short");
		registerAttribute<unsigned int>("unsigned int");
		registerAttribute<unsigned long>("unsigned long");

		registerAttribute<float>("float");
		registerAttribute<double>("double");

		registerAttribute<Geom::Vec2f>(Geom::Vec2f::CGoGNnameOfType());
		registerAttribute<Geom::Vec3f>(Geom::Vec3f::CGoGNnameOfType());
		registerAttribute<Geom::Vec4f>(Geom::Vec4f::CGoGNnameOfType());

		registerAttribute<Geom::Vec2d>(Geom::Vec2d::CGoGNnameOfType());
		registerAttribute<Geom::Vec3d>(Geom::Vec3d::CGoGNnameOfType());
		registerAttribute<Geom::Vec4d>(Geom::Vec4d::CGoGNnameOfType());

		registerAttribute<Geom::Matrix33f>(Geom::Matrix33f::CGoGNnameOfType());
		registerAttribute<Geom::Matrix44f>(Geom::Matrix44f::CGoGNnameOfType());

		registerAttribute<Geom::Matrix33d>(Geom::Matrix33d::CGoGNnameOfType());
		registerAttribute<Geom::Matrix44d>(Geom::Matrix44d::CGoGNnameOfType());

		registerAttribute<MarkerBool>("MarkerBool");
	}

	if (s_instances == NULL)
		s_instances = new std::vector<GenericMap*>;

	if (sp != NULL)
	{
		sp->att_registry = m_attributes_registry_map;
		sp->instances = s_instances;
	}
}

GenericMap::~GenericMap()
{
	for(unsigned int i = 0; i < NB_ORBITS; ++i)
	{
		if(isOrbitEmbedded(i))
			m_attribs[i].clear(true) ;
	}

	for(std::multimap<AttributeMultiVectorGen*, AttributeHandlerGen*>::iterator it = attributeHandlers.begin(); it != attributeHandlers.end(); ++it)
		(*it).second->setInvalid() ;
	attributeHandlers.clear() ;

	// remove instance of table
	auto it = std::find(s_instances->begin(), s_instances->end(), this);
	*it = s_instances->back();
	s_instances->pop_back();

	// clean type registry if necessary

//	if (s_instances->size() == 0)
//	{
//		for (std::map<std::string, RegisteredBaseAttribute*>::iterator it =  m_attributes_registry_map->begin(); it != m_attributes_registry_map->end(); ++it)
//			delete it->second;

//		delete m_attributes_registry_map;
//		m_attributes_registry_map = NULL;

//		for(unsigned int i = 0; i < NB_THREADS; ++i)
//		{
//			for (auto it =s_vdartsBuffers[i].begin(); it != s_vdartsBuffers[i].end(); ++it)
//				delete *it;
//			for (auto it =s_vintsBuffers[i].begin(); it != s_vintsBuffers[i].end(); ++it)
//				delete *it;
//		}
//	}
}

bool GenericMap::askManipulate(MapManipulator* ptr)
{
	if (m_manipulator == NULL)
	{
		CGoGNerr << "Map already manipulated by other manipulator" << CGoGNendl;
		return false;
	}

	m_manipulator = ptr;
	return true;
}

MapManipulator* GenericMap::getManipulator()
{
	return m_manipulator;
}

bool GenericMap::releaseManipulate(MapManipulator* ptr)
{
	if (m_manipulator != ptr)
	{
		CGoGNerr << "Wrong manipulator want to release the map" << CGoGNendl;
		return false;
	}

	m_manipulator = NULL;
	return true;
}

void GenericMap::init(bool addBoundaryMarkers)
{
	for(unsigned int i = 0; i < NB_ORBITS; ++i)
	{
		m_attribs[i].clear(true) ;
		m_embeddings[i] = NULL ;
		m_quickTraversal[i] = NULL;

		for(unsigned int j = 0; j < NB_ORBITS; ++j)
		{
			m_quickLocalIncidentTraversal[i][j] = NULL ;
			m_quickLocalAdjacentTraversal[i][j] = NULL ;
		}

		for(unsigned int j = 0; j < NB_THREADS; ++j)
			m_markVectors_free[i][j].clear();
	}

	if (addBoundaryMarkers)
	{
		m_boundaryMarkers[0] = m_attribs[DART].addMarkerAttribute("BoundaryMark0") ;
		m_boundaryMarkers[1] = m_attribs[DART].addMarkerAttribute("BoundaryMark1") ;
	}

	for(std::multimap<AttributeMultiVectorGen*, AttributeHandlerGen*>::iterator it = attributeHandlers.begin(); it != attributeHandlers.end(); ++it)
		(*it).second->setInvalid() ;
	attributeHandlers.clear() ;
}

void GenericMap::clear(bool removeAttrib)
{
	if (removeAttrib)
	{
#ifndef NDEBUG
		for(unsigned int i = 0; i < NB_ORBITS; ++i)
			if (m_attribs[i].hasMarkerAttribute())
				CGoGNout << "Warning removing marker attribute on orbit, need update ? "<<orbitName(i)<< CGoGNendl;
#endif
		init();
	}
	else
	{
		for(unsigned int i = 0; i < NB_ORBITS; ++i)
			m_attribs[i].clear(false) ;
	}
}

/****************************************
 *        ATTRIBUTES MANAGEMENT         *
 ****************************************/

void GenericMap::swapEmbeddingContainers(unsigned int orbit1, unsigned int orbit2)
{
	assert(orbit1 != orbit2 || !"Cannot swap a container with itself") ;
	assert((orbit1 != DART && orbit2 != DART) || !"Cannot swap the darts container") ;

	m_attribs[orbit1].swap(m_attribs[orbit2]) ;
	m_attribs[orbit1].setOrbit(orbit1) ;	// to update the orbit information
	m_attribs[orbit2].setOrbit(orbit2) ;	// in the contained AttributeMultiVectors

	m_embeddings[orbit1]->swap(m_embeddings[orbit2]) ;


	// NE MARCHE PLUS PAS POSSIBLE DE CHANGER LES CONTAINER

//	for (unsigned int i = 0; i < NB_THREADS; ++i)
//	{
//		for(std::vector<CellMarkerGen*>::iterator it = cellMarkers[i].begin(); it != cellMarkers[i].end(); ++it)
//		{
//			if((*it)->m_cell == orbit1)
//				(*it)->m_cell = orbit2 ;
//			else if((*it)->m_cell == orbit2)
//				(*it)->m_cell = orbit1 ;
//		}
//	}
}

void GenericMap::viewAttributesTables()
{
	std::cout << "======================="<< std::endl ;
	for (unsigned int i = 0; i < NB_ORBITS; ++i)
	{
		std::cout << "ATTRIBUTE_CONTAINER " << i << std::endl ;
		AttributeContainer& cont = m_attribs[i] ;

		// get the list of attributes
		std::vector<std::string> listeNames ;
		cont.getAttributesNames(listeNames) ;
		for (std::vector<std::string>::iterator it = listeNames.begin(); it != listeNames.end(); ++it)
		{
			unsigned int id = cont.getAttributeIndex(*it);
			std::cout << "    " << *it << " ("<<id<<")"<<std::endl ;
		}

		std::cout << "-------------------------" << std::endl ;
	}
	std::cout << "m_embeddings: " << std::hex ;
	for (unsigned int i = 0; i < NB_ORBITS; ++i)
		std::cout << (long)(m_embeddings[i]) << " / " ;
	std::cout << std::endl << "-------------------------" << std::endl ;

//	std::cout << "m_markTables: " ;
//	for (unsigned int i = 0; i < NB_ORBITS; ++i)
//		std::cout << (long)(m_markTables[i][0]) << " / " ;
//	std::cout << std::endl << "-------------------------" << std::endl << std::dec ;
}

void GenericMap::printDartsTable()
{
	std::cout << "======================="<< std::endl ;

	//m_attribs[DART]
}

/****************************************
 *          THREAD MANAGEMENT           *
 ****************************************/

//void GenericMap::addThreadMarker(unsigned int nb)
//{
//	unsigned int th ;

//	for (unsigned int j = 0; j < nb; ++j)
//	{
//		th = m_nbThreadMarkers ;
//		m_nbThreadMarkers++ ;

//		for (unsigned int i = 0; i < NB_ORBITS; ++i)
//		{
//			std::stringstream ss ;
//			ss << "Mark_"<< th ;
//			AttributeContainer& cellCont = m_attribs[i] ;
//			AttributeMultiVector<Mark>* amvMark = cellCont.addAttribute<Mark>(ss.str()) ;
//			m_markTables[i][th] = amvMark ;
//		}
//	}
//}

//unsigned int GenericMap::getNbThreadMarkers() const
//{
//	return m_nbThreadMarkers;
//}

//void GenericMap::removeThreadMarker(unsigned int nb)
//{
//	unsigned int th = 0;
//	while ((m_nbThreadMarkers > 1) && (nb > 0))
//	{
//		th = --m_nbThreadMarkers ;
//		--nb;
//		for (unsigned int i = 0; i < NB_ORBITS; ++i)
//		{
//			std::stringstream ss ;
//			ss << "Mark_"<< th ;
//			AttributeContainer& cellCont = m_attribs[i] ;
//			cellCont.removeAttribute<Mark>(ss.str()) ;
//			m_markTables[i][th] = NULL ;
//		}
//	}
//}

/****************************************
 *             SAVE & LOAD              *
 ****************************************/

void GenericMap::restore_shortcuts()
{
	// EMBEDDING

	// get container of dart orbit
	AttributeContainer& cont = m_attribs[DART] ;

	// get the list of attributes
	std::vector<std::string> listeNames;
	cont.getAttributesNames(listeNames);

	// check if there are EMB_X attributes
	for (unsigned int i = 0;  i < listeNames.size(); ++i)
	{
		std::string sub = listeNames[i].substr(0, listeNames[i].size() - 1);
		if (sub == "EMB_")
		{
			unsigned int orb = listeNames[i][4]-'0'; // easy atoi computation for one char;
			AttributeMultiVector<unsigned int>* amv = cont.getDataVector<unsigned int>(i);
			m_embeddings[orb] = amv ;
		}
	}

	// MARKERS
	std::vector<AttributeMultiVector<MarkerBool>*>& amvv = m_attribs[DART].getMarkerAttributes();
	for (auto it=amvv.begin(); it != amvv.end(); ++it)
	{
		if ((*it)->getName() == "BoundaryMark0")
			m_boundaryMarkers[0] = *it;
		if ((*it)->getName() == "BoundaryMark1")
			m_boundaryMarkers[1] = *it;
	}


	// QUICK TRAVERSAL

	for(unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		AttributeContainer& cont = m_attribs[orbit];
		m_quickTraversal[orbit] = cont.getDataVector<Dart>("quick_traversal") ;
		for(unsigned int j = 0; j < NB_ORBITS; ++j)
		{
			std::stringstream ss;
			ss << "quickLocalIncidentTraversal_" << j;
			m_quickLocalIncidentTraversal[orbit][j] = cont.getDataVector< NoTypeNameAttribute<std::vector<Dart> > >(ss.str()) ;
			std::stringstream ss2;
			ss2 << "quickLocalAdjacentTraversal_" << j;
			m_quickLocalAdjacentTraversal[orbit][j] = cont.getDataVector< NoTypeNameAttribute<std::vector<Dart> > >(ss2.str()) ;
		}
	}

	// set Attribute handlers invalid
	for(std::multimap<AttributeMultiVectorGen*, AttributeHandlerGen*>::iterator it = attributeHandlers.begin(); it != attributeHandlers.end(); ++it)
		(*it).second->setInvalid() ;
	attributeHandlers.clear() ;
}

void GenericMap::dumpAttributesAndMarkers()
{
	for (unsigned int i = 0; i < NB_ORBITS; ++i)
	{
		std::vector<std::string> names;
		names.reserve(32); 				//just to limit reallocation
		m_attribs[i].getAttributesNames(names);
		unsigned int nb = uint32(names.size());
		if (nb > 0)
		{
			CGoGNout << "ORBIT "<< i << CGoGNendl;
			std::vector<std::string> types;
			types.reserve(nb);
			m_attribs[i].getAttributesTypes(types);
			for (unsigned int j = 0; j < nb; ++j)
				CGoGNout << "    " << j << " : " << types[j] << " " << names[j] << CGoGNendl;
		}
	}
//	CGoGNout << "RESERVED MARKERS "<< CGoGNendl;
//	for (unsigned int i = 0; i < NB_ORBITS; ++i)
//	{
//		for (unsigned int j = 0; j < NB_THREADS; ++j)
//		{
//			MarkSet ms = m_marksets[i][j];
//			if (!ms.isClear())
//			{
//				CGoGNout << "Orbit " << i << "  thread " << j << " : ";
//				Mark m(1);
//				for (unsigned i = 0; i < Mark::getNbMarks(); ++i)
//				{
//					if (ms.testMark(m))
//						CGoGNout << m.getMarkVal() << ", ";
//					m.setMarkVal(m.getMarkVal()<<1);
//				}
//				CGoGNout << CGoGNendl;
//			}
//		}
//	}
}


void GenericMap::compact(bool topoOnly)
{
	compactTopo();

	if (topoOnly)
		return;

	std::vector<unsigned int> oldnew;

	for (unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		if ((orbit != DART) && (isOrbitEmbedded(orbit)))
		{
			m_attribs[orbit].compact(oldnew);
			for (unsigned int i = m_attribs[DART].begin(); i != m_attribs[DART].end(); m_attribs[DART].next(i))
			{
				unsigned int& idx = m_embeddings[orbit]->operator[](i);
				unsigned int jdx = oldnew[idx];
				if (jdx != 0xffffffff)
					idx = jdx;
			}
		}
	}
}

void GenericMap::compactOrbitContainer(unsigned int orbit, float frag)
{
	std::vector<unsigned int> oldnew;

	if (isOrbitEmbedded(orbit) && (fragmentation(orbit)< frag))
	{
		m_attribs[orbit].compact(oldnew);
		for (unsigned int i = m_attribs[DART].begin(); i != m_attribs[DART].end(); m_attribs[DART].next(i))
		{
			unsigned int& idx = m_embeddings[orbit]->operator[](i);
			unsigned int jdx = oldnew[idx];
			if (jdx != 0xffffffff)
				idx = jdx;
		}
	}
}


void GenericMap::compactIfNeeded(float frag, bool topoOnly)
{
	if (fragmentation(DART)< frag)
		compactTopo();

	if (topoOnly)
		return;

	std::vector<unsigned int> oldnew;

	for (unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		if ((orbit != DART) && (isOrbitEmbedded(orbit)) && (fragmentation(orbit)< frag))
		{
			m_attribs[orbit].compact(oldnew);
			for (unsigned int i = m_attribs[DART].begin(); i != m_attribs[DART].end(); m_attribs[DART].next(i))
			{
				unsigned int& idx = m_embeddings[orbit]->operator[](i);
				unsigned int jdx = oldnew[idx];
				if (jdx != 0xffffffff)
					idx = jdx;
			}
		}
	}
}


void GenericMap::dumpCSV() const
{
	for (unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		CGoGNout << "Container of "<<orbitName(orbit)<< CGoGNendl;
		m_attribs[orbit].dumpCSV();
	}
	CGoGNout << CGoGNendl;
}


void GenericMap::moveData(GenericMap &mapf)
{
	GenericMap::init(false);

	for (unsigned int i = 0; i < NB_ORBITS; ++i)
	{
		this->m_attribs[i].swap(mapf.m_attribs[i]);
		this->m_embeddings[i] = mapf.m_embeddings[i];
		this->m_quickTraversal[i] = mapf.m_quickTraversal[i];
		mapf.m_embeddings[i] = NULL ;
		mapf.m_quickTraversal[i] = NULL;

		for (unsigned int j = 0; j < NB_ORBITS; ++j)
		{
			this->m_quickLocalIncidentTraversal[i][j] = mapf.m_quickLocalIncidentTraversal[i][j];
			this->m_quickLocalAdjacentTraversal[i][j] = mapf.m_quickLocalAdjacentTraversal[i][j];
			mapf.m_quickLocalIncidentTraversal[i][j] = NULL ;
			mapf.m_quickLocalAdjacentTraversal[i][j] = NULL ;
		}

		for (unsigned int j = 0; j < NB_THREADS; ++j)
			this->m_markVectors_free[i][j].swap(mapf.m_markVectors_free[i][j]);
	}

	for (unsigned int i = 0; i < NB_THREADS; ++i)
	{
		this->s_vdartsBuffers[i].swap(mapf.s_vdartsBuffers[i]);
		this->s_vintsBuffers[i].swap(mapf.s_vintsBuffers[i]);
	}

	this->m_boundaryMarkers[0] = mapf.m_boundaryMarkers[0];
	this->m_boundaryMarkers[1] = mapf.m_boundaryMarkers[1];

	this->m_nextMarkerId = mapf.m_nextMarkerId;

	for(auto it = mapf.attributeHandlers.begin(); it != mapf.attributeHandlers.end(); ++it)
	   (*it).second->setInvalid() ;
	mapf.attributeHandlers.clear() ;
}

void GenericMap::garbageMarkVectors()
{
	unsigned int maxId=0;

	for (unsigned int orbit=0; orbit<NB_ORBITS;++orbit)
	{
		std::vector<AttributeMultiVector<MarkerBool>*>& amvv = m_attribs[orbit].getMarkerAttributes();

		for (auto it = amvv.begin(); it != amvv.end(); ++it )
		{
			AttributeMultiVector<MarkerBool>* amv = *it;
			const std::string& name = amv->getName();
			if (name.substr(0,7) == "marker_")
			{
				// store tne next free index for unique numbering
				std::string num = name.substr(name.length()-3,3);
				unsigned int id = 100*(num[0]-'0')+10*(num[1]-'0')+(num[2]-'0');
				if (id > maxId)
					maxId = id;

				amv->allFalse();
				m_markVectors_free[orbit][0].push_back(amv);
			}
		}
	}
	m_nextMarkerId = maxId+1;
}

void GenericMap::removeMarkVectors()
{
	for (unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		std::vector<std::string> attNames;
		m_attribs[orbit].getAttributesNames(attNames);
		for (auto sit = attNames.begin(); sit != attNames.end(); ++sit)
		{
			if (sit->substr(0,7) == "marker_")
			{
				m_attribs[orbit].removeAttribute<MarkerBool>(*sit);
			}
		}
	}
	m_nextMarkerId = 0;
}

} // namespace CGoGN
