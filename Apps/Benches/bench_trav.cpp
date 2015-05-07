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


#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap3.h"
#include "Algo/Tiling/Volume/cubic.h"
#include "Algo/Geometry/area.h"
#include "Algo/Geometry/volume.h"
#include "Utils/chrono.h"


using namespace CGoGN ;

/**
 * Struct that contains some informations about the types of the manipulated objects
 * Mainly here to be used by the algorithms that are parameterized by it
 */
struct PFP: public PFP_STANDARD
{
	// definition of the type of the map
	typedef EmbeddedMap3 MAP;
	typedef double REAL;
	typedef Geom::Vector<3,REAL> VEC3;
};

typedef PFP::MAP MAP;
typedef PFP::MAP::IMPL MAP_IMPL;
typedef PFP::VEC3 VEC3;

int main()
{
	// declare a map to handle the mesh
	MAP myMap;

	Utils::Chrono ch;
	ch.start();
	// add position attribute on vertices and get handler on it
	VertexAttribute<VEC3, MAP> position = myMap.addAttribute<VEC3, VERTEX, MAP>("position");
	const int nb = 100;
	Algo::Volume::Tilings::Cubic::Grid<PFP> cubic(myMap, nb, nb, nb);
	cubic.embedIntoGrid(position, 10.0f, 10.0f, 10.0f);
	std::cout<< "construct grid in " << ch.elapsed()<< " ms"<< std::endl;

	ch.start();
	VEC3 centerMesh(0,0,0);
	int nbVols=0;
	foreach_cell<VOLUME>(myMap, [&](Vol w) // foreach volume
	{
		VEC3 centerVol(0,0,0);
		int nbFaces=0;
		foreach_incident3<FACE>(myMap, w, [&](Face f) // foreach face of each volume
		{
			VEC3 centerFace(0,0,0);
			int nbVert=0;
			foreach_incident3<VERTEX>(myMap, f, [&](Vertex v) // foreach vertex of each face of each volume
			{
				centerFace += position[v];
				nbVert++;
			});
			centerFace /=nbVert;
			centerVol += centerFace;
			nbFaces++;
		});
		centerVol /= nbFaces;
		centerMesh += centerVol;
		nbVols++;
	});
	centerMesh /= nbVols;
	CGoGNout<< "Traverse with foreach in " << ch.elapsed()<< " ms"<< CGoGNendl;


	ch.start();
	centerMesh=VEC3(0,0,0);
	nbVols=0;
	TraversorW<MAP> tw(myMap);	// alias for Traversor<MAP,VERTEX>
	for (Dart dw=tw.begin(); dw!=tw.end(); dw=tw.next())
	{
		VEC3 centerVol(0,0,0);
		int nbFaces=0;
		Traversor3WF<MAP> trwf(myMap, dw);
		for (Dart df = trwf.begin(); df != trwf.end(); df = trwf.next())
		{
			VEC3 centerFace(0,0,0);
			int nbVert=0;
			Traversor3FV<MAP> trfv(myMap, df);
			for (Dart dv = trfv.begin(); dv != trfv.end(); dv = trfv.next())
			{
				centerFace += position[dv];
				nbVert++;
			}
			centerFace /=nbVert;
			centerVol += centerFace;
			nbFaces++;
		}
		centerVol /= nbFaces;
		centerMesh += centerVol;
		nbVols++;
	}
	CGoGNout<< "Traverse with traversor in " << ch.elapsed()<< " ms"<< CGoGNendl;

	ch.start();
	PFP::REAL vol = Algo::Geometry::totalVolume<PFP>(myMap, position);
	CGoGNout<< "Parallel volume:" << ch.elapsed()<< " ms  val="<<vol<< CGoGNendl;

	vol = 0;
	foreach_cell<VOLUME>(myMap, [&](Vol w) // foreach volume
	{
		vol += Algo::Geometry::convexPolyhedronVolume<PFP>(myMap, w, position) ;
	});

	CGoGNout<< "Linear volume:" << ch.elapsed()<< " ms  val="<<vol<< CGoGNendl;


	return 0;
}
