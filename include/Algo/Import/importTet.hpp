/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2011, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.u-strasbg.fr/                                         *
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


template <typename PFP>
bool importTet(typename PFP::MAP& map, const std::string& filename, std::vector<std::string>& attrNames, float scaleFactor)
{
	typedef typename PFP::VEC3 VEC3;

	AttributeHandler<VEC3> position = map.template addAttribute<VEC3>(VERTEX, "position") ;
	attrNames.push_back(position.name()) ;

	AttributeContainer& container = map.getAttributeContainer(VERTEX) ;

	unsigned int m_nbVertices = 0, m_nbVolumes = 0;
	AutoAttributeHandler<  NoMathIONameAttribute< std::vector<Dart> > > vecDartsPerVertex(map, VERTEX, "incidents");

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

//		CGoGNout << "VEC3 = " << pos << CGoGNendl;

		unsigned int id = container.insertLine();
		position[id] = pos;

		verticesID.push_back(id);
	}
	m_nbVertices = nbv;

	m_nbVolumes = nbt;

	CGoGNout << "nb points = " << m_nbVertices  << " / nb tet = " << m_nbVolumes << CGoGNendl;

	//Read and embed all tetrahedrons
	for(unsigned int i = 0; i < m_nbVolumes ; ++i)
	{
		//start one tetra

		int nbe;
		do
		{
			std::getline(fp,ligne);
		} while(ligne.size() == 0);

		std::stringstream oss(ligne);
		oss >> nbe; //number of vertices =4
		assert(nbe == 4);

		Dart d = Algo::Modelisation::Polyhedron<PFP>::createPolyhedron(map,4);
		Geom::Vec4ui pt;
		oss >> pt[0];
		oss >> pt[1];
		oss >> pt[2];
		oss >> pt[3];

		//if regions are defined use this number
		oss >> nbe; //ignored here

//		CGoGNout << "\t embedding number : " << pt[0] << " " << pt[1] << " " << pt[2] << " " << pt[3] << CGoGNendl;

		// Embed three "base" vertices
		for(unsigned int j = 0 ; j < 3 ; ++j)
		{
			FunctorSetEmb<typename PFP::MAP> femb(map, VERTEX, verticesID[pt[j]]);
			map.foreach_dart_of_vertex(d,femb); //apply foreach for gmaps : cannot be included in the following loop

			//store darts per vertices to optimize reconstruction
			Dart dd = d;
			do
			{
				vecDartsPerVertex[pt[j]].push_back(dd);
				dd = map.phi1(map.phi2(dd));
			} while(dd != d);

			d = map.phi1(d);
		}

		//Embed the last "top" vertex
		d = map.phi_1(map.phi2(d));

		FunctorSetEmb<typename PFP::MAP> femb(map, VERTEX, verticesID[pt[3]]);
		map.foreach_dart_of_vertex(d,femb); //apply foreach for gmaps : cannot be included in the following loop

		//store darts per vertices to optimize reconstruction
		Dart dd = d;
		do
		{
			vecDartsPerVertex[pt[3]].push_back(dd);
			dd = map.phi1(map.phi2(dd));
		} while(dd != d);

		//end of tetra
	}

//	CGoGNout << "end 1/2" << CGoGNendl;

	//Association des phi3
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		const std::vector<Dart>& vec = vecDartsPerVertex[d];
		for(typename std::vector<Dart>::const_iterator it = vec.begin(); it!=vec.end(); ++it)
		{
			if(map.phi3(*it) == *it)
			{
				bool sewn = false;
				for(typename std::vector<Dart>::const_iterator itnext = it+1; itnext != vec.end() && !sewn; ++itnext)
				{
					if(map.getEmbedding(VERTEX,map.phi1(*it))==map.getEmbedding(VERTEX,map.phi_1(*itnext))
					&& map.getEmbedding(VERTEX,map.phi_1(*it))==map.getEmbedding(VERTEX,map.phi1(*itnext)))
					{
						map.PFP::MAP::TOPO_MAP::sewVolumes(*it, map.phi_1(*itnext));
						sewn = true;
					}
				}
			}
		}
	}

	fp.close();
	return true;
}

} // namespace Import

} // namespace Algo

} // namespace CGoGN
