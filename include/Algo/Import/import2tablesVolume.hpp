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

#include "Geometry/orientation.h"

namespace CGoGN
{

namespace Algo
{

namespace Volume
{

namespace Import
{

template <typename PFP>
bool MeshTablesVolume<PFP>::importMesh(const std::string& filename, std::vector<std::string>& attrNames)
{
	ImportType kind = getFileType(filename);

    attrNames.clear() ;

	switch (kind)
	{
	case TET:
        return importTet(filename, attrNames);
		break;
    case OFF:
    {
        size_t pos = filename.rfind(".");
        std::string fileEle = filename;
        fileEle.erase(pos);
        fileEle.append(".ele");
        return importOFFWithELERegions(filename, fileEle, attrNames);
        break;
    }
	case NODE:
    {
        size_t pos = filename.rfind(".");
        std::string fileEle = filename;
        fileEle.erase(pos);
        fileEle.append(".ele");
        return importNodeWithELERegions(filename, fileEle, attrNames);
		break;
    }
    case TETMESH:
        return importTetmesh(filename, attrNames);
        break;
	case TS:
        return importTs(filename, attrNames);
        break;
    case MSH:
        return importMSH(filename, attrNames);
        break;
    case VTU:
        //return importVTU(filename, attrNames);
        break;
    case NAS:
        return importNAS(filename, attrNames);
        break;
    case VBGZ:
        //return importVBGZ>(filename, attrNames);
        break;
//	case ImportVolumique::MOKA:
//		return importMoka(filename,attrNames);
//		break;      
//	case OVM:
//		return importOVM(filename, attrNames);
//		break;
	default:
		CGoGNerr << "Not yet supported" << CGoGNendl;
		break;
	}
	return false;
}

template <typename PFP>
bool MeshTablesVolume<PFP>::importTet(const std::string& filename, std::vector<std::string>& attrNames)
{
    VertexAttribute<VEC3> position =  m_map.template getAttribute<VEC3, VERTEX>("position") ;

    if (!position.isValid())
        position = m_map.template addAttribute<VEC3, VERTEX>("position") ;

    attrNames.push_back(position.name()) ;

	AttributeContainer& container = m_map.template getAttributeContainer<VERTEX>() ;

	//open file
	std::ifstream fp(filename.c_str(), std::ios::in);
	if (!fp.good())
	{
		CGoGNerr << "Unable to open file " << filename << CGoGNendl;
		return false;
	}

    std::string ligne;

	// reading number of vertices
	std::getline (fp, ligne);
	std::stringstream oss(ligne);
    oss >> m_nbVertices;

	// reading number of tetrahedra
	std::getline (fp, ligne);
	std::stringstream oss2(ligne);
    oss2 >> m_nbVolumes;

	//reading vertices
    std::vector<unsigned int> verticesID;
    verticesID.reserve(m_nbVertices);

    for(unsigned int i = 0; i < m_nbVertices; ++i)
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
        VEC3 pos(x,y,z);

		unsigned int id = container.insertLine();
        position[id] = pos;

        verticesID.push_back(id);
	}

    // reading volumes
    m_nbFaces.reserve(m_nbVolumes*4);
    m_emb.reserve(m_nbVolumes*12);

	for (unsigned int i = 0; i < m_nbVolumes ; ++i)
	{
    	do
    	{
    		std::getline (fp, ligne);
    	} while (ligne.size()==0);

		std::stringstream oss(ligne);
		int n;
        oss >> n; // type of volumes

        //tetrahedron
        if(n == 4)
        {
            m_nbFaces.push_back(4);

            int s0,s1,s2,s3;

            oss >> s0;
            oss >> s1;
            oss >> s2;
            oss >> s3;

            typename PFP::VEC3 P = position[verticesID[s0]];
            typename PFP::VEC3 A = position[verticesID[s1]];
            typename PFP::VEC3 B = position[verticesID[s2]];
            typename PFP::VEC3 C = position[verticesID[s3]];

            if (Geom::testOrientation3D<typename PFP::VEC3>(P,A,B,C) == Geom::UNDER)
            {
                int ui = s1;
                s1 = s2;
                s2 = ui;
            }

            m_emb.push_back(verticesID[s0]);
            m_emb.push_back(verticesID[s1]);
            m_emb.push_back(verticesID[s2]);
            m_emb.push_back(verticesID[s3]);
        }
        //pyramid
        else if(n == 5)
        {
            m_nbFaces.push_back(5);

            int s0,s1,s2,s3,s4;

            oss >> s0;
            oss >> s1;
            oss >> s2;
            oss >> s3;
            oss >> s4;

            m_emb.push_back(verticesID[s0]);
            m_emb.push_back(verticesID[s1]);
            m_emb.push_back(verticesID[s2]);
            m_emb.push_back(verticesID[s3]);
            m_emb.push_back(verticesID[s4]);
        }
        //prism
        else if(n == 6)
        {
            m_nbFaces.push_back(6);

            int s0,s1,s2,s3,s4,s5;

            oss >> s0;
            oss >> s1;
            oss >> s2;
            oss >> s3;
            oss >> s4;
            oss >> s5;

            typename PFP::VEC3 P = position[verticesID[s4]];
            typename PFP::VEC3 A = position[verticesID[s0]];
            typename PFP::VEC3 B = position[verticesID[s1]];
            typename PFP::VEC3 C = position[verticesID[s2]];

            if (Geom::testOrientation3D<typename PFP::VEC3>(P,A,B,C) == Geom::OVER)
            {
                int ui = s0;
                s0 = s4;
                s4 = ui;

                ui = s1;
                s1 = s5;
                s5 = ui;

                ui = s2;
                s2 = s3;
                s3 = ui;
            }

            m_emb.push_back(verticesID[s0]);
            m_emb.push_back(verticesID[s1]);
            m_emb.push_back(verticesID[s2]);
            m_emb.push_back(verticesID[s3]);
            m_emb.push_back(verticesID[s4]);
            m_emb.push_back(verticesID[s5]);
        }
        //hexahedron
        else if(n == 8)
        {
            m_nbFaces.push_back(8);

            int s0,s1,s2,s3,s4,s5,s6,s7;

            oss >> s0;
            oss >> s1;
            oss >> s2;
            oss >> s3;
            oss >> s4;
            oss >> s5;
            oss >> s6;
            oss >> s7;

            typename PFP::VEC3 P = position[verticesID[s4]];
            typename PFP::VEC3 A = position[verticesID[s0]];
            typename PFP::VEC3 B = position[verticesID[s1]];
            typename PFP::VEC3 C = position[verticesID[s2]];

            if (Geom::testOrientation3D<typename PFP::VEC3>(P,A,B,C) == Geom::OVER)
            {
                int ui = s0;
                s0 = s3;
                s3 = ui;

                ui = s1;
                s1 = s2;
                s2 = ui;

                ui = s4;
                s4 = s7;
                s7 = ui;

                ui = s5;
                s5 = s6;
                s6 = ui;
            }

            m_emb.push_back(verticesID[s0]);
            m_emb.push_back(verticesID[s1]);
            m_emb.push_back(verticesID[s2]);
            m_emb.push_back(verticesID[s3]);
            m_emb.push_back(verticesID[s4]);
            m_emb.push_back(verticesID[s5]);
            m_emb.push_back(verticesID[s6]);
            m_emb.push_back(verticesID[s7]);
        }

    }

	fp.close();
	return true;
}

template <typename PFP>
bool MeshTablesVolume<PFP>::importOFFWithELERegions(const std::string& filenameOFF, const std::string& filenameELE, std::vector<std::string>& attrNames)
{
    VertexAttribute<VEC3> position =  m_map.template getAttribute<VEC3, VERTEX>("position") ;

    if (!position.isValid())
        position = m_map.template addAttribute<VEC3, VERTEX>("position") ;

    attrNames.push_back(position.name()) ;

    AttributeContainer& container = m_map.template getAttributeContainer<VERTEX>() ;

    // open files
    std::ifstream foff(filenameOFF.c_str(), std::ios::in);
    if (!foff.good())
    {
        CGoGNerr << "Unable to open OFF file " << CGoGNendl;
        return false;
    }

    std::ifstream fele(filenameELE.c_str(), std::ios::in);
    if (!fele.good())
    {
        CGoGNerr << "Unable to open ELE file " << CGoGNendl;
        return false;
    }

    std::string line;

    //OFF reading
    std::getline(foff, line);
    if(line.rfind("OFF") == std::string::npos)
    {
        CGoGNerr << "Problem reading off file: not an off file"<<CGoGNendl;
        CGoGNerr << line << CGoGNendl;
        return false;
    }

    //Reading number of vertex/faces/edges in OFF file
    unsigned int nbe;
    {
        do
        {
            std::getline(foff,line);
        }while(line.size() == 0);

        std::stringstream oss(line);
        oss >> m_nbVertices;
        oss >> nbe;
        oss >> nbe;
        oss >> nbe;
    }

    //Reading number of tetrahedra in ELE file
    unsigned int nbv;
    {
        do
        {
            std::getline(fele,line);
        }while(line.size() == 0);

        std::stringstream oss(line);
        oss >> m_nbVolumes;
        oss >> nbv ;
        oss >> nbv;
    }

    //Reading vertices
    std::vector<unsigned int> verticesID;
    verticesID.reserve(m_nbVertices);

    for(unsigned int i = 0 ; i < m_nbVertices ; ++i)
    {
        do
        {
            std::getline(foff,line);
        }while(line.size() == 0);

        std::stringstream oss(line);

        float x,y,z;
        oss >> x;
        oss >> y;
        oss >> z;
        //we can read colors informations if exists
        VEC3 pos(x,y,z);

        unsigned int id = container.insertLine();
        position[id] = pos;
        verticesID.push_back(id);
    }

    // reading tetrahedra
    m_nbFaces.reserve(m_nbVolumes*4);
    m_emb.reserve(m_nbVolumes*12);

    for(unsigned i = 0; i < m_nbVolumes ; ++i)
    {
        do
        {
            std::getline(fele,line);
        } while(line.size() == 0);

        std::stringstream oss(line);
        oss >> nbe;

        m_nbFaces.push_back(4);

        int s0,s1,s2,s3;

        oss >> s0;
        oss >> s1;
        oss >> s2;
        oss >> s3;

        typename PFP::VEC3 P = position[verticesID[s0]];
        typename PFP::VEC3 A = position[verticesID[s1]];
        typename PFP::VEC3 B = position[verticesID[s2]];
        typename PFP::VEC3 C = position[verticesID[s3]];

        if (Geom::testOrientation3D<typename PFP::VEC3>(P,A,B,C) == Geom::UNDER)
        {
            int ui= s0;
            s0 = s3;
            s3 = s2;
            s2 = s1;
            s1 = ui;
        }

        m_emb.push_back(verticesID[s0]);
        m_emb.push_back(verticesID[s1]);
        m_emb.push_back(verticesID[s2]);
        m_emb.push_back(verticesID[s3]);
    }

    foff.close();
    fele.close();

    return true;
}

template <typename PFP>
bool MeshTablesVolume<PFP>::importNodeWithELERegions(const std::string& filenameNode, const std::string& filenameELE, std::vector<std::string>& attrNames)
{
    VertexAttribute<VEC3> position =  m_map.template getAttribute<VEC3, VERTEX>("position") ;

    if (!position.isValid())
        position = m_map.template addAttribute<VEC3, VERTEX>("position") ;

    attrNames.push_back(position.name()) ;

    AttributeContainer& container = m_map.template getAttributeContainer<VERTEX>() ;

    //open file
    std::ifstream fnode(filenameNode.c_str(), std::ios::in);
    if (!fnode.good())
    {
        CGoGNerr << "Unable to open file " << filenameNode << CGoGNendl;
        return false;
    }

    std::ifstream fele(filenameELE.c_str(), std::ios::in);
    if (!fele.good())
    {
        CGoGNerr << "Unable to open file " << filenameELE << CGoGNendl;
        return false;
    }

    std::string line;

    //Reading NODE file
    //First line: [# of points] [dimension (must be 3)] [# of attributes] [# of boundary markers (0 or 1)]
    unsigned int nbe;
    {
        do
        {
            std::getline(fnode,line);
        }while(line.size() == 0);

        std::stringstream oss(line);
        oss >> m_nbVertices;
        oss >> nbe;
        oss >> nbe;
        oss >> nbe;
    }

    //Reading number of tetrahedra in ELE file
    unsigned int nbv;
    {
        do
        {
            std::getline(fele,line);
        }while(line.size() == 0);

        std::stringstream oss(line);
        oss >> m_nbVolumes;
        oss >> nbv ;
        oss >> nbv;
    }

    //Reading vertices
    std::map<unsigned int,unsigned int> verticesMapID;

    for(unsigned int i = 0 ; i < m_nbVertices ; ++i)
    {
        do
        {
            std::getline(fnode,line);
        }while(line.size() == 0);

        std::stringstream oss(line);

        int idv;
        oss >> idv;

        float x,y,z;
        oss >> x;
        oss >> y;
        oss >> z;
        //we can read colors informations if exists
        VEC3 pos(x,y,z);

        unsigned int id = container.insertLine();
        position[id] = pos;
        verticesMapID.insert(std::pair<unsigned int, unsigned int>(idv,id));
    }

    // reading tetrahedra
    m_nbFaces.reserve(m_nbVolumes*4);
    m_emb.reserve(m_nbVolumes*12);

    for(unsigned i = 0; i < m_nbVolumes ; ++i)
    {
        do
        {
            std::getline(fele,line);
        } while(line.size() == 0);

        std::stringstream oss(line);
        oss >> nbe;

        m_nbFaces.push_back(4);

        int s0,s1,s2,s3;

        oss >> s0;
        oss >> s1;
        oss >> s2;
        oss >> s3;

        typename PFP::VEC3 P = position[verticesMapID[s0]];
        typename PFP::VEC3 A = position[verticesMapID[s1]];
        typename PFP::VEC3 B = position[verticesMapID[s2]];
        typename PFP::VEC3 C = position[verticesMapID[s3]];

        if (Geom::testOrientation3D<typename PFP::VEC3>(P,A,B,C) == Geom::UNDER)
        {
            int ui= s0;
            s0 = s3;
            s3 = s2;
            s2 = s1;
            s1 = ui;
        }

        m_emb.push_back(verticesMapID[s0]);
        m_emb.push_back(verticesMapID[s1]);
        m_emb.push_back(verticesMapID[s2]);
        m_emb.push_back(verticesMapID[s3]);
    }

    fnode.close();
    fele.close();

    return true;
}

template <typename PFP>
bool MeshTablesVolume<PFP>::importTetmesh(const std::string& filename, std::vector<std::string>& attrNames)
{
    VertexAttribute<VEC3> position =  m_map.template getAttribute<VEC3, VERTEX>("position") ;

    if (!position.isValid())
        position = m_map.template addAttribute<VEC3, VERTEX>("position") ;

    attrNames.push_back(position.name()) ;

    AttributeContainer& container = m_map.template getAttributeContainer<VERTEX>() ;

    //open file
    std::ifstream fp(filename.c_str(), std::ios::in);
    if (!fp.good())
    {
        CGoGNerr << "Unable to open file " << filename << CGoGNendl;
        return false;
    }

    std::string line;

    fp >> line;

    if (line!="Vertices")
        CGoGNerr << "Warning tetmesh file problem"<< CGoGNendl;

    fp >> m_nbVertices;

    std::cout << "READ: "<< m_nbVertices << std::endl;

    std::getline (fp, line);

    //reading vertices
    std::vector<unsigned int> verticesID;
    verticesID.reserve(m_nbVertices+1);
    verticesID.push_back(0xffffffff);

    for(unsigned int i = 0; i < m_nbVertices; ++i)
    {
        do
        {
            std::getline (fp, line);
        } while (line.size() == 0);

        std::stringstream oss(line);

        float x,y,z;
        oss >> x;
        oss >> y;
        oss >> z;
        // TODO : if required read other vertices attributes here
        VEC3 pos(x,y,z);

        unsigned int id = container.insertLine();
        position[id] = pos;

        verticesID.push_back(id);
    }

    fp >> line;
    if (line!="Tetrahedra")
        CGoGNerr << "Warning tetmesh file problem"<< CGoGNendl;

    fp >> m_nbVolumes;
    std::getline (fp, line);

    // reading tetrahedra
    m_nbFaces.reserve(m_nbVolumes*4);
    m_emb.reserve(m_nbVolumes*12);

    for(unsigned i = 0; i < m_nbVolumes ; ++i)
    {
        do
        {
            std::getline(fp,line);
        } while(line.size() == 0);

        std::stringstream oss(line);

        m_nbFaces.push_back(4);

        int s0,s1,s2,s3;

        oss >> s0;
        oss >> s1;
        oss >> s2;
        oss >> s3;

        typename PFP::VEC3 P = position[verticesID[s0]];
        typename PFP::VEC3 A = position[verticesID[s1]];
        typename PFP::VEC3 B = position[verticesID[s2]];
        typename PFP::VEC3 C = position[verticesID[s3]];

        if (Geom::testOrientation3D<typename PFP::VEC3>(P,A,B,C) == Geom::UNDER)
        {
            int ui=s1;
            s1 = s2;
            s2 = ui;
        }

        m_emb.push_back(verticesID[s0]);
        m_emb.push_back(verticesID[s1]);
        m_emb.push_back(verticesID[s2]);
        m_emb.push_back(verticesID[s3]);
    }

    fp.close();
    return true;
}

template <typename PFP>
bool MeshTablesVolume<PFP>::importTs(const std::string& filename, std::vector<std::string>& attrNames)
{
    //
    VertexAttribute<VEC3> position =  m_map.template getAttribute<VEC3, VERTEX>("position") ;

    if (!position.isValid())
        position = m_map.template addAttribute<VEC3, VERTEX>("position") ;

    attrNames.push_back(position.name()) ;

    //
    VertexAttribute<REAL> scalar = m_map.template getAttribute<REAL, VERTEX>("scalar");

    if (!scalar.isValid())
        scalar = m_map.template addAttribute<REAL, VERTEX>("scalar") ;

    attrNames.push_back(scalar.name()) ;

    //
    AttributeContainer& container = m_map.template getAttributeContainer<VERTEX>() ;

    // open file
    std::ifstream fp(filename.c_str(), std::ios::in);
    if (!fp.good())
    {
        CGoGNerr << "Unable to open file " << filename << CGoGNendl;
        return false;
    }

    std::string ligne;

    // reading number of vertices/tetrahedra
    std::getline (fp, ligne);
    std::stringstream oss(ligne);
    oss >> m_nbVertices;
    oss >> m_nbVolumes;

    //reading vertices
    std::vector<unsigned int> verticesID;
    verticesID.reserve(m_nbVertices);

    for(unsigned int i = 0; i < m_nbVertices; ++i)
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

        VEC3 pos(x,y,z);

        unsigned int id = container.insertLine();

        position[id] = pos;
        verticesID.push_back(id);

        float scal;
        oss >> scal;
        scalar[id] = scal;
    }

    //Read and embed all tetrahedrons
    for(unsigned int i = 0; i < m_nbVolumes ; ++i)
    {        
        do
        {
            std::getline(fp,ligne);
        } while(ligne.size() == 0);

        std::stringstream oss(ligne);

        m_nbFaces.push_back(4);

        int s0,s1,s2,s3,nbe;

        oss >> s0;
        oss >> s1;
        oss >> s2;
        oss >> s3;

        typename PFP::VEC3 P = position[verticesID[s0]];
        typename PFP::VEC3 A = position[verticesID[s1]];
        typename PFP::VEC3 B = position[verticesID[s2]];
        typename PFP::VEC3 C = position[verticesID[s3]];

        if(Geom::testOrientation3D<typename PFP::VEC3>(P,A,B,C) == Geom::UNDER)
        {
            int ui = s1;
            s1 = s2;
            s2 = ui;
        }


        //if regions are defined use this number
        oss >> nbe; //ignored here

        m_emb.push_back(verticesID[s0]);
        m_emb.push_back(verticesID[s1]);
        m_emb.push_back(verticesID[s2]);
        m_emb.push_back(verticesID[s3]);
    }

    fp.close();
    return true;
}

/*
inline float floatFromNas(std::string& s_v)
{
    float x = 0.0f;

    std::size_t pos1 = s_v.find_last_of('-');
    if ((pos1!=std::string::npos) && (pos1!=0))
    {
        std::string res = s_v.substr(0,pos1) + "e" + s_v.substr(pos1,8-pos1);
        x = atof(res.c_str());
    }
    else
    {
        std::size_t pos2 = s_v.find_last_of('+');
        if ((pos2!=std::string::npos) && (pos2!=0))
        {
            std::string res = s_v.substr(0,pos2) + "e" + s_v.substr(pos2,8-pos2);
            x = atof(res.c_str());
        }
        else
        {
            x = atof(s_v.c_str());
        }
    }
    return x;
}
*/
template <typename PFP>
bool MeshTablesVolume<PFP>::importNAS(const std::string& filename, std::vector<std::string>& attrNames)
{
/*    //
    VertexAttribute<VEC3> position =  m_map.template getAttribute<VEC3, VERTEX>("position") ;

    if (!position.isValid())
        position = m_map.template addAttribute<VEC3, VERTEX>("position") ;

    attrNames.push_back(position.name()) ;

    //
    AttributeContainer& container = m_map.template getAttributeContainer<VERTEX>() ;

    // open file
    std::ifstream fp(filename.c_str(), std::ios::in);
    if (!fp.good())
    {
        CGoGNerr << "Unable to open file " << filename << CGoGNendl;
        return false;
    }

    std::string ligne;
    std::string tag;

    std::getline (fp, ligne);
    do
    {
        std::getline (fp, ligne);
        tag = ligne.substr(0,4);
    } while (tag !="GRID");

    unsigned int m_nbVertices = 0;
    //reading vertices
    std::map<unsigned int, unsigned int> verticesMapID;
    do
    {
        std::string s_v = ligne.substr(8,8);
        unsigned int ind = atoi(s_v.c_str());

        s_v = ligne.substr(24,8);
        float x = floatFromNas(s_v);
        s_v = ligne.substr(32,8);
        float y = floatFromNas(s_v);
        s_v = ligne.substr(40,8);
        float z = floatFromNas(s_v);

        VEC3 pos(x,y,z);
        unsigned int id = container.insertLine();
        position[id] = pos;
        verticesMapID.insert(std::pair<unsigned int, unsigned int>(ind,id));
//		std::cout << "P: "<< ind << "   "<<x<<", "<<y<<", "<<z << std::endl;
        std::getline (fp, ligne);
        tag = ligne.substr(0,4);
        m_nbVertices++;
    } while (tag =="GRID");

    do
    {
        std::string s_v = ligne.substr(0,6);

        if (s_v == "CHEXA ")
        {
            m_nbFaces.push_back(8);

            s_v = ligne.substr(24,8);
            unsigned int ind1 = atoi(s_v.c_str());
            s_v = ligne.substr(32,8);
            unsigned int ind2 = atoi(s_v.c_str());
            s_v = ligne.substr(40,8);
            unsigned int ind3 = atoi(s_v.c_str());
            s_v = ligne.substr(48,8);
            unsigned int ind4 = atoi(s_v.c_str());
            s_v = ligne.substr(56,8);
            unsigned int ind5 = atoi(s_v.c_str());
            s_v = ligne.substr(64,8);
            unsigned int ind6 = atoi(s_v.c_str());

            std::getline (fp, ligne);
            s_v = ligne.substr(8,8);
            unsigned int ind7 = atoi(s_v.c_str());
            s_v = ligne.substr(16,8);
            unsigned int ind8 = atoi(s_v.c_str());

            typename PFP::VEC3 P = position[verticesMapID[ind5]];
            typename PFP::VEC3 A = position[verticesMapID[ind1]];
            typename PFP::VEC3 B = position[verticesMapID[ind2]];
            typename PFP::VEC3 C = position[verticesMapID[ind3]];

            Geom::Vec4ui v1,v2;

            if (Geom::testOrientation3D<typename PFP::VEC3>(P,A,B,C) == Geom::OVER)
            {
                v1[0] = ind4;
                v1[1] = ind3;
                v1[2] = ind2;
                v1[3] = ind1;
                //hexa.push_back(v);
                v2[0] = ind8;
                v2[1] = ind7;
                v2[2] = ind6;
                v2[3] = ind5;
                //hexa.push_back(v);
            }
            else
            {
                v1[0] = ind1;
                v1[1] = ind2;
                v1[2] = ind3;
                v1[3] = ind4;
                //hexa.push_back(v);
                v2[0] = ind5;
                v2[1] = ind6;
                v2[2] = ind7;
                v2[3] = ind8;
                //hexa.push_back(v);
            }

            m_emb.push_back(verticesMapID[v1[0]]);
            m_emb.push_back(verticesMapID[v1[1]]);
            m_emb.push_back(verticesMapID[v1[2]]);
            m_emb.push_back(verticesMapID[v1[3]]);
            m_emb.push_back(verticesMapID[v2[0]]);
            m_emb.push_back(verticesMapID[v2[1]]);
            m_emb.push_back(verticesMapID[v2[2]]);
            m_emb.push_back(verticesMapID[v2[3]]);
        }
        if (s_v == "CTETRA")
        {
            m_nbFaces.push_back(4);

            s_v = ligne.substr(24,8);
            unsigned int ind1 = atoi(s_v.c_str());
            s_v = ligne.substr(32,8);
            unsigned int ind2 = atoi(s_v.c_str());
            s_v = ligne.substr(40,8);
            unsigned int ind3 = atoi(s_v.c_str());
            s_v = ligne.substr(48,8);
            unsigned int ind4 = atoi(s_v.c_str());

            typename PFP::VEC3 P = position[verticesMapID[ind1]];
            typename PFP::VEC3 A = position[verticesMapID[ind2]];
            typename PFP::VEC3 B = position[verticesMapID[ind3]];
            typename PFP::VEC3 C = position[verticesMapID[ind4]];

            Geom::Vec4ui v;

            if (Geom::testOrientation3D<typename PFP::VEC3>(P,A,B,C) == Geom::OVER)
            {
                v[0] = ind4;
                v[1] = ind3;
                v[2] = ind2;
                v[3] = ind1;
            }
            else
            {
                v[0] = ind1;
                v[1] = ind2;
                v[2] = ind3;
                v[3] = ind4;
            }
            //tet.push_back(v);
            m_emb.push_back(verticesMapID[v[0]]);
            m_emb.push_back(verticesMapID[v[1]]);
            m_emb.push_back(verticesMapID[v[2]]);
            m_emb.push_back(verticesMapID[v[3]]);
        }
        std::getline (fp, ligne);
        tag = ligne.substr(0,4);
    } while (!fp.eof());

    fp.close();
*/
    return true;
}

template <typename PFP>
bool MeshTablesVolume<PFP>::importMSH(const std::string& filename, std::vector<std::string>& attrNames)
{
    VertexAttribute<VEC3> position =  m_map.template getAttribute<VEC3, VERTEX>("position") ;

    if (!position.isValid())
        position = m_map.template addAttribute<VEC3, VERTEX>("position") ;

    attrNames.push_back(position.name()) ;

    //
    AttributeContainer& container = m_map.template getAttributeContainer<VERTEX>() ;

    // open file
    std::ifstream fp(filename.c_str(), std::ios::in);
    if (!fp.good())
    {
        CGoGNerr << "Unable to open file " << filename << CGoGNendl;
        return false;
    }

    std::string ligne;
    unsigned int nbv=0;
    //read $NODE
    std::getline (fp, ligne);

    // reading number of vertices
    std::getline (fp, ligne);
    std::stringstream oss(ligne);
    oss >> nbv;

    //reading vertices
//	std::vector<unsigned int> verticesID;
    std::map<unsigned int, unsigned int> verticesMapID;


//	verticesID.reserve(nbv);
    for(unsigned int i = 0; i < nbv;++i)
    {
        do
        {
            std::getline (fp, ligne);
        } while (ligne.size() == 0);

        std::stringstream oss(ligne);
        unsigned int pipo;
        float x,y,z;
        oss >> pipo;
        oss >> x;
        oss >> y;
        oss >> z;
        // TODO : if required read other vertices attributes here
        VEC3 pos(x,y,z);

        unsigned int id = container.insertLine();
        position[id] = pos;

        verticesMapID.insert(std::pair<unsigned int, unsigned int>(pipo,id));
//		verticesID.push_back(id);
    }

    // ENNODE
    std::getline (fp, ligne);

    m_nbVertices = nbv;

    // ELM
    std::getline (fp, ligne);

    // reading number of elements
    std::getline (fp, ligne);
    unsigned int nbe=0;
    std::stringstream oss2(ligne);
    oss2 >> nbe;

    bool invertVol = false;

    for(unsigned int i=0; i<nbe; ++i)
    {
        unsigned int pipo,type_elm,nb;
        fp >> pipo;
        fp >> type_elm;
        fp >> pipo;
        fp >> pipo;
        fp >> nb;

        if ((type_elm==4) && (nb==4))
        {
            Geom::Vec4ui v;

            m_nbFaces.push_back(4);


            // test orientation of first tetra
            if (i==0)
            {
                fp >> v[0];
                fp >> v[1];
                fp >> v[2];
                fp >> v[3];

                typename PFP::VEC3 P = position[verticesMapID[v[0]]];
                typename PFP::VEC3 A = position[verticesMapID[v[1]]];
                typename PFP::VEC3 B = position[verticesMapID[v[2]]];
                typename PFP::VEC3 C = position[verticesMapID[v[3]]];

                if (Geom::testOrientation3D<typename PFP::VEC3>(P,A,B,C) == Geom::OVER)
                {
                    invertVol=true;
                    unsigned int ui=v[0];
                    v[0] = v[3];
                    v[3] = v[2];
                    v[2] = v[1];
                    v[1] = ui;
                }
            }
            else
            {
                if (invertVol)
                {
                    fp >> v[1];
                    fp >> v[2];
                    fp >> v[3];
                    fp >> v[0];
                }
                else
                {
                    fp >> v[0];
                    fp >> v[1];
                    fp >> v[2];
                    fp >> v[3];
                }
            }
            m_emb.push_back(verticesMapID[v[0]]);
            m_emb.push_back(verticesMapID[v[1]]);
            m_emb.push_back(verticesMapID[v[2]]);
            m_emb.push_back(verticesMapID[v[3]]);

        }
        else
        {
            if ((type_elm==5) && (nb==8))
            {
                Geom::Vec4ui v;

                m_nbFaces.push_back(8);

                if (i==0)
                {
                    unsigned int last;
                    fp >> v[0];
                    fp >> v[1];
                    fp >> v[2];
                    fp >> v[3];
                    fp >> last;

                    typename PFP::VEC3 P = position[verticesMapID[last]];
                    typename PFP::VEC3 A = position[verticesMapID[v[0]]];
                    typename PFP::VEC3 B = position[verticesMapID[v[1]]];
                    typename PFP::VEC3 C = position[verticesMapID[v[2]]];

                    if (Geom::testOrientation3D<typename PFP::VEC3>(P,A,B,C) == Geom::OVER)
                    {

                        invertVol=true;
                        unsigned int ui = v[3];
                        v[3] = v[0];
                        v[0] = ui;
                        ui = v[2];
                        v[2] = v[1];
                        v[1] = ui;
                       // hexa.push_back(v);
                        m_emb.push_back(verticesMapID[v[0]]);
                        m_emb.push_back(verticesMapID[v[1]]);
                        m_emb.push_back(verticesMapID[v[2]]);
                        m_emb.push_back(verticesMapID[v[3]]);
                        v[3] = last;
                        fp >> v[2];
                        fp >> v[1];
                        fp >> v[0];
                        //hexa.push_back(v);
                        m_emb.push_back(verticesMapID[v[0]]);
                        m_emb.push_back(verticesMapID[v[1]]);
                        m_emb.push_back(verticesMapID[v[2]]);
                        m_emb.push_back(verticesMapID[v[3]]);
                    }
                    else
                    {
                        m_emb.push_back(verticesMapID[v[0]]);
                        m_emb.push_back(verticesMapID[v[1]]);
                        m_emb.push_back(verticesMapID[v[2]]);
                        m_emb.push_back(verticesMapID[v[3]]);
                        //hexa.push_back(v);
                        v[0] = last;
                        fp >> v[1];
                        fp >> v[2];
                        fp >> v[3];
                        //hexa.push_back(v);
                        m_emb.push_back(verticesMapID[v[0]]);
                        m_emb.push_back(verticesMapID[v[1]]);
                        m_emb.push_back(verticesMapID[v[2]]);
                        m_emb.push_back(verticesMapID[v[3]]);
                    }
                }
                else
                {
                    if (invertVol)
                    {
                        fp >> v[3];
                        fp >> v[2];
                        fp >> v[1];
                        fp >> v[0];
                        m_emb.push_back(verticesMapID[v[0]]);
                        m_emb.push_back(verticesMapID[v[1]]);
                        m_emb.push_back(verticesMapID[v[2]]);
                        m_emb.push_back(verticesMapID[v[3]]);
                        //hexa.push_back(v);
                        fp >> v[3];
                        fp >> v[2];
                        fp >> v[1];
                        fp >> v[0];
                        m_emb.push_back(verticesMapID[v[0]]);
                        m_emb.push_back(verticesMapID[v[1]]);
                        m_emb.push_back(verticesMapID[v[2]]);
                        m_emb.push_back(verticesMapID[v[3]]);
                        //hexa.push_back(v);

                    }
                    else
                    {
                        fp >> v[0];
                        fp >> v[1];
                        fp >> v[2];
                        fp >> v[3];
                        m_emb.push_back(verticesMapID[v[0]]);
                        m_emb.push_back(verticesMapID[v[1]]);
                        m_emb.push_back(verticesMapID[v[2]]);
                        m_emb.push_back(verticesMapID[v[3]]);
                        //hexa.push_back(v);
                        fp >> v[0];
                        fp >> v[1];
                        fp >> v[2];
                        fp >> v[3];
                        m_emb.push_back(verticesMapID[v[0]]);
                        m_emb.push_back(verticesMapID[v[1]]);
                        m_emb.push_back(verticesMapID[v[2]]);
                        m_emb.push_back(verticesMapID[v[3]]);
                        //hexa.push_back(v);
                    }
                }
            }
            else
            {
                for (unsigned int j=0; j<nb; ++j)
                {
                    unsigned int v;
                    fp >> v;
                }
            }
        }
    }
}

} // namespace Import

} // namespace Volume

} // namespace Algo

} // namespace CGoGN
