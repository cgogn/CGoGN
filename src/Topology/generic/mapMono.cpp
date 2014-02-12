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

#include "Topology/generic/mapMono.h"

namespace CGoGN
{

/****************************************
 *             SAVE & LOAD              *
 ****************************************/

bool MapMono::saveMapBin(const std::string& filename) const
{
	CGoGNostream fs(filename.c_str(), std::ios::out|std::ios::binary);
	if (!fs)
	{
		CGoGNerr << "Unable to open file for writing: " << filename << CGoGNendl;
		return false;
	}

	// Entete
	char* buff = new char[256];
	for (int i = 0; i < 256; ++i)
		buff[i] = char(255);

	memcpy(buff, "CGoGN_Map", 10);

	std::string mt = mapTypeName();
	const char* mtc = mt.c_str();
	memcpy(buff+32, mtc, mt.size()+1);
	unsigned int *buffi = reinterpret_cast<unsigned int*>(buff + 64);
	*buffi = NB_ORBITS;
	fs.write(reinterpret_cast<const char*>(buff), 256);
	delete buff;

	// save all attribs
	for (unsigned int i = 0; i < NB_ORBITS; ++i)
		m_attribs[i].saveBin(fs, i);

	return true;
}

bool MapMono::loadMapBin(const std::string& filename)
{
	CGoGNistream fs(filename.c_str(), std::ios::in|std::ios::binary);
	if (!fs)
	{
		CGoGNerr << "Unable to open file for loading" << CGoGNendl;
		return false;
	}

	GenericMap::clear(true);

	// read info
	char* buff = new char[256];
	fs.read(reinterpret_cast<char*>(buff), 256);

	std::string buff_str(buff);
	// Check file type
	if (buff_str == "CGoGN_MRMap")
	{
		CGoGNerr<< "Wrong binary file format, file is a MR-Map"<< CGoGNendl;
		return false;
	}
	if (buff_str != "CGoGN_Map")
	{
		CGoGNerr<< "Wrong binary file format"<< CGoGNendl;
		return false;
	}

	// Check map type
	buff_str = std::string(buff + 32);

	std::string localType = this->mapTypeName();

	std::string fileType = buff_str;

	if (fileType != localType)
	{
		CGoGNerr << "Not possible to load "<< fileType << " into " << localType << " object" << CGoGNendl;
		return false;
	}

	// Check max nb orbit
	unsigned int *ptr_nbo = reinterpret_cast<unsigned int*>(buff + 64);
	unsigned int nbo = *ptr_nbo;
	if (nbo != NB_ORBITS)
	{
		CGoGNerr << "Wrond max orbit number in file" << CGoGNendl;
		return  false;
	}

	// load attrib container
	for (unsigned int i = 0; i < NB_ORBITS; ++i)
	{
		unsigned int id = AttributeContainer::loadBinId(fs);
		m_attribs[id].loadBin(fs);
	}

	// retrieve m_embeddings (from m_attribs)
	update_m_emb_afterLoad();

	// recursive call from real type of map (for topo relation attributes pointers) down to GenericMap (for Marker_cleaning & pointers)
	update_topo_shortcuts();

	// restore nbThreads
	std::vector<std::string> typeMark;
	unsigned int nbatt0 = m_attribs[0].getAttributesTypes(typeMark);
	m_nbThreads = 0;
	for (unsigned int i = 0; i < nbatt0; ++i)
	{
		if (typeMark[i] == "Mark")
			++m_nbThreads;
	}

	// restore quick traversals pointers if necessary (containers  already ok)
	for (unsigned int orb=0; orb<NB_ORBITS; ++orb)
	{
		m_quickTraversal[orb] = m_attribs[orb].getDataVector<Dart>("quick_traversal") ;
		for(unsigned int j = 0; j < NB_ORBITS; ++j)
		{
			std::stringstream ss;
			ss << "quickLocalIncidentTraversal_" << j;
			m_quickLocalIncidentTraversal[orb][j] = m_attribs[orb].getDataVector< NoTypeNameAttribute<std::vector<Dart> > >(ss.str()) ;
			std::stringstream ss2;
			ss2 << "quickLocalAdjacentTraversal" << j;
			m_quickLocalAdjacentTraversal[orb][j] = m_attribs[orb].getDataVector< NoTypeNameAttribute<std::vector<Dart> > >(ss2.str()) ;
		}
	}

	return true;
}

bool MapMono::copyFrom(const GenericMap& map)
{
	const MapMono& mapM = reinterpret_cast<const MapMono&>(map);

	if (mapTypeName() != map.mapTypeName())
	{
		CGoGNerr << "try to copy from incompatible type map" << CGoGNendl;
		return false;
	}

	GenericMap::clear(true);

	// load attrib container
	for (unsigned int i = 0; i < NB_ORBITS; ++i)
		m_attribs[i].copyFrom(mapM.m_attribs[i]);

	// retrieve m_embeddings (from m_attribs)
	update_m_emb_afterLoad();

	// recursive call from real type of map (for topo relation attributes pointers) down to GenericMap (for Marker_cleaning & pointers)
	update_topo_shortcuts();

	return true;
}

} //namespace CGoGN
