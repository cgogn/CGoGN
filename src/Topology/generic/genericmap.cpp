/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009, IGG Team, LSIIT, University of Strasbourg                *
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
* Web site: https://iggservis.u-strasbg.fr/CGoGN/                              *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include "Topology/generic/genericmap.h"
#include "Topology/generic/dartmarker.h"
#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"
#include "Container/registered.h"

namespace CGoGN
{

std::map<std::string, RegisteredBaseAttribute*>* GenericMap::m_attributes_registry_map = NULL ;

GenericMap::GenericMap() : m_nbThreads(1)
{
	if(m_attributes_registry_map == NULL)
		m_attributes_registry_map = new std::map<std::string, RegisteredBaseAttribute*> ;

	// register all known types
	registerAttribute<Dart>("Dart");
	registerAttribute<Mark>("Mark");

	registerAttribute<long>("long");
	registerAttribute<int>("int");
	registerAttribute<short>("short");
	registerAttribute<char>("char");

	registerAttribute<unsigned long>("unsigned long");
	registerAttribute<unsigned int>("unsigned int");
	registerAttribute<unsigned short>("unsigned short");
	registerAttribute<unsigned char>("unsigned char");

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

	for(unsigned int i = 0; i < NB_ORBITS; ++i)
	{
		m_attribs[i].setOrbit(i) ;
		m_attribs[i].setRegistry(m_attributes_registry_map) ;
		m_embeddings[i] = NULL ;
		for (unsigned int j = 0; j < NBTHREAD; ++j)
			m_markerTables[i][j] = NULL ;
	}
}

GenericMap::~GenericMap()
{
	for(unsigned int i = 0; i < NB_ORBITS; ++i)
	{
		if(isOrbitEmbedded(i))
			m_attribs[i].clear(true) ;
	}
	if(m_attributes_registry_map)
		delete m_attributes_registry_map;
}

/****************************************
 *         EMBEDDING MANAGEMENT         *
 ****************************************/

void GenericMap::setDartEmbedding(unsigned int orbit, Dart d, unsigned int emb)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");
	unsigned int old = getDartEmbedding(orbit, d);

	// if same emb nothing to do
	if (old == emb)
		return;
	// if different then unref the old emb
	if (old != EMBNULL)
	{
		if(m_attribs[orbit].unrefLine(old))
			for (unsigned int t=0; t<m_nbThreads; ++t)
				m_markerTables[orbit][t]->operator[](old).clear();
	}
	// ref the new emb
	if (emb != EMBNULL)
		m_attribs[orbit].refLine(emb);
	// affect the embedding to the dart
	(*m_embeddings[orbit])[d.index] = emb ;
}

/****************************************
 *        ATTRIBUTES MANAGEMENT         *
 ****************************************/

void GenericMap::initOrbitEmbedding(unsigned int orbit, bool realloc)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded") ;
	DartMarker mark(*this) ;
	for(Dart d = begin(); d != end(); next(d))
	{
		if(!mark.isMarked(d))
		{
			mark.markOrbit(orbit, d) ;
			if(realloc || getEmbedding(d, orbit) == EMBNULL)
				embedNewCell(orbit, d) ;
		}
	}
}

template <typename R>
bool GenericMap::registerAttribute(const std::string &nameType)
{
	RegisteredBaseAttribute* ra = new RegisteredAttribute<R>;
	if (ra == NULL)
	{
		std::cerr << "Erreur enregistrement attribut" << std::endl;
		return false;
	}

	ra->setTypeName(nameType);

	m_attributes_registry_map->insert(std::pair<std::string, RegisteredBaseAttribute*>(nameType,ra));
	return true;
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
			if (isOrbitEmbedded(i))
			{
				std::stringstream ss ;
				ss << "Mark_"<< th ;
				AttributeContainer& cellCont = m_attribs[i] ;
				AttributeMultiVector<Mark>* amvMark = cellCont.addAttribute<Mark>(ss.str()) ;
				m_markerTables[i][th] = amvMark ;
			}
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
			if (isOrbitEmbedded(i))
			{
				std::stringstream ss ;
				ss << "Mark_"<< th ;
				AttributeContainer& cellCont = m_attribs[i] ;
				cellCont.removeAttribute<Mark>(ss.str()) ;
				m_markerTables[i][th] = NULL ;
			}
		}
	}
}

/****************************************
 *             SAVE & LOAD              *
 ****************************************/

void GenericMap::update_m_emb_afterLoad()
{
	// get container of dart orbit
	AttributeContainer& cont = m_attribs[DART_ORBIT] ;

	// get the list of attributes
	std::vector<std::string> listeNames;
	cont.getAttributesNames(listeNames);

	// check if there are EMB_X attributes
	for (unsigned int i = 0;  i < listeNames.size(); ++i)
	{
		std::string sub = listeNames[i].substr(0,listeNames[i].size()-1);
		if (sub == "EMB_")
		{
			unsigned int orb = listeNames[i][4]-'0'; // easy atoi computation for one char;

			AttributeMultiVector<unsigned int>* amv = cont.getDataVector<unsigned int>(i);
			m_embeddings[orb] = amv ;
			std::cout << "Ajoute m_emb[" << orb << "] : " << i << std::endl;
		}
	}
}

bool GenericMap::saveMapXml(const std::string& filename, bool compress)
{
	xmlTextWriterPtr writer = xmlNewTextWriterFilename(filename.c_str(), 0);
	xmlTextWriterStartDocument(writer,NULL,NULL,NULL);

	// Entete
	int rc = xmlTextWriterStartElement(writer, BAD_CAST "CGoGN_Map");
	rc = xmlTextWriterWriteAttribute(writer,  BAD_CAST "type", BAD_CAST this->mapTypeName().c_str());
	rc = xmlTextWriterWriteFormatAttribute(writer,  BAD_CAST "nb_max_orbits","%u",NB_ORBITS);

	// save m_attribs
	for (unsigned int i=0; i<NB_ORBITS; ++i)
	{
		m_attribs[i].saveXml(writer,i);
	}


	// save m_orbMarker
	rc = xmlTextWriterStartElement(writer, BAD_CAST "Orbit_MarkerSet");
	for (unsigned int i=0; i<NB_ORBITS; ++i)
	{
		int rc = xmlTextWriterStartElement(writer, BAD_CAST "MarkerSet");
		rc = xmlTextWriterWriteFormatAttribute(writer,  BAD_CAST "orbit", "%u", i);
		rc = xmlTextWriterWriteFormatAttribute(writer,  BAD_CAST "val", "%u", m_orbMarker[i][0].getMarkVal());
//		rc = xmlTextWriterWriteAttribute(writer,  BAD_CAST "bin", BAD_CAST m_orbMarker[i][0].getMarkerAsBinaryString().c_str());
//		m_orbMarker[i] ;
		rc = xmlTextWriterEndElement(writer);
	}
	rc = xmlTextWriterEndElement(writer);

	// save m_BoundaryMarkerVal
//	rc = xmlTextWriterStartElement(writer, BAD_CAST "Boundary_Marker");
//	rc = xmlTextWriterWriteFormatAttribute(writer,  BAD_CAST "val", "%u", m_BoundaryMarkerVal.getMarkVal());
//	rc = xmlTextWriterEndElement(writer);

	rc = xmlTextWriterEndElement(writer); // of map

	xmlTextWriterEndDocument(writer);
	xmlFreeTextWriter(writer);

	return true;
}


bool GenericMap::loadMapXml(const std::string& filename, bool compress)
{

	xmlDocPtr doc = xmlReadFile(filename.c_str(), NULL, 0);
	xmlNodePtr map_node = xmlDocGetRootElement(doc);

	// Check if it is a CGoGN_Map file
//	if (strcmp((char*)(map_node->name),(char*)"CGoGN_Map")!=0)
	if (!chechXmlNode(map_node,"CGoGN_Map"))
	{
		std::cerr << "Wrong xml format: Root node != CGoGN_Map"<< std::endl;
		return false;
	}

	// check the file type
	xmlChar *prop = xmlGetProp(map_node, BAD_CAST "type");
	std::cout << "Loading "<< prop <<" xml file"<< std::endl;

	// check the nb max orbits
	prop = xmlGetProp(map_node, BAD_CAST "nb_max_orbits");
	unsigned int nbo = atoi((char*)prop);
	if (nbo != NB_ORBITS)
	{
		std::cerr << "Wrong nb max orbits in xml map"<< std::endl;
		return false;
	}

	/***********************************************
	*	 			 load attributs
	************************************************/
	for (xmlNode* cur_node = map_node->children; cur_node; cur_node = cur_node->next)
	{
		// for each attribute
//		if (strcmp((char*)(cur_node->name),(char*)"Attributes_Container")==0)
		if (chechXmlNode(cur_node, "Attributes_Container"))
		{
			std::cout << "LOAD ATTRIBUT"<< std::endl;
			// get the orbit id
			unsigned int id = AttributeContainer::getIdXmlNode(cur_node);
			// and load container
			unsigned int nba = m_attribs[id].getNbAttributes();


			std::cout << "attribut "<<id<<" size="<< m_attribs[id].size()<< "  nbatt="<< nba<< std::endl;

			m_attribs[id].loadXml(cur_node);
		}
	}

	/***********************************************
	*   creation of the m_embeddings pointers table
	************************************************/
//	// get attribute names of dart orbit
//	AttributeContainer& contDart = m_attribs[DART_ORBIT] ;
//	std::vector< std::string > tableNames;
//	contDart.getAttributesStrings(tableNames);
//
//	// find orbit frome name and store pointer in right place
//	for (unsigned int i = 0; i< tableNames.size(); ++i)
//	{
////		std::cout << i <<" : "<< tableNames[i]<<std::endl;
//
//		std::string& name = tableNames[i];
//		std::string is_an_emb = name.substr(0,4);
//		if (is_an_emb == "EMB_")
//		{
//			AttributeMultiVector<unsigned int>& amv = contDart.getDataVector<unsigned int>(i) ;
//
//			std::string orbitname = name.substr(4, name.size()-4);
//			std::istringstream iss(orbitname);
//			unsigned int orbit;
//			iss >> orbit;
//
//			m_embeddings[orbit] = &amv ;
//		}
//	}
//
//
	update_m_emb_afterLoad();

	/***********************************************
	*	 load Orbit_MarkerSet & BoundaryMarker
	************************************************/
	xmlNode* cur_node = map_node->children;
	bool read1=false;
	bool read2=false;
	while (!(read1 || read2) && cur_node)// scan nodes to find the one with right name
	{
//		if (strcmp((char*)(cur_node->name),(char*)"Orbit_MarkerSet") == 0)
		if (chechXmlNode(cur_node, "Orbit_MarkerSet"))
		{
			for (xmlNode* mark_node = cur_node->children; mark_node; mark_node = mark_node->next)
			{
				xmlChar* prop = xmlGetProp(mark_node, BAD_CAST "orbit");
				unsigned int orb = atoi((char*)prop);
				prop = xmlGetProp(mark_node, BAD_CAST "val");
				unsigned int val = atoi((char*)prop);
				m_orbMarker[orb][0].setMarkVal(val);
			}
			read1 =true;
		}
		else
		{
//			if (strcmp((char*)(cur_node->name),(char*)"Boundary_Marker") == 0)
			std::cout << "Orbit_MarkerSet"<<std::endl;
//			if (chechXmlNode(cur_node, "Boundary_Marker"))
//			{
//				xmlChar* prop = xmlGetProp(cur_node, BAD_CAST "val");
//				unsigned int val = atoi((char*)prop);
//				m_BoundaryMarkerVal.setMarkVal(val);
//				read2 =true;
//			}
		}
		// next node
		cur_node = cur_node->next;
	}

	if (!(read1 && read2))
	{
		std::cerr <<"Error reading Marker in xml node"<<std::endl;
		return false;
	}

	xmlFreeDoc(doc);

	update_m_emb_afterLoad();

	return true ;
}

bool GenericMap::saveMapBin(const std::string& filename)
{
	CGoGNostream fs(filename.c_str(), std::ios::out|std::ios::binary);
	if (!fs)
	{
		std::cerr << "Unable to open file for writing: " << filename << std::endl;
		return false;
	}

	// Entete
	char* buff = new char[256];
	for (int i=0; i< 256; ++i)
		buff[i] = char(255);
	const char* cgogn = "CGoGN_Map";
	memcpy(buff,cgogn,10);
	std::string mt = mapTypeName();
	const char* mtc = mt.c_str();
	memcpy(buff+32,mtc,mt.size()+1);
	unsigned int *buffi = reinterpret_cast<unsigned int*>(buff+64);
	*buffi = NB_ORBITS;
	fs.write(reinterpret_cast<const char*>(buff), 256);

	delete buff;

	// save m_attribs
	for (unsigned int i=0; i<NB_ORBITS; ++i)
	{
		m_attribs[i].saveBin(fs,i);
	}


	std::vector<unsigned int> buffer;
	buffer.reserve(NB_ORBITS+1);
	for (unsigned int i=0; i<NB_ORBITS; ++i)
	{
		buffer.push_back(m_orbMarker[i][0].getMarkVal());
	}
//	buffer.push_back(m_BoundaryMarkerVal.getMarkVal());

	fs.write(reinterpret_cast<const char*>(&(buffer[0])), buffer.size()*sizeof(unsigned int));

	return true;
}

bool GenericMap::loadMapBin(const std::string& filename)
{
	CGoGNistream fs(filename.c_str(), std::ios::in|std::ios::binary);
	if (!fs)
	{
		std::cerr << "Unable to open file for loading" << std::endl;
		return false;
	}

	// entete
	char* buff = new char[256];
	fs.read(reinterpret_cast<char*>(buff), 256);

	std::string buff_str(buff);
	// Check file type
	if (buff_str != "CGoGN_Map")
	{
		std::cerr<< "Wrong binary file format"<< std::endl;
		return false;
	}

	// Check map type
	buff_str = std::string(buff+32);
	std::cout << "Map type file = "<< buff_str<< std::endl;

	std::string localType = this->mapTypeName();
	localType = localType.substr(0,localType.size()-1);

	std::string fileType = buff_str.substr(0,buff_str.size()-1);

	if (fileType != localType)
	{
		std::cerr << "Not possible to load "<< fileType<< " into "<< localType << " object"<<std::endl;
		return false;
	}

	// Check max nb orbit
	unsigned int *ptr_nbo = reinterpret_cast<unsigned int*>(buff+64);
	unsigned int nbo = *ptr_nbo;
	if (nbo != NB_ORBITS)
	{
		std::cerr << "Wrond max orbit number in file" << std::endl;
		return  false;
	}

	// load attrib container
	for (unsigned int i=0; i<NB_ORBITS; ++i)
	{
		unsigned int id = AttributeContainer::loadBinId(fs);
		m_attribs[id].loadBin(fs);
	}

	// load MarkerSet & boundary
	std::vector<unsigned int> buffer;
	buffer.resize(NB_ORBITS);
	fs.read(reinterpret_cast<char*>(&(buffer[0])), (NB_ORBITS+1)*sizeof(unsigned int));
	for (unsigned int i=0; i<NB_ORBITS; ++i)
	{
		m_orbMarker[i][0].setMarkVal(buffer[i]);
	}
//	m_BoundaryMarkerVal.setMarkVal(buffer[NB_ORBITS]);

	update_m_emb_afterLoad();

	return true;
}

/****************************************
 *           DARTS TRAVERSALS           *
 ****************************************/

bool GenericMap::foreach_dart(FunctorType& f, const FunctorSelect& good)
{
	for (Dart d = begin(); d != end(); next(d))
	{
		if(good(d))
			if (f(d))
				return true;
	}
	return false;
}

bool GenericMap::foreach_dart_of_orbit(unsigned int orbit, Dart d, FunctorType& f, unsigned int thread)
{
	switch(orbit)
	{
		case  DART_ORBIT: return f(d);
		case  VERTEX_ORBIT: return foreach_dart_of_vertex(d, f,thread);
		case  EDGE_ORBIT: return foreach_dart_of_edge(d, f,thread);
		case  FACE_ORBIT: return foreach_dart_of_face(d, f,thread);
		case  VOLUME_ORBIT: return foreach_dart_of_volume(d, f, thread);
//		case -1: return foreach_dart_of_cc(d,f,thread);
		default: assert(!"Cells of this dimension are not handled");
	}
	return false;
}

bool GenericMap::foreach_orbit(unsigned int orbit, FunctorType& fonct, const FunctorSelect& good, unsigned int thread)
{
	DartMarker marker(*this,thread);	// Lock a marker
	bool found = false;			// Store the result

	// Scan all darts of the map
	for (Dart d = begin(); !found && d != end(); next(d))
	{
		if (good(d))							// If d is selected
		{
			if (!marker.isMarked(d))			// If d is in a not yet visited cell
			{
				if ((fonct)(d))					// Apply functor
					found = true;
				else
					marker.markOrbit(orbit, d);	// Mark the cell as visited
			}
		}
	}
	return found;
}

unsigned int GenericMap::getNbOrbits(unsigned int orbit, const FunctorSelect& good)
{
	FunctorCount fcount;
	foreach_orbit(orbit, fcount, good);
	return fcount.getNb();
}

} // namespace CGoGN
