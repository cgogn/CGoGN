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
#include "Algo/Modelisation/primitives3d.h"
#include <vector>
#include "Geometry/bounding_box.h"
#include <set>

namespace CGoGN
{
namespace Algo
{
namespace Import 
{

template <typename PFP>
typename PFP::VEC3 stringToEmb(std::string s)
{
	if(s[0] == '(')
		s.erase(0,2);
	if(s[s.size()-1] == ')')
		s.erase(s.size()-2,2);

	std::stringstream oss(s);

	int x,y,z;
	oss >> x;
	oss >> y;
	oss >> z;

	typename PFP::VEC3 coord(x,y,z);

	//CGoGNout << coord << CGoGNendl;

	return coord;
}

inline unsigned int gcd(unsigned int a, unsigned int b)
{

	while ( true )
	{
		if ( a == 0 )
			return b;
		b %= a;

		if ( b == 0 )
			return a;
		a %= b;
	}
}

template <typename PFP>
bool importInESSSurfacique(typename PFP::MAP& map, const std::string& filename, std::vector<std::string>& attrNames, std::vector<std::pair<typename PFP::VEC3, typename  PFP::VEC3> > &influences)
{
	typedef typename PFP::VEC3 VEC3;

	AttributeHandler<VEC3> position = map.template addAttribute<VEC3>(VERTEX, "position") ;
	attrNames.push_back(position.name()) ;

	AttributeHandler<int> objects = map.template addAttribute<int>(FACE, "objects") ;
	attrNames.push_back(objects.name()) ;


	//stocker les boundingbox contenu dans le fichier
	std::vector<std::pair<VEC3,VEC3> > coordonnees; //tableau de paires (x,y)
	coordonnees.reserve(50);

	//classer les coordonnees en x
	std::multimap<int, unsigned int>  coordonneesX;
	//classer les coordonnees en y
	std::multimap<int, unsigned int>  coordonneesY;

	std::set<int> sx;
	std::set<int> sy;

	// open file
	std::ifstream fp(filename.c_str(), std::ios::in);
	if (!fp.good())
	{
		CGoGNerr << "Unable to open file " << filename << CGoGNendl;
		return false;
	}


	//
	// Lecture du fichier
	//

	std::string line;
	size_t posData, posCoord;
	std::string bg;

	unsigned int size = 0;

	//two columns cutted
	std::getline(fp, line);
	posData = line.find("\t");

	// First column
	//Bounding box : first coord & second coord
	bg = line.substr(0, posData);
	posCoord = bg.find(") (");
	VEC3 c1 = stringToEmb<PFP>(bg.substr(0, posCoord));
	VEC3 c2 = stringToEmb<PFP>(bg.substr(posCoord+3));
	std::cout << std::endl;

	//coordonnees.push_back(std::pair<VEC3,VEC3>(c1,c2));
//	coordonneesX.insert(std::pair<int, unsigned int >(c1[0],size));
//	coordonneesY.insert(std::pair<int, unsigned int >(c1[1],size));
	sx.insert(c1[0]);
	sx.insert(c2[0]);

	sy.insert(c1[1]);
	sy.insert(c2[1]);

	// Second Column (nil)
	bg = line.substr(posData);
	//std::cout << bg << std::endl;

	size++;

	while ( std::getline( fp, line ) )
	{
		posData = line.find("\t");

		// First column
		//Bounding box : first coord & second coord
		bg = line.substr(0, posData);
		posCoord = bg.find(") (");

		VEC3 c1 = stringToEmb<PFP>(bg.substr(0, posCoord));
		VEC3 c2 = stringToEmb<PFP>(bg.substr(posCoord+3));
		//std::cout << std::endl;

		coordonnees.push_back(std::pair<VEC3,VEC3>(c1,c2));

		sx.insert(c1[0]);
		sx.insert(c2[0]);

		sy.insert(c1[1]);
		sy.insert(c2[1]);


		//Second Column
		bg = line.substr(posData);
		posCoord = bg.find(") (");
		//std::cout << bg << std::endl;
		c1 = stringToEmb<PFP>(bg.substr(1, posCoord+1));
		//CGoGNout << c1 << CGoGNendl;
		c2 = stringToEmb<PFP>(bg.substr(posCoord+3));
		//CGoGNout << c2 << CGoGNendl;
		influences.push_back(std::pair<VEC3,VEC3>(c1,c2));



		size++;
	}

	fp.close();


	//
	// Creation de la grille
	//
	Algo::Modelisation::Polyhedron<PFP> prim(map,position);

	std::set<int>::iterator sity = sy.begin();

	int cx = sx.size();
	int cy = sy.size();
	prim.grid_topo(cx-1,cy-1);

	//tableau des brins (un par sommet)
	std::vector<Dart> tableVertDarts = prim.getVertexDarts();


	for(int i=0; i<cy; ++i)
	{
		//std::cout << "cood y =" << *sity << std::endl;
		std::set<int>::iterator sitx = sx.begin();

		for(int j=0; j<cx;++j)
		{
			Dart d = tableVertDarts[i*(cx)+j];
			position[d] = VEC3(*sitx, *sity, 0);

			objects[d] = 0;

			++sitx;
		}
		++sity;
	}

	sity = sy.begin();

	//
	// Coloriage
	//
	for(int i=0; i<cy; ++i)
	{
		//std::cout << "cood y =" << *sity << std::endl;
		std::set<int>::iterator sitx = sx.begin();

		for(int j=0; j<cx;++j)
		{
			Dart d = tableVertDarts[i*(cx)+j];

			if(objects[d] == 0)
			{

				//Calcul du centre de ce quad
				VEC3 c = Algo::Geometry::faceCentroid<PFP>(map, d, position);

				//std::cout << "center=" << c << std::endl;

				int i = 1;
				//Comparaison avec toutes les coordonnees si le point se trouve dedans
				for(typename std::vector<std::pair<VEC3,VEC3> >::iterator it  = coordonnees.begin(); it != coordonnees.end() ; ++it)
				{
					//comparaison si la coord du centre se situe dans l'intervalle de la boite anglobante
					//std::cout << "c1=" << (*it).first << " c2=" << (*it).second  << std::endl;

					if((*it).first[0] <= c[0] && c[0] < (*it).second[0] &&
							(*it).first[1] <= c[1] && c[1] < (*it).second[1])
					{
						objects[d] = i ;
					}
					++i;
				}

			}
			++sitx;
		}
		++sity;
	}

	std::cout << "coord size : " << coordonnees.size() << std::endl;


	return true;
}

template <typename PFP>
bool importInESSVolumique(typename PFP::MAP& map, const std::string& filename, std::vector<std::string>& attrNames)
{
	typedef typename PFP::VEC3 VEC3;

	AttributeHandler<VEC3> position = map.template addAttribute<VEC3>(VERTEX, "position") ;
	attrNames.push_back(position.name()) ;

	AttributeHandler<int> objects = map.template addAttribute<int>(FACE, "objects") ;
	attrNames.push_back(objects.name()) ;


	//stocker les boundingbox contenu dans le fichier
	std::vector<std::pair<VEC3,VEC3> > coordonnees; //tableau de paires (x,y)
	coordonnees.reserve(50);

	//classer les coordonnees en x
	std::multimap<int, unsigned int>  coordonneesX;
	//classer les coordonnees en y
	std::multimap<int, unsigned int>  coordonneesY;

	std::set<int> sx;
	std::set<int> sy;

	// open file
	std::ifstream fp(filename.c_str(), std::ios::in);
	if (!fp.good())
	{
		CGoGNerr << "Unable to open file " << filename << CGoGNendl;
		return false;
	}


	//
	// Lecture du fichier
	//

	std::string line;
	size_t posData, posCoord;
	std::string bg;

	unsigned int size = 0;

	//two columns cutted
	std::getline(fp, line);
	posData = line.find("\t");

	// First column
	//Bounding box : first coord & second coord
	bg = line.substr(0, posData);
	posCoord = bg.find(") (");
	VEC3 c1 = stringToEmb<PFP>(bg.substr(0, posCoord));
	VEC3 c2 = stringToEmb<PFP>(bg.substr(posCoord+3));
	std::cout << std::endl;

	float x = c1[0];
	float y = c1[1];
	float z = c1[2];

	//coordonnees.push_back(std::pair<VEC3,VEC3>(c1,c2));
//	coordonneesX.insert(std::pair<int, unsigned int >(c1[0],size));
//	coordonneesY.insert(std::pair<int, unsigned int >(c1[1],size));
	sx.insert(c1[0]);
	sx.insert(c2[0]);

	sy.insert(c1[1]);
	sy.insert(c2[1]);
	size++;

	while ( std::getline( fp, line ) )
	{
		posData = line.find("\t");

		// First column
		//Bounding box : first coord & second coord
		bg = line.substr(0, posData);
		posCoord = bg.find(") (");

		VEC3 c1 = stringToEmb<PFP>(bg.substr(0, posCoord));
		VEC3 c2 = stringToEmb<PFP>(bg.substr(posCoord+3));
		std::cout << std::endl;

		coordonnees.push_back(std::pair<VEC3,VEC3>(c1,c2));
//		coordonneesX.insert(std::pair<int, unsigned int >(c1[0],size));
//		coordonneesY.insert(std::pair<int, unsigned int >(c1[1],size));

		sx.insert(c1[0]);
		sx.insert(c2[0]);

		sy.insert(c1[1]);
		sy.insert(c2[1]);

		size++;
	}

	fp.close();


//	std::cout << "setx" << std::endl;
//	 for (std::set<int>::iterator it = sx.begin() ; it != sx.end(); ++it)
//	    std::cout << " " << *it;
//	 std::cout << std::endl;
//
//	 std::cout << "sety" << std::endl;
//	 for (std::set<int>::iterator it = sy.begin() ; it != sy.end(); ++it)
//	 	    std::cout << " " << *it;
//	 std::cout << std::endl;

	//
	// Creation de la grille
	//
	Algo::Modelisation::Primitive3D<PFP> prim(map, position);

	std::set<int>::iterator sity = sy.begin();

	int cx = sx.size();
	int cy = sy.size();
	prim.hexaGrid_topo(cx-1,cy-1, cx);

	std::vector<Dart> tableVertDarts = prim.getVertexDarts();


	prim.embedHexaGrid(x,y,z);

//	std::pair<std::multimap<int, unsigned int>::iterator,std::multimap<int, unsigned int>::iterator > retX;
//	int nbs = (cx+1)*(cy+1);
//
//	for(int i=0; i<= 1; ++i) //z
//	{
//		for(int j=0; j<= cy; ++j) //y
//		{
//			std::set<int>::iterator sitx = sx.begin();
//
//			for(int k=0; k<=cx;++k)	//x
//			{
//				Dart d = tableVertDarts[ i*nbs+j*(cx+1)+k ];
//
//				map.embedNewCell(VERTEX,d);
//
//				position[d] = VEC3(float(k), float(j), float(i));
//
////				if(i == 0)
////					position[d] = VEC3(*sitx, *sity, 0);
////				else
////					position[d] = VEC3(*sitx,*sity, 0);
//
//				++sitx;
//			}
//
//			++sity;
//		}
//	}

//	for(int i=0; i<cy; ++i)
//	{
//		//std::cout << "cood y =" << *sity << std::endl;
//		std::set<int>::iterator sitx = sx.begin();
//
//		for(int j=0; j<cx;++j)
//		{
//			Dart d = tableVertDarts[i*(cx)+j];
//
//			position[d] = VEC3(*sitx, *sity, 0);
//
//			position[map.phi1(map.phi1(map.phi2(d)))] = VEC3(*sitx, *sity, 14000);
//
//
//			objects[d] = -1;
//
//			++sitx;
//		}
//		++sity;
//	}

	sity = sy.begin();

	//
	// Coloriage
	//

//	for(int i=0; i<cy; ++i)
//	{
//		//std::cout << "cood y =" << *sity << std::endl;
//		std::set<int>::iterator sitx = sx.begin();
//
//		for(int j=0; j<cx;++j)
//		{
//			Dart d = tableVertDarts[i*(cx)+j];
//
//			//Calcul du centre de ce quad
//			VEC3 c = Algo::Geometry::faceCentroid<PFP>(map, d, position);
//
//			//std::cout << "center=" << c << std::endl;
//			int place = 0;
//
//			//Comparaison avec toutes les coordonnees si le point se trouve dedans
//			for(typename std::vector<std::pair<VEC3,VEC3> >::iterator it  = coordonnees.begin(); it != coordonnees.end() ; ++it)
//			{
//				//comparaison si la coord du centre se situe dans l'intervalle de la boite anglobante
//				//std::cout << "c1=" << (*it).first << " c2=" << (*it).second  << std::endl;
//
//				if((*it).first[0] < c[0] && c[0] < (*it).second[0] && (*it).first[1] < c[1] && c[1] < (*it).second[1])
//				{
//					objects[d] = place;
//					//std::cout << "i=" << place << std::endl;
//					std::cout << "place=" << place << std::endl;
//				}
//				++place;
//			}
//
//			++sitx;
//		}
//		++sity;
//	}

	return true;
}

} // namespace Import

} // namespace Algo

} // namespace CGoGN

/*
	unsigned int diffX=std::numeric_limits<unsigned int>::max();

	unsigned int highestX = coordonneesX.rbegin()->first;
	unsigned int lowestX = coordonneesX.begin()->first;

	unsigned int id = (*(coordonneesX.begin())).first;

	//calcul de la min diff en x et min diff en y
	for(typename std::multimap<unsigned int, unsigned int>::iterator it = coordonneesX.begin() ; it != coordonneesX.end() ; ++it)
	{
		std::cout << "x=" << (*it).first << " VEC3(a) =" << coordonnees[(*it).second].first << " VEC3(b) =" << coordonnees[(*it).second].second << std::endl;
		if((*it).first != id)
		{
		//typename std::multimap<unsigned int, unsigned int>::iterator itup = coordonneesX.upper_bound(id);
		//unsigned int idup = (*itup).first;

		//if(id != highestX)
		//{
			//diffX = std::min(idup - id, diffX);
			diffX = std::min(gcd((*it).first,id), diffX);
			//std::cout << idup << " - " <<  id << " = " << (idup- id) << std::endl;
			std::cout << (*it).first << "pgcd" <<  id << " = " << gcd((*it).first, id) << std::endl;
			std::cout << "diffX = " << diffX << std::endl;
		//}

			id = (*it).first;
		}
	}
	std::cout << "diffX=" << diffX << std::endl;


	//
	//
	//
	unsigned int diffY=std::numeric_limits<unsigned int>::max();

	unsigned int highestY = coordonneesY.rbegin()->first;
	unsigned int lowestY = coordonneesY.begin()->first;

	id = (*(coordonneesY.begin())).first;
	//calcul de la min diff en x et min diff en y
	for(typename std::multimap<unsigned int, unsigned int>::iterator it = coordonneesY.begin() ; it != coordonneesY.end() ; ++it)
	{
		std::cout << "y=" << (*it).first << " VEC3(a) =" << coordonnees[(*it).second].first << " VEC3(b) =" << coordonnees[(*it).second].second << std::endl;

		if((*it).first != id)
		{
			//typename std::multimap<unsigned int, unsigned int>::iterator itup = coordonneesY.upper_bound(id);
			//unsigned int idup = (*itup).first;

			//if(id != highestY)
			//{
				//diffY = std::min(idup - id, diffY);
				diffY = std::min(gcd((*it).first,id), diffY);
				//std::cout << idup << " - " <<  id << " = " << (idup- id) << std::endl;
				std::cout << (*it).first << " pgcd " <<  id << " = " << gcd((*it).first, id) << std::endl;
				std::cout << "diffY = " << diffY << std::endl;
			//}

			id = (*it).first;
		}
	}
	std::cout << "diffY=" << diffY << std::endl;
*/


