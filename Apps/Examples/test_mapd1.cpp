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
	CGoGNout << "First edge : ";
	for (face.begin(d1); !face.end(); ++face) CGoGNout << **face << " ";
	CGoGNout << "(" << face.length() << " traversed darts)" << CGoGNendl;

	// Cut first edge gives two edges
	f1 = myMap.cutEdge(d1);
	CGoGNout << "Second edge : ";
	for (face.begin(d1); !face.end(); ++face) CGoGNout << **face << " ";
	CGoGNout << "(" << face.length() << " traversed darts)" << CGoGNendl;

	// New face with 10 edges
	d1 = myMap.newFace(10);
	CGoGNout << "First face : ";
	for (face.begin(d1); !face.end(); ++face) CGoGNout << **face << " ";
	CGoGNout << "(" << face.length() << " traversed darts)" << CGoGNendl;

	// Delete two edges
	myMap.deleteEdgeAfter(d1);
	e1 = myMap.phi1(d1);
	myMap.deleteEdge(e1);
	CGoGNout << "Face with 2 less edges : ";
	for (face.begin(d1); !face.end(); ++face) CGoGNout << **face << " ";
	CGoGNout << "(" << face.length() << " traversed darts)" << CGoGNendl;

	// Delete face
	myMap.deleteOrientedFace(d1);
	
	// New face with 10 edges
	d1 = myMap.newFace(10);
	CGoGNout << "Second face : ";
	for (face.begin(d1); !face.end(); ++face) CGoGNout << **face << " ";
	CGoGNout << "(" << face.length() << " traversed darts)" << CGoGNendl;

	FunctorCount fcount1;
	myMap.foreach_dart_of_face(d1,fcount1);
	CGoGNout << "nbEdges = " << fcount1.getNb() << CGoGNendl;
	
	// Search for the middle of the face and split face
	e1 = d1;
	for (unsigned i=0; i<fcount1.getNb()/2; ++i) e1 = myMap.phi1(e1);
	myMap.splitFace(d1,e1);

	CGoGNout << "nbEdges d1 = " << myMap.faceDegree(d1) << CGoGNendl;
	CGoGNout << "nbEdges e1 = " << myMap.faceDegree(e1) << CGoGNendl;

	CGoGNout << "First splitted face : ";
	for (face.begin(d1); !face.end(); ++face) CGoGNout << **face << " ";
	CGoGNout << "(" << face.length() << " traversed darts)" << CGoGNendl;

	CGoGNout << "Second splitted face : ";
	for (face.begin(e1); !face.end(); ++face) CGoGNout << **face << " ";
	CGoGNout << "(" << face.length() << " traversed darts)" << CGoGNendl;

	// Merge splitted faces
	myMap.mergeFaces(d1,e1);
	CGoGNout << "nbEdges d1+e1 = " << myMap.faceDegree(d1) << CGoGNendl;

	// Reverse linking in the face
	myMap.reverseFace(d1);
	if (myMap.sameFace(d1,e1)) CGoGNout << "ok" << CGoGNendl;
	
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
	CGoGNout << "MarkerSet = " << myMap.getMarkerVal() << CGoGNendl;
	CGoGNout << "Boundary Marker = " << myMap.m_BoundaryMarkerVal.getMarkerVal() << CGoGNendl;
	for (Dart d = myMap.begin(); d != myMap.end(); ++d) {
		CGoGNout << "[" << d->getLabel() << "] " << *d << " (" << d->getMarkerVal() << ")" << CGoGNendl;
	}

	// Load a second map (append the load darts)
	myMap.loadMapBin("test.bmap");
	CGoGNout << "MarkerSet = " << myMap.getMarkerVal() << CGoGNendl;
	CGoGNout << "Boundary Marker = " << myMap.m_BoundaryMarkerVal.getMarkerVal() << CGoGNendl;
	for (Dart d = myMap.begin(); d != myMap.end(); ++d) {
		CGoGNout << "[" << d->getLabel() << "] " << *d << " (" << d->getMarkerVal() << ")" << CGoGNendl;
	}

	myMap.unmarkAll(m1);
	myMap.releaseMarker(m1);
	while (myMap.begin() != myMap.end()) {
		myMap.deleteOrientedFace(myMap.begin());
	};
	
	return 0;
}
