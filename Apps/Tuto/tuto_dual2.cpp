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
#include "Algo/Export/export.h"
#include <typeinfo>

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

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		CGoGNout << "Usage : " << argv[0] << " filename" << CGoGNendl;
		return 0;
	}

	std::string filename(argv[1]);

	// declaration of the map
	PFP::MAP myMap;

	std::vector<std::string> attrNames ;
	Algo::Surface::Import::importMesh<PFP>(myMap, argv[1], attrNames);

	// get a handler to the 3D vector attribute created by the import
	VertexAttribute<PFP::VEC3> position = myMap.getAttribute<PFP::VEC3, VERTEX>(attrNames[0]);


	//
	// Dual
	//
	//	FaceAttribute<PFP::VEC3> positionF = myMap.getAttribute<PFP::VEC3, FACE>("position") ;
	//	if(!positionF.isValid())
	//		positionF = myMap.addAttribute<PFP::VEC3, FACE>("position") ;
	//
	//	Algo::Surface::Geometry::computeCentroidFaces<PFP>(myMap, position, positionF) ;
	//	myMap.computeDual();
	//	position = positionF ;


//	AttributeHandler<PFP::VEC3, PFP::MAP::EDGE_OF_PARENT> positionE = myMap.getAttribute<PFP::VEC3, PFP::MAP::EDGE_OF_PARENT>("position") ;
//	if(!positionE.isValid())
//		positionE = myMap.addAttribute<PFP::VEC3, PFP::MAP::EDGE_OF_PARENT>("position") ;
//
//	SelectorDartBoundary<PFP::MAP> sdb(myMap);
//	TraversorE<PFP::MAP> te(myMap, sdb);
//	for(Dart dit = te.begin() ; dit != te.end() ; dit = te.next())
//	{
//		positionE[dit] = (position[dit] + position[myMap.phi1(dit)]) * PFP::REAL(0.5);
//	}
//
//	std::cout << "boundary edges centroids : ok" << std::endl;
//
//	//triangule old boundary faces
//	std::vector<Dart> oldb;
//
//	std::cout << "nb darts : " << myMap.getNbDarts() << std::endl;
//
//	CellMarker<FACE> cmf(myMap);
//	for(Dart d = myMap.begin(); d != myMap.end(); myMap.next(d))
//	{
//		if(!cmf.isMarked(d) && myMap.isBoundaryMarked2(d))
//		{
//			oldb.push_back(d);
//			cmf.mark(d);
//			std::cout << "d = " << d << std::endl;
//		}
//	}
//
//	for(std::vector<Dart>::iterator it = oldb.begin() ; it != oldb.end() ; ++it)
//	{
//		Dart db = *it;
//		Dart d1 = myMap.phi1(db);
//		myMap.splitFace(db, d1) ;
//		myMap.cutEdge(myMap.phi_1(db)) ;
//		Dart x = myMap.phi2(myMap.phi_1(db)) ;
//		Dart dd = myMap.phi1(myMap.phi1(myMap.phi1(x)));
//		while(dd != x)
//		{
//			Dart next = myMap.phi1(dd) ;
//			myMap.splitFace(dd, myMap.phi1(x)) ;
//			dd = next ;
//		}
//
//	}
//
//	std::cout << "boundary face triangulation : ok" << std::endl;
//
//	myMap.swapEmbeddingContainers(FACE, PFP::MAP::EDGE_OF_PARENT) ;
//
//	std::cout << "swap containers : ok" << std::endl;
//
//	FaceAttribute<PFP::VEC3> positionF;
//	positionF = positionE;
//
//	Algo::Surface::Geometry::computeCentroidFaces<PFP>(myMap, position, positionF) ;
//
//	for(std::vector<Dart>::iterator it = oldb.begin() ; it != oldb.end() ; ++it)
//	{
//		myMap.fillHole(*it);
//	}
//
//	std::cout << "fillHole : ok" << std::endl;
//
//	myMap.computeDual();
//
//	//myMap.closeMap();
//
//	position = positionF ;
//
//	myMap.check();

	Algo::Surface::Export::exportOFF<PFP>(myMap, position, "result.off");
	std::cout << "Exported" << std::endl;

	return 0;
}
