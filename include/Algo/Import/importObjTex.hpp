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

#include "Topology/generic/attributeHandler.h"
#include "Container/fakeAttribute.h"
#include <fstream>


namespace CGoGN
{
namespace Algo
{
namespace Surface
{
namespace Import 
{


template <typename PFP>
OBJModel<PFP>::OBJModel(typename PFP::MAP& map):
	m_map(map),
	m_tagV(0),m_tagVT(0),m_tagVN(0),m_tagG(0),m_tagF(0),
	m_specialVertices(map),m_dirtyEdges(map)
{
}

template <typename PFP>
inline typename PFP::VEC3 OBJModel<PFP>::getPosition(Dart d)
{
	return m_positions[d];
}


template <typename PFP>
inline typename PFP::VEC3 OBJModel<PFP>::getNormal(Dart d)
{
	if (m_specialVertices.isMarked(d))
		return m_normalsF[d];
	return m_normals[d];
}


template <typename PFP>
inline Geom::Vec2f OBJModel<PFP>::getTexCoord(Dart d)
{
	if (m_specialVertices.isMarked(d))
		return m_texCoordsF[d];
	return m_texCoords[d];
}


template <typename PFP>
void OBJModel<PFP>::setPositionAttribute(VertexAttribute<Geom::Vec3f> position)
{
	m_positions = position;
}

template <typename PFP>
void OBJModel<PFP>::setNormalAttribute(VertexAttribute<Geom::Vec3f> normal)
{
	m_normals = normal;
}

template <typename PFP>
void OBJModel<PFP>::setTexCoordAttribute(VertexAttribute<Geom::Vec2f>texcoord)
{
	m_texCoords = texcoord;
}




template <typename PFP>
void OBJModel<PFP>::readMaterials(const std::string& filename, std::vector<MaterialOBJ>& materials)
{
	materials.reserve(m_materialNames.size());
		
	// open file
	std::ifstream fp(filename.c_str());
	if (!fp.good())
	{
		CGoGNerr << "Unable to open file " << filename << CGoGNendl;
		return ;
	}
	
	std::vector<MaterialOBJ>::iterator mit;

	std::string ligne;
	std::string tag;
	fp >> tag;
	do
	{
		std::getline (fp, ligne);
		if (tag == "newmtl")
		{

			std::map<std::string,int>::iterator it = m_materialNames.find(ligne);
			if (it ==  m_materialNames.end())
			{
				CGoGNerr << "Skipping material "<< ligne << CGoGNendl;
				do
				{
					fp >> tag;
					
				}while (!fp.eof() && (tag == "new mtl")); 
			}
			else
			{
				CGoGNout << "Reading material "<< ligne << CGoGNendl;

				if (materials.empty())
				{
					materials.resize(1);
					mit = materials.begin();
				}
				else
				{
					materials.resize(materials.size()+1);
					++mit;
				}
				it->second = (mit-materials.begin());
				mit->name = ligne;
			}
		}
		else
		{
			std::stringstream oss(ligne);
			if (tag == "Ka")
			{
				oss >> mit->ambiantColor[0];
				oss >> mit->ambiantColor[1];
				oss >> mit->ambiantColor[2];
			}
			if (tag == "Kd")
			{
				oss >> mit->diffuseColor[0];
				oss >> mit->diffuseColor[1];
				oss >> mit->diffuseColor[2];
				
			}
			if (tag == "Ks")
			{
				oss >> mit->specularColor[0];
				oss >> mit->specularColor[1];
				oss >> mit->specularColor[2];
			}
			if (tag == "Ns")
			{
				oss >> mit->shininess;
			}
			if (tag == "Tf")
			{
				oss >> mit->transparentFilter[0];
				oss >> mit->transparentFilter[1];
				oss >> mit->transparentFilter[2];
			}

			if ((tag == "Tr") || (tag == "d"))
			{
				oss >> mit->transparency;
			}
			
			if (tag == "map_Ka") // ambiant texture
			{
				CGoGNerr << tag << " not yet supported in OBJ material reading" << CGoGNendl;
			}
			if (tag == "map_Kd") // diffuse texture
			{
				mit->textureDiffuse = new Utils::Texture<2,Geom::Vec3uc>(GL_UNSIGNED_BYTE);
				std::string tname;
				oss >> tname;
				mit->textureDiffuse->load(tname);
				CGoGNout << "Loading texture "<< tname << CGoGNendl;

			}
			if (tag == "map_d") // opacity texture
			{
				CGoGNerr << tag << " not yet supported in OBJ material reading" << CGoGNendl;
			}
			if ((tag == "map_bump") || (tag == "bump"))
			{
				CGoGNerr << tag << " not yet supported in OBJ material reading" << CGoGNendl;
			}
			
			fp >> tag;

		}
	}while (!fp.eof());
}





template <typename PFP>
bool OBJModel<PFP>::generateBrowsers(std::vector<MapBrowser*>& browsers)
{
	browsers.clear();
	
	if (m_groupNames.empty())
		return false;
	
	MapBrowserLinked* MBLptr = new MapBrowserLinked(m_map);
	DartAttribute<Dart>& links = MBLptr->getLinkAttr();
	browsers.push_back(MBLptr);
	
	for (unsigned int i = 1; i<m_groupNames.size(); ++i)
	{
		MapBrowser* MBptr = new MapBrowserLinked(m_map,links);
		browsers.push_back(MBptr);
//		std::string& matName =  m_materialNames[i];
		m_groupMaterialID[i]= m_materialNames[m_groupMaterialNames[i]];
	}
	
	for (Dart d=m_map.begin(); d!=m_map.end(); m_map.next(d))
	{
		unsigned int g = m_groups[d] -1 ; // groups are name from 1
		MapBrowserLinked* mb = static_cast<MapBrowserLinked*>(browsers[g]);
		mb->pushBack(d);
	}
	return true;
}




	
template <typename PFP>
short OBJModel<PFP>::readObjLine(std::stringstream& oss, std::vector<unsigned int>& indices)
{
	indices.clear();
	
	unsigned int nb=0;
	while (!oss.eof())  // lecture de tous les indices
	{
		int index;
		oss >> index;

		indices.push_back(index);

		int slash = 0;
		char sep='_';
		do
		{
			oss >> sep;
			if (sep =='/')
				++slash;
		} while ( ((sep=='/') || (sep ==' ')) && !oss.eof() ) ;

		if ((sep>='0') && (sep<='9'))
			oss.seekg(-1,std::ios_base::cur);

		if (slash == 0)
		{
			if (indices.size()%3 == 1)
			{
				indices.push_back(0);
				indices.push_back(0);
			}
			if (indices.size()%3 == 2)
			{
				indices.push_back(0);
			}
			nb++;
		}


		if (slash == 2)
		{
			indices.push_back(0);
		}
	}
	return nb;
}


template <typename PFP>
unsigned int OBJModel<PFP>::createSimpleVBO_P(Utils::VBO* positionVBO)
{
	TraversorF<typename PFP::MAP> traf(m_map);
	std::vector<Geom::Vec3f> posBuff;
	posBuff.reserve(16384);

	unsigned int nbtris = 0;
	for (Dart d=traf.begin(); d!= traf.end(); d = traf.next())
	{
		Dart e = m_map.phi1(d);
		Dart f = m_map.phi1(e);
		do
		{
			posBuff.push_back(m_positions[d]);
			posBuff.push_back(m_positions[e]);
			posBuff.push_back(m_positions[f]);
			e=f;
			f = m_map.phi1(e);
			nbtris++;
		}while (f!=d);
	}

	positionVBO->setDataSize(3);
	positionVBO->allocate(posBuff.size());
	Geom::Vec3f* ptrPos = reinterpret_cast<Geom::Vec3f*>(positionVBO->lockPtr());
	memcpy(ptrPos,&posBuff[0],posBuff.size()*sizeof(Geom::Vec3f));
	positionVBO->releasePtr();

	return 3*nbtris;
}


template <typename PFP>
unsigned int OBJModel<PFP>::createSimpleVBO_PT(Utils::VBO* positionVBO, Utils::VBO* texcoordVBO)
{
	TraversorF<typename PFP::MAP> traf(m_map);
	std::vector<Geom::Vec3f> posBuff;
	std::vector<Geom::Vec2f> TCBuff;
	posBuff.reserve(16384);
	TCBuff.reserve(16384);

	unsigned int nbtris = 0;
	for (Dart d=traf.begin(); d!= traf.end(); d = traf.next())
	{
		Dart e = m_map.phi1(d);
		Dart f = m_map.phi1(e);
		do
		{
			posBuff.push_back(m_positions[d]);
			if (m_specialVertices.isMarked(d))
				TCBuff.push_back(m_texCoordsF[d]);
			else
				TCBuff.push_back(m_texCoords[d]);

			posBuff.push_back(m_positions[e]);
			if (m_specialVertices.isMarked(e))
				TCBuff.push_back(m_texCoordsF[e]);
			else
				TCBuff.push_back(m_texCoords[e]);

			posBuff.push_back(m_positions[f]);
			if (m_specialVertices.isMarked(f))
				TCBuff.push_back(m_texCoordsF[f]);
			else
				TCBuff.push_back(m_texCoords[f]);
			e=f;
			f = m_map.phi1(e);
			nbtris++;
		}while (f!=d);
	}

	positionVBO->setDataSize(3);
	positionVBO->allocate(posBuff.size());
	Geom::Vec3f* ptrPos = reinterpret_cast<Geom::Vec3f*>(positionVBO->lockPtr());
	memcpy(ptrPos,&posBuff[0],posBuff.size()*sizeof(Geom::Vec3f));
	positionVBO->releasePtr();

	texcoordVBO->setDataSize(2);
	texcoordVBO->allocate(TCBuff.size());
	Geom::Vec2f* ptrTC = reinterpret_cast<Geom::Vec2f*>(texcoordVBO->lockPtr());
	memcpy(ptrTC,&TCBuff[0],TCBuff.size()*sizeof(Geom::Vec2f));
	texcoordVBO->releasePtr();

	return 3*nbtris;
}

template <typename PFP>
unsigned int OBJModel<PFP>::createSimpleVBO_PTN(Utils::VBO* positionVBO, Utils::VBO* texcoordVBO, Utils::VBO* normalVBO )
{
	if (!m_normals.isValid())
	{
		CGoGNerr << "no normal attribute "<< CGoGNendl;
		return 0;
	}
	if (!m_texCoords.isValid())
	{
		CGoGNerr << "no tex coords attribute "<< CGoGNendl;
		return 0;
	}


	TraversorF<typename PFP::MAP> traf(m_map);
	std::vector<Geom::Vec3f> posBuff;
	std::vector<Geom::Vec2f> TCBuff;
	std::vector<Geom::Vec3f> normalBuff;
	posBuff.reserve(16384);
	TCBuff.reserve(16384);
	normalBuff.reserve(16384);

	unsigned int nbtris = 0;
	for (Dart d=traf.begin(); d!= traf.end(); d = traf.next())
	{
		Dart e = m_map.phi1(d);
		Dart f = m_map.phi1(e);
		do
		{
			posBuff.push_back(m_positions[d]);
			if (m_specialVertices.isMarked(d))
			{
				if (hasTexCoords())
					TCBuff.push_back(m_texCoordsF[d]);
				else
					TCBuff.push_back(m_texCoords[d]);
				if (hasNormals())
					normalBuff.push_back(m_normalsF[d]);
				else
					normalBuff.push_back(m_normals[d]);
			}
			else
			{
				TCBuff.push_back(m_texCoords[d]);
				normalBuff.push_back(m_normals[d]);
			}

			posBuff.push_back(m_positions[e]);
			if (m_specialVertices.isMarked(e))
			{
				if (hasTexCoords())
					TCBuff.push_back(m_texCoordsF[e]);
				else
					TCBuff.push_back(m_texCoords[e]);
				if (hasNormals())
					normalBuff.push_back(m_normalsF[e]);
				else
					normalBuff.push_back(m_normals[e]);
			}
			else
			{
				TCBuff.push_back(m_texCoords[e]);
				normalBuff.push_back(m_normals[e]);
			}

			posBuff.push_back(m_positions[f]);
			if (m_specialVertices.isMarked(f))
			{
				if (hasTexCoords())
					TCBuff.push_back(m_texCoordsF[f]);
				else
					TCBuff.push_back(m_texCoords[f]);
				if (hasNormals())
					normalBuff.push_back(m_normalsF[f]);
				else
					normalBuff.push_back(m_normals[f]);
			}
			else
			{
				TCBuff.push_back(m_texCoords[f]);
				normalBuff.push_back(m_normals[f]);
			}
			e=f;
			f = m_map.phi1(e);
			nbtris++;
		}while (f!=d);
	}

	positionVBO->setDataSize(3);
	positionVBO->allocate(posBuff.size());
	Geom::Vec3f* ptrPos = reinterpret_cast<Geom::Vec3f*>(positionVBO->lockPtr());
	memcpy(ptrPos,&posBuff[0],posBuff.size()*sizeof(Geom::Vec3f));
	positionVBO->releasePtr();

	texcoordVBO->setDataSize(2);
	texcoordVBO->allocate(TCBuff.size());
	Geom::Vec2f* ptrTC = reinterpret_cast<Geom::Vec2f*>(texcoordVBO->lockPtr());
	memcpy(ptrTC,&TCBuff[0],TCBuff.size()*sizeof(Geom::Vec2f));
	texcoordVBO->releasePtr();

	normalVBO->setDataSize(3);
	normalVBO->allocate(normalBuff.size());
	Geom::Vec3f* ptrNormal = reinterpret_cast<Geom::Vec3f*>(normalVBO->lockPtr());
	memcpy(ptrNormal, &normalBuff[0], normalBuff.size()*sizeof(Geom::Vec3f));
	normalVBO->releasePtr();


	return 3*nbtris;
}


template <typename PFP>
unsigned int OBJModel<PFP>::createSimpleVBO_PN(Utils::VBO* positionVBO, Utils::VBO* normalVBO )
{
	TraversorF<typename PFP::MAP> traf(m_map);
	std::vector<Geom::Vec3f> posBuff;
	std::vector<Geom::Vec2f> normalBuff;
	posBuff.reserve(16384);
	normalBuff.reserve(16384);

	unsigned int nbtris = 0;
	for (Dart d=traf.begin(); d!= traf.end(); d = traf.next())
	{
		Dart e = m_map.phi1(d);
		Dart f = m_map.phi1(e);
		do
		{
			posBuff.push_back(m_positions[d]);
			if (m_specialVertices.isMarked(d))
			{
				normalBuff.push_back(m_normalsF[d]);
			}
			else
			{
				normalBuff.push_back(m_normals[d]);
			}

			posBuff.push_back(m_positions[e]);
			if (m_specialVertices.isMarked(e))
			{
				normalBuff.push_back(m_normalsF[e]);
			}
			else
			{
				normalBuff.push_back(m_normals[e]);
			}

			posBuff.push_back(m_positions[f]);
			if (m_specialVertices.isMarked(f))
			{
				normalBuff.push_back(m_normalsF[f]);
			}
			else
			{
				normalBuff.push_back(m_normals[f]);
			}
			e=f;
			f = m_map.phi1(e);
			nbtris++;
		}while (f!=d);
	}

	positionVBO->setDataSize(3);
	positionVBO->allocate(posBuff.size());
	Geom::Vec3f* ptrPos = reinterpret_cast<Geom::Vec3f*>(positionVBO->lockPtr());
	memcpy(ptrPos,&posBuff[0],posBuff.size()*sizeof(Geom::Vec3f));
	positionVBO->releasePtr();

	normalVBO->setDataSize(3);
	normalVBO->allocate(normalBuff.size());
	Geom::Vec3f* ptrNormal = reinterpret_cast<Geom::Vec3f*>(normalVBO->lockPtr());
	memcpy(ptrNormal, &normalBuff[0], normalBuff.size()*sizeof(Geom::Vec3f));
	normalVBO->releasePtr();


	return 3*nbtris;
}


// template <typename PFP>
// bool importObjTex(typename PFP::MAP& map, const std::string& filename,
// 				  std::vector<std::string>& attrNames,/* std::vector<MapBrowserLinked*>& browsers,*/
// 				  CellMarker<VERTEX>& specialVertices, DartMarker& dirtyEdges)

template <typename PFP>
bool OBJModel<PFP>::import( const std::string& filename, std::vector<std::string>& attrNames)
{
	typedef  typename PFP::VEC3 VEC3;
	typedef Geom::Vec2f VEC2;

	attrNames.clear();
	// open file
	std::ifstream fp(filename.c_str()/*, std::ios::binary*/);
	if (!fp.good())
	{
		CGoGNerr << "Unable to open file " << filename << CGoGNendl;
		return false;
	}

	std::string ligne;
	std::string tag;
	do
	{
		fp >> tag;
		std::getline (fp, ligne);
		if (tag == "v")
			m_tagV++;
		if (tag == "vn")
			m_tagVN++;
		if (tag == "vt")
			m_tagVT++;
		if (tag == "g")
			m_tagG++;
		if (tag == "f")
			m_tagF++;
	}while (!fp.eof());

	std::cout << "Parsing OBJ"<< m_tagV<< std::endl;
	std::cout << "Vertices:"<< m_tagV<< std::endl;
	std::cout << "Normals:"<< m_tagVN<< std::endl;
	std::cout << "TexCoords:"<< m_tagVT<< std::endl;
	std::cout << "Groups:"<< m_tagG<< std::endl;
	std::cout << "Faces:"<< m_tagF<< std::endl;


	m_positions =  m_map.template getAttribute<typename PFP::VEC3, VERTEX>("position") ;
	if (!m_positions.isValid())
		m_positions = m_map.template addAttribute<VEC3, VERTEX>("position") ;
	attrNames.push_back(m_positions.name()) ;

	if (m_tagVT != 0)
	{
		m_texCoords =  m_map.template getAttribute<VEC2, VERTEX>("texCoord") ;
		if (!m_texCoords.isValid())
			m_texCoords = m_map.template addAttribute<VEC2, VERTEX>("texCoord") ;
		attrNames.push_back(m_texCoords.name()) ;

		m_texCoordsF =  m_map.template getAttribute<VEC2, VERTEX1>("texCoordF") ;
		if (!m_texCoordsF.isValid())
			m_texCoordsF = m_map.template addAttribute<VEC2, VERTEX1>("texCoordF") ;
	}

	if (m_tagVN != 0)
	{
		m_normals =  m_map.template getAttribute<typename PFP::VEC3, VERTEX>("normal") ;
		if (!m_normals.isValid())
			m_normals = m_map.template addAttribute<VEC3, VERTEX>("normal") ;
		attrNames.push_back(m_normals.name()) ;

		m_normalsF =  m_map.template getAttribute<VEC3, VERTEX1>("normalF") ;
		if (!m_normalsF.isValid())
			m_normalsF = m_map.template addAttribute<VEC3, VERTEX1>("normalF") ;
	}

	
	if (m_tagG != 0)
	{
		m_groups =  m_map.template getAttribute<unsigned int, FACE>("groups") ;
		if (!m_groups.isValid())
			m_groups = m_map.template addAttribute<unsigned int, FACE>("groups") ;
		attrNames.push_back(m_groups.name()) ;
	}
	

	AttributeContainer& container = m_map.template getAttributeContainer<VERTEX>() ;

	fp.close();
	fp.clear();
	fp.open(filename.c_str());

	std::vector<VEC3> normalsBuffer;
	normalsBuffer.reserve(m_tagVN);
	
	std::vector<VEC2> texCoordsBuffer;
	texCoordsBuffer.reserve(m_tagVT);
	
	std::vector<unsigned int> verticesID;
	verticesID.reserve(m_tagV);
	
	std::vector<unsigned int> normalsID;
	normalsID.reserve(m_tagV);
	std::vector<unsigned int> texCoordsID;
	texCoordsID.reserve(m_tagV);
	
	

	std::vector<unsigned int> localIndices;
	localIndices.reserve(64*3);
	FunctorInitEmb<typename PFP::MAP, VERTEX> fsetemb(m_map);

	VertexAutoAttribute< NoMathIONameAttribute< std::vector<Dart> > > vecDartsPerVertex(m_map, "incidents");
	VertexAutoAttribute< NoMathIONameAttribute< std::vector<unsigned int> > > vecNormIndPerVertex(m_map, "incidentsN");
	VertexAutoAttribute< NoMathIONameAttribute< std::vector<unsigned int> > > vecTCIndPerVertex(m_map, "incidentsTC");

	unsigned int currentGroup = 0;

	DartMarkerNoUnmark mk(m_map) ;
	unsigned int i = 0;
	fp >> tag;
	std::getline(fp, ligne);
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
			m_positions[id] = pos;

			verticesID.push_back(id);
			i++;
		}

		if (tag == std::string("vn"))
		{
			std::stringstream oss(ligne);

			VEC3 norm;
			oss >> norm[0];
			oss >> norm[1];
			oss >> norm[2];
			normalsBuffer.push_back(norm);
		}

		if (tag == std::string("vt"))
		{
			std::stringstream oss(ligne);
			VEC2 tc;
			oss >> tc[0];
			oss >> tc[1];
			texCoordsBuffer.push_back(tc);
		}

		if (tag == std::string("g"))
		{
			m_groupNames.push_back(ligne);
			std::string buf;
			fp >> buf;
			if (buf != "usemtl")
			{
				CGoGNerr << "problem reading OBJ, waiting for usemtl get "<< buf << CGoGNendl;
			}
			fp >> buf;
			m_materialNames.insert(std::pair<std::string,int>(buf,-1));
			m_groupMaterialNames.push_back(buf);
			currentGroup++;
		}

		if (tag == std::string("f"))
		{
			std::stringstream oss(ligne);

			short nbe = readObjLine(oss,localIndices);

			Dart d = m_map.newFace(nbe, false);
			if (m_tagG!=0)
				m_groups[d] = currentGroup;
			
			for (short j = 0; j < nbe; ++j)
			{
				unsigned int em = localIndices[3*j]-1;		// get embedding
				fsetemb.changeEmb(em) ;
				m_map.template foreach_dart_of_orbit<PFP::MAP::VERTEX_OF_PARENT>(d, fsetemb);
				mk.mark(d) ;								// mark on the fly to unmark on second loop
				vecDartsPerVertex[em].push_back(d);		// store incident darts for fast adjacency reconstruction
				vecTCIndPerVertex[em].push_back(localIndices[3*j+1]-1);
				vecNormIndPerVertex[em].push_back(localIndices[3*j+2]-1);
				d = m_map.phi1(d);
			}
		}
		fp >> tag;
		std::getline(fp, ligne);
	} while (!fp.eof());
	fp.close ();

	// reconstruct neighbourhood
	unsigned int nbBoundaryEdges = 0;
	for (Dart d = m_map.begin(); d != m_map.end(); m_map.next(d))
	{
		if (mk.isMarked(d))
		{
			// darts incident to end vertex of edge
			std::vector<Dart>& vec = vecDartsPerVertex[m_map.phi1(d)];

			unsigned int embd = m_map.template getEmbedding<VERTEX>(d);
			Dart good_dart = NIL;
			for (typename std::vector<Dart>::iterator it = vec.begin(); it != vec.end() && good_dart == NIL; ++it)
			{
				if (m_map.template getEmbedding<VERTEX>(m_map.phi1(*it)) == embd)
					good_dart = *it;
			}

			if (good_dart != NIL)
			{
				if (good_dart == m_map.phi2(good_dart) && (d == m_map.phi2(d)))
					m_map.sewFaces(d, good_dart, false);
				else
					m_dirtyEdges.mark(d);
				mk.unmarkOrbit<EDGE>(d);
			}
			else
			{
				mk.unmark(d);
				m_dirtyEdges.mark(d);
				++nbBoundaryEdges;
			}
		}
	}

	TraversorV<typename PFP::MAP> tra(m_map);
	for (Dart d = tra.begin(); d != tra.end(); d = tra.next())
	{
		std::vector<Dart>& vec			= vecDartsPerVertex[d];
		std::vector<unsigned int>& vtc	= vecTCIndPerVertex[d];
		std::vector<unsigned int>& vn	= vecNormIndPerVertex[d];

		// test if normal vertex or multi-attrib vertex
		unsigned int nb = vtc.size();
		bool same=true;
		for (unsigned int j=1; (j<nb)&&(same); ++j)
		{
			if ( (vtc[j] != vtc[0]) || (vn[j] != vn[0]) )
				same = false;
		}

		// if not all faces the same embedding
//		if (!same)
//		{
			for (unsigned int j=0; j<nb; ++j)
			{
				Dart e = vec[j];
				if (m_tagVT)
					m_texCoordsF[e] = texCoordsBuffer[ vecTCIndPerVertex[e][j] ];
				if (m_tagVN)
					m_normalsF[e] = normalsBuffer[ vecNormIndPerVertex[e][j] ];
			}
			m_specialVertices.mark(d);
//		}
//		else
//		{
//			if (tagVT)
//				m_texCoords[d] = texCoordsBuffer[ vecTCIndPerVertex[d][0] ];
//			if (tagVN)
//				m_normals[d] = normalsBuffer[ vecNormIndPerVertex[d][0] ];
//			m_specialVertices.unmark(d);
//		}
	}

	return true;

}



}
}
} // end namespaces
}
