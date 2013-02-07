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

#include "Topology/generic/mapBrowser.h"
#include "Topology/generic/cellmarker.h"


namespace CGoGN
{
namespace Algo
{
namespace Surface
{
namespace Import 
{


short readObjLine(std::stringstream& oss, std::vector<unsigned int>& indices)
{
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
bool importObjTex(typename PFP::MAP& map, const std::string& filename,
				  std::vector<std::string>& attrNames, std::vector<MapBrowserLinked*>& browsers, CellMarker<VERTEX>& markV )
{
	typedef  typename PFP::VEC3 VEC3;
	typedef Geom::Vec2f VEC2;

	attrNames.clear();
	browsers.clear();
// parse file
	// open file
	std::ifstream fp(filename.c_str(), std::ios::binary);
	if (!fp.good())
	{
		CGoGNerr << "Unable to open file " << filename << CGoGNendl;
		return false;
	}

	unsigned int tagV = 0;
	unsigned int tagVT = 0;
	unsigned int tagVN = 0;
	unsigned int tagG = 0;
	unsigned int tagF = 0;

	std::string ligne;
	std::string tag;
	do
	{
		fp >> tag;
		std::getline (fp, ligne);
		if (tag == "v")
			tagV++;
		if (tag == "vn")
			tagVN++;
		if (tag == "vt")
			tagVT++;
		if (tag == "g")
			tagG++;
		if (tag == "f")
			tagF++;
	}while (!fp.eof());

	std::cout << "Parsing OBJ"<< tagV<< std::endl;
	std::cout << "Vertices:"<< tagV<< std::endl;
	std::cout << "Normals:"<< tagVN<< std::endl;
	std::cout << "TexCoords:"<< tagVT<< std::endl;
	std::cout << "Groups:"<< tagG<< std::endl;
	std::cout << "Faces:"<< tagF<< std::endl;


	VertexAttribute<VEC3> positions =  m_map.template getAttribute<typename PFP::VEC3, VERTEX>("position") ;
	if (!positions.isValid())
		positions = m_map.template addAttribute<VEC3, VERTEX>("position") ;
	attrNames.push_back(positions.name()) ;

	if (tagVN != 0)
	{
		VertexAttribute<VEC3> normals =  m_map.template getAttribute<typename PFP::VEC3, VERTEX>("normal") ;
		if (!normals.isValid())
			normals = m_map.template addAttribute<VEC3, VERTEX>("normal") ;
		attrNames.push_back(normals.name()) ;

		AttributeHandler<VEC3,VERTEX1> normalsF =  m_map.template getAttribute<VEC3, VERTEX1>("normalF") ;
		if (!normalsF.isValid())
			normalsF = m_map.template addAttribute<VEC3, VERTEX1>("normalF") ;
	}

	if (tagVT != 0)
	{
		VertexAttribute<Geom::Vec2f> texCoords =  m_map.template getAttribute<VEC2, VERTEX>("texCoord") ;
		if (!texCoords.isValid())
			texCoords = m_map.template addAttribute<VEC2, VERTEX>("texCoord") ;
		attrNames.push_back(texCoords.name()) ;

		AttributeHandler<Geom::Vec2f,VERTEX1> texCoordsF =  m_map.template getAttribute<VEC2, VERTEX1>("texCoordF") ;
		if (!texCoordsF.isValid())
			texCoordsF = m_map.template addAttribute<VEC2, VERTEX1>("texCoordF") ;
	}

	if (tagG != 0)
		browsers.reserve(tagG);

	AttributeContainer& container = m_map.template getAttributeContainer<VERTEX>() ;

	fp.close();
	fp.clear();
	fp.open(filename.c_str());

	std::vector<VEC3> normalsBuffer;
	std::vector<VEC2> texCoordsBuffer;
	std::vector<unsigned int> verticesID;
	std::vector<unsigned int> noramlsID;
	std::vector<unsigned int> texCoordsID;

	normalsBuffer.reserve(tagVN);
	texCoordsBuffer.reserve(tagVT);
	verticesID.reserve(tagV); // approx
	noramlsID.reserve(tagV);
	texCoordsID.reserve(tagV);

	std::vector<unsigned int> localIndices;
	localIndices.reserve(64*3);
	FunctorInitEmb<typename PFP::MAP, VERTEX> fsetemb(map);

	VertexAutoAttribute< NoMathIONameAttribute< std::vector<Dart> > > vecDartsPerVertex(map, "incidents");
	VertexAutoAttribute< NoMathIONameAttribute< std::vector<unsigned int> > > vecNormIndPerVertex(map, "incidentsN");
	VertexAutoAttribute< NoMathIONameAttribute< std::vector<unsigned int> > > vecTCIndPerVertex(map, "incidentsTC");


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
			positions[id] = pos;

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
			texcoordsBuffer.push_back(tc);
		}

		if (tag == std::string("g"))
		{
			currentBrowser = new MapBrowserLinked(map);
			browsers.push_back();
		}

		if (tag == std::string("f"))
		{
			std::stringstream oss(ligne);

			short nbe = readObjLine(oss,localIndices);

			Dart d = map.newFace(nbe, false);
			for (short j = 0; j < nbe; ++j)
			{
				unsigned int em = localIndices[3*j]-1;		// get embedding
				fsetemb.changeEmb(em) ;
				map.template foreach_dart_of_orbit<PFP::MAP::VERTEX_OF_PARENT>(d, fsetemb);
				m.mark(d) ;								// mark on the fly to unmark on second loop
				vecDartsPerVertex[em].push_back(d);		// store incident darts for fast adjacency reconstruction
				vecTCIndPerVertex[em].push_back(localIndices[3*j+1]-1);
				vecNormIndPerVertex[em].push_back(localIndices[3*j+2]-1);
				d = map.phi1(d);
			}
		}
		fp >> tag;
		std::getline(fp, ligne);
	} while (!fp.eof());



	// reconstruct neighbourhood
	unsigned int nbBoundaryEdges = 0;
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (m.isMarked(d))
		{
			// darts incident to end vertex of edge
			std::vector<Dart>& vec = vecDartsPerVertex[map.phi1(d)];

			unsigned int embd = map.template getEmbedding<VERTEX>(d);
			Dart good_dart = NIL;
			for (typename std::vector<Dart>::iterator it = vec.begin(); it != vec.end() && good_dart == NIL; ++it)
			{
				if (map.template getEmbedding<VERTEX>(map.phi1(*it)) == embd)
					good_dart = *it;
			}

			if (good_dart != NIL)
			{
				map.sewFaces(d, good_dart, false);
				m.unmarkOrbit<EDGE>(d);
			}
			else
			{
				m.unmark(d);
				++nbBoundaryEdges;
			}
		}
	}

	TraversorV tra(map);
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
		if (!same)
		{
			for (unsigned int j=0; j<nb; ++j)
			{
				Dart e = vec[i];
				texCoordsF[e] = vecTCIndPerVertex[e][j];
				normalsF[e] = vecNormIndPerVertex[e][j];
			}
			markV.mark(d);
		}
		else
		{
			texCoords[d] = vecTCIndPerVertex[d][0];
			normals[d] = vecNormIndPerVertex[d][0];
			markV.unmark(d);
		}
	}







////	std::string ligne;
////	std::string tag;

////	do
////	{
////		fp >> tag;
////		std::getline (fp, ligne);
////	}while (tag != std::string("v"));

//	// lecture des sommets
//	std::vector<unsigned int> verticesID;
//	verticesID.reserve(102400); // on tape large (400Ko wahouuuuu !!)

//	m_texcoordsBuffer.reserve(102400);

//	m_normalsBuffer.reserve(102400);

//	unsigned int i = 0;
//	do
//	{
//		if (tag == std::string("v"))
//		{
//			std::stringstream oss(ligne);

//			float x,y,z;
//			oss >> x;
//			oss >> y;
//			oss >> z;

//			VEC3 pos(x,y,z);

//			unsigned int id = container.insertLine();
//			positions[id] = pos;

//			verticesID.push_back(id);
//			i++;
//		}

//		if (tag == std::string("vn"))
//		{
//			std::stringstream oss(ligne);

//			VEC3 norm;
//			oss >> norm[0];
//			oss >> norm[1];
//			oss >> norm[2];
//			m_normalsBuffer.push_back(norm);
//		}

//		if (tag == std::string("vt"))
//		{
//			std::stringstream oss(ligne);
//			VEC2 tc;
//			oss >> tc[0];
//			oss >> tc[1];
//			m_texcoordsBuffer.push_back(tc);
//		}

//		fp >> tag;
//		std::getline(fp, ligne);
//	} while (!fp.eof());

//	m_nbVertices = verticesID.size();

//	// close/clear/open only way to go back to beginning of file
//	fp.close();
//	fp.clear();
//	fp.open(filename.c_str());

//	do
//	{
//		fp >> tag;
//		std::getline (fp, ligne);
//	} while (tag != std::string("f"));

//	m_nbEdges.reserve(verticesID.size()*2);
//	m_emb.reserve(verticesID.size()*8);

//	std::vector<int> table;
//	table.reserve(64); // NBV cotes pour une face devrait suffire
//	m_nbFaces = 0;
//	do
//	{
//		if (tag == std::string("f")) // lecture d'une face
//		{
//			std::stringstream oss(ligne);
//			table.clear();
//			while (!oss.eof())  // lecture de tous les indices
//			{
//				std::string str;
//				oss >> str;

//				unsigned int ind = 0;

//// TODO A MODIFIER POUR LIRE P/T/N ou P//N ou P/T ...

//				while ((ind<str.length()) && (str[ind]!='/'))
//					ind++;

//				if (ind > 0)
//				{
//					long index;
//					std::stringstream iss(str.substr(0, ind));
//					iss >> index;
//					table.push_back(index);
//				}
//			}

//			unsigned int n = table.size();
//			m_nbEdges.push_back(short(n));
//			for (unsigned int j = 0; j < n; ++j)
//			{
//				int index = table[j] - 1; // les index commencent a 1 (boufonnerie d'obj ;)
//				m_emb.push_back(verticesID[index]);
//			}
//			m_nbFaces++;
//		}
//		fp >> tag;
//		std::getline(fp, ligne);
//	 } while (!fp.eof());

//	fp.close ();
//	return true;
}











template<typename PFP>
bool importObjWithTex(typename PFP::MAP& map, const std::string& filename)
{
	typedef typename PFP::EMB VertexEmb;
	

	// open file
	std::ifstream fp(filename.c_str(), std::ios::binary);
	if (!fp.good())
	{
		CGoGNerr << "Unable to open file " << filename<< CGoGNendl;
		return false;
	}

    std::string ligne;
    std::string tag;

    do
    {
    	fp >> tag;
    	std::getline (fp, ligne);
    }while (tag != std::string("v"));

    // lecture des sommets

	std::vector<VertexEmb*> vertices;
	vertices.reserve(102400); 

	std::vector<Emb::TexCoord2*> texcoords;
	texcoords.reserve(102400);


	// coordonnees de texture "fake" pour le cas des faces ne soit pas texturée
	Emb::TexCoord2* tc = Emb::TexCoord2::create(gmtl::Vec2f(0.0f,0.0f));
	tc->setLabel(0);
	texcoords.push_back(tc);

	int i=0;
	int j=1;
    do
    {
		if (tag == std::string("v"))
		{
			std::stringstream oss(ligne);
		
			float x,y,z;
			oss >> x;
			oss >> y;
			oss >> z;

			VertexEmb* em = VertexEmb::create(gmtl::Vec3f(x,y,z));
			em->setLabel(i);
			vertices.push_back(em);
			i++;
		}
		else if (tag == std::string("vt"))
		{
			std::stringstream oss(ligne);
		
			float u,v;
			oss >> u;
			oss >> v;

			Emb::TexCoord2* tc = Emb::TexCoord2::create(gmtl::Vec2f(u,v)); 
			tc->setLabel(j);
			texcoords.push_back(tc);
// 			CGoGNout << "vt :"<<tc->getTexCoord()<<CGoGNendl;
			j++;
		}

		fp >> tag;
    	std::getline (fp, ligne);

    }while (!fp.eof());


	// pour la reconstruction du phi2
	std::vector< std::vector<Dart> > dartsOfVertex;
	dartsOfVertex.resize(vertices.size());
	for (int i=0; i<dartsOfVertex.size(); ++i)
	{
		dartsOfVertex[i].reserve(16);
	}


	// close/clear/open only way to go back to beginning of file
	fp.close();
	fp.clear();
	fp.open(filename.c_str());


	// saute tout jusqu'à la premiere face
	do
    {
    	fp >> tag;
    	std::getline (fp, ligne);
    }while (tag != std::string("f"));


	std::vector<int> table;
	table.reserve(64); // 64 cotes pour une face ca devrait suffire
	std::vector<int> tableTex;
	tableTex.reserve(64); // 64 cotes pour une face ca devrait suffire

	do
	{
		if  (tag == std::string("f")) // lecture d'une face
		{
			std::stringstream oss(ligne);
     		table.clear();
			tableTex.clear();

    		while (!oss.eof())  // lecture de tous les indices
    		{
    			std::string str;
    			oss >> str;

				//position du premier /
    			unsigned ind=0;
    			while ( (str[ind]!='/')&& (ind<str.length()) )
    			{
    				ind++;
    			}
				// position du deuxieme /
  				unsigned ind2=ind;
     			while ( (str[ind]!='/')&& (ind2<str.length()) )
     			{
     				ind2++;
     			}
				ind2;

				if (ind>0)
				{
    				int index;
					std::stringstream iss(str.substr(0,ind));
					iss >> index;
		   			table.push_back(index);
				}

				if (ind2<str.length())
				{
    				int index;
					std::stringstream iss(str.substr(ind2+1,str.length()));
					iss >> index;

//					CGoGNout << "Tex: "<< index<<CGoGNendl;
					tableTex.push_back(index);
				}
    		}

// 			CGoGNout << "Face: ";
			int n = table.size();	
			Dart d = map.newFace(n);
			for (int j=0;j<n; ++j)
    		{
				int k = table[j]-1;
				map.setVertexEmb(d, vertices[k]); // pas direct sur le brin pour les GCartes
				dartsOfVertex[k].push_back(d);
				if (!tableTex.empty())
				{
					map.setFaceVertexEmb(d,texcoords[tableTex[j]]); // pas -1 car on a utilisé
// 					CGoGNout <<texcoords[tableTex[j]]->getTexCoord()<< " / ";
				}
				else 
					map.setFaceVertexEmb(d,texcoords[0]);
				d = map.phi1(d);
    		}
// 			CGoGNout << CGoGNendl;;
    	}
		fp >> tag;
    	std::getline (fp, ligne);
     }while (!fp.eof());


	for (Dart d = map.begin(); d != map.end(); ++d)
	{
		if (map.phi2(d) == d)
		{
			Emb::Embedding* em0 = map.getVertexEmb(d);
			Emb::Embedding* em1 = NULL;

			int k = map.getVertexEmb(map.phi1(d))->getLabel();
			typename std::vector<Dart>::iterator it = dartsOfVertex[k].begin();

			do
			{
				em1 = map.getVertexEmb( map.phi1(*it) );
				if (em1 == em0)
				{
					map.sewFaces(d,*it);
				}
				it++;
			} while ((em1 != em0) && (it!= dartsOfVertex[k].end()));
		}
	}

	fp.close ();
	return true;
}

}
}
} // end namespaces
}
