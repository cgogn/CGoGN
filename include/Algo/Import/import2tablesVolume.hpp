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

namespace CGoGN
{

namespace Algo
{

namespace Import
{

template <typename PFP>
ImportVolumique::ImportType MeshTablesVolume<PFP>::getFileType(const std::string& filename)
{
	if ((filename.rfind(".tet")!=std::string::npos) || (filename.rfind(".TET")!=std::string::npos))
		return ImportVolumique::TET;

	if ((filename.rfind(".node")!=std::string::npos) || (filename.rfind(".NODE")!=std::string::npos))
		return ImportVolumique::NODE;

	if ((filename.rfind(".ts")!=std::string::npos) || (filename.rfind(".TS")!=std::string::npos))
		return ImportVolumique::TS;

	return ImportVolumique::UNKNOWNVOLUME;
}

template <typename PFP>
bool MeshTablesVolume<PFP>::importMesh(const std::string& filename, std::vector<std::string>& attrNames, float scaleFactor)
{
	ImportVolumique::ImportType kind = getFileType(filename);

	switch (kind)
	{
	case ImportVolumique::TET:
		return importTet(filename, attrNames, scaleFactor);
		break;
	case ImportVolumique::NODE:
		break;
	case ImportVolumique::TS:
		break;
	default:
		CGoGNerr << "Not yet supported" << CGoGNendl;
		break;
	}
	return false;
}

template <typename PFP>
bool MeshTablesVolume<PFP>::importTet(const std::string& filename, std::vector<std::string>& attrNames, float scaleFactor)
{
	AttributeHandler<VEC3> positions =  m_map.template getAttribute<VEC3>(VERTEX, "position") ;

	if (!positions.isValid())
		positions = m_map.template addAttribute<VEC3>(VERTEX, "position") ;

	attrNames.push_back(positions.name()) ;

	AttributeContainer& container = m_map.getAttributeContainer(VERTEX) ;

	//open file
	std::ifstream fp(filename.c_str(), std::ios::in);
	if (!fp.good())
	{
		CGoGNerr << "Unable to open file " << filename << CGoGNendl;
		return false;
	}

	std::string ligne;
	unsigned int nbv, nbt;
	// reading number of vertices
	std::getline (fp, ligne);
	std::stringstream oss(ligne);
	oss >> nbv;

	// reading number of tetrahedra
	std::getline (fp, ligne);
	std::stringstream oss2(ligne);
	oss2 >> nbt;

	//reading vertices
	std::vector<unsigned int> verticesID;
	verticesID.reserve(nbv);
	for(unsigned int i = 0; i < nbv;++i)
	{
		do
		{
			std::getline (fp, ligne);
		} while (ligne.size() == 0);

		std::stringstream oss(ligne);

		float x,y,z;
		oss >> x;
		oss >> y;
		oss >> z;
		// TODO : if required read other vertices attributes here
		VEC3 pos(x*scaleFactor,y*scaleFactor,z*scaleFactor);

		unsigned int id = container.insertLine();
		positions[id] = pos;

		verticesID.push_back(id);
	}

	m_nbVertices = nbv;
	m_nbVolumes = nbt;


	// lecture tetra
	// normalement m_nbVolumes*12 (car on ne charge que des tetra)

	m_nbFaces=nbt*4;
	m_emb.reserve(nbt*12);

	for (unsigned int i = 0; i < m_nbVolumes ; ++i)
	{
    	do
    	{
    		std::getline (fp, ligne);
    	} while (ligne.size()==0);

		std::stringstream oss(ligne);
		int n;
		oss >> n; // nb de faces d'un volume ?

		m_nbEdges.push_back(3);
		m_nbEdges.push_back(3);
		m_nbEdges.push_back(3);
		m_nbEdges.push_back(3);

		int s0,s1,s2,s3;

		oss >> s0;
		oss >> s1;
		oss >> s2;
		oss >> s3;

		m_emb.push_back(verticesID[s0]);
		m_emb.push_back(verticesID[s1]);
		m_emb.push_back(verticesID[s2]);

		m_emb.push_back(verticesID[s1]);
		m_emb.push_back(verticesID[s0]);
		m_emb.push_back(verticesID[s3]);

		m_emb.push_back(verticesID[s2]);
		m_emb.push_back(verticesID[s1]);
		m_emb.push_back(verticesID[s3]);

		m_emb.push_back(verticesID[s0]);
		m_emb.push_back(verticesID[s2]);
		m_emb.push_back(verticesID[s3]);
	}

	fp.close();
	return true;
}


} // namespace Import

} // namespace Algo

} // namespace CGoGN
