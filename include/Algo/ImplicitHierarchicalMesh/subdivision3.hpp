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

#include "Algo/Geometry/centroid.h"
#include "Algo/Modelisation/subdivision.h"

namespace CGoGN
{

namespace Algo
{

namespace IHM3
{

//OK
template <typename PFP>
void subdivideEdge(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position)
{
	assert(map.getDartLevel(d) <= map.getCurrentLevel() || !"Access to a dart introduced after current level") ;
	assert(!map.edgeIsSubdivided(d) || !"Trying to subdivide an already subdivided edge") ;

	unsigned int eLevel = map.edgeLevel(d) ;

	unsigned int cur = map.getCurrentLevel() ;
	map.setCurrentLevel(eLevel) ;

	CGoGNout << "eLevel=" << eLevel << CGoGNendl;

	Dart dd = map.phi2(d) ;
	typename PFP::VEC3 p1 = position[d] ;
	typename PFP::VEC3 p2 = position[map.phi1(d)] ;

	map.setCurrentLevel(eLevel + 1) ;

	map.cutEdge(d) ;
	unsigned int eId = map.getEdgeId(d) ;
	map.setEdgeId(map.phi1(d), eId, EDGE_ORBIT) ; //sur chaque moitie d'arete
	map.setEdgeId(map.phi1(dd), eId, EDGE_ORBIT) ; //TODO peut etre a optimiser

	unsigned int fId = map.getFaceId(d);
	map.setFaceId(map.phi1(d), fId, EDGE_ORBIT) ;
	map.setFaceId(map.phi1(dd), fId, EDGE_ORBIT) ;

	position[map.phi1(d)] = (p1 + p2) * typename PFP::REAL(0.5) ;

	map.setCurrentLevel(cur) ;
}

//OK
template <typename PFP>
Dart subdivideFace(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position)
{
	assert(map.getDartLevel(d) <= map.getCurrentLevel() || !"Access to a dart introduced after current level") ;
	assert(!map.faceIsSubdivided(d) || !"Trying to subdivide an already subdivided face") ;

	unsigned int fLevel = map.faceLevel(d) ;
	Dart old = map.faceOldestDart(d) ;

	unsigned int cur = map.getCurrentLevel() ;
	map.setCurrentLevel(fLevel) ;		// go to the level of the face to subdivide its edges

	CGoGNout << "fLevel=" << fLevel << CGoGNendl;

	unsigned int degree = 0 ;
	typename PFP::VEC3 p ;
	Dart it = old ;
	do
	{
		++degree;
		p += position[it] ;
		if(!map.edgeIsSubdivided(it))							// first cut the edges (if they are not already)
			Algo::IHM3::subdivideEdge<PFP>(map, it, position) ;	// and compute the degree of the face
		it = map.phi1(it) ;
	} while(it != old) ;
	p /= typename PFP::REAL(degree) ;

	map.setCurrentLevel(fLevel + 1) ;			// go to the next level to perform face subdivision

	Dart dd = map.phi1(old) ;
	map.splitFace(dd, map.phi1(map.phi1(dd))) ;
	unsigned int idface = map.getFaceId(d);
	map.setFaceId(map.phi1(map.phi1(dd)), idface, FACE_ORBIT) ;
	map.setFaceId(map.phi2(map.phi1(map.phi1(dd))), idface, FACE_ORBIT) ;

	Dart ne = map.phi2(map.phi_1(dd));
	Dart ne2 = map.phi2(ne);

	map.cutEdge(ne) ;
	unsigned int id = map.getNewEdgeId() ;
	map.setEdgeId(ne, id, EDGE_ORBIT) ;
	id = map.getNewEdgeId() ;
	map.setEdgeId(ne2, id, EDGE_ORBIT) ;

	Dart res = map.phi2(ne);

	dd = map.phi1(map.phi1(map.phi1(map.phi1(ne)))) ;
	while(dd != ne)
	{
		Dart next = map.phi1(map.phi1(dd)) ;
		map.splitFace(map.phi1(ne), dd) ;
		Dart nne = map.phi2(map.phi_1(dd)) ;

		id = map.getNewEdgeId() ;
		map.setEdgeId(nne, id, EDGE_ORBIT) ;

		map.setFaceId(nne, idface, FACE_ORBIT) ;
		dd = next ;
	}

	position[res] = p ;


	map.setCurrentLevel(cur) ;

	return res;
}

template <typename PFP>
void subdivideVolume(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position)
{
	assert(map.getDartLevel(d) <= map.getCurrentLevel() || !"Access to a dart introduced after current level") ;
	assert(!map.volumeIsSubdivided(d) || !"Trying to subdivide an already subdivided volume") ;

	unsigned int vLevel = map.volumeLevel(d) ;
	Dart old = map.volumeOldestDart(d) ;


	unsigned int cur = map.getCurrentLevel() ;
	map.setCurrentLevel(vLevel) ;		// go to the level of the face to subdivide its edges

	// compute volume centroid
	typename PFP::VEC3 volCenter;// = Algo::Geometry::volumeCentroidGen<PFP, typename PFP::TVEC3, typename PFP::VEC3>(map, d, position);


	CGoGNout << "vLevel=" << vLevel << CGoGNendl;

	/*
	 * ou niveau du volume courant i
	 * stockage d'un brin de chaque face de celui-ci
	 */

	DartMarkerStore mf(map);		// Lock a face marker
	DartMarkerStore mv(map);		// Lock a vertex marker

	//Store faces that are traversed and start with the face of d
	std::vector<Dart> visitedFaces;
	visitedFaces.reserve(20);
	visitedFaces.push_back(old);

	//Store the edges before the cutEdge
	std::vector<Dart> oldEdges;
	oldEdges.reserve(20);

	//Store the darts from quadrangulated faces
	std::vector<Dart> quadfaces;
	quadfaces.reserve(100);


	mf.markOrbit(FACE_ORBIT, old) ;
	for(std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;

		do	// add all face neighbours to the table
		{
			//if(!mv.isMarked(e))
			//{
				oldEdges.push_back(e);
			//	mv.markOrbit(VERTEX_ORBIT, e);
			//}
			Dart ee = map.phi2(e) ;
			if(!mf.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mf.markOrbit(FACE_ORBIT, ee) ;
			}
			e = map.phi1(e) ;
		} while(e != *face) ;
	}



	DartMarker me(map) ; //mark edge

	unsigned int degree = 0 ;
	DartMarker mvolcent(map); //mark vertices vol centroid computing

	//First step : subdivide edges and faces
	//creates a i+1 edge level and i+1 face level
	for (std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart d = *face;


			Dart it = d ;
			do
			{
				if(!mvolcent.isMarked(it))
				{
					++degree;
					volCenter += position[it] ;
					mvolcent.markOrbit(VERTEX_ORBIT, it);
				}
				it = map.phi1(it) ;
			} while(it != d) ;



		CGoGNout << "subdiv de toutes les faces" << CGoGNendl;
		if(!map.faceIsSubdivided(d))
		{

			Dart cf = Algo::IHM3::subdivideFace<PFP>(map, *face, position);
			CGoGNout << "face faite" << CGoGNendl;


			unsigned int fLevel = map.getDartLevel(cf) ;

			map.setCurrentLevel(fLevel) ;

			Dart e = cf;
			do
			{
				quadfaces.push_back(e);
				quadfaces.push_back(map.phi2(e));
				e = map.phi2(map.phi_1(e));
			}while (e != cf);
		}
	}

	volCenter /= typename PFP::REAL(degree) ;

	map.setCurrentLevel(vLevel + 1) ; // go to the next level to perform volume subdivision

	CGoGNout << "vLevel+1=" << vLevel + 1 << CGoGNendl;

	DartMarker moe(map) ; //mark edge

	//Third pass : deconnect the corners
	for (std::vector<Dart>::iterator edge = oldEdges.begin(); edge != oldEdges.end(); ++edge)
	{

//					Dart e = *edge;
//					do
//					{
//						map.unsewFaces(map.phi1(map.phi1(*edge)));
//						map.unsewFaces(map.phi1(*edge));
//						e = map.phi2(map.phi_1(e));
//					}while (e != *edge);
		map.unsewFaces(map.phi1(*edge));
		moe.markOrbit(DART_ORBIT,map.phi1(*edge));
	}

	//Thourth pass : close the hole
	for (std::vector<Dart>::iterator edge = oldEdges.begin(); edge != oldEdges.end(); ++edge)
	{
		if(moe.isMarked(map.phi1(*edge)))
		{
			map.closeHole(map.phi1(*edge));

			Dart h = map.phi2(map.phi1(*edge));
			do
			{
				map.setEdgeId(h,map.getEdgeId(map.phi2(h)), EDGE_ORBIT) ;
				h = map.phi1(h);
			}while(h != map.phi2(map.phi1(*edge)));

			moe.unmark(map.phi1(*edge));
			moe.unmark(map.phi1(map.phi2(map.phi_1(*edge))));
			moe.unmark(map.phi1(map.phi1(map.phi2(*edge))));

			Dart d = map.phi1(map.phi2(map.phi1(*edge)));
			Dart old = d;
			Dart dd = map.phi1(old) ;
			map.splitFace(dd, map.phi1(map.phi1(dd))) ;

			Dart ne = map.phi2(map.phi_1(dd));
			Dart ne2 = map.phi2(ne);

			map.cutEdge(ne) ;
			unsigned int id = map.getNewEdgeId() ;
			map.setEdgeId(ne, id, EDGE_ORBIT) ;

			id = map.getNewEdgeId() ;
			map.setEdgeId(ne2, id, EDGE_ORBIT) ;

			position[map.phi2(ne)] = volCenter;

			dd = map.phi1(map.phi1(map.phi1(map.phi1(ne)))) ;
			while(dd != ne)
			{
				Dart next = map.phi1(map.phi1(dd)) ;
				map.splitFace(map.phi1(ne), dd) ;
				Dart nne = map.phi2(map.phi_1(dd)) ;

				id = map.getNewEdgeId() ;
				map.setEdgeId(nne, id, EDGE_ORBIT) ;

				dd = next ;
			}
		}
	}

	moe.unmarkAll();


	for (std::vector<Dart>::iterator nvol = quadfaces.begin(); nvol != quadfaces.end(); nvol = nvol + 2)
	{
		if(map.phi3(map.phi2(*nvol)) == map.phi2(*nvol) && map.phi3(map.phi2(*(nvol+1))) == map.phi2(*(nvol+1)))
		{
			map.sewVolumes(map.phi2(*nvol), map.phi2(*(nvol+1)));
			unsigned int id = map.getNewFaceId();
			map.setFaceId(map.phi2(*nvol),id, FACE_ORBIT);
		}
	}



	map.setCurrentLevel(cur) ;

	CGoGNout << CGoGNendl << CGoGNendl;
}

template <typename PFP>
void catmullClarck(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position)
{


}

} //namespace IHM

} //namespace Algo

} //namespace CGoGN
