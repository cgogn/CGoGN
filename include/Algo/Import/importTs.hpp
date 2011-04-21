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

/*
 * TODO Quoi faire des champs scalaires
 *
 */
template <typename PFP>
bool importTs(typename PFP::MAP& map, const std::string& filename, std::vector<std::string>& attrNames, float scaleFactor)
{
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;

	AttributeHandler<VEC3> position = map.template addAttribute<VEC3>(VERTEX_ORBIT, "position") ;
	attrNames.push_back(position.name()) ;
	AttributeHandler<REAL> scalaire = map.template addAttribute<REAL>(VERTEX_ORBIT, "scalar");
	attrNames.push_back(scalaire.name()) ;

	AttributeContainer& container = map.getAttributeContainer(VERTEX_CELL) ;

	unsigned int m_nbVertices = 0, m_nbFaces = 0, m_nbEdges = 0, m_nbVolumes = 0;
	AutoAttributeHandler< NoMathIONameAttribute< std::vector<Dart> > > vecDartsPerVertex(map, VERTEX_ORBIT, "incidents");

	// open file
	std::ifstream fp(filename.c_str(), std::ios::in);
	if (!fp.good())
	{
		CGoGNerr << "Unable to open file " << filename<< CGoGNendl;
		return false;
	}

	//Difference entre un .tet et un point .ts se situe a la lecture
	//du nb de sommets et tetrahedres
	std::string ligne;
	unsigned int nbv, nbt;
	// lecture des nombres de sommets/tetra
	std::getline (fp, ligne);
	std::stringstream oss(ligne);
	oss >> nbv;

	//CGoGNout << "nbV = " << nbv << CGoGNendl;
	oss >> nbt;

	//CGoGNout << "nbT = " << nbt << CGoGNendl;

	//lecture sommets
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
		// on peut ajouter ici la lecture de couleur si elle existe
		VEC3 pos(x*scaleFactor,y*scaleFactor,z*scaleFactor);

		//CGoGNout << "VEC3 = " << pos << CGoGNendl;

		unsigned int id = container.insertLine();
		position[id] = pos;

		float scal;
		oss >> scal;
		scalaire[id] = scal;

		verticesID.push_back(id);
	}
	m_nbVertices = verticesID.size();

	//CGoGNout << "nbVertices = " << m_nbVertices << CGoGNendl;
	m_nbVolumes = nbt;
	//CGoGNout << "nbVolumes = " << m_nbVolumes << CGoGNendl;

	// lecture tetra
	// normalement m_nbVolumes*12 (car on ne charge que des tetra)

	m_nbFaces = nbt*4;

	CGoGNout << "nb points = " << m_nbVertices << " / nb faces = " << m_nbFaces << " / nb edges = " << m_nbEdges << " / nb tet = " << m_nbVolumes << CGoGNendl;

	//Read and embed tetrahedra TODO
	for(unsigned int i = 0; i < m_nbVolumes ; ++i)
	{
		int nbe;
		do
		{
			std::getline(fp,ligne);
		} while(ligne.size() == 0);

		std::stringstream oss(ligne);
//		CGoGNout << "tetra number : " << nbe << CGoGNendl;

		//Algo::Modelisation::Polyhedron<PFP>::createOrientedTetra(map);
		Dart d = Algo::Modelisation::Polyhedron<PFP>::createOrientedPolyhedron(map,4);
		Geom::Vec4ui pt;
		oss >> pt[0];
		oss >> pt[1];
		oss >> pt[2];
		oss >> pt[3];

		//regions ?
		oss >> nbe;

//		CGoGNout << "\t embedding number : " << pt[0] << " " << pt[1] << " " << pt[2] << " " << pt[3] << CGoGNendl;

		// Embed three vertices
		for(unsigned int j = 0 ; j < 3 ; ++j)
		{
//			CGoGNout << "\t embedding number : " << pt[j];

			FunctorSetEmb<typename PFP::MAP> femb(map, VERTEX_ORBIT, verticesID[pt[j]]);

			Dart dd = d;
			do
			{
				femb(dd);
				//vecDartPtrEmb[pt[j]].push_back(dd);
				vecDartsPerVertex[pt[j]].push_back(dd);
				dd = map.phi1(map.phi2(dd));
			} while(dd != d);

			d = map.phi1(d);

//			CGoGNout << " done" << CGoGNendl;
		}

		//Embed the last vertex
//		CGoGNout << "\t embedding number : " << pt[3] << CGoGNendl;
		d = map.phi_1(map.phi2(d));

		FunctorSetEmb<typename PFP::MAP> femb(map, VERTEX_ORBIT, verticesID[pt[3]]);
		Dart dd = d;
		do
		{
			femb(dd);
//			CGoGNout << "embed" << CGoGNendl;
			//vecDartPtrEmb[pt[3]].push_back(dd);
			vecDartsPerVertex[pt[3]].push_back(dd);
			dd = map.phi1(map.phi2(dd));
		} while(dd != d);

//		CGoGNout << "end tetra" << CGoGNendl;
	}

//	CGoGNout << "end 1/2" << CGoGNendl;

	//Association des phi3
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		std::vector<Dart>& vec = vecDartsPerVertex[d];

		for(typename std::vector<Dart>::iterator it = vec.begin(); it != vec.end(); ++it)
		{
			if(map.phi3(*it)==*it)
			{
				bool sewn=false;
				for(typename std::vector<Dart>::iterator itnext = it+1; itnext != vec.end() && !sewn; ++itnext)
				{
					if(map.getDartEmbedding(VERTEX_ORBIT,map.phi1(*it))==map.getDartEmbedding(VERTEX_ORBIT,map.phi_1(*itnext))
					&& map.getDartEmbedding(VERTEX_ORBIT,map.phi_1(*it))==map.getDartEmbedding(VERTEX_ORBIT,map.phi1(*itnext)))
					{
						map.sewVolumes(*it, map.phi_1(*itnext));
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
