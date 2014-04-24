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

#include "Topology/generic/genericmap.h"
#include "Topology/generic/attributeHandler.h"
#include "Topology/generic/traversor/traversorCell.h"

#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"
#include "Container/registered.h"

namespace CGoGN
{

std::map<std::string, RegisteredBaseAttribute*>* GenericMap::m_attributes_registry_map = NULL;
int GenericMap::m_nbInstances = 0;

GenericMap::GenericMap() : m_nbThreads(1)
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
	}

	m_nbInstances++;

	for(unsigned int i = 0; i < NB_ORBITS; ++i)
	{
		m_attribs[i].setOrbit(i) ;
		m_attribs[i].setRegistry(m_attributes_registry_map) ;
		for(unsigned int j = 0; j < NB_THREAD; ++j)
		{
			m_marksets[i][j].clear() ;
			m_markTables[i][j] = NULL ;
		}
	}

	init();

	for (unsigned int i = 0; i < NB_THREAD; ++i)
	{
		dartMarkers[i].reserve(16) ;
		cellMarkers[i].reserve(16) ;
	}

	// get & lock marker for boundary
	m_boundaryMarkers[0] = m_marksets[DART][0].getNewMark();
	m_boundaryMarkers[1] = m_marksets[DART][0].getNewMark();
}

GenericMap::~GenericMap()
{
	// release marker for boundary
	m_marksets[DART][0].releaseMark(m_boundaryMarkers[0]);
	m_marksets[DART][0].releaseMark(m_boundaryMarkers[1]);

	for(unsigned int i = 0; i < NB_ORBITS; ++i)
	{
		if(isOrbitEmbedded(i))
			m_attribs[i].clear(true) ;
	}

	for(std::multimap<AttributeMultiVectorGen*, AttributeHandlerGen*>::iterator it = attributeHandlers.begin(); it != attributeHandlers.end(); ++it)
		(*it).second->setInvalid() ;
	attributeHandlers.clear() ;

	for (unsigned int i = 0; i < NB_THREAD; ++i)
	{
		for(std::vector<DartMarkerGen*>::iterator it = dartMarkers[i].begin(); it != dartMarkers[i].end(); ++it)
			(*it)->setReleaseOnDestruct(false) ;
		dartMarkers[i].clear() ;

		for(std::vector<CellMarkerGen*>::iterator it = cellMarkers[i].begin(); it != cellMarkers[i].end(); ++it)
			(*it)->setReleaseOnDestruct(false) ;
		cellMarkers[i].clear() ;
	}

	// clean type registry if necessary
	m_nbInstances--;
	if (m_nbInstances <= 0)
	{
		for (std::map<std::string, RegisteredBaseAttribute*>::iterator it =  m_attributes_registry_map->begin(); it != m_attributes_registry_map->end(); ++it)
			delete it->second;

		delete m_attributes_registry_map;
		m_attributes_registry_map = NULL;
	}
}

void GenericMap::init()
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

		AttributeContainer& cont = m_attribs[i];
		for (unsigned int t = 0; t < m_nbThreads; ++t)
		{
			std::stringstream ss ;
			ss << "Mark_" << t ;
			AttributeMultiVector<Mark>* amvMark = cont.addAttribute<Mark>(ss.str()) ;
//			for(unsigned int idx = cont.begin(); idx < cont.end(); cont.next(idx))
//				amvMark->operator[](idx).clear() ;
			m_markTables[i][t] = amvMark ;
		}
	}

	for (unsigned int j = 0; j < NB_THREAD; ++j)
	{
		std::vector<CellMarkerGen*>& cmg = cellMarkers[j];
		for(unsigned int i = 0; i < cmg.size(); ++i)
		{
			CellMarkerGen* cm = cmg[i] ;
			cm->updateMarkVector(m_markTables[cm->getCell()][cm->getThread()]) ;
		}

		std::vector<DartMarkerGen*>& dmg = dartMarkers[j];
		for(unsigned int i = 0; i < dmg.size(); ++i)
		{
			DartMarkerGen* dm = dmg[i] ;
			dm->updateMarkVector(m_markTables[DART][dm->getThread()]) ;
		}
	}

	for(std::multimap<AttributeMultiVectorGen*, AttributeHandlerGen*>::iterator it = attributeHandlers.begin(); it != attributeHandlers.end(); ++it)
		(*it).second->setInvalid() ;
	attributeHandlers.clear() ;
}

void GenericMap::clear(bool removeAttrib)
{
	if (removeAttrib)
		init();
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

	for(unsigned int t = 0; t < m_nbThreads; ++t)
	{
		AttributeMultiVector<Mark>* m = m_markTables[orbit1][t] ;
		m_markTables[orbit1][t] = m_markTables[orbit2][t] ;
		m_markTables[orbit2][t] = m ;

		MarkSet ms = m_marksets[orbit1][t] ;
		m_marksets[orbit1][t] = m_marksets[orbit2][t] ;
		m_marksets[orbit2][t] = ms ;
	}

	for (unsigned int i = 0; i < NB_THREAD; ++i)
	{
		for(std::vector<CellMarkerGen*>::iterator it = cellMarkers[i].begin(); it != cellMarkers[i].end(); ++it)
		{
			if((*it)->m_cell == orbit1)
				(*it)->m_cell = orbit2 ;
			else if((*it)->m_cell == orbit2)
				(*it)->m_cell = orbit1 ;
		}
	}
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

	std::cout << "m_markTables: " ;
	for (unsigned int i = 0; i < NB_ORBITS; ++i)
		std::cout << (long)(m_markTables[i][0]) << " / " ;
	std::cout << std::endl << "-------------------------" << std::endl << std::dec ;
}

void GenericMap::printDartsTable()
{
	std::cout << "======================="<< std::endl ;

	//m_attribs[DART]
}

/****************************************
 *          THREAD MANAGEMENT           *
 ****************************************/

void GenericMap::addThreadMarker(unsigned int nb)
{
	unsigned int th ;

	for (unsigned int j = 0; j < nb; ++j)
	{
		th = m_nbThreads ;
		m_nbThreads++ ;

		for (unsigned int i = 0; i < NB_ORBITS; ++i)
		{
			std::stringstream ss ;
			ss << "Mark_"<< th ;
			AttributeContainer& cellCont = m_attribs[i] ;
			AttributeMultiVector<Mark>* amvMark = cellCont.addAttribute<Mark>(ss.str()) ;
			m_markTables[i][th] = amvMark ;
		}
	}
}

unsigned int GenericMap::getNbThreadMarkers()
{
	return m_nbThreads;
}

void GenericMap::removeThreadMarker(unsigned int nb)
{
	unsigned int th = 0;
	while ((m_nbThreads > 1) && (nb > 0))
	{
		th = --m_nbThreads ;
		--nb;
		for (unsigned int i = 0; i < NB_ORBITS; ++i)
		{
			std::stringstream ss ;
			ss << "Mark_"<< th ;
			AttributeContainer& cellCont = m_attribs[i] ;
			cellCont.removeAttribute<Mark>(ss.str()) ;
			m_markTables[i][th] = NULL ;
		}
	}
}

/****************************************
 *             SAVE & LOAD              *
 ****************************************/

void GenericMap::restore_shortcuts()
{
	// NB THREADS

	std::vector<std::string> typeMark;
	unsigned int nbatt0 = m_attribs[0].getAttributesTypes(typeMark);
	m_nbThreads = 0;
	for (unsigned int i = 0; i < nbatt0; ++i)
	{
		if (typeMark[i] == "Mark")
			++m_nbThreads;
	}

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

	// MARKERS & QUICK TRAVERSAL

	for(unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		AttributeContainer& cont = m_attribs[orbit];

		// QUICK TRAVERSAL

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

		// MARKERS

		std::vector<std::string> listeNames;
		cont.getAttributesNames(listeNames);

		for (unsigned int i = 0;  i < listeNames.size(); ++i)
		{
			std::string sub = listeNames[i].substr(0, 5);
			if (sub == "Mark_")
			{
				// get thread number
				unsigned int thread = listeNames[i][5] - '0';
				if (listeNames[i].size() > 6) 					// thread number is >9
					thread = 10 * thread + (listeNames[i][6] - '0');

				AttributeMultiVector<Mark>* amvMark = cont.getDataVector<Mark>(i);
				m_markTables[orbit][thread] = amvMark ;

				if ((orbit == DART) && (thread == 0))	// for Dart Marker of thread O
				{										// clear all marks expect boundary marks
					Mark m(m_boundaryMarkers[0] + m_boundaryMarkers[1]);
					m.invert();
					for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
						amvMark->operator[](i).unsetMark(m);
				}
				else									// for others clear all
				{
					for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
						amvMark->operator[](i).clear();
				}
			}
		}
	}

	// restore mark vectors in Dart & Cell Markers
	for (unsigned int j = 0; j < NB_THREAD; ++j)
	{
		for (std::vector<DartMarkerGen*>::iterator it = dartMarkers[j].begin(); it != dartMarkers[j].end(); ++it)
			(*it)->updateMarkVector(m_markTables[DART][(*it)->getThread()]);

		for (std::vector<CellMarkerGen*>::iterator it = cellMarkers[j].begin(); it != cellMarkers[j].end(); ++it)
			(*it)->updateMarkVector(m_markTables[(*it)->getCell()][(*it)->getThread()]);
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
		unsigned int nb = names.size();
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
	CGoGNout << "RESERVED MARKERS "<< CGoGNendl;
	for (unsigned int i = 0; i < NB_ORBITS; ++i)
	{
		for (unsigned int j = 0; j < NB_THREAD; ++j)
		{
			MarkSet ms = m_marksets[i][j];
			if (!ms.isClear())
			{
				CGoGNout << "Orbit " << i << "  thread " << j << " : ";
				Mark m(1);
				for (unsigned i = 0; i < Mark::getNbMarks(); ++i)
				{
					if (ms.testMark(m))
						CGoGNout << m.getMarkVal() << ", ";
					m.setMarkVal(m.getMarkVal()<<1);
				}
				CGoGNout << CGoGNendl;
			}
		}
	}
}

void GenericMap::compact()
{
	// compact embedding attribs
	std::vector< std::vector<unsigned int>* > oldnews;
	oldnews.resize(NB_ORBITS);
	for (unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		if ((orbit != DART) && (isOrbitEmbedded(orbit)))
		{
			oldnews[orbit] = new std::vector<unsigned int>;
			m_attribs[orbit].compact(*(oldnews[orbit]));
		}
	}

	// update embedding indices of topo
	for (unsigned int i = m_attribs[DART].begin(); i != m_attribs[DART].end(); m_attribs[DART].next(i))
	{
		for (unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
		{
			if ((orbit != DART) && (isOrbitEmbedded(orbit)))
			{
				unsigned int& idx = m_embeddings[orbit]->operator[](i);
				unsigned int jdx = oldnews[orbit]->operator[](idx);
				if ((jdx != 0xffffffff) && (jdx != idx))
					idx = jdx;
			}
		}
	}

	// delete allocated vectors
	for (unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
		if ((orbit != DART) && (isOrbitEmbedded(orbit)))
			delete[] oldnews[orbit];

	// compact topo (depends on map implementation)
	compactTopo();
}

} // namespace CGoGN
