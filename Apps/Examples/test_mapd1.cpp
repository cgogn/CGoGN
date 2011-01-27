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

#include <iostream>

#include "Topology/map/map2.h"

using namespace CGoGN;

struct PFP {
	typedef DartObj<DP::MAPD1_V0> DART;
	typedef tMap1<DART> MAP;
};

typedef PFP::MAP::Dart Dart;

int main(int, char**) {
	PFP::MAP myMap;

	Dart d1;
	Dart e1;
	Dart f1;

	// One first edge
	d1 = myMap.newEdge();
	PFP::MAP::OrientedFaceIterator face;
	std::cout << "First edge : ";
	for (face.begin(d1); !face.end(); ++face) std::cout << **face << " ";
	std::cout << "(" << face.length() << " traversed darts)" << std::endl;

	// Cut first edge gives two edges
	f1 = myMap.cutEdge(d1);
	std::cout << "Second edge : ";
	for (face.begin(d1); !face.end(); ++face) std::cout << **face << " ";
	std::cout << "(" << face.length() << " traversed darts)" << std::endl;

	// New face with 10 edges
	d1 = myMap.newFace(10);
	std::cout << "First face : ";
	for (face.begin(d1); !face.end(); ++face) std::cout << **face << " ";
	std::cout << "(" << face.length() << " traversed darts)" << std::endl;

	// Delete two edges
	myMap.deleteEdgeAfter(d1);
	e1 = myMap.phi1(d1);
	myMap.deleteEdge(e1);
	std::cout << "Face with 2 less edges : ";
	for (face.begin(d1); !face.end(); ++face) std::cout << **face << " ";
	std::cout << "(" << face.length() << " traversed darts)" << std::endl;

	// Delete face
	myMap.deleteOrientedFace(d1);
	
	// New face with 10 edges
	d1 = myMap.newFace(10);
	std::cout << "Second face : ";
	for (face.begin(d1); !face.end(); ++face) std::cout << **face << " ";
	std::cout << "(" << face.length() << " traversed darts)" << std::endl;

	FunctorCount fcount1;
	myMap.foreach_dart_of_face(d1,fcount1);
	std::cout << "nbEdges = " << fcount1.getNb() << std::endl;
	
	// Search for the middle of the face and split face
	e1 = d1;
	for (unsigned i=0; i<fcount1.getNb()/2; ++i) e1 = myMap.phi1(e1);
	myMap.splitFace(d1,e1);

	std::cout << "nbEdges d1 = " << myMap.faceDegree(d1) << std::endl;
	std::cout << "nbEdges e1 = " << myMap.faceDegree(e1) << std::endl;

	std::cout << "First splitted face : ";
	for (face.begin(d1); !face.end(); ++face) std::cout << **face << " ";
	std::cout << "(" << face.length() << " traversed darts)" << std::endl;

	std::cout << "Second splitted face : ";
	for (face.begin(e1); !face.end(); ++face) std::cout << **face << " ";
	std::cout << "(" << face.length() << " traversed darts)" << std::endl;

	// Merge splitted faces
	myMap.mergeFaces(d1,e1);
	std::cout << "nbEdges d1+e1 = " << myMap.faceDegree(d1) << std::endl;

	// Reverse linking in the face
	myMap.reverseFace(d1);
	if (myMap.sameFace(d1,e1)) std::cout << "ok" << std::endl;
	
	// Test of markers
	Marker m1 = myMap.getNewMarker();
	myMap.markFace(d1,m1);
	myMap.saveMap("test.map",true);
	myMap.saveMapBin("test.bmap");
	myMap.unmarkFace(d1,m1);

	// Free memory
	myMap.deleteOrientedFace(d1);
	myMap.deleteOrientedFace(f1);
	
	// Release Markers
	myMap.releaseMarker(m1);
	
	// Load the saved map
	myMap.loadMap("test.map",true);
	std::cout << "MarkerSet = " << myMap.getMarkerVal() << std::endl;
	std::cout << "Boundary Marker = " << myMap.m_BoundaryMarkerVal.getMarkerVal() << std::endl;
	for (Dart d = myMap.begin(); d != myMap.end(); ++d) {
		std::cout << "[" << d->getLabel() << "] " << *d << " (" << d->getMarkerVal() << ")" << std::endl;
	}

	// Load a second map (append the load darts)
	myMap.loadMapBin("test.bmap");
	std::cout << "MarkerSet = " << myMap.getMarkerVal() << std::endl;
	std::cout << "Boundary Marker = " << myMap.m_BoundaryMarkerVal.getMarkerVal() << std::endl;
	for (Dart d = myMap.begin(); d != myMap.end(); ++d) {
		std::cout << "[" << d->getLabel() << "] " << *d << " (" << d->getMarkerVal() << ")" << std::endl;
	}

	myMap.unmarkAll(m1);
	myMap.releaseMarker(m1);
	while (myMap.begin() != myMap.end()) {
		myMap.deleteOrientedFace(myMap.begin());
	};
	
	return 0;
}
