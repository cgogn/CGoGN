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
* FITNESS FOR A PARTICULAR PURVEC3E. See the GNU Lesser General Public License  *
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

#include "Algo/Import/importPlyData.h"

#include "openctm.h"

#include "assimp.h"
#include "aiPostProcess.h"
#include "aiScene.h"


namespace CGoGN
{

namespace Algo
{

namespace Import
{

template<typename PFP>
ImportSurfacique::ImportType MeshTablesSurface<PFP>::getFileType(const std::string& filename)
{
	if ((filename.rfind(".trianbgz")!=std::string::npos) || (filename.rfind(".TRIANBGZ")!=std::string::npos))
		return ImportSurfacique::TRIANBGZ;

	if ((filename.rfind(".trian")!=std::string::npos) || (filename.rfind(".TRIAN")!=std::string::npos))
		return ImportSurfacique::TRIAN;

	if ((filename.rfind(".plyptm")!=std::string::npos) || (filename.rfind(".PLYGEN")!=std::string::npos))
		return ImportSurfacique::PLYPTM;

	if ((filename.rfind(".ply")!=std::string::npos) || (filename.rfind(".PLY")!=std::string::npos))
		return ImportSurfacique::PLY;

	if ((filename.rfind(".off")!=std::string::npos) || (filename.rfind(".OFF")!=std::string::npos))
		return ImportSurfacique::OFF;

	if ((filename.rfind(".obj")!=std::string::npos) || (filename.rfind(".OBJ")!=std::string::npos))
		return ImportSurfacique::OBJ;

	if ((filename.rfind(".ctm")!=std::string::npos) || (filename.rfind(".OBJ")!=std::string::npos))
		return ImportSurfacique::CTM;

	return ImportSurfacique::UNKNOWNSURFACE;
}

template<typename PFP>
bool MeshTablesSurface<PFP>::importMesh(const std::string& filename, std::vector<std::string>& attrNames, ImportSurfacique::ImportType kind)
{
	if (kind == ImportSurfacique::UNKNOWNSURFACE)
		kind = getFileType(filename);

	attrNames.clear() ;

	switch (kind)
	{
	case ImportSurfacique::TRIAN:
		std::cout << "TYPE: TRIAN" << std::endl;
		return importTrian(filename, attrNames);
		break;
	case ImportSurfacique::TRIANBGZ:
		std::cout << "TYPE: TRIANBGZ" << std::endl;
		return importTrianBinGz(filename, attrNames);
		break;
	case ImportSurfacique::CTM:
		std::cout << "TYPE: CTM" << std::endl;
		return importCTM(filename, attrNames);
		break;
	case ImportSurfacique::OFF:
		std::cout << "TYPE: OFF" << std::endl;
		return importOff(filename, attrNames);
		break;
	case ImportSurfacique::PLY:
		std::cout << "TYPE: PLY" << std::endl;
		return importPly(filename, attrNames);
		break;
	case ImportSurfacique::PLYPTM:
		std::cout << "TYPE: PLYPTM" << std::endl;
		return importPlyPTM(filename, attrNames);
		break;
	case ImportSurfacique::OBJ:
		std::cout << "TYPE: OBJ" << std::endl;
		return importObj(filename, attrNames);
		break;
	default:
		std::cout << "TYPE: ASSIMP" << std::endl;
		return importASSIMP(filename, attrNames);
		break;
	}
	return false;
}

template<typename PFP>
bool MeshTablesSurface<PFP>::importTrian(const std::string& filename, std::vector<std::string>& attrNames)
{
	AttributeHandler<typename PFP::VEC3> positions = m_map.template addAttribute<typename PFP::VEC3>(VERTEX_ORBIT, "position") ;
	attrNames.push_back(positions.name()) ;

	AttributeContainer& container = m_map.getAttributeContainer(VERTEX_CELL) ;

	// open file
	std::ifstream fp(filename.c_str(), std::ios::in);
	if (!fp.good())
	{
		std::cerr << "Unable to open file " << filename << std::endl;
		return false;
	}

	// read nb of points
	fp >> m_nbVertices;

	// read points
	std::vector<unsigned int> verticesID;
	verticesID.reserve(m_nbVertices);

	for (unsigned int i = 0; i < m_nbVertices; ++i)
	{
		VEC3 pos;
		fp >> pos[0];
		fp >> pos[1];
		fp >> pos[2];
		unsigned int id = container.insertLine();
		positions[id] = pos;
		verticesID.push_back(id);
	}

	// read nb of faces
	fp >> m_nbFaces;
	m_nbEdges.reserve(m_nbFaces);
	m_emb.reserve(3*m_nbFaces);

	// read indices of faces
	for (unsigned int i = 0; i < m_nbFaces; ++i)
	{
		m_nbEdges.push_back(3);
		// read the three vertices of triangle
		int pt;
		fp >> pt;
		m_emb.push_back(verticesID[pt]);
		fp >> pt;
		m_emb.push_back(verticesID[pt]);
		fp >> pt;
		m_emb.push_back(verticesID[pt]);

		// neighbour not always good in files !!
		int neigh;
		fp >> neigh;
		fp >> neigh;
		fp >> neigh;
	}

	fp.close();
	return true;
}

template<typename PFP>
bool MeshTablesSurface<PFP>::importTrianBinGz(const std::string& filename, std::vector<std::string>& attrNames)
{
	AttributeHandler<typename PFP::VEC3> positions = m_map.template addAttribute<typename PFP::VEC3>(VERTEX_ORBIT, "position") ;
	attrNames.push_back(positions.name()) ;

	AttributeContainer& container = m_map.getAttributeContainer(VERTEX_CELL) ;

	// open file
	igzstream fs(filename.c_str(), std::ios::in|std::ios::binary);

	if (!fs.good())
	{
		std::cerr << "Unable to open file " << filename << std::endl;
		return false;
	}

	// read nb of points
	fs.read(reinterpret_cast<char*>(&m_nbVertices), sizeof(int));

	// read points
	std::vector<unsigned int> verticesID;
	{	// juste pour limiter la portee des variables
		verticesID.reserve(m_nbVertices);
		float* buffer = new float[m_nbVertices*3];
		fs.read(reinterpret_cast<char*>(buffer), 3*m_nbVertices*sizeof(float));
		float *ptr = buffer;
		for (unsigned int i = 0; i < m_nbVertices; ++i)
		{
			VEC3 pos;
			pos[0]= *ptr++;
			pos[1]= *ptr++;
			pos[2]= *ptr++;

			unsigned int id = container.insertLine();
			positions[id] = pos;

			verticesID.push_back(id);
		}
		delete[] buffer;
	}

	// read nb of faces
	fs.read(reinterpret_cast<char*>(&m_nbFaces), sizeof(int));
	m_nbEdges.reserve(m_nbFaces);
	m_emb.reserve(3*m_nbFaces);

	// read indices of faces
	{	// juste pour limiter la portee des variables
		int* buffer = new int[m_nbFaces*6];
		fs.read(reinterpret_cast<char*>(buffer),6*m_nbFaces*sizeof(float));
		int *ptr = buffer;

		for (unsigned int i = 0; i < m_nbFaces; i++)
		{
			m_nbEdges.push_back(3);
			m_emb.push_back(verticesID[*ptr++]);
			m_emb.push_back(verticesID[*ptr++]);
			m_emb.push_back(verticesID[*ptr++]);
		}
	}
	
	fs.close();
	return true;
}

template<typename PFP>
bool MeshTablesSurface<PFP>::importOff(const std::string& filename, std::vector<std::string>& attrNames)
{
	AttributeHandler<typename PFP::VEC3> positions = m_map.template addAttribute<typename PFP::VEC3>(VERTEX_ORBIT, "position") ;
	attrNames.push_back(positions.name()) ;

	AttributeContainer& container = m_map.getAttributeContainer(VERTEX_CELL) ;

	// open file
	std::ifstream fp(filename.c_str(), std::ios::in);
	if (!fp.good())
	{
		std::cerr << "Unable to open file " << filename << std::endl;
		return false;
	}

    std::string ligne;

    // lecture de OFF
    std::getline (fp, ligne);
    if (ligne.rfind("OFF") == std::string::npos)
    {
		std::cerr << "Problem reading off file: not an off file" << std::endl;
		std::cerr << ligne << std::endl;
		return false;
    }

    // lecture des nombres de sommets/faces/aretes
	int nbe;
    {
    	do
    	{
    		std::getline (fp, ligne);
    	} while (ligne.size()==0);

	    std::stringstream oss(ligne);
		oss >> m_nbVertices;
		oss >> m_nbFaces;
		oss >> nbe;
    }

	//lecture sommets
	std::vector<unsigned int> verticesID;
	verticesID.reserve(m_nbVertices);
	for (unsigned int i = 0; i < m_nbVertices;++i)
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
		// on peut ajouter ici la lecture de couleur si elle existe
		VEC3 pos(x,y,z);

		unsigned int id = container.insertLine();
		positions[id] = pos;

		verticesID.push_back(id);
	}

	// lecture faces
	// normalement nbVertices*8 devrait suffire largement
	m_nbEdges.reserve(m_nbFaces);
	m_emb.reserve(m_nbVertices*8);

	for (unsigned int i = 0; i < m_nbFaces; ++i)
	{
    	do
    	{
    		std::getline (fp, ligne);
    	} while (ligne.size() == 0);

		std::stringstream oss(ligne);
		int n;
		oss >> n;
		m_nbEdges.push_back(n);
		for (int j=0;j<n; ++j)
		{
			int index; // index du plongement
			oss >> index;
			m_emb.push_back(verticesID[index]);
		}
		// on peut ajouter ici la lecture de couleur si elle existe
	}

	fp.close();
	return true;
}

template <typename PFP>
bool MeshTablesSurface<PFP>::importObj(const std::string& filename, std::vector<std::string>& attrNames)
{
	AttributeHandler<typename PFP::VEC3> positions = m_map.template addAttribute<typename PFP::VEC3>(VERTEX_ORBIT, "position") ;
	attrNames.push_back(positions.name()) ;

	AttributeContainer& container = m_map.getAttributeContainer(VERTEX_CELL) ;

	// open file
	std::ifstream fp(filename.c_str(), std::ios::binary);
	if (!fp.good())
	{
		std::cerr << "Unable to open file " << filename << std::endl;
		return false;
	}

//	fp.seekg(0, std::ios::end);
//	int ab = fp.tellg();
//	fp.seekg(0, std::ios::beg);
//	int ac = fp.tellg();

    std::string ligne;
    std::string tag;

    do
    {
    	fp >> tag;
    	std::getline (fp, ligne);
    }while (tag != std::string("v"));

    // lecture des sommets
	std::vector<unsigned int> verticesID;
	verticesID.reserve(102400); // on tape large (400Ko wahouuuuu !!)

	unsigned int i = 0;
    do
    {
		if (tag == std::string("v"))
		{
			std::stringstream oss(ligne);
		
			float x,y,z;
			oss >> x;
			oss >> y;
			oss >> z;

			VEC3 pos(x,y,z);

			unsigned int id = container.insertLine();
			positions[id] = pos;

			verticesID.push_back(id);
			i++;
		}

		fp >> tag;
    	std::getline(fp, ligne);
    } while (!fp.eof());

	m_nbVertices = verticesID.size();

	// close/clear/open only way to go back to beginning of file
	fp.close();
	fp.clear();
	fp.open(filename.c_str());

	do
    {
    	fp >> tag;
    	std::getline (fp, ligne);
    } while (tag != std::string("f"));

	m_nbEdges.reserve(verticesID.size()*2);
	m_emb.reserve(verticesID.size()*8);

	std::vector<int> table;
	table.reserve(64); // 64 cotes pour une face devrait suffire
	m_nbFaces = 0;
    do
    {
    	if (tag == std::string("f")) // lecture d'une face
    	{
    		std::stringstream oss(ligne);
     		table.clear();
    		while (!oss.eof())  // lecture de tous les indices
    		{
    			std::string str;
    			oss >> str;

    			unsigned int ind = 0;
    			while ( (str[ind]!='/')&& (ind<str.length()) )
    				ind++;

				if (ind > 0)
				{
    				long index;
					std::stringstream iss(str.substr(0, ind));
					iss >> index;
		   			table.push_back(index);
				}
    		}

    		unsigned int n = table.size();
			m_nbEdges.push_back(short(n));
    		for (unsigned int j = 0; j < n; ++j)
    		{
    			int index = table[j] - 1; // les index commencent a 1 (boufonnerie d'obj ;)
				m_emb.push_back(verticesID[index]);
    		}
			m_nbFaces++;
    	}
		fp >> tag;
    	std::getline(fp, ligne);
     } while (!fp.eof());

	fp.close ();
	return true;
}

template<typename PFP>
bool MeshTablesSurface<PFP>::importPly(const std::string& filename, std::vector<std::string>& attrNames)
{
	AttributeHandler<typename PFP::VEC3> positions = m_map.template addAttribute<typename PFP::VEC3>(VERTEX_ORBIT, "position") ;
	attrNames.push_back(positions.name()) ;

	AttributeContainer& container = m_map.getAttributeContainer(VERTEX_CELL) ;

	PlyImportData pid;

	if (! pid.read_file(filename) )
	{
		std::cerr << "Unable to open file " << filename << std::endl;
		return false;
	}
	
    // lecture des nombres de sommets/aretes/faces
	m_nbVertices = pid.nbVertices();
	m_nbFaces = pid.nbFaces();
 
	//lecture sommets
	std::vector<unsigned int> verticesID;
	verticesID.reserve(m_nbVertices);
	for (unsigned int i = 0; i < m_nbVertices; ++i)
	{
		VEC3 pos;
		pid.vertexPosition(i, pos);

		unsigned int id = container.insertLine();
		positions[id] = pos;

		verticesID.push_back(id);
	}

	m_nbEdges.reserve(m_nbFaces);
	m_emb.reserve(m_nbVertices*8);

	for (unsigned int i = 0; i < m_nbFaces; ++i)
	{
		unsigned int n = pid.getFaceValence(i);
		m_nbEdges.push_back(n);
		int* indices = pid.getFaceIndices(i);
		for (unsigned int j = 0; j < n; ++j)
		{
			m_emb.push_back(verticesID[indices[j]]);
		}
	}

	return true;
}

template <typename PFP>
bool MeshTablesSurface<PFP>::importPlyPTM(const std::string& filename, std::vector<std::string>& attrNames)
{
	AttributeHandler<typename PFP::VEC3> positions = m_map.template addAttribute<typename PFP::VEC3>(VERTEX_ORBIT, "position") ;
	attrNames.push_back(positions.name()) ;
	AttributeHandler<typename PFP::MATRIX33> Frame = m_map.template addAttribute<typename PFP::MATRIX33>(VERTEX_ORBIT, "Frame") ;
	attrNames.push_back(Frame.name()) ;
	AttributeHandler<typename PFP::MATRIX36> RGBfunctions = m_map.template addAttribute<typename PFP::MATRIX36>(VERTEX_ORBIT, "RGBfunctions") ;
	attrNames.push_back(RGBfunctions.name()) ;

	AttributeContainer& container = m_map.getAttributeContainer(VERTEX_CELL) ;

	std::ifstream fp(filename.c_str(), std::ios::binary);
	if (!fp.good())
	{
		std::cerr << "Unable to open file " << filename<< std::endl;
		return false;
	}

    std::string ligne;
    std::string tag;

	fp >> tag;
	if (tag != std::string("ply"))
	{
		std::cerr <<filename<< " is not a ply file !" <<  std::endl;
		return false;
	}

	// va au nombre de sommets
	do
	{
		fp >> tag;
	} while (tag != std::string("vertex"));

	unsigned int nbp;
	fp >> nbp;
	// read points
	std::vector<unsigned int> verticesID;
	verticesID.reserve(nbp);

	// va au nombre de faces en comptant le nombre de "property"
	unsigned int nb_props = 0;
	do
	{
		fp >> tag;
		if (tag == std::string("property"))
			nb_props++;
	} while (tag != std::string("face"));

	fp >> m_nbFaces;
 	m_nbEdges.reserve(m_nbFaces);
	m_emb.reserve(3*m_nbFaces);

	// lecture des sommets

	// saute à la fin du header
	do
	{
		fp >> tag;
	} while (tag != std::string("end_header"));

	float* properties = new float[nb_props];

	for (unsigned int i = 0; i < nbp; ++i)
	{
		unsigned int id = container.insertLine();
		verticesID.push_back(id);

		for (unsigned int j = 0; j < nb_props; ++j)
		{
			fp >> properties[j];
		}

		positions[id] = VEC3(properties[0],properties[1],properties[2]);

		for (unsigned int k = 0 ; k < 3 ; ++k)
			for (unsigned int l = 0 ; l < 3 ; ++l)
				Frame[id](k,l) = properties[3+(3*k+l)] ;

		for (unsigned int k = 0 ; k < 3 ; ++k)
			for (unsigned int l = 0 ; l < 6 ; ++l)
				RGBfunctions[id](k,l) = properties[12+(6*k+l)];
	}

	m_nbVertices = verticesID.size();
	delete[] properties;

// read indices of faces
	for (unsigned int i = 0; i < m_nbFaces; i++)
	{
		// read the indices vertices of face
		int nbe;
		fp >> nbe;
		m_nbEdges.push_back(nbe);

		int pt;
		for (int j=0; j<nbe; ++j)
		{
			fp >> pt;
			m_emb.push_back(verticesID[pt]);
		}
	}

	fp.close();
	return true;
}

template <typename PFP>
bool MeshTablesSurface<PFP>::importCTM(const std::string& filename, std::vector<std::string>& attrNames)
{
	AttributeHandler<typename PFP::VEC3> positions = m_map.template addAttribute<typename PFP::VEC3>(VERTEX_ORBIT, "position") ;
	attrNames.push_back(positions.name()) ;

	AttributeContainer& container = m_map.getAttributeContainer(VERTEX_CELL) ;

	// Load the file using the OpenCTM API
	CTMimporter ctm;
	// Load the file
	ctm.Load(filename.c_str());

 	m_nbVertices = ctm.GetInteger(CTM_VERTEX_COUNT);

 	// read points
	std::vector<unsigned int> verticesID;
	verticesID.reserve(m_nbVertices);

	const CTMfloat* vertices = ctm.GetFloatArray(CTM_VERTICES);
	for (unsigned int i = 0; i < m_nbVertices; ++i)
	{
		VEC3 pos(vertices[0], vertices[1], vertices[2]);
		vertices += 3; // next vertex in float*
		unsigned int id = container.insertLine();
		positions[id] = pos;
		verticesID.push_back(id);
	}

	// read nb of faces
	m_nbFaces = ctm.GetInteger(CTM_TRIANGLE_COUNT);
	m_nbEdges.reserve(m_nbFaces);
	m_emb.reserve(3*m_nbFaces);

	const CTMuint * indices = ctm.GetIntegerArray(CTM_INDICES);

	// read indices of faces
	for (unsigned i=0; i<m_nbFaces; i++)
	{
		m_nbEdges.push_back(3);
		// read the three vertices of triangle
		m_emb.push_back(verticesID[*indices++]);
		m_emb.push_back(verticesID[*indices++]);
		m_emb.push_back(verticesID[*indices++]);
	}

	return true;
}

template<typename PFP>
void MeshTablesSurface<PFP>::extractMeshRec(AttributeContainer& container, AttributeHandler<typename PFP::VEC3>& positions, const struct aiScene* scene, const struct aiNode* nd, struct aiMatrix4x4* trafo)
{
	struct aiMatrix4x4 prev;

	prev = *trafo;
	aiMultiplyMatrix4(trafo,&nd->mTransformation);

	std::vector<unsigned int> verticesID;

	// foreach mesh of node
	for (unsigned int n = 0; n < nd->mNumMeshes; ++n)
	{
		const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];

		verticesID.clear();
		verticesID.reserve(mesh->mNumVertices);
		//read positions
		for (unsigned int t = 0; t < mesh->mNumVertices; ++t)
		{
			// transform position
			struct aiVector3D tmp = mesh->mVertices[t];
			aiTransformVecByMatrix4(&tmp, trafo);
			// now store it
			unsigned int id = container.insertLine();
			positions[id] = VEC3(tmp[0], tmp[1], tmp[2]);
			verticesID.push_back(id);
		}
		m_nbVertices += mesh->mNumVertices;

		// read faces indices
		for (unsigned int t = 0; t < mesh->mNumFaces; ++t)
		{
			const struct aiFace* face = &mesh->mFaces[t];
			m_nbEdges.push_back(face->mNumIndices);
			for(unsigned int i = 0; i < face->mNumIndices; i++)
			{
				unsigned int pt = face->mIndices[i];
				m_emb.push_back(verticesID[pt]);
			}
		}
		m_nbFaces += mesh->mNumFaces;
	}

	// recurse on all children of node
	for (unsigned int n = 0; n < nd->mNumChildren; ++n)
	{
//		std::cout << "Children "<<n<< std::endl;
		extractMeshRec(container, positions, scene, nd->mChildren[n], trafo);
	}
	*trafo = prev;
}

template <typename PFP>
bool MeshTablesSurface<PFP>::importASSIMP(const std::string& filename, std::vector<std::string>& attrNames)
{
	AttributeContainer& container = m_map.getAttributeContainer(VERTEX_CELL) ;
	AttributeHandler<typename PFP::VEC3> positions = m_map.template addAttribute<typename PFP::VEC3>(VERTEX_ORBIT, "position") ;
	attrNames.push_back(positions.name()) ;

	m_nbVertices = 0;
	m_nbFaces = 0;

 	m_nbEdges.reserve(5000);
	m_emb.reserve(15000);

	struct aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);

	m_lab = 0;
	const struct aiScene* scene = aiImportFile(filename.c_str(), aiProcess_FindDegenerates | aiProcess_JoinIdenticalVertices);
	extractMeshRec(container, positions, scene, scene->mRootNode, &trafo);

	return true;
}

} // namespace Import

} // namespace Algo

} // namespace CGoGN
