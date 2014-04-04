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

#include <iostream>

#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"

#include "Geometry/vector_gen.h"

#include "Algo/Import/import.h"

#include "Topology/generic/multiAttribs.h"
#include "Algo/Geometry/centroid.h"
#include "Algo/Geometry/normal.h"


#include "Algo/Modelisation/subdivision.h"

using namespace CGoGN ;

/**
 * Struct that contains some informations about the types of the manipulated objects
 * Mainly here to be used by the algorithms that are parameterized by it
 */
struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef EmbeddedMap2 MAP;
};

typedef PFP::MAP MAP;
typedef PFP::MAP::IMPL MAP_IMPL;
typedef PFP::VEC3 VEC3;
typedef PFP::VEC4 VEC4;

/**
 * Simple example of function that work with generic type of attribute
 */
template <typename PFP, typename V_ATT>
typename V_ATT::DATA_TYPE smooth(typename PFP::MAP& map, Dart d, const V_ATT& attributs)
{
	typename V_ATT::DATA_TYPE res(0);
	int count=0;

	if (attributs.getOrbit() == VERTEX)
	{
		Traversor2VVaE<typename PFP::MAP> tra(map,d);
		for (Dart e = tra.begin(); e != tra.end(); e = tra.next())
		{
			res += attributs[d];
			count ++;
		}
		res /= typename PFP::REAL(count);
		return res;
	}
	else if (attributs.getOrbit() == FACE)
		{
			Traversor2FFaV<typename PFP::MAP> tra(map,d);
			for (Dart e = tra.begin(); e != tra.end(); e = tra.next())
			{
				res += attributs[d];
				count ++;
			}
			res /= typename PFP::REAL(count);
			return res;
		}

	return res;
}

template <typename PFP, typename V_ATT>
void applySmooth(typename PFP::MAP& map, const V_ATT& att_in, V_ATT& att_out)
{

	if (att_in.getOrbit() == VERTEX)
	{
		TraversorV<typename PFP::MAP> tra(map);
		for (Dart d = tra.begin(); d != tra.end(); d = tra.next())
		{
			att_out[d] = smooth<PFP>(map,d,att_in);
		}
	}
	else if (att_in.getOrbit() == FACE)
	{
		TraversorF<typename PFP::MAP> tra(map);
		for (Dart d = tra.begin(); d != tra.end(); d = tra.next())
		{
			att_out[d] = smooth<PFP>(map,d,att_in);
		}
	}
	else
		CGoGNerr << "unsupported orbit"<< CGoGNendl;
}

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		CGoGNout << "Usage : " << argv[0] << " filename nbSteps" << CGoGNendl;
		return 0;
	}

	std::string filename(argv[1]);

	unsigned int nbSteps;
	std::istringstream iss(argv[2]);
	iss >> nbSteps;

	// declaration of the map
	MAP myMap;

	std::vector<std::string> attrNames ;
	Algo::Surface::Import::importMesh<PFP>(myMap, argv[1], attrNames);


	// get a handler to the 3D vector attribute created by the import
	VertexAttribute<VEC3, MAP_IMPL> position = myMap.getAttribute<VEC3, VERTEX>(attrNames[0]);
	VertexAttribute<VEC3, MAP_IMPL> pos2 = myMap.addAttribute<VEC3, VERTEX>("pos2");
	VertexAttribute<VEC4, MAP_IMPL> vc = myMap.addAttribute<VEC4, VERTEX>("vertexColor");
	VertexAttribute<VEC4, MAP_IMPL> vc2 = myMap.addAttribute<VEC4, VERTEX>("vertexColor2");

	// classic usage with simple vertex attributes
	applySmooth<PFP>(myMap, position, pos2);

	// multi attributes usage
	Vertex2Attributes<VEC3, VEC4, MAP_IMPL> pv_in(position, vc);
	Vertex2Attributes<VEC3, VEC4, MAP_IMPL> pv_out(pos2, vc2);
	applySmooth<PFP>(myMap, pv_in, pv_out);

	// usage with with a face attribute
	FaceAttribute<VEC4, MAP_IMPL> fc = myMap.addAttribute<VEC4, FACE>("faceColor");
	FaceAttribute<VEC4, MAP_IMPL> fc2 = myMap.addAttribute<VEC4, FACE>("faceColor2");
	applySmooth<PFP>(myMap, fc, fc2);


////	for(unsigned int i = 0; i < nbSteps; ++i)
////		Algo::Surface::Modelisation::LoopSubdivision<PFP>(myMap, position);


////	PFP::VEC3 c = Algo::Surface::Geometry::faceCentroidGen<PFP, VertexAttribute<PFP::VEC3> > (myMap, myMap.begin(),position);

//	T2 cc = Algo::Surface::Geometry::faceCentroid<PFP,T1> (myMap, myMap.begin(), xx);

//	typedef Face2Attributes<PFP::VEC3,PFP::VEC3>  TF;
//	TF yy(fc,fco);
//	Algo::Surface::Geometry::computeCentroidFaces< PFP >(myMap,xx,yy);

////	Algo::Surface::Geometry::computeCentroidFacesGen< PFP,VertexAttribute<PFP::VEC3> , FaceAttribute<PFP::VEC3> >(myMap,position,fc);
//	Algo::Surface::Geometry::computeCentroidFaces<PFP>(myMap,position,fc);
//	FaceAttribute<PFP::VEC3> fc2 = myMap.addAttribute<PFP::VEC3, FACE>("faceCenter2");
//	Algo::Surface::Geometry::computeCentroidFaces<PFP>(myMap,position,fc2);


//	PFP::VEC3 cx = Algo::Surface::Geometry::faceCentroid<PFP>(myMap,myMap.begin(),position);

//	for (unsigned int i=fc.begin(); i!= fc.end(); fc.next(i))
//	{
//		if (fc[i] != fc2[i])
//			std::cout << "PROBLEM" << std::endl;
//	}


//	for(unsigned int i = 0; i < nbSteps; ++i)
//		Algo::Surface::Modelisation::LoopSubdivision<PFP>(myMap, xx) ;

////	Algo::Surface::Modelisation::LoopSubdivision<PFP>(myMap, position);

    return 0;
}
