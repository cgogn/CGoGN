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
#include "Topology/map/embeddedMap3.h"

#include "Geometry/vector_gen.h"

#include "Algo/Import/import.h"
#include "Algo/Export/export.h"
#include <typeinfo>

#include "Algo/Modelisation/subdivision3.h"

using namespace CGoGN ;

/**
 * Struct that contains some informations about the types of the manipulated objects
 * Mainly here to be used by the algorithms that are parameterized by it
 */
struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef EmbeddedMap3 MAP;
};

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		CGoGNout << "Usage : " << argv[0] << " filename" << CGoGNendl;
		return 0;
	}

	std::string filename(argv[1]);

	size_t pos = filename.rfind(".");    // position of "." in filename
	std::string extension = filename.substr(pos);

	// declaration of the map
	PFP::MAP myMap;

	std::vector<std::string> attrNames ;

	if (extension == std::string(".map"))
		myMap.loadMapBin(filename);
	else
		Algo::Volume::Import::importMesh<PFP>(myMap, filename, attrNames);

	// get a handler to the 3D vector attribute created by the import
	VertexAttribute<PFP::VEC3> position = myMap.getAttribute<PFP::VEC3, VERTEX>(attrNames[0]);

	// Les faces vont devenir des aretes -> echange de FACE ET EDGE

	VolumeAttribute<PFP::VEC3> positionV = myMap.getAttribute<PFP::VEC3, VOLUME>("position") ;
	if(!positionV.isValid())
		positionV = myMap.addAttribute<PFP::VEC3, VOLUME>("position") ;

	Algo::Volume::Geometry::computeCentroidVolumes<PFP>(myMap, position, positionV) ;

	Dart dsave = NIL;
	for(Dart d = myMap.begin() ; d != myMap.end() ; myMap.next(d))
	{
		if(myMap.isBoundaryMarked3(d))
		{
			dsave = d;
			break;
		}
	}


	Dart dcenter = myMap.explodBorderTopo(dsave);

	DartMarker mf(myMap);
	for(Dart dit = myMap.begin() ; dit != myMap.end() ; myMap.next(dit))
	{
		if(myMap.isBoundaryMarked3(dit) && !mf.isMarked(dit))
		{
			mf.markOrbit<FACE>(dit);
			positionV[dit] = Algo::Surface::Geometry::faceCentroid<PFP>(myMap,dit,position);
		}
	}

	for(Dart dit = myMap.begin() ; dit != myMap.end() ; myMap.next(dit))
	{
		if(myMap.isBoundaryMarked3(dit))
		{
			myMap.fillHole(dit);
		}
	}

	//
	//Compute Dual Test -- begin
	//
	DartAttribute<Dart> old_phi1 = myMap.getAttribute<Dart, DART>("phi1") ;
	DartAttribute<Dart> old_phi_1 = myMap.getAttribute<Dart, DART>("phi_1") ;
	DartAttribute<Dart> new_phi1 = myMap.addAttribute<Dart, DART>("new_phi1") ;
	DartAttribute<Dart> new_phi_1 = myMap.addAttribute<Dart, DART>("new_phi_1") ;

	DartAttribute<Dart> old_phi2 = myMap.getAttribute<Dart, DART>("phi2") ;
	DartAttribute<Dart> new_phi2 = myMap.addAttribute<Dart, DART>("new_phi2") ;

	for(Dart d = myMap.begin(); d != myMap.end(); myMap.next(d))
	{
		Dart dd = myMap.phi2(myMap.phi3(d)) ;
		new_phi1[d] = dd ;
		new_phi_1[dd] = d ;

		Dart ddd = myMap.phi1(myMap.phi3(d));
		new_phi2[d] = ddd;
		new_phi2[ddd] = d;
	}

	myMap.swapAttributes<Dart>(old_phi1, new_phi1) ;
	myMap.swapAttributes<Dart>(old_phi_1, new_phi_1) ;
	myMap.swapAttributes<Dart>(old_phi2, new_phi2) ;

	myMap.removeAttribute(new_phi1) ;
	myMap.removeAttribute(new_phi_1) ;
	myMap.removeAttribute(new_phi2) ;

	myMap.swapEmbeddingContainers(VERTEX, VOLUME) ;

	//
	//ComputeDualTest -- end
	//

	myMap.createHole(dcenter);

	VolumeAttribute<PFP::VEC3> del;
	del = position;
	position = positionV ;

	myMap.removeAttribute(del);

	myMap.dumpAttributesAndMarkers();

	myMap.saveMapBin("result.map");
	std::cout << "Exported" << std::endl;

	return 0;
}
