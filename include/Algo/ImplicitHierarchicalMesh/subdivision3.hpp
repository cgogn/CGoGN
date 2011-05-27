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

namespace IHM
{

template <typename PFP>
void subdivideEdge(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position)
{
	assert(map.getDartLevel(d) <= map.getCurrentLevel() || !"Access to a dart introduced after current level") ;
	assert(!map.edgeIsSubdivided(d) || !"Trying to subdivide an already subdivided edge") ;

	unsigned int eLevel = map.edgeLevel(d) ;

	unsigned int cur = map.getCurrentLevel() ;
	map.setCurrentLevel(eLevel) ;

	Dart dd = map.phi2(d) ;
	typename PFP::VEC3 p1 = position[d] ;
	typename PFP::VEC3 p2 = position[map.phi1(d)] ;

	map.setCurrentLevel(eLevel + 1) ;

	map.cutEdge(d) ;
	unsigned int eId = map.getEdgeId(d) ;
	map.setEdgeId(map.phi1(d), eId, EDGE_ORBIT) ; //mise a jour de l'id d'arrete sur chaque moitie d'arete
	map.setEdgeId(map.phi1(dd), eId, EDGE_ORBIT) ;


	map.setFaceId(EDGE_ORBIT, d) ; //mise a jour de l'id de face sur chaque brin de chaque moitie d'arete
	map.setFaceId(EDGE_ORBIT, dd) ;

	position[map.phi1(d)] = (p1 + p2) * typename PFP::REAL(0.5) ;

	map.setCurrentLevel(cur) ;
}

template <typename PFP>
void subdivideFace(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position, SubdivideType sType)
{
	assert(map.getDartLevel(d) <= map.getCurrentLevel() || !"Access to a dart introduced after current level") ;
	assert(!map.faceIsSubdivided(d) || !"Trying to subdivide an already subdivided face") ;

	unsigned int fLevel = map.faceLevel(d) ;
	Dart old = map.faceOldestDart(d) ;

	unsigned int cur = map.getCurrentLevel() ;
	map.setCurrentLevel(fLevel) ;		// go to the level of the face to subdivide its edges

	unsigned int degree = 0 ;
	typename PFP::VEC3 p ;
	Dart it = old ;
	do
	{
		++degree;
		p += position[it] ;
		if(!map.edgeIsSubdivided(it))							// first cut the edges (if they are not already)
			Algo::IHM::subdivideEdge<PFP>(map, it, position) ;	// and compute the degree of the face
		it = map.phi1(it) ;
	} while(it != old) ;
	p /= typename PFP::REAL(degree) ;

	map.setCurrentLevel(fLevel + 1) ;			// go to the next level to perform face subdivision

	Dart res;

	if(degree == 3 && sType ==  S_TRI)		//subdiviser une face triangulaire
	{
		Dart dd = map.phi1(old) ;
		Dart e = map.phi1(map.phi1(dd)) ;
		map.splitFace(dd, e) ;					// insert a new edge
		unsigned int id = map.getNewEdgeId() ;
		map.setEdgeId(map.phi_1(dd), id, EDGE_ORBIT) ;		// set the edge id of the inserted edge to the next available id

		unsigned int idface = map.getFaceId(old);
		map.setFaceId(dd, idface, FACE_ORBIT) ;
		map.setFaceId(e, idface, FACE_ORBIT) ;

		dd = e ;
		e = map.phi1(map.phi1(dd)) ;
		map.splitFace(dd, e) ;
		id = map.getNewEdgeId() ;
		map.setEdgeId(map.phi_1(dd), id, EDGE_ORBIT) ;

		map.setFaceId(dd, idface, FACE_ORBIT) ;
		map.setFaceId(e, idface, FACE_ORBIT) ;

		dd = e ;
		e = map.phi1(map.phi1(dd)) ;
		map.splitFace(dd, e) ;
		id = map.getNewEdgeId() ;
		map.setEdgeId(map.phi_1(dd), id, EDGE_ORBIT) ;

		map.setFaceId(dd, idface, FACE_ORBIT) ;
		map.setFaceId(e, idface, FACE_ORBIT) ;
	}
	else
	{
		Dart dd = map.phi1(old) ;
		map.splitFace(dd, map.phi1(map.phi1(dd))) ;

		Dart ne = map.phi2(map.phi_1(dd));
		Dart ne2 = map.phi2(ne);

		map.cutEdge(ne) ;
		unsigned int id = map.getNewEdgeId() ;
		map.setEdgeId(ne, id, EDGE_ORBIT) ;
		id = map.getNewEdgeId() ;
		map.setEdgeId(ne2, id, EDGE_ORBIT) ;

		position[map.phi2(ne)] = p ;

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

		unsigned int idface = map.getFaceId(old);
		Dart e = dd;
		do
		{
			map.setFaceId(dd, idface, DART_ORBIT) ;
			map.setFaceId(map.phi2(dd), idface, DART_ORBIT) ;
			dd = map.phi2(map.phi1(dd));
		}
		while(dd != ne);
	}

	map.setCurrentLevel(cur) ;
}

template <typename PFP>
Dart subdivideVolume(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position)
{
	assert(map.getDartLevel(d) <= map.getCurrentLevel() || !"Access to a dart introduced after current level") ;
	assert(!map.volumeIsSubdivided(d) || !"Trying to subdivide an already subdivided volume") ;

	std::cout << "SUUUUUUB marine" << std::endl;

	unsigned int vLevel = map.volumeLevel(d) ;
	Dart old = map.volumeOldestDart(d) ;

	unsigned int cur = map.getCurrentLevel() ;
	map.setCurrentLevel(vLevel) ;		// go to the level of the face to subdivide its edges


	/*
	 * au niveau du volume courant i
	 * stockage d'un brin de chaque face de celui-ci
	 * avec calcul du centroid
	 */

	DartMarkerStore mf(map);		// Lock a face marker to save one dart per face
	DartMarkerStore mv(map);		// Lock a vertex marker to compute volume center

	typename PFP::VEC3 volCenter;
	unsigned count = 0 ;

	//Store faces that are traversed and start with the face of d
	std::vector<Dart> visitedFaces;
	visitedFaces.reserve(20);
	visitedFaces.push_back(old);

	//Store the edges before the cutEdge
	std::vector<Dart> oldEdges;
	oldEdges.reserve(20);

	mf.markOrbit(FACE_ORBIT, old) ;

	for(std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;
		do
		{
			//add one old edge per vertex to the old edge list
			//compute volume centroid
			if(!mv.isMarked(e))
			{
				volCenter += position[e];
				++count;
				mv.markOrbit(VERTEX_ORBIT, e);
				oldEdges.push_back(e);
			}

			// add all face neighbours to the table
			Dart ee = map.phi2(e) ;
			if(!mf.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mf.markOrbit(FACE_ORBIT, ee) ;
			}

			e = map.phi1(e) ;
		} while(e != *face) ;
	}

	volCenter /= typename PFP::REAL(count) ;

	/*
	 * Subdivision
	 */

	//Store the darts from quadrangulated faces
	std::vector<std::pair<Dart,Dart> > subdividedfaces;
	subdividedfaces.reserve(25);

	//First step : subdivide edges and faces
	//creates a i+1 edge level and i+1 face level
	for (std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart d = *face;

		//if needed subdivide face
		if(!map.faceIsSubdivided(d))
			Algo::IHM::subdivideFace<PFP>(map, d, position);

		//save a dart from the subdivided face
		unsigned int cur = map.getCurrentLevel() ;

		unsigned int fLevel = map.faceLevel(d) + 1; //puisque dans tous les cas, la face est subdivisee
		map.setCurrentLevel(fLevel) ;


		//le brin est forcement du niveau cur
		Dart cf = map.phi1(d);
		Dart e = cf;
		do
		{
			subdividedfaces.push_back(std::pair<Dart,Dart>(e,map.phi2(e)));
			e = map.phi2(map.phi1(e));
		}while (e != cf);

		map.setCurrentLevel(cur);
	}

	map.setCurrentLevel(vLevel + 1) ; // go to the next level to perform volume subdivision

	std::vector<Dart> newEdges;	//save darts from inner edges
	newEdges.reserve(50);

	//Second step : deconnect each corner, close each hole, subdivide each new face into 3
	for (std::vector<Dart>::iterator edge = oldEdges.begin(); edge != oldEdges.end(); ++edge)
	{
		Dart e = *edge;

		Dart f1 = map.phi1(*edge);

		do
		{
			map.unsewFaces(map.phi1(map.phi1(e)));

			//TODO utile ?
			//if(map.phi2(map.phi1(e)) != map.phi1(e))
				map.unsewFaces(map.phi1(e));

			e = map.phi2(map.phi_1(e));
		}
		while(e != *edge);


		map.closeHole(f1);

		Dart old = map.phi2(map.phi1(e));
		Dart dd = map.phi1(map.phi1(old)) ;
		map.splitFace(old,dd) ;

		Dart ne = map.phi1(map.phi1(old)) ;

		map.cutEdge(ne);
		position[map.phi1(ne)] = volCenter; //plonger a la fin de la boucle ????
		newEdges.push_back(ne);
		newEdges.push_back(map.phi1(ne));


		Dart stop = map.phi2(map.phi1(ne));
		ne = map.phi2(ne);
		do
		{
			dd = map.phi1(map.phi1(map.phi1(ne)));

			//A Verifier !!
			map.splitFace(ne, dd) ;

			newEdges.push_back(map.phi1(dd));

			ne = map.phi2(map.phi_1(ne));
			dd = map.phi1(map.phi1(dd));
		}
		while(dd != stop);
	}

	//Third step : 3-sew internal faces
	for (std::vector<std::pair<Dart,Dart> >::iterator it = subdividedfaces.begin(); it != subdividedfaces.end(); ++it)
	{
		Dart f1 = (*it).first;
		Dart f2 = (*it).second;

		if(map.phi3(map.phi2(f1)) == map.phi2(f1) && map.phi3(map.phi2(f2)) == map.phi2(f2))
		{
				//id pour toutes les faces interieures
				map.sewVolumes(map.phi2(f1), map.phi2(f2));

				//Fais a la couture !!!!!
				unsigned int idface = map.getNewFaceId();
				map.setFaceId(map.phi2(f1),idface, FACE_ORBIT);
		}

		//FAIS a la couture !!!!!!!
		//id pour toutes les aretes exterieurs des faces quadrangulees
		unsigned int idedge = map.getEdgeId(f1);
		map.setEdgeId(map.phi2(f1), idedge, DART_ORBIT);
		map.setEdgeId( map.phi2(f2), idedge, DART_ORBIT);
	}

	//LA copie de L'id est a gerer avec le sewVolumes normalement !!!!!!
	//id pour les aretes interieurs : (i.e. 6 pour un hexa)
	DartMarker mne(map);
	for(std::vector<Dart>::iterator it = newEdges.begin() ; it != newEdges.end() ; ++it)
	{
		if(!mne.isMarked(*it))
		{
			unsigned int idedge = map.getNewEdgeId();
			map.setEdgeId(*it, idedge, EDGE_ORBIT);
			mne.markOrbit(EDGE_ORBIT,*it);
		}
	}

	map.setCurrentLevel(cur) ;

	return subdividedfaces.begin()->first;
}


template <typename PFP>
void subdivideLoop(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position)
{
	assert(map.getDartLevel(d) <= map.getCurrentLevel() || !"Access to a dart introduced after current level") ;
	assert(!map.volumeIsSubdivided(d) || !"Trying to subdivide an already subdivided volume") ;

	unsigned int vLevel = map.volumeLevel(d) ;
	Dart old = map.volumeOldestDart(d) ;

	unsigned int cur = map.getCurrentLevel() ;
	map.setCurrentLevel(vLevel) ;		// go to the level of the face to subdivide its edges


	/*
	 * au niveau du volume courant i
	 * stockage d'un brin de chaque face de celui-ci
	 * avec calcul du centroid
	 */

	DartMarkerStore mf(map);		// Lock a face marker to save one dart per face
	DartMarkerStore mv(map);		// Lock a vertex marker to compute volume center

	typename PFP::VEC3 volCenter;
	unsigned count = 0 ;

	//Store faces that are traversed and start with the face of d
	std::vector<Dart> visitedFaces;
	visitedFaces.reserve(20);
	visitedFaces.push_back(old);

	//Store the edges before the cutEdge
	std::vector<Dart> oldEdges;
	oldEdges.reserve(20);

	mf.markOrbit(FACE_ORBIT, old) ;

	for(std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;
		do
		{
			//add one old edge per vertex to the old edge list
			//compute volume centroid
			if(!mv.isMarked(e))
			{
				volCenter += position[e];
				++count;
				mv.markOrbit(VERTEX_ORBIT, e);
				oldEdges.push_back(e);
			}

			// add all face neighbours to the table
			Dart ee = map.phi2(e) ;
			if(!mf.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mf.markOrbit(FACE_ORBIT, ee) ;
			}

			e = map.phi1(e) ;
		} while(e != *face) ;
	}

	volCenter /=  typename PFP::REAL(count) ;

	/*
	 * Subdivision
	 */
	//Store the darts from quadrangulated faces
		std::vector<std::pair<Dart,Dart> > subdividedfaces;
		subdividedfaces.reserve(25);

		//First step : subdivide edges and faces
		//creates a i+1 edge level and i+1 face level
		for (std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
		{
			Dart d = *face;

			//if needed subdivide face
			if(!map.faceIsSubdivided(d))
				Algo::IHM::subdivideFace<PFP>(map, d, position, Algo::IHM::S_TRI);

			//save a dart from the subdivided face
			unsigned int cur = map.getCurrentLevel() ;

			unsigned int fLevel = map.faceLevel(d) + 1; //puisque dans tous les cas, la face est subdivisee
			map.setCurrentLevel(fLevel) ;

			//le brin est forcement du niveau cur
			Dart cf = map.phi2(map.phi1(d));
			Dart e = cf;
			do
			{
				subdividedfaces.push_back(std::pair<Dart,Dart>(e,map.phi2(e)));
				e = map.phi1(e);
			}while (e != cf);

			map.setCurrentLevel(cur);
		}

		map.setCurrentLevel(vLevel + 1) ; // go to the next level to perform volume subdivision

		std::vector<Dart> newEdges;	//save darts from inner edges
		newEdges.reserve(50);

		bool istet = true;

		//Second step : deconnect each corner, close each hole, subdivide each new face into 3
		for (std::vector<Dart>::iterator edge = oldEdges.begin(); edge != oldEdges.end(); ++edge)
		{
			Dart e = *edge;

			Dart f1 = map.phi1(*edge);
			Dart f2 = map.phi2(f1);

			do
			{

					map.unsewFaces(map.phi1(e));

				e = map.phi2(map.phi_1(e));
			}
			while(e != *edge);

			map.closeHole(f1);
			map.closeHole(f2);
			map.sewVolumes(map.phi2(f1),map.phi2(f2));

			unsigned int fdeg = map.faceDegree(map.phi2(f1));

			if(fdeg > 3)
			{
				istet = false;
				Dart old = map.phi2(map.phi1(*edge));
				Dart dd = map.phi1(old) ;
				map.splitFace(old,dd) ;

				Dart ne = map.phi1(old);

				map.cutEdge(ne);
				position[map.phi1(ne)] = volCenter; //plonger a la fin de la boucle ????
				newEdges.push_back(ne);
				newEdges.push_back(map.phi1(ne));

				Dart stop = map.phi2(map.phi1(ne));
				ne = map.phi2(ne);
				do
				{
					dd = map.phi1(map.phi1(ne));

					map.splitFace(ne, dd) ;

					newEdges.push_back(map.phi1(dd));

					ne = map.phi2(map.phi_1(ne));
					dd = map.phi1(dd);
				}
				while(dd != stop);
			}
			else
			{
				unsigned int idface = map.getNewFaceId();
				map.setFaceId(map.phi2(f1),idface, FACE_ORBIT);
			}

		}

		if(!istet)
		{
			for (std::vector<Dart>::iterator edge = oldEdges.begin(); edge != oldEdges.end(); ++edge)
			{
				Dart e = *edge;

				Dart x = map.phi_1(map.phi2(map.phi1(*edge)));
				Dart f = x;

				do
				{
					Dart f3 = map.phi3(f);
					Dart tmp =  map.phi_1(map.phi2(map.phi_1(map.phi2(map.phi_1(f3)))));

					map.unsewFaces(f3);
					map.unsewFaces(tmp);
					map.sewFaces(f3, tmp);

					unsigned int idface = map.getNewFaceId();
					map.setFaceId(map.phi2(f3),idface, FACE_ORBIT);


					f = map.phi2(map.phi_1(f));
				}while(f != x);

			}
		}

		//Third step : 3-sew internal faces
		for (std::vector<std::pair<Dart,Dart> >::iterator it = subdividedfaces.begin(); it != subdividedfaces.end(); ++it)
		{
			Dart f1 = (*it).first;
			Dart f2 = (*it).second;

			//FAIS a la couture !!!!!!!
			//id pour toutes les aretes exterieurs des faces quadrangulees
			unsigned int idedge = map.getEdgeId(f1);
			map.setEdgeId(map.phi2(f1), idedge, DART_ORBIT);
			map.setEdgeId( map.phi2(f2), idedge, DART_ORBIT);

		}

		//LA copie de L'id est a gerer avec le sewVolumes normalement !!!!!!
		//id pour les aretes interieurs : (i.e. 16 pour un octa)
		DartMarker mne(map);
		for(std::vector<Dart>::iterator it = newEdges.begin() ; it != newEdges.end() ; ++it)
		{
			if(!mne.isMarked(*it))
			{
				unsigned int idedge = map.getNewEdgeId();
				map.setEdgeId(*it, idedge, EDGE_ORBIT);
				mne.markOrbit(EDGE_ORBIT,*it);
			}
		}


		map.setCurrentLevel(cur) ;
}

/***********************************************************************************
 *
 */


template <typename PFP>
void subdivideVolume(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position, SubdivideType sType)
{
	assert(map.getDartLevel(d) <= map.getCurrentLevel() || !"Access to a dart introduced after current level") ;
	assert(!map.volumeIsSubdivided(d) || !"Trying to subdivide an already subdivided volume") ;

	unsigned int vLevel = map.volumeLevel(d) ;
	Dart old = map.volumeOldestDart(d) ;

	unsigned int cur = map.getCurrentLevel() ;
	map.setCurrentLevel(vLevel) ;		// go to the level of the face to subdivide its edges


	/*
	 * au niveau du volume courant i
	 * stockage d'un brin de chaque face de celui-ci
	 * avec calcul du centroid
	 */

	DartMarkerStore mf(map);		// Lock a face marker to save one dart per face
	DartMarkerStore mv(map);		// Lock a vertex marker to compute volume center

	typename PFP::VEC3 volCenter;
	unsigned count = 0 ;

	//Store faces that are traversed and start with the face of d
	std::vector<Dart> visitedFaces;
	visitedFaces.reserve(20);
	visitedFaces.push_back(old);

	//Store the edges before the cutEdge
	std::vector<Dart> oldEdges;
	oldEdges.reserve(20);

	mf.markOrbit(FACE_ORBIT, old) ;

	for(std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;
		do
		{
			//add one old edge per vertex to the old edge list
			//compute volume centroid
			if(!mv.isMarked(e))
			{
				volCenter += position[e];
				++count;
				mv.markOrbit(VERTEX_ORBIT, e);
				oldEdges.push_back(e);
			}

			// add all face neighbours to the table
			Dart ee = map.phi2(e) ;
			if(!mf.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mf.markOrbit(FACE_ORBIT, ee) ;
			}

			e = map.phi1(e) ;
		} while(e != *face) ;
	}

	volCenter /= typename PFP::REAL(count) ;

	/*
	 * Subdivision
	 */

	//Store the darts from quadrangulated faces
	std::vector<std::pair<Dart,Dart> > subdividedTriFaces;
	subdividedTriFaces.reserve(15);

	std::vector<std::pair<Dart,Dart> > subdividedQuadFaces;
	subdividedQuadFaces.reserve(15);

	//First step : subdivide edges and faces
	//creates a i+1 edge level and i+1 face level
	for (std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart d = *face;

		//if needed subdivide face
		if(!map.faceIsSubdivided(d))
			Algo::IHM::subdivideFace<PFP>(map, d, position, Algo::IHM::S_TRI);

		//save a dart from the subdivided face
		unsigned int cur = map.getCurrentLevel() ;

		unsigned int fLevel = map.faceLevel(d) + 1; //puisque dans tous les cas, la face est subdivisee
		map.setCurrentLevel(fLevel) ;

		//Face tri
		if(map.phi1(map.phi1(map.phi1(d))) == d)
		{

			//le brin est forcement du niveau cur
			Dart cf = map.phi2(map.phi1(d));
			Dart e = cf;
			do
			{
				subdividedTriFaces.push_back(std::pair<Dart,Dart>(e,map.phi2(e)));
				e = map.phi1(e);
			}while (e != cf);
		}
		else
		{
			//le brin est forcement du niveau cur
			Dart cf = map.phi1(d);
			Dart e = cf;
			do
			{
				subdividedQuadFaces.push_back(std::pair<Dart,Dart>(e,map.phi2(e)));
				e = map.phi2(map.phi1(e));
			}while (e != cf);
		}

		map.setCurrentLevel(cur);
	}

	map.setCurrentLevel(vLevel + 1) ; // go to the next level to perform volume subdivision

	//Second step : create corners
	for (std::vector<Dart>::iterator edge = oldEdges.begin(); edge != oldEdges.end(); ++edge)
	{
		Dart e = *edge;

		Dart f1 = map.phi1(*edge);
		Dart f2 = map.phi2(map.phi1(*edge));

		do
		{
			if(map.phi1(map.phi1(map.phi1(e))) != e)
				map.unsewFaces(map.phi1(map.phi1(e)));
			if(map.phi2(map.phi1(e)) != map.phi1(e))
				map.unsewFaces(map.phi1(e));

			e = map.phi2(map.phi_1(e));

		}while(e != *edge);

		map.closeHole(f1);
		map.closeHole(f2);
		map.sewVolumes(map.phi2(f1),map.phi2(f2));

		//un passage sur chaque nouvelle face pour recopier l'id d'arete
		Dart d = map.phi2(f1);
		e = d;
		do
		{
			unsigned int idedge = map.getEdgeId(map.phi2(e));
			map.setEdgeId(e, idedge, EDGE_ORBIT);
			e = map.phi1(e);
		}while(e != d);


		//Identifiant de face pour les 2 faces orientees
		unsigned int idface = map.getNewFaceId();
		map.setFaceId(map.phi2(f1),idface, FACE_ORBIT);
	}

	std::vector<Dart> inFaces;
	inFaces.reserve(10);

	//DartMarkerStore mv(map);

//	//Third Step : split inner faces if quad faces exists
//	for(std::vector<std::pair<Dart,Dart> >::iterator quadFaces = subdividedQuadFaces.begin() ; quadFaces != subdividedQuadFaces.end() ; ++quadFaces)
//	{
//		Dart f = (*quadFaces).first;
//
//		//if(!map.isMarked(f))
//		//{
//			map.splitFace(map.phi2(f), map.phi_1(map.phi_1(map.phi2(f))));
//			//mv.markOrbit(VERTEX_ORBIT(map.phi2(f));
//		//}
//
//		inFaces.push_back(map.phi1(map.phi3(map.phi2(f))));
//
//
//		unsigned int idface = map.getNewFaceId();
//		map.setFaceId(map.phi2(f),idface, FACE_ORBIT);
//
//		idface = map.getNewFaceId();
//		map.setFaceId(map.phi2(map.phi_1(map.phi2(f))),idface, FACE_ORBIT);
//	}
//
//	for(std::vector<Dart>::iterator inFace = inFaces.begin() ; inFace != inFaces.end() ; ++inFace)
//	{
//		Dart e = *inFace;
//		Dart e2 = map.phi2(*inFace);
//
//		do
//		{
//			map.unsewFaces(e);
//			e = map.phi1(map.phi2(map.phi1(e)));
//		}while(e != *inFace);
//
//		map.closeHole(*inFace);
//		map.closeHole(e2);
//
//		map.sewVolumes(map.phi2(*inFace), map.phi2(e2));
//
//		//Identifiant de face pour les 2 faces orientees
//		unsigned int idface = map.getNewFaceId();
//		map.setFaceId(map.phi2(*inFace),idface, FACE_ORBIT);
//
//		//unsigned int idedge = map.getEdgeId(map.phi2(map.phi1(map.phi2(*inFace))));
//		//map.setEdgeId(map.phi1(map.phi2(*inFace)), idedge, EDGE_ORBIT);
//
//	}
//
//	for(std::vector<Dart>::iterator inFace = inFaces.begin() ; inFace != inFaces.end() ; ++inFace)
//	{
//		Dart e = *inFace;
//
//		unsigned int idedge = map.getNewEdgeId();
//		map.setEdgeId(e, idedge, EDGE_ORBIT);
//
//		idedge = map.getEdgeId(map.phi2(map.phi1(map.phi1(map.phi2(*inFace)))));
//		map.setEdgeId(map.phi1(map.phi1(map.phi2(*inFace))), idedge, EDGE_ORBIT);
//
//	}

	map.setCurrentLevel(cur);
}


//				Dart old = map.phi1(map.phi2(map.phi1(e)));
//				Dart dd = map.phi1(old) ;
//				map.splitFace(dd, map.phi1(map.phi1(dd))) ;
//
//				Dart ne = map.phi2(map.phi_1(dd));
//				Dart ne2 = map.phi2(ne);
//
//				map.cutEdge(ne) ;
//				position[map.phi2(ne)] = volCenter;
//
//				newEdges.push_back(ne);
//				newEdges.push_back(map.phi1(ne));
//
//				dd = map.phi1(map.phi1(map.phi1(map.phi1(ne)))) ;
//				while(dd != ne)
//				{
//					Dart next = map.phi1(map.phi1(dd)) ;
//					map.splitFace(map.phi1(ne), dd) ;
//
//					newEdges.push_back(ne);
//					newEdges.push_back(map.phi1(ne));
//
//					dd = next ;
//				}

template <typename PFP>
void catmullClark(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position)
{
	assert(map.getDartLevel(d) <= map.getCurrentLevel() || !"Access to a dart introduced after current level") ;
	assert(!map.volumeIsSubdivided(d) || !"Trying to subdivide an already subdivided volume") ;

	typedef typename PFP::VEC3 VEC3;

//	unsigned int vLevel = map.volumeLevel(d) ;
	Dart old = map.volumeOldestDart(d) ;

//	unsigned int cur = map.getCurrentLevel() ;
//	map.setCurrentLevel(vLevel) ;		// go to the level of the face to subdivide its edges


	/*
	 * au niveau du volume courant i
	 * stockage d'un brin de chaque face de celui-ci
	 * avec calcul du centroid
	 */

	DartMarkerStore mf(map);		// Lock a face marker to save one dart per face
	DartMarkerStore mv(map);		// Lock a vertex marker to compute volume center

	//Store faces that are traversed and start with the face of d
	std::vector<Dart> visitedFaces;
	visitedFaces.reserve(20);
	visitedFaces.push_back(old);

	//Store the edges before the cutEdge
	std::vector<Dart> oldEdges;
	oldEdges.reserve(20);

	mf.markOrbit(FACE_ORBIT, old) ;

	for(std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;
		do
		{
			//add one old edge per vertex to the old edge list
			//compute volume centroid
			if(!mv.isMarked(e))
			{
				mv.markOrbit(VERTEX_ORBIT, e);

				oldEdges.push_back(e);
			}

			// add all face neighbours to the table
			Dart ee = map.phi2(e) ;
			if(!mf.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mf.markOrbit(FACE_ORBIT, ee) ;
			}

			e = map.phi1(e) ;
		} while(e != *face) ;
	}

	mf.unmarkAll();

	//subdivision topologique
	subdivideVolume<PFP>(map,d,position);

	//unsigned int vLevel = map.getCurrentLevel();
	//map.setCurrentLevel(vLevel-1);


	for(std::vector<Dart>::iterator face = oldEdges.begin(); face != oldEdges.end(); ++face)
	{
		Dart f = *face;

		if(map.phi3(f) == f)
		{
			Dart x = map.phi2(map.phi1(f));
			// other side of the edge
			Dart y = map.phi2(x);

			Dart f1 = map.phi_1(x);
			Dart f2 = map.phi2(map.phi1(y));
			VEC3 temp = VEC3(0);
			temp =  position[f1];
			temp += position[f2];			// E' = (V0+V1+F1+F2)/4
			temp *= 0.25;
			position[x] *= 0.5;
			position[x] += temp;
		}

	}

	//map.setCurrentLevel(vLevel);
}

template <typename PFP>
void macCrackenJoy(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position)
{
	assert(map.getDartLevel(d) <= map.getCurrentLevel() || !"Access to a dart introduced after current level") ;
	assert(!map.volumeIsSubdivided(d) || !"Trying to subdivide an already subdivided volume") ;

	typedef typename PFP::VEC3 VEC3;

//	unsigned int vLevel = map.volumeLevel(d) ;
	Dart old = map.volumeOldestDart(d) ;

//	unsigned int cur = map.getCurrentLevel() ;
//	map.setCurrentLevel(vLevel) ;		// go to the level of the face to subdivide its edges


	/*
	 * au niveau du volume courant i
	 * stockage d'un brin de chaque face de celui-ci
	 * avec calcul du centroid
	 */

	DartMarkerStore mf(map);		// Lock a face marker to save one dart per face
	DartMarkerStore mv(map);		// Lock a vertex marker to compute volume center

	//Store faces that are traversed and start with the face of d
	std::vector<Dart> visitedFaces;
	visitedFaces.reserve(20);
	visitedFaces.push_back(old);

	//Store the edges before the cutEdge
	std::vector<Dart> oldEdges;
	oldEdges.reserve(20);

	mf.markOrbit(FACE_ORBIT, old) ;

	for(std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;
		do
		{
			//add one old edge per vertex to the old edge list
			//compute volume centroid
			if(!mv.isMarked(e))
			{
				mv.markOrbit(VERTEX_ORBIT, e);

				oldEdges.push_back(e);
			}

			// add all face neighbours to the table
			Dart ee = map.phi2(e) ;
			if(!mf.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mf.markOrbit(FACE_ORBIT, ee) ;
			}

			e = map.phi1(e) ;
		} while(e != *face) ;
	}

	mf.unmarkAll();

	//subdivision topologique
	subdivideVolume<PFP>(map,d,position);


	//parcours de chaque sommet topologique de niveau vLevel - 1
	for (std::vector<Dart>::iterator edge = oldEdges.begin(); edge != oldEdges.end(); ++edge)
	{
		//parcour des faces autour du sommet
		Dart e = *edge;
		do
		{
			//si la face n'a pas encore ete visite
			if(!mf.isMarked(e))
			{
				//on la marque pour ne plus la visiter
				mf.markOrbit(FACE_ORBIT, e);

				//on calcul la valeur de F = (C0 + 2A + C1 ) / 4
				VEC3 C0 = position[map.phi_1(map.phi2(map.phi1(e)))];
				VEC3 C1 = VEC3(0);
				if(map.phi3(e) != e)
					C1 = position[map.phi1(map.phi2(map.phi3(map.phi1(e))))];

				VEC3 A = position[map.phi2(map.phi1(e))];

				VEC3 F = (C0 + A + A + C1) / 4;

				position[map.phi2(map.phi1(e))] = F;


				//On calcul la valeur de E = (Cavg + 2 Aavg + (n-3)M ) / n

			}

			e = map.phi2(map.phi_1(e));
		}while(e != *edge);
	}
}


template <typename PFP>
void bajaj(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position)
{
	assert(map.getDartLevel(d) <= map.getCurrentLevel() || !"Access to a dart introduced after current level") ;
	assert(!map.volumeIsSubdivided(d) || !"Trying to subdivide an already subdivided volume") ;

	typedef typename PFP::VEC3 VEC3;
	Dart old = map.volumeOldestDart(d) ;

	/*
	 * au niveau du volume courant i
	 * stockage d'un brin de chaque face de celui-ci
	 * avec calcul du centroid
	 */

	DartMarkerStore mf(map);		// Lock a face marker to save one dart per face
	DartMarkerStore mv(map);		// Lock a vertex marker to compute volume center

	//Store faces that are traversed and start with the face of d
	std::vector<Dart> visitedFaces;
	visitedFaces.reserve(20);
	visitedFaces.push_back(old);

	//Store the edges before the cutEdge
	std::vector<Dart> oldEdges;
	oldEdges.reserve(20);

	mf.markOrbit(FACE_ORBIT, old) ;

	for(std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;
		do
		{
			//add one old edge per vertex to the old edge list
			//compute volume centroid
			if(!mv.isMarked(e))
			{
				mv.markOrbit(VERTEX_ORBIT, e);

				oldEdges.push_back(e);
			}

			// add all face neighbours to the table
			Dart ee = map.phi2(e) ;
			if(!mf.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mf.markOrbit(FACE_ORBIT, ee) ;
			}

			e = map.phi1(e) ;
		} while(e != *face) ;
	}


	//subdivision topologique
	subdivideVolume<PFP>(map,d,position);


	//Algo :
	/*

	 Parcours de chaque sommet de niveau 0
	 calcul des sommets 1-voisin
	  ->//recalcul du map.phi2(map.phi1())
	  ->//recalcul du map.phi2()
	 -> A la fin de la boucle //repositionner ce sommet lui aussi
  	 marquage de l'orbit sommet recalcule

	 */

	DartMarkerStore mvp(map);

	//Modification de la position des sommets
	//parcours de chaque sommet topologique de niveau vLevel - 1
	for (std::vector<Dart>::iterator edge = oldEdges.begin(); edge != oldEdges.end(); ++edge)
	{
		//parcour des sommets autour du sommet de niveau inferieur
		Dart e = *edge;
		Dart v;

		do
		{
			v = map.phi2(map.phi1(e));
			if(!mvp.isMarked(v))
			{
				position[v] = bajajMask<PFP>(map, v, position);
				mvp.markOrbit(VERTEX_ORBIT, v);
			}

			v = map.phi2(e);
			if(!mvp.isMarked(v))
			{
				position[v] = bajajMask<PFP>(map, v, position);
				mvp.markOrbit(VERTEX_ORBIT, v);
			}

			e = map.phi2(map.phi_1(e));
		}while(e != *edge);

		if(!mvp.isMarked(e))
		{
			position[e] = bajajMask<PFP>(map, e, position);
			mvp.markOrbit(VERTEX_ORBIT, e);
		}
	}

}

template <typename PFP>
typename PFP::VEC3 bajajMask(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position)
{
	typedef typename PFP::VEC3 VEC3;

	unsigned int vLevel = map.volumeLevel(d) ;
	unsigned int cur = map.getCurrentLevel() ;
	map.setCurrentLevel(vLevel-1) ;		// go to the level of the face to subdivide its edges

	VEC3 res = position[d] / 4;

	Dart e = d;
	do
	{
		res += position[map.phi2(e)] / 8;
		res += position[map.phi1(map.phi1(e))] / 16;
		e = map.phi2(map.phi3(map.phi2(map.phi_1(e))));
	}while(e != d);


	map.setCurrentLevel(cur);

//	VEC3 res;
//	unsigned int count = 0;
//
//	Dart e = d;
//	do
//	{
//		res += Algo::Geometry::volumeCentroid<PFP>(map,e,position);
//		++count;
//		e = map.phi2(map.phi3(map.phi2(map.phi_1(e))));
//	}while(e != d);
//
//	res /= typename PFP::REAL(count);

	return res;
}

template <typename PFP>
Dart subdivideFaceTri(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position)
{
	assert(map.getDartLevel(d) <= map.getCurrentLevel() || !"Access to a dart introduced after current level") ;
	assert(!map.faceIsSubdivided(d) || !"Trying to subdivide an already subdivided face") ;

	unsigned int fLevel = map.faceLevel(d) ;
	Dart old = map.faceOldestDart(d) ;

	unsigned int cur = map.getCurrentLevel() ;
	map.setCurrentLevel(fLevel) ;		// go to the level of the face to subdivide its edges

	Dart d1 = map.phi1(d);

	typename PFP::VEC3 center = Algo::Geometry::faceCentroidGen<PFP, typename PFP::TVEC3, typename PFP::VEC3>(map, d, position);	// compute center


	map.splitFace(d, d1) ;
	map.cutEdge(map.phi_1(d)) ;
	Dart x = map.phi2(map.phi_1(d)) ;
	Dart dd = map.template phi<111>(x) ;
	while(dd != x)
	{
		Dart next = map.phi1(dd) ;
		map.splitFace(dd, map.phi1(x)) ;
		dd = next ;
	}

	position[map.phi2(x)] = center;

	map.setCurrentLevel(cur) ;

	return map.phi2(x);	// Return a dart of the central vertex
}

template <typename PFP>
void subdivideVolumeTri(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position)
{
//	assert(map.getDartLevel(d) <= map.getCurrentLevel() || !"Access to a dart introduced after current level") ;
//	assert(!map.volumeIsSubdivided(d) || !"Trying to subdivide an already subdivided volume") ;
//
//	unsigned int vLevel = map.volumeLevel(d) ;
//	Dart old = map.volumeOldestDart(d) ;
//
//
//	unsigned int cur = map.getCurrentLevel() ;
//	map.setCurrentLevel(vLevel) ;		// go to the level of the face to subdivide its edges
//
//	/*
//	 * ou niveau du volume courant i
//	 * stockage d'un brin de chaque face de celui-ci
//	 */
//
//	DartMarkerStore mf(map);		// Lock a face marker
//
//	//Store faces that are traversed and start with the face of d
//	std::vector<Dart> visitedFaces;
//	visitedFaces.reserve(20);
//	visitedFaces.push_back(old);
//
//	//Store the edges before the cutEdge
//	std::vector<Dart> oldEdges;
//	oldEdges.reserve(20);
//
//	mf.markOrbit(FACE_ORBIT, old) ;
//	for(std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
//	{
//		Dart e = *face ;
//
//		do	// add all face neighbours to the table
//		{
//
//			oldEdges.push_back(e);
//
//			Dart ee = map.phi2(e) ;
//			if(!mf.isMarked(ee)) // not already marked
//			{
//				visitedFaces.push_back(ee) ;
//				mf.markOrbit(FACE_ORBIT, ee) ;
//			}
//			e = map.phi1(e) ;
//		} while(e != *face) ;
//	}
//
//	//First step : subdivide edges and faces
//	//creates a i+1 edge level and i+1 face level
//	for (std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
//	{
//		Dart d = *face;
//
//		std::cout << "subdiv de toutes les faces" << std::endl;
//		if(!map.faceIsSubdivided(d))
//		{
//			Dart cf = Algo::IHM::subdivideFaceTri<PFP>(map, *face, position);
//		}
//	}


//	map.setCurrentLevel(vLevel + 1) ; // go to the next level to perform volume subdivision
//
//	std::vector<Dart> newEdges;	//save darts from inner edges
//	newEdges.reserve(100);
//
//	//Second step : deconnect each corner and close the hole
//	for (std::vector<Dart>::iterator edge = oldEdges.begin(); edge != oldEdges.end(); ++edge)
//	{
//		Dart e = *edge;
//
//		do
//		{
//			map.unsewFaces(map.phi1(map.phi1(e)));
//			map.unsewFaces(map.phi1(e));
//			e = map.phi2(map.phi_1(e));
//		}while(e != *edge);
//
//		map.closeHole(map.phi1(e));
//
//		Dart old = map.phi1(map.phi2(map.phi1(e)));
//		Dart dd = map.phi1(old) ;
//		map.splitFace(dd, map.phi1(map.phi1(dd))) ;
//
//		Dart ne = map.phi2(map.phi_1(dd));
//		Dart ne2 = map.phi2(ne);
//
//		map.cutEdge(ne) ;
//		position[map.phi2(ne)] = volCenter;
//
//		newEdges.push_back(ne);
//		newEdges.push_back(map.phi1(ne));
//
//		dd = map.phi1(map.phi1(map.phi1(map.phi1(ne)))) ;
//		while(dd != ne)
//		{
//			Dart next = map.phi1(map.phi1(dd)) ;
//			map.splitFace(map.phi1(ne), dd) ;
//
//			newEdges.push_back(ne);
//			newEdges.push_back(map.phi1(ne));
//
//			dd = next ;
//		}
//	}
//
//	for (std::vector<Dart>::iterator nvol = quadfaces.begin(); nvol != quadfaces.end(); nvol = nvol + 2)
//	{
//		if(map.phi3(map.phi2(*nvol)) == map.phi2(*nvol) && map.phi3(map.phi2(*(nvol+1))) == map.phi2(*(nvol+1)))
//		{
//			//id pour toutes les faces interieures
//			map.sewVolumes(map.phi2(*nvol), map.phi2(*(nvol+1)));
//			unsigned int idface = map.getNewFaceId();
//			map.setFaceId(map.phi2(*nvol),idface, FACE_ORBIT);
//		}
//
//		//id pour toutes les aretes exterieurs des faces quadrangulees
//		unsigned int idedge = map.getEdgeId(*nvol);
//		map.setEdgeId(map.phi2(*nvol), idedge, DART_ORBIT);
//		map.setEdgeId( map.phi2(*(nvol+1)), idedge, DART_ORBIT);
//	}
//
//	//manque id pour les aretes interieurs : 6
//	DartMarker mne(map);
//	for(std::vector<Dart>::iterator it = newEdges.begin() ; it != newEdges.end() ; ++it)
//	{
//		if(!mne.isMarked(*it))
//		{
//			unsigned int idedge = map.getNewEdgeId();
//			map.setEdgeId(*it, idedge, EDGE_ORBIT);
//			mne.markOrbit(EDGE_ORBIT,*it);
//			std::cout << "plop" << std::endl;
//		}
//	}
//	std::cout << std::endl;

//	map.setCurrentLevel(cur) ;
}


} //namespace IHM

} //namespace Algo

} //namespace CGoGN

/*
 	//Second step : deconnect the corners
	for (std::vector<Dart>::iterator edge = oldEdges.begin(); edge != oldEdges.end(); ++edge)
	{
		Dart e = *edge;

		do
		{
			map.unsewFaces(map.phi1(map.phi1(e)));
			map.unsewFaces(map.phi1(e));
			e = map.phi2(map.phi_1(e));
		}while(e != *edge);

//		map.unsewFaces(map.phi1(*edge));
//		moe.markOrbit(DART_ORBIT,map.phi1(*edge));
//	}
//
//	//Third step : close the hole
//	for (std::vector<Dart>::iterator edge = oldEdges.begin(); edge != oldEdges.end(); ++edge)
//	{
//		if(moe.isMarked(map.phi1(*edge)))
//		{
			map.closeHole(map.phi1(*edge));

			Dart h = map.phi2(map.phi1(*edge));
			unsigned int degree=0;
			do
			{
				map.setEdgeId(h,map.getEdgeId(map.phi2(h)), DART_ORBIT) ;
				h = map.phi1(h);
				++degree;
			}while(h != map.phi2(map.phi1(*edge)));


//			moe.unmark(map.phi1(*edge));
//			moe.unmark(map.phi1(map.phi2(map.phi_1(*edge))));
//			moe.unmark(map.phi1(map.phi1(map.phi2(*edge))));


			//si nouvelle diff de tri alors subdiviser la nouvelle face
			if(degree != 3)
			{
				Dart old = map.phi1(map.phi2(map.phi1(*edge)));
				Dart dd = map.phi1(old) ;
				map.splitFace(dd, map.phi1(map.phi1(dd))) ;

				Dart ne = map.phi2(map.phi_1(dd));
				Dart ne2 = map.phi2(ne);

				map.cutEdge(ne) ;
				//unsigned int id = map.getNewEdgeId() ;
				//map.setEdgeId(ne, id, EDGE_ORBIT) ;

				//id = map.getNewEdgeId() ;
				//map.setEdgeId(ne2, id, EDGE_ORBIT) ;

				position[map.phi2(ne)] = volCenter;

				dd = map.phi1(map.phi1(map.phi1(map.phi1(ne)))) ;
				while(dd != ne)
				{
					Dart next = map.phi1(map.phi1(dd)) ;
					map.splitFace(map.phi1(ne), dd) ;
					Dart nne = map.phi2(map.phi_1(dd)) ;

					//id = map.getNewEdgeId() ;
					//map.setEdgeId(nne, id, EDGE_ORBIT) ;

					dd = next ;
				}
			}
		//}
	}

//	moe.unmarkAll();

	for (std::vector<Dart>::iterator nvol = quadfaces.begin(); nvol != quadfaces.end(); nvol = nvol + 2)
	{
		if(map.phi3(map.phi2(*nvol)) == map.phi2(*nvol) && map.phi3(map.phi2(*(nvol+1))) == map.phi2(*(nvol+1)))
		{
			map.sewVolumes(map.phi2(*nvol), map.phi2(*(nvol+1)));
			unsigned int idface = map.getNewFaceId();
			map.setFaceId(map.phi2(*nvol),idface, FACE_ORBIT);

			unsigned int idedge = map.getNewEdgeId();
			map.setEdgeId(map.phi1(map.phi2(*nvol)), idedge, EDGE_ORBIT);
		}
	}

//	Dart ec = quadfaces.front();
//	unsigned int idedge = map.getNewEdgeId();
//	map.setEdgeId(map.phi1(map.phi1(ec)), idedge, EDGE_ORBIT);
//
//	ec = map.phi_1(map.phi2(map.phi3(map.phi2(map.phi1(ec)))));
//	idedge = map.getNewEdgeId();
//	map.setEdgeId(ec, idedge, EDGE_ORBIT);
 */

