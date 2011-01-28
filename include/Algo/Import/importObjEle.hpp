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

#include "Algo/Modelisation/polyhedron.h"
#include <vector>

namespace CGoGN
{

namespace Algo
{

namespace Import
{

template<typename PFP>
bool importOFFWithELERegions(typename PFP::MAP& map, char* filenameOFF, char * filenameELE, std::vector<std::string>& attrNames)
{
	typedef typename PFP::VEC3 VEC3;

	AttribContainer& container = map.getAttributeContainer(VERTEX_CELL) ;
	AttributeHandler<VEC3> position = map.template addAttribute<VEC3>(VERTEX_ORBIT, "position") ;
	attrNames.push_back(position.name()) ;

	unsigned int m_nbVertices=0, m_nbFaces=0, m_nbEdges=0, m_nbVolumes=0;

	AutoAttributeHandler<  NoMathIONameAttribute< std::vector<Dart> > > vecDartsPerVertex(map, VERTEX_ORBIT, "incidents");

	// open files
	std::ifstream foff(filenameOFF, std::ios::in);
	if (!foff.good())
	{
		std::cerr << "Unable to open OFF file " << std::endl;
		return false;
	}

	std::ifstream fele(filenameELE, std::ios::in);
	if (!fele.good())
	{
		std::cerr << "Unable to open ELE file " << std::endl;
		return false;
	}

	std::string line;

	//OFF reading
	std::getline(foff,line);
	if(line.rfind("OFF") == std::string::npos)
	{
		std::cerr << "Problem reading off file: not an off file"<<std::endl;
		std::cerr << line << std::endl;
		return false;
	}

	//Reading number of vertex/faces/edges in OFF file
	int nbe;
	{
		do
		{
			std::getline(foff,line);
		}while(line.size() == 0);

		std::stringstream oss(line);
		oss >> m_nbVertices;
		oss >> m_nbFaces;
		oss >> m_nbEdges;
		oss >> nbe;
	}

	//Reading number of tetrahedra in ELE file
	int nbv;
	{
		do
		{
			std::getline(fele,line);
		}while(line.size() == 0);

		std::stringstream oss(line);
		oss >> m_nbVolumes;
		oss >> nbv ; oss >> nbv;
	}

	std::cout << "nb points = " << m_nbVertices << " / nb faces = " << m_nbFaces << " / nb edges = " << m_nbEdges << " / nb tet = " << m_nbVolumes << std::endl;

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

//		std::cout << "emb : " << pos << " / id = " << id << std::endl;
		verticesID.push_back(id);
	}

	std::vector<std::vector<Dart> > vecDartPtrEmb;
	vecDartPtrEmb.reserve(m_nbVertices);

	//Read and embed tetrahedra TODO
	for(unsigned i=0; i < m_nbVolumes ; ++i)
	{
		do
		{
			std::getline(fele,line);
		} while(line.size() == 0);

		std::stringstream oss(line);
		oss >> nbe;
//		std::cout << "tetra number : " << nbe << std::endl;

		//Algo::Modelisation::Polyhedron<PFP>::createOrientedTetra(map);
		Dart d = Algo::Modelisation::Polyhedron<PFP>::createOrientedPolyhedron(map,4);
		Geom::Vec4ui pt;
		oss >> pt[0];
		oss >> pt[1];
		oss >> pt[2];
		oss >> pt[3];

		//regions ?
		oss >> nbe;

//		std::cout << "\t embedding number : " << pt[0] << " " << pt[1] << " " << pt[2] << " " << pt[3] << std::endl;

		// Embed three vertices
		for(unsigned int j=0 ; j<3 ; ++j)
		{
//			std::cout << "\t embedding number : " << pt[j];

			FunctorSetEmb<typename PFP::MAP> femb(map,VERTEX_ORBIT,verticesID[pt[j]]);

			Dart dd = d;
			do {
				femb(dd);
				//vecDartPtrEmb[pt[j]].push_back(dd);
				vecDartsPerVertex[pt[j]].push_back(dd);
				dd = map.phi1(map.phi2(dd));
			} while(dd!=d);

			d = map.phi1(d);

//			std::cout << " done" << std::endl;
		}

		//Embed the last vertex
//		std::cout << "\t embedding number : " << pt[3] << std::endl;
		d = map.phi_1(map.phi2(d));

		FunctorSetEmb<typename PFP::MAP> femb(map,VERTEX_ORBIT,verticesID[pt[3]]);
		Dart dd = d;
		do {
			femb(dd);
//			std::cout << "embed" << std::endl;
			//vecDartPtrEmb[pt[3]].push_back(dd);
			vecDartsPerVertex[pt[3]].push_back(dd);
			dd = map.phi1(map.phi2(dd));
		} while(dd != d);

//		std::cout << "end tetra" << std::endl;
	}

//	std::cout << "end 1/2" << std::endl;

	foff.close();
	fele.close();

	//Association des phi3
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		std::vector<Dart>& vec = vecDartsPerVertex[d];

		for(typename std::vector<Dart>::iterator it = vec.begin(); it!=vec.end(); ++it)
		{
			if(map.phi3(*it)==*it)
			{
				bool sewn=false;
				for(typename std::vector<Dart>::iterator itnext=it+1; itnext!=vec.end() && !sewn; ++itnext)
				{
					if(map.getDartEmbedding(VERTEX_ORBIT,map.phi1(*it))==map.getDartEmbedding(VERTEX_ORBIT,map.phi_1(*itnext))
					&& map.getDartEmbedding(VERTEX_ORBIT,map.phi_1(*it))==map.getDartEmbedding(VERTEX_ORBIT,map.phi1(*itnext)))
					{
						map.sewVolumes(*it,map.phi_1(*itnext));
						sewn = true;
					}
				}
			}
		}
	}

//	std::cout << "end 2/2" << std::endl;

	return true;
}

} // namespace Import

} // namespace Algo

} // namespace CGoGN
