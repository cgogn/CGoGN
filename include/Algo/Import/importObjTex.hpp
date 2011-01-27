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

#include "texCoord2.h"

namespace CGoGN
{
namespace Algo
{
namespace Import 
{

template<typename PFP>
bool importObjWithTex(typename PFP::MAP& map, const std::string& filename)
{
	typedef typename PFP::EMB VertexEmb;
	

	// open file
	std::ifstream fp(filename.c_str(), std::ios::binary);
	if (!fp.good())
	{
		std::cerr << "Unable to open file " << filename<< std::endl;
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
// 			std::cout << "vt :"<<tc->getTexCoord()<<std::endl;
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

//					std::cout << "Tex: "<< index<<std::endl;
					tableTex.push_back(index);
				}
    		}

// 			std::cout << "Face: ";
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
// 					std::cout <<texcoords[tableTex[j]]->getTexCoord()<< " / ";
				}
				else 
					map.setFaceVertexEmb(d,texcoords[0]);
				d = map.phi1(d);
    		}
// 			std::cout << std::endl;;
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
} // end namespaces
}
