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

#include "Algo/Geometry/centroid.h"
#include "Algo/Modelisation/subdivision.h"
#include "Algo/Modelisation/extrusion.h"

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
	map.setEdgeId(map.phi1(d), eId, EDGE) ; //mise a jour de l'id d'arrete sur chaque moitie d'arete
	map.setEdgeId(map.phi1(dd), eId, EDGE) ;


	map.setFaceId(EDGE, d) ; //mise a jour de l'id de face sur chaque brin de chaque moitie d'arete
	map.setFaceId(EDGE, dd) ;

	position[map.phi1(d)] = (p1 + p2) * typename PFP::REAL(0.5) ;

	map.setCurrentLevel(cur) ;
}

template <typename PFP>
void subdivideFace(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position)
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

	if(degree == 3)	//subdiviser une face triangulaire
	{
		Dart dd = map.phi1(old) ;
		Dart e = map.phi1(map.phi1(dd)) ;
		map.splitFace(dd, e) ;					// insert a new edge
		unsigned int id = map.getNewEdgeId() ;
		map.setEdgeId(map.phi_1(dd), id, EDGE) ;		// set the edge id of the inserted edge to the next available id

		unsigned int idface = map.getFaceId(old);
		map.setFaceId(dd, idface, FACE) ;
		map.setFaceId(e, idface, FACE) ;

		dd = e ;
		e = map.phi1(map.phi1(dd)) ;
		map.splitFace(dd, e) ;
		id = map.getNewEdgeId() ;
		map.setEdgeId(map.phi_1(dd), id, EDGE) ;

		map.setFaceId(dd, idface, FACE) ;
		map.setFaceId(e, idface, FACE) ;

		dd = e ;
		e = map.phi1(map.phi1(dd)) ;
		map.splitFace(dd, e) ;
		id = map.getNewEdgeId() ;
		map.setEdgeId(map.phi_1(dd), id, EDGE) ;

		map.setFaceId(dd, idface, FACE) ;
		map.setFaceId(e, idface, FACE) ;
	}
	else
	{
		Dart dd = map.phi1(old) ;
		map.splitFace(dd, map.phi1(map.phi1(dd))) ;

		Dart ne = map.phi2(map.phi_1(dd));
		Dart ne2 = map.phi2(ne);

		map.cutEdge(ne) ;
		unsigned int id = map.getNewEdgeId() ;
		map.setEdgeId(ne, id, EDGE) ;
		id = map.getNewEdgeId() ;
		map.setEdgeId(ne2, id, EDGE) ;

		position[map.phi2(ne)] = p ;

		dd = map.phi1(map.phi1(map.phi1(map.phi1(ne)))) ;
		while(dd != ne)
		{
			Dart next = map.phi1(map.phi1(dd)) ;
			map.splitFace(map.phi1(ne), dd) ;
			Dart nne = map.phi2(map.phi_1(dd)) ;

			id = map.getNewEdgeId() ;
			map.setEdgeId(nne, id, EDGE) ;

			dd = next ;
		}

		unsigned int idface = map.getFaceId(old);
		Dart e = dd;
		do
		{
			map.setFaceId(dd, idface, DART) ;
			map.setFaceId(map.phi2(dd), idface, DART) ;
			dd = map.phi2(map.phi1(dd));
		}
		while(dd != ne);
	}

	map.setCurrentLevel(cur) ;
}

template <typename PFP>
Dart subdivideVolumeGen(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position)
{
	assert(map.getDartLevel(d) <= map.getCurrentLevel() || !"Access to a dart introduced after current level") ;
	assert(!map.volumeIsSubdivided(d) || !"Trying to subdivide an already subdivided volume") ;

	unsigned int vLevel = map.volumeLevel(d);
	Dart old = map.volumeOldestDart(d);

	unsigned int cur = map.getCurrentLevel();
	map.setCurrentLevel(vLevel);

	/*
	 * au niveau du volume courant i
	 * stockage d'un brin de chaque face de celui-ci
	 * avec calcul du centroid
	 */

	DartMarkerStore mf(map);		// Lock a face marker to save one dart per face
	CellMarker mv(map, VERTEX);

	typename PFP::VEC3 volCenter;
	unsigned count = 0 ;

	//Store faces that are traversed and start with the face of d
	std::vector<Dart> visitedFaces;
	visitedFaces.reserve(20);
	visitedFaces.push_back(old);

	//Store the edges before the cutEdge
	std::vector<Dart> oldEdges;
	oldEdges.reserve(20);

	mf.markOrbit(FACE, old) ;

	for(std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;
		do
		{
			//add one old edge per vertex to the old edge list
			//compute volume centroid
			if(!mv.isMarked(e))
			{
				mv.mark(e);
				volCenter += position[e];
				++count;
				oldEdges.push_back(e);
			}

			// add all face neighbours to the table
			Dart ee = map.phi2(e) ;
			if(!mf.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mf.markOrbit(FACE, ee) ;
			}

			e = map.phi1(e) ;
		} while(e != *face) ;
	}

	volCenter /= typename PFP::REAL(count) ;

	/*
	 * Subdivision
	 */
	//Store the darts from quadrangulated faces
	std::vector<std::pair<Dart,Dart> > subdividedfacesQ;
	subdividedfacesQ.reserve(25);

	std::vector<std::pair<Dart,Dart> > subdividedfacesT;
	subdividedfacesT.reserve(25);


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



		//test si la face est triangulaire ou non
		if(map.phi1(map.phi1(map.phi1(d))) == d)
		{
			std::cout << "trian" << std::endl;
			Dart cf = map.phi2(map.phi1(d));
			Dart e = cf;
			do
			{
				subdividedfacesT.push_back(std::pair<Dart,Dart>(e,map.phi2(e)));
				e = map.phi1(e);
			}while (e != cf);
		}
		else
		{
			std::cout << "quad" << std::endl;
			Dart cf = map.phi1(d);
			Dart e = cf;
			do
			{
				subdividedfacesQ.push_back(std::pair<Dart,Dart>(e,map.phi2(e)));
				e = map.phi2(map.phi1(e));
			}while (e != cf);


		}

		map.setCurrentLevel(cur);
	}

	map.setCurrentLevel(vLevel + 1) ; // go to the next level to perform volume subdivision

	std::vector<Dart> newEdges;	//save darts from inner edges
	newEdges.reserve(50);

	//Second step : deconnect each corner, close each hole, subdivide each new face into 3
	for (std::vector<Dart>::iterator edge = oldEdges.begin(); edge != oldEdges.end(); ++edge)
	{
		//std::vector<Dart>::iterator edge = oldEdges.begin();
		Dart e = *edge;

		Dart f1 = map.phi1(*edge);
		Dart f2 = map.phi2(f1);

		do
		{
			if(map.phi1(map.phi1(map.phi1(e))) != e)
			{
				map.unsewFaces(map.phi1(map.phi1(e))); //remplacer par une boucle qui découd toute la face et non juste une face carre (jusqu'a phi_1(e))
			}

			map.unsewFaces(map.phi1(e));


			e = map.phi2(map.phi_1(e));
		}
		while(e != *edge);

		map.closeHole(f1);

		//degree du sommet exterieur
		unsigned int cornerDegree = map.Map2::vertexDegree(*edge);

		//tourner autour du sommet pour connaitre le brin d'un sommet de valence < cornerDegree
		bool found = false;
		Dart stop = e;
		do
		{

			if(map.Map2::vertexDegree(map.phi2(map.phi1(e))) < cornerDegree)
			{
				stop = map.phi2(map.phi1(e));
				found = true;
			}

			e = map.phi2(map.phi_1(e));
		}
		while(!found && e != *edge);

		//si il existe un sommet de degre inferieur au degree du coin
		if(found)
		{
			//chercher le brin de faible degree suivant
			bool found2 = false;
			Dart dd = map.phi1(stop);

			do
			{
				if(map.Map2::vertexDegree(dd) < cornerDegree)
					found2 = true;
				else
					dd = map.phi1(dd);
			}
			while(!found2);

			//cas de la pyramide
			if(dd == stop)
			{
				std::cout << "pyramide" << std::endl;
				map.splitFace(dd, map.phi1(map.phi1(dd)));
			}
			else
			{
				map.splitFace(dd, stop);

				//calcul de la taille des faces de chaque cote de stop
				if(!( (map.Map2::faceDegree(map.phi_1(stop)) == 3 && map.Map2::faceDegree(map.phi2(map.phi_1(stop))) == 4) ||
						(map.Map2::faceDegree(map.phi_1(stop)) == 4 && map.Map2::faceDegree(map.phi2(map.phi_1(stop))) == 3) ))
				{
					std::cout << "octaedre ou hexaedre" << std::endl;

					Dart ne = map.phi_1(stop) ;
					map.cutEdge(ne);
					position[map.phi1(ne)] = volCenter;
					stop = map.phi2(map.phi1(ne));

					bool finished = false;
					Dart it = map.phi2(ne);

					do
					{
						//chercher le brin de faible degree suivant
						bool found2 = false;
						Dart dd = map.phi1(it);

						do
						{
							if(dd == stop)
								finished = true;
							else if(map.Map2::vertexDegree(dd) < cornerDegree)
								found2 = true;
							else
								dd = map.phi1(dd);
						}
						while(!found2 & !finished);

						if(found2)
						{
							map.splitFace(it,dd);
						}

						it = map.phi_1(dd);

						if(it == stop)
							finished = true;

					}
					while(!finished);

				}
				else
				{
					std::cout << "prisme" << std::endl;
					//tester si besoin de fermer f2 (par exemple pas besoin pour hexa... mais pour tet, octa, prisme oui)
					//map.closeHole(f2);
				}

			}

		}
		//sinon cas du tetraedre
		else
		{
			std::cout << "tetraedre" << std::endl;
			//tester si besoin de fermer f2 (par exemple pas besoin pour hexa... mais pour tet, octa, prisme oui)
			//map.closeHole(f2);
		}

	}

	std::cout << "1ere etape finished" << std::endl;

	CellMarker mtf(map, FACE);

	//Etape 2
	for (std::vector<std::pair<Dart,Dart> >::iterator edges = subdividedfacesT.begin(); edges != subdividedfacesT.end(); ++edges)
	{
		Dart f1 = (*edges).first;
		Dart f2 = (*edges).second;

//Fonction isTetrahedron ??
//		//if(Algo::Modelisation::Tetrahedron::isTetrahedron<PFP>(map,f2))
		if(  (map.Map2::faceDegree(f2) == 3 && map.Map2::faceDegree(map.phi2(f2)) == 3 &&
				map.Map2::faceDegree(map.phi2(map.phi_1(f2))) == 3) && map.Map2::vertexDegree(f2) == 3)
		{ //cas du tetrahedre

			std::cout << "ajout d'une face" << std::endl;

			if(map.phi3(map.phi2(f2)) == map.phi2(f2))
			{
				Dart nf = map.newFace(3);
				map.sewVolumes(map.phi2(f2),nf);
			}

			if(map.phi2(map.phi3(map.phi2(f2))) == map.phi3(map.phi2(f2)))
			{
				map.sewFaces(map.phi3(map.phi2(f2)), f1);
			}
		}
		else
		{
			if(!mtf.isMarked(f1))
			{
				mtf.mark(f1);

				map.closeHole(f1);

				if(map.Map2::faceDegree(map.phi2(f2)) == 3)
				{
					std::cout << "ajout d'un tetraedre" << std::endl;
					Dart x = Algo::Modelisation::trianguleFace<PFP>(map, map.phi2(f1));
					position[x] = volCenter;
				}
				else
				{
					std::cout << "ajout d'un prisme" << std::endl;
					//Dart x = Algo::Modelisation::extrudeFace<PFP>(map,position,map.phi2(f1),5.0);
					Dart c = Algo::Modelisation::trianguleFace<PFP>(map, map.phi2(f1));

					Dart cc = c;
					// cut edges
					do
					{

						typename PFP::VEC3 p1 = position[cc] ;
						typename PFP::VEC3 p2 = position[map.phi1(cc)] ;

						map.cutEdge(cc);

						position[map.phi1(cc)] = (p1 + p2) * typename PFP::REAL(0.5) ;

						cc = map.phi2(map.phi_1(cc));
					}while (cc != c);

					// cut faces
					do
					{
						Dart d1 = map.phi1(cc);
						Dart d2 = map.phi_1(cc);
						map.splitFace(d1,d2);
						cc = map.phi2(map.phi_1(cc));//map.Map2::alpha1(cc);
					}while (cc != c);

					//merge central faces by removing edges
					bool notFinished=true;
					do
					{
						Dart d1 = map.Map2::alpha1(cc);
						if (d1 == cc)			// last edge is pending edge inside of face
							notFinished = false;
						map.deleteFace(cc);
						cc = d1;
					} while (notFinished);


					map.closeHole(map.phi1(map.phi1(map.phi2(f1))));

				}
			}
		}

	}

	std::cout << "2e etape finished" << std::endl;


//	{
//		//Third step : 3-sew internal faces
//		for (std::vector<std::pair<Dart,Dart> >::iterator it = subdividedfacesT.begin(); it != subdividedfacesT.end(); ++it)
//		{
//			Dart f1 = (*it).first;
//			Dart f2 = (*it).second;
//
//
//
//			if(map.phi3(map.phi2(f1)) == map.phi2(f1) && map.phi3(map.phi2(f2)) == map.phi2(f2))
//			{
//				if(map.getEmbedding(VERTEX, map.phi_1(map.phi2(f2))) == map.getEmbedding(VERTEX, map.phi_1(map.phi2(f1))))
//				{
//					map.Map3::sewVolumes(map.phi2(f2), map.phi2(f1));
//				}
//				else
//				{
//
//				//id pour toutes les faces interieures
//				map.sewVolumes(map.phi2(f2), map.phi2(f1));
//
//
//				}
//
//				//Fais a la couture !!!!!
//				unsigned int idface = map.getNewFaceId();
//				map.setFaceId(map.phi2(f1),idface, FACE);
//			}
//
//
//			//FAIS a la couture !!!!!!!
//			//id pour toutes les aretes exterieurs des faces quadrangulees
//			unsigned int idedge = map.getEdgeId(f1);
//			map.setEdgeId(map.phi2(f1), idedge, DART);
//			map.setEdgeId( map.phi2(f2), idedge, DART);
//
//		}
//
//		//LA copie de L'id est a gerer avec le sewVolumes normalement !!!!!!
//		//id pour les aretes interieurs : (i.e. 16 pour un octa)
//		DartMarker mne(map);
//		for(std::vector<Dart>::iterator it = newEdges.begin() ; it != newEdges.end() ; ++it)
//		{
//			if(!mne.isMarked(*it))
//			{
//				unsigned int idedge = map.getNewEdgeId();
//				map.setEdgeId(*it, idedge, EDGE);
//				mne.markOrbit(EDGE,*it);
//			}
//		}
//	}
//
//	{
//		//Third step : 3-sew internal faces
//		for (std::vector<std::pair<Dart,Dart> >::iterator it = subdividedfacesQ.begin(); it != subdividedfacesQ.end(); ++it)
//		{
//			Dart f1 = (*it).first;
//			Dart f2 = (*it).second;
//
//			if(map.phi3(map.phi2(f1)) == map.phi2(f1) && map.phi3(map.phi2(f2)) == map.phi2(f2))
//			{
//				//id pour toutes les faces interieures
//				map.sewVolumes(map.phi2(f2), map.phi2(f1));
//
//				//Fais a la couture !!!!!
//				unsigned int idface = map.getNewFaceId();
//				map.setFaceId(map.phi2(f1),idface, FACE);
//			}
//
//			//FAIS a la couture !!!!!!!
//			//id pour toutes les aretes exterieurs des faces quadrangulees
//			unsigned int idedge = map.getEdgeId(f1);
//			map.setEdgeId(map.phi2(f1), idedge, DART);
//			map.setEdgeId( map.phi2(f2), idedge, DART);
//		}
//		//LA copie de L'id est a gerer avec le sewVolumes normalement !!!!!!
//		//id pour les aretes interieurs : (i.e. 16 pour un octa)
//		DartMarker mne(map);
//		for(std::vector<Dart>::iterator it = newEdges.begin() ; it != newEdges.end() ; ++it)
//		{
//			if(!mne.isMarked(*it))
//			{
//				unsigned int idedge = map.getNewEdgeId();
//				map.setEdgeId(*it, idedge, EDGE);
//				mne.markOrbit(EDGE,*it);
//			}
//		}
//	}
//
//
//	//cas tordu pour le prisme
//	for (std::vector<std::pair<Dart,Dart> >::iterator it = subdividedfacesT.begin(); it != subdividedfacesT.end(); ++it)
//	{
//		Dart f1 = (*it).first;
//		Dart f2 = (*it).second;
//
//		if(  !(map.Map2::faceDegree(f2) == 3 && map.Map2::faceDegree(map.phi2(f2)) == 3 &&
//				map.Map2::faceDegree(map.phi2(map.phi1(f2))) == 3 && map.Map2::faceDegree(map.phi2(map.phi_1(f2))) == 3))
//		{
//
//
//			if(map.phi2(map.phi1(map.phi1(map.phi2(f1)))) == map.phi3(map.phi2(map.phi1(map.phi1(map.phi2(f1))))) &&
//					map.phi2(map.phi3(map.phi2(map.phi3(map.phi2(map.phi3(map.phi2(map.phi2(map.phi1(map.phi1(map.phi2(f1))))))))))) ==
//							map.phi3(map.phi2(map.phi3(map.phi2(map.phi3(map.phi2(map.phi3(map.phi2(map.phi2(map.phi1(map.phi1(map.phi2(f1))))))))))))
//			)
//			{
//				map.sewVolumes(map.phi2(map.phi1(map.phi1(map.phi2(f1)))),
//						map.phi2(map.phi3(map.phi2(map.phi3(map.phi2(map.phi3(map.phi1(map.phi1(map.phi2(f1))))))))));
//			}
//
//		}
//
//	}


	map.setCurrentLevel(cur) ;

	return subdividedfacesQ.begin()->first;
}




template <typename PFP>
Dart subdivideVolume(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position)
{
	assert(map.getDartLevel(d) <= map.getCurrentLevel() || !"Access to a dart introduced after current level") ;
	assert(!map.volumeIsSubdivided(d) || !"Trying to subdivide an already subdivided volume") ;
	assert(!map.neighborhoodLevelDiffersByOne(d) || !"Trying to subdivide a volume with neighborhood Level difference greater than 1");

	unsigned int vLevel = map.volumeLevel(d);
	Dart old = map.volumeOldestDart(d);

	unsigned int cur = map.getCurrentLevel();
	map.setCurrentLevel(vLevel);

	/*
	 * au niveau du volume courant i
	 * stockage d'un brin de chaque face de celui-ci
	 * avec calcul du centroid
	 */

	DartMarkerStore mf(map);		// Lock a face marker to save one dart per face
	CellMarker mv(map, VERTEX);

	typename PFP::VEC3 volCenter;
	unsigned count = 0 ;

	//Store faces that are traversed and start with the face of d
	std::vector<Dart> visitedFaces;
	visitedFaces.reserve(20);
	visitedFaces.push_back(old);

	//Store the edges before the cutEdge
	std::vector<Dart> oldEdges;
	oldEdges.reserve(20);

	mf.markOrbit(FACE, old) ;

	for(std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;
		do
		{
			//add one old edge per vertex to the old edge list
			//compute volume centroid
			if(!mv.isMarked(e))
			{
				mv.mark(e);
				volCenter += position[e];
				++count;
				oldEdges.push_back(e);
			}

			// add all face neighbours to the table
			Dart ee = map.phi2(e) ;
			if(!mf.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mf.markOrbit(FACE, ee) ;
			}

			e = map.phi1(e) ;
		} while(e != *face) ;
	}

	volCenter /= typename PFP::REAL(count) ;

	/*
	 * Subdivision
	 */
	//Store the darts from quadrangulated faces
	std::vector<std::pair<Dart,Dart> > subdividedfacesQ;
	subdividedfacesQ.reserve(25);

	std::vector<std::pair<Dart,Dart> > subdividedfacesT;
	subdividedfacesT.reserve(25);


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



		//test si la face est triangulaire ou non
		if(map.phi1(map.phi1(map.phi1(d))) == d)
		{
			std::cout << "trian" << std::endl;
			Dart cf = map.phi2(map.phi1(d));
			Dart e = cf;
			do
			{
				subdividedfacesT.push_back(std::pair<Dart,Dart>(e,map.phi2(e)));
				e = map.phi1(e);
			}while (e != cf);
		}
		else
		{
			std::cout << "quad" << std::endl;
			Dart cf = map.phi1(d);
			Dart e = cf;
			do
			{
				subdividedfacesQ.push_back(std::pair<Dart,Dart>(e,map.phi2(e)));
				e = map.phi2(map.phi1(e));
			}while (e != cf);


		}

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
			if(map.phi1(map.phi1(map.phi1(e))) != e)
				map.unsewFaces(map.phi1(map.phi1(e)));

			map.unsewFaces(map.phi1(e));


			e = map.phi2(map.phi_1(e));
		}
		while(e != *edge);

		map.closeHole(f1);


		//fonction qui calcule le degree max des faces atour d'un sommet
		unsigned int fdeg = map.faceDegree(map.phi2(f1));


		if(fdeg > 4)
		{
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
		else if(fdeg > 3)
		{
			map.closeHole(f2);
			map.sewVolumes(map.phi2(f1),map.phi2(f2));

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
			map.closeHole(f2);
			map.sewVolumes(map.phi2(f1),map.phi2(f2));

			unsigned int idface = map.getNewFaceId();
			map.setFaceId(map.phi2(f1),idface, FACE);
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
				map.setFaceId(map.phi2(f3),idface, FACE);


				f = map.phi2(map.phi_1(f));
			}while(f != x);

		}

	}

	{
	//Third step : 3-sew internal faces
	for (std::vector<std::pair<Dart,Dart> >::iterator it = subdividedfacesT.begin(); it != subdividedfacesT.end(); ++it)
	{
		Dart f1 = (*it).first;
		Dart f2 = (*it).second;

		//FAIS a la couture !!!!!!!
		//id pour toutes les aretes exterieurs des faces quadrangulees
		unsigned int idedge = map.getEdgeId(f1);
		map.setEdgeId(map.phi2(f1), idedge, DART);
		map.setEdgeId( map.phi2(f2), idedge, DART);

	}

	//LA copie de L'id est a gerer avec le sewVolumes normalement !!!!!!
	//id pour les aretes interieurs : (i.e. 16 pour un octa)
	DartMarker mne(map);
	for(std::vector<Dart>::iterator it = newEdges.begin() ; it != newEdges.end() ; ++it)
	{
		if(!mne.isMarked(*it))
		{
			unsigned int idedge = map.getNewEdgeId();
			map.setEdgeId(*it, idedge, EDGE);
			mne.markOrbit(EDGE,*it);
		}
	}
	}

	{
	//Third step : 3-sew internal faces
	for (std::vector<std::pair<Dart,Dart> >::iterator it = subdividedfacesQ.begin(); it != subdividedfacesQ.end(); ++it)
	{
		Dart f1 = (*it).first;
		Dart f2 = (*it).second;

		if(map.phi3(map.phi2(f1)) == map.phi2(f1) && map.phi3(map.phi2(f2)) == map.phi2(f2))
		{
			//id pour toutes les faces interieures
			map.sewVolumes(map.phi2(f1), map.phi2(f2));

			//Fais a la couture !!!!!
			unsigned int idface = map.getNewFaceId();
			map.setFaceId(map.phi2(f1),idface, FACE);
		}

		//FAIS a la couture !!!!!!!
		//id pour toutes les aretes exterieurs des faces quadrangulees
		unsigned int idedge = map.getEdgeId(f1);
		map.setEdgeId(map.phi2(f1), idedge, DART);
		map.setEdgeId( map.phi2(f2), idedge, DART);
	}

	//LA copie de L'id est a gerer avec le sewVolumes normalement !!!!!!
	//id pour les aretes interieurs : (i.e. 6 pour un hexa)
	DartMarker mne(map);
	for(std::vector<Dart>::iterator it = newEdges.begin() ; it != newEdges.end() ; ++it)
	{
		if(!mne.isMarked(*it))
		{
			unsigned int idedge = map.getNewEdgeId();
			map.setEdgeId(*it, idedge, EDGE);
			mne.markOrbit(EDGE,*it);
		}
	}
	}

	map.setCurrentLevel(cur) ;

	return subdividedfacesQ.begin()->first;
}


template <typename PFP>
Dart subdivideVolumeOld(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position)
{
	assert(map.getDartLevel(d) <= map.getCurrentLevel() || !"Access to a dart introduced after current level") ;
	assert(!map.volumeIsSubdivided(d) || !"Trying to subdivide an already subdivided volume") ;

	unsigned int vLevel = map.volumeLevel(d);
	Dart old = map.volumeOldestDart(d);

	unsigned int cur = map.getCurrentLevel();
	map.setCurrentLevel(vLevel);


	/*
	 * au niveau du volume courant i
	 * stockage d'un brin de chaque face de celui-ci
	 * avec calcul du centroid
	 */

	DartMarkerStore mf(map);		// Lock a face marker to save one dart per face
	CellMarker mv(map, VERTEX);

	typename PFP::VEC3 volCenter;
	unsigned count = 0 ;

	//Store faces that are traversed and start with the face of d
	std::vector<Dart> visitedFaces;
	visitedFaces.reserve(20);
	visitedFaces.push_back(old);

	//Store the edges before the cutEdge
	std::vector<Dart> oldEdges;
	oldEdges.reserve(20);

	mf.markOrbit(FACE, old) ;

	for(std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;
		do
		{
			//add one old edge per vertex to the old edge list
			//compute volume centroid
			if(!mv.isMarked(e))
			{
				mv.mark(e);
				volCenter += position[e];
				++count;
				oldEdges.push_back(e);
			}

			// add all face neighbours to the table
			Dart ee = map.phi2(e) ;
			if(!mf.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mf.markOrbit(FACE, ee) ;
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
				map.setFaceId(map.phi2(f1),idface, FACE);
		}

		//FAIS a la couture !!!!!!!
		//id pour toutes les aretes exterieurs des faces quadrangulees
		unsigned int idedge = map.getEdgeId(f1);
		map.setEdgeId(map.phi2(f1), idedge, DART);
		map.setEdgeId( map.phi2(f2), idedge, DART);
	}

	//LA copie de L'id est a gerer avec le sewVolumes normalement !!!!!!
	//id pour les aretes interieurs : (i.e. 6 pour un hexa)
	DartMarker mne(map);
	for(std::vector<Dart>::iterator it = newEdges.begin() ; it != newEdges.end() ; ++it)
	{
		if(!mne.isMarked(*it))
		{
			unsigned int idedge = map.getNewEdgeId();
			map.setEdgeId(*it, idedge, EDGE);
			mne.markOrbit(EDGE,*it);
		}
	}

	map.setCurrentLevel(cur) ;

	return subdividedfaces.begin()->first;
}
//
//template <typename PFP>
//void subdivideLoop(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position)
//{
//	assert(map.getDartLevel(d) <= map.getCurrentLevel() || !"Access to a dart introduced after current level") ;
//	assert(!map.volumeIsSubdivided(d) || !"Trying to subdivide an already subdivided volume") ;
//
//	unsigned int vLevel = map.volumeLevel(d) ;
//	Dart old = map.volumeOldestDart(d) ;
//
//	unsigned int cur = map.getCurrentLevel() ;
//	map.setCurrentLevel(vLevel) ;		// go to the level of the face to subdivide its edges
//
//
//	/*
//	 * au niveau du volume courant i
//	 * stockage d'un brin de chaque face de celui-ci
//	 * avec calcul du centroid
//	 */
//
//	DartMarkerStore mf(map);		// Lock a face marker to save one dart per face
//	DartMarkerStore mv(map);		// Lock a vertex marker to compute volume center
//
//	typename PFP::VEC3 volCenter;
//	unsigned count = 0 ;
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
//	mf.markOrbit(FACE, old) ;
//
//	for(std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
//	{
//		Dart e = *face ;
//		do
//		{
//			//add one old edge per vertex to the old edge list
//			//compute volume centroid
//			if(!mv.isMarked(e))
//			{
//				volCenter += position[e];
//				++count;
//				mv.markOrbit(VERTEX, e);
//				oldEdges.push_back(e);
//			}
//
//			// add all face neighbours to the table
//			Dart ee = map.phi2(e) ;
//			if(!mf.isMarked(ee)) // not already marked
//			{
//				visitedFaces.push_back(ee) ;
//				mf.markOrbit(FACE, ee) ;
//			}
//
//			e = map.phi1(e) ;
//		} while(e != *face) ;
//	}
//
//	volCenter /=  typename PFP::REAL(count) ;
//
//	/*
//	 * Subdivision
//	 */
//	//Store the darts from quadrangulated faces
//		std::vector<std::pair<Dart,Dart> > subdividedfaces;
//		subdividedfaces.reserve(25);
//
//		//First step : subdivide edges and faces
//		//creates a i+1 edge level and i+1 face level
//		for (std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
//		{
//			Dart d = *face;
//
//			//if needed subdivide face
//			if(!map.faceIsSubdivided(d))
//				Algo::IHM::subdivideFace<PFP>(map, d, position, Algo::IHM::S_TRI);
//
//			//save a dart from the subdivided face
//			unsigned int cur = map.getCurrentLevel() ;
//
//			unsigned int fLevel = map.faceLevel(d) + 1; //puisque dans tous les cas, la face est subdivisee
//			map.setCurrentLevel(fLevel) ;
//
//			//le brin est forcement du niveau cur
//			Dart cf = map.phi2(map.phi1(d));
//			Dart e = cf;
//			do
//			{
//				subdividedfaces.push_back(std::pair<Dart,Dart>(e,map.phi2(e)));
//				e = map.phi1(e);
//			}while (e != cf);
//
//			map.setCurrentLevel(cur);
//		}
//
//		map.setCurrentLevel(vLevel + 1) ; // go to the next level to perform volume subdivision
//
//		std::vector<Dart> newEdges;	//save darts from inner edges
//		newEdges.reserve(50);
//
//		bool istet = true;
//
//		//Second step : deconnect each corner, close each hole, subdivide each new face into 3
//		for (std::vector<Dart>::iterator edge = oldEdges.begin(); edge != oldEdges.end(); ++edge)
//		{
//			Dart e = *edge;
//
//			Dart f1 = map.phi1(*edge);
//			Dart f2 = map.phi2(f1);
//
//			do
//			{
//
//					map.unsewFaces(map.phi1(e));
//
//				e = map.phi2(map.phi_1(e));
//			}
//			while(e != *edge);
//
//			map.closeHole(f1);
//			map.closeHole(f2);
//			map.sewVolumes(map.phi2(f1),map.phi2(f2));
//
//			unsigned int fdeg = map.faceDegree(map.phi2(f1));
//
//			if(fdeg > 3)
//			{
//				istet = false;
//				Dart old = map.phi2(map.phi1(*edge));
//				Dart dd = map.phi1(old) ;
//				map.splitFace(old,dd) ;
//
//				Dart ne = map.phi1(old);
//
//				map.cutEdge(ne);
//				position[map.phi1(ne)] = volCenter; //plonger a la fin de la boucle ????
//				newEdges.push_back(ne);
//				newEdges.push_back(map.phi1(ne));
//
//				Dart stop = map.phi2(map.phi1(ne));
//				ne = map.phi2(ne);
//				do
//				{
//					dd = map.phi1(map.phi1(ne));
//
//					map.splitFace(ne, dd) ;
//
//					newEdges.push_back(map.phi1(dd));
//
//					ne = map.phi2(map.phi_1(ne));
//					dd = map.phi1(dd);
//				}
//				while(dd != stop);
//			}
//			else
//			{
//				unsigned int idface = map.getNewFaceId();
//				map.setFaceId(map.phi2(f1),idface, FACE);
//			}
//
//		}
//
//		if(!istet)
//		{
//			for (std::vector<Dart>::iterator edge = oldEdges.begin(); edge != oldEdges.end(); ++edge)
//			{
//				Dart e = *edge;
//
//				Dart x = map.phi_1(map.phi2(map.phi1(*edge)));
//				Dart f = x;
//
//				do
//				{
//					Dart f3 = map.phi3(f);
//					Dart tmp =  map.phi_1(map.phi2(map.phi_1(map.phi2(map.phi_1(f3)))));
//
//					map.unsewFaces(f3);
//					map.unsewFaces(tmp);
//					map.sewFaces(f3, tmp);
//
//					unsigned int idface = map.getNewFaceId();
//					map.setFaceId(map.phi2(f3),idface, FACE);
//
//
//					f = map.phi2(map.phi_1(f));
//				}while(f != x);
//
//			}
//		}
//
//		//Third step : 3-sew internal faces
//		for (std::vector<std::pair<Dart,Dart> >::iterator it = subdividedfaces.begin(); it != subdividedfaces.end(); ++it)
//		{
//			Dart f1 = (*it).first;
//			Dart f2 = (*it).second;
//
//			//FAIS a la couture !!!!!!!
//			//id pour toutes les aretes exterieurs des faces quadrangulees
//			unsigned int idedge = map.getEdgeId(f1);
//			map.setEdgeId(map.phi2(f1), idedge, DART);
//			map.setEdgeId( map.phi2(f2), idedge, DART);
//
//		}
//
//		//LA copie de L'id est a gerer avec le sewVolumes normalement !!!!!!
//		//id pour les aretes interieurs : (i.e. 16 pour un octa)
//		DartMarker mne(map);
//		for(std::vector<Dart>::iterator it = newEdges.begin() ; it != newEdges.end() ; ++it)
//		{
//			if(!mne.isMarked(*it))
//			{
//				unsigned int idedge = map.getNewEdgeId();
//				map.setEdgeId(*it, idedge, EDGE);
//				mne.markOrbit(EDGE,*it);
//			}
//		}
//
//
//		map.setCurrentLevel(cur) ;
//}


template <typename PFP>
void coarsenEdge(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position)
{
	assert(map.getDartLevel(d) <= map.getCurrentLevel() || !"Access to a dart introduced after current level") ;
	assert(map.edgeCanBeCoarsened(d) || !"Trying to coarsen an edge that can not be coarsened") ;

	unsigned int cur = map.getCurrentLevel() ;
	map.setCurrentLevel(cur + 1) ;
	map.uncutEdge(d) ;
	map.setCurrentLevel(cur) ;
}

template <typename PFP>
void coarsenFace(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position)
{
	assert(map.getDartLevel(d) <= map.getCurrentLevel() || !"Access to a dart introduced after current level") ;
	assert(map.faceIsSubdividedOnce(d) || !"Trying to coarsen a non-subdivided face or a more than once subdivided face") ;

	unsigned int cur = map.getCurrentLevel() ;

	unsigned int degree = 0 ;
	Dart fit = d ;
	do
	{
		++degree ;
		fit = map.phi1(fit) ;
	} while(fit != d) ;

	if(degree == 3)
	{
//		fit = d ;
//		do
//		{
//			map.setCurrentLevel(cur + 1) ;
//			Dart innerEdge = map.phi1(fit) ;
//			map.setCurrentLevel(map.getMaxLevel()) ;
//			map.mergeFaces(innerEdge) ;
//			map.setCurrentLevel(cur) ;
//			fit = map.phi1(fit) ;
//		} while(fit != d) ;
	}
	else
	{
		map.setCurrentLevel(cur + 1) ;
		Dart centralV = map.phi1(map.phi1(d)) ;
		map.setCurrentLevel(map.getMaxLevel()) ;
		map.deleteVertex(centralV) ;
		map.setCurrentLevel(cur) ;
	}

//	fit = d ;
//	do
//	{
//		if(map.edgeCanBeCoarsened(fit))
//			coarsenEdge<PFP>(map, fit, position) ;
//		fit = map.phi1(fit) ;
//	} while(fit != d) ;
}

template <typename PFP>
void coarsenVolume(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position)
{
	assert(map.getDartLevel(d) <= map.getCurrentLevel() || !"Access to a dart introduced after current level") ;
	//assert(map.volumeIsSubdivdedOnce(d) || !"Trying to coarsen a non-subdivided volume or a more than once subdivided volume") ;

	unsigned int cur = map.getCurrentLevel() ;

	/*
	 * au niveau du volume courant i
	 * stockage d'un brin de chaque face de celui-ci
	 * avec calcul du centroid
	 */

	DartMarkerStore mf(map);		// Lock a face marker to save one dart per face

	//Store faces that are traversed and start with the face of d
	std::vector<Dart> visitedFaces;
	visitedFaces.reserve(20);
	visitedFaces.push_back(d);

	mf.markOrbit(FACE, d) ;

	for(std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;
		do
		{
			// add all face neighbours to the table
			Dart ee = map.phi2(e) ;
			if(!mf.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mf.markOrbit(FACE, ee) ;
			}

			e = map.phi1(e) ;
		} while(e != *face) ;
	}

	/*
	 * fusionner tous les volumes internes
	 */
	map.setCurrentLevel(cur + 1) ;
	Dart centralV = map.phi_1(map.phi2(map.phi1(d)));
	map.deleteVertex(centralV) ;
	map.setCurrentLevel(cur) ;

	/*
	 * simplifier les faces
	 */
	for(std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart d = *face;

		if(map.faceIsSubdividedOnce(d))
		{
			std::cout << "once : " << d << std::endl;
			coarsenFace<PFP>(map, d, position);
		}
	}

	map.setCurrentLevel(cur) ;

	/*
	 * simplifier les aretes
	 */
	for(std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
			Dart fit = *face ;
			do
			{
				if(map.edgeCanBeCoarsened(fit))
				{
					std::cout << "edge coarsened = " << fit << std::endl;
				}
					//coarsenEdge<PFP>(map, fit, position) ;
				std::cout << "boucle = " << fit << std::endl;
				fit = map.phi1(fit) ;
			} while(fit != *face) ;
	}

	std::cout << "end" << std::endl;

	map.setCurrentLevel(cur) ;
}

/***********************************************************************************
 *												Raffinement
 ***********************************************************************************/
template <typename PFP>
void splitVolume(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position)
{
	assert(map.getDartLevel(d) <= map.getCurrentLevel() || !"Access to a dart introduced after current level") ;
	assert(!map.volumeIsSubdivided(d) || !"Trying to subdivide an already subdivided volume") ;

	unsigned int cur = map.getCurrentLevel() ;
	unsigned int vLevel = map.volumeLevel(d) ;
	map.setCurrentLevel(vLevel) ;

	// first cut the edges (if they are not already)
	Dart t = d;
	do
	{
		if(!map.edgeIsSubdivided(map.phi1(map.phi2(t))))
			Algo::IHM::subdivideEdge<PFP>(map, map.phi1(map.phi2(t)), position) ;
		t = map.phi1(t);
	}
	while(t != d);

	unsigned int fLevel = map.faceLevel(map.phi1(d));
	map.setCurrentLevel(fLevel+1) ;		// go to the level of the face to subdivide its edges
	Dart neighboordVolume = map.phi1(map.phi1(map.phi2(d)));
	//map.setCurrentLevel(cur) ; // go to the next level to perform volume subdivision

	//Split the faces and open the midlle
	do
	{
		Dart t2 = map.phi2(t);

		unsigned int fLevel = map.faceLevel(t2) ;
		map.setCurrentLevel(fLevel+1) ;		// go to the level of the face to subdivide its edges

		Dart face2 = map.phi1(map.phi1(t2));
		map.splitFace(map.phi_1(t2), face2);
		map.unsewFaces(map.phi1(map.phi1(t2)));

		//id de face pour les 2 nouveaux brins
		unsigned int idface = map.getFaceId(t2);
		map.setFaceId(map.phi1(map.phi1(t2)), idface, DART);
		map.setFaceId(map.phi_1(face2), idface, DART);

		t = map.phi1(t);
	}
	while(t != d);

	//close the middle to create volumes & sew them
	map.setCurrentLevel(vLevel + 1) ; // go to the next level to perform volume subdivision

	map.closeHole(map.phi1(map.phi1(map.phi2(d))));
	map.closeHole(map.phi_1(neighboordVolume));
	map.sewVolumes(map.phi2(map.phi1(map.phi1(map.phi2(d)))), map.phi2(map.phi_1(neighboordVolume)));

	unsigned int idface = map.getNewFaceId();
	map.setFaceId(map.phi2(map.phi_1(neighboordVolume)), idface, FACE);

	do
	{
		Dart t211 = map.phi1(map.phi1(map.phi2(t)));

		unsigned int idedge = map.getNewEdgeId();
		map.setEdgeId(t211, idedge, EDGE);
		t = map.phi1(t);
	}
	while(t != d);

	map.setCurrentLevel(cur) ;
}










/*************************************************************************************
 *
 */

//template <typename PFP>
//void subdivideVolume(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position, SubdivideType sType)
//{
//	assert(map.getDartLevel(d) <= map.getCurrentLevel() || !"Access to a dart introduced after current level") ;
//	assert(!map.volumeIsSubdivided(d) || !"Trying to subdivide an already subdivided volume") ;
//
//	unsigned int vLevel = map.volumeLevel(d) ;
//	Dart old = map.volumeOldestDart(d) ;
//
//	unsigned int cur = map.getCurrentLevel() ;
//	map.setCurrentLevel(vLevel) ;		// go to the level of the face to subdivide its edges
//
//
//	/*
//	 * au niveau du volume courant i
//	 * stockage d'un brin de chaque face de celui-ci
//	 * avec calcul du centroid
//	 */
//
//	DartMarkerStore mf(map);		// Lock a face marker to save one dart per face
//	DartMarkerStore mv(map);		// Lock a vertex marker to compute volume center
//
//	typename PFP::VEC3 volCenter;
//	unsigned count = 0 ;
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
//	mf.markOrbit(FACE, old) ;
//
//	for(std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
//	{
//		Dart e = *face ;
//		do
//		{
//			//add one old edge per vertex to the old edge list
//			//compute volume centroid
//			if(!mv.isMarked(e))
//			{
//				volCenter += position[e];
//				++count;
//				mv.markOrbit(VERTEX, e);
//				oldEdges.push_back(e);
//			}
//
//			// add all face neighbours to the table
//			Dart ee = map.phi2(e) ;
//			if(!mf.isMarked(ee)) // not already marked
//			{
//				visitedFaces.push_back(ee) ;
//				mf.markOrbit(FACE, ee) ;
//			}
//
//			e = map.phi1(e) ;
//		} while(e != *face) ;
//	}
//
//	volCenter /= typename PFP::REAL(count) ;
//
//	/*
//	 * Subdivision
//	 */
//
//	//Store the darts from quadrangulated faces
//	std::vector<std::pair<Dart,Dart> > subdividedTriFaces;
//	subdividedTriFaces.reserve(15);
//
//	std::vector<std::pair<Dart,Dart> > subdividedQuadFaces;
//	subdividedQuadFaces.reserve(15);
//
//	//First step : subdivide edges and faces
//	//creates a i+1 edge level and i+1 face level
//	for (std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
//	{
//		Dart d = *face;
//
//		//if needed subdivide face
//		if(!map.faceIsSubdivided(d))
//			Algo::IHM::subdivideFaceQuad<PFP>(map, d, position);
//
//		//save a dart from the subdivided face
//		unsigned int cur = map.getCurrentLevel() ;
//
//		unsigned int fLevel = map.faceLevel(d) + 1; //puisque dans tous les cas, la face est subdivisee
//		map.setCurrentLevel(fLevel) ;
//
//		//Face tri
//		if(map.phi1(map.phi1(map.phi1(d))) == d)
//		{
//
//			//le brin est forcement du niveau cur
//			Dart cf = map.phi2(map.phi1(d));
//			Dart e = cf;
//			do
//			{
//				subdividedTriFaces.push_back(std::pair<Dart,Dart>(e,map.phi2(e)));
//				e = map.phi1(e);
//			}while (e != cf);
//		}
//		else
//		{
//			//le brin est forcement du niveau cur
//			Dart cf = map.phi1(d);
//			Dart e = cf;
//			do
//			{
//				subdividedQuadFaces.push_back(std::pair<Dart,Dart>(e,map.phi2(e)));
//				e = map.phi2(map.phi1(e));
//			}while (e != cf);
//		}
//
//		map.setCurrentLevel(cur);
//	}
//
//	map.setCurrentLevel(vLevel + 1) ; // go to the next level to perform volume subdivision
//
//	//Second step : create corners
//	for (std::vector<Dart>::iterator edge = oldEdges.begin(); edge != oldEdges.end(); ++edge)
//	{
//		Dart e = *edge;
//
//		Dart f1 = map.phi1(*edge);
//		Dart f2 = map.phi2(map.phi1(*edge));
//
//		do
//		{
//			if(map.phi1(map.phi1(map.phi1(e))) != e)
//				map.unsewFaces(map.phi1(map.phi1(e)));
//			if(map.phi2(map.phi1(e)) != map.phi1(e))
//				map.unsewFaces(map.phi1(e));
//
//			e = map.phi2(map.phi_1(e));
//
//		}while(e != *edge);
//
//		map.closeHole(f1);
//		map.closeHole(f2);
//		map.sewVolumes(map.phi2(f1),map.phi2(f2));
//
//		//un passage sur chaque nouvelle face pour recopier l'id d'arete
//		Dart d = map.phi2(f1);
//		e = d;
//		do
//		{
//			unsigned int idedge = map.getEdgeId(map.phi2(e));
//			map.setEdgeId(e, idedge, EDGE);
//			e = map.phi1(e);
//		}while(e != d);
//
//
//		//Identifiant de face pour les 2 faces orientees
//		unsigned int idface = map.getNewFaceId();
//		map.setFaceId(map.phi2(f1),idface, FACE);
//	}
//
//	std::vector<Dart> inFaces;
//	inFaces.reserve(10);

	//DartMarkerStore mv(map);

//	//Third Step : split inner faces if quad faces exists
//	for(std::vector<std::pair<Dart,Dart> >::iterator quadFaces = subdividedQuadFaces.begin() ; quadFaces != subdividedQuadFaces.end() ; ++quadFaces)
//	{
//		Dart f = (*quadFaces).first;
//
//		//if(!map.isMarked(f))
//		//{
//			map.splitFace(map.phi2(f), map.phi_1(map.phi_1(map.phi2(f))));
//			//mv.markOrbit(VERTEX(map.phi2(f));
//		//}
//
//		inFaces.push_back(map.phi1(map.phi3(map.phi2(f))));
//
//
//		unsigned int idface = map.getNewFaceId();
//		map.setFaceId(map.phi2(f),idface, FACE);
//
//		idface = map.getNewFaceId();
//		map.setFaceId(map.phi2(map.phi_1(map.phi2(f))),idface, FACE);
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
//		map.setFaceId(map.phi2(*inFace),idface, FACE);
//
//		//unsigned int idedge = map.getEdgeId(map.phi2(map.phi1(map.phi2(*inFace))));
//		//map.setEdgeId(map.phi1(map.phi2(*inFace)), idedge, EDGE);
//
//	}
//
//	for(std::vector<Dart>::iterator inFace = inFaces.begin() ; inFace != inFaces.end() ; ++inFace)
//	{
//		Dart e = *inFace;
//
//		unsigned int idedge = map.getNewEdgeId();
//		map.setEdgeId(e, idedge, EDGE);
//
//		idedge = map.getEdgeId(map.phi2(map.phi1(map.phi1(map.phi2(*inFace)))));
//		map.setEdgeId(map.phi1(map.phi1(map.phi2(*inFace))), idedge, EDGE);
//
//	}

//	map.setCurrentLevel(cur);
//}


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

	mf.markOrbit(FACE, old) ;

	for(std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;
		do
		{
			//add one old edge per vertex to the old edge list
			//compute volume centroid
			if(!mv.isMarked(e))
			{
				mv.markOrbit(VERTEX, e);

				oldEdges.push_back(e);
			}

			// add all face neighbours to the table
			Dart ee = map.phi2(e) ;
			if(!mf.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mf.markOrbit(FACE, ee) ;
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

	mf.markOrbit(FACE, old) ;

	for(std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;
		do
		{
			//add one old edge per vertex to the old edge list
			//compute volume centroid
			if(!mv.isMarked(e))
			{
				mv.markOrbit(VERTEX, e);

				oldEdges.push_back(e);
			}

			// add all face neighbours to the table
			Dart ee = map.phi2(e) ;
			if(!mf.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mf.markOrbit(FACE, ee) ;
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
				mf.markOrbit(FACE, e);

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

	mf.markOrbit(FACE, old) ;

	for(std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;
		do
		{
			//add one old edge per vertex to the old edge list
			//compute volume centroid
			if(!mv.isMarked(e))
			{
				mv.markOrbit(VERTEX, e);

				oldEdges.push_back(e);
			}

			// add all face neighbours to the table
			Dart ee = map.phi2(e) ;
			if(!mf.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mf.markOrbit(FACE, ee) ;
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
				mvp.markOrbit(VERTEX, v);
			}

			v = map.phi2(e);
			if(!mvp.isMarked(v))
			{
				position[v] = bajajMask<PFP>(map, v, position);
				mvp.markOrbit(VERTEX, v);
			}

			e = map.phi2(map.phi_1(e));
		}while(e != *edge);

		if(!mvp.isMarked(e))
		{
			position[e] = bajajMask<PFP>(map, e, position);
			mvp.markOrbit(VERTEX, e);
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
//	mf.markOrbit(FACE, old) ;
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
//				mf.markOrbit(FACE, ee) ;
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
//			map.setFaceId(map.phi2(*nvol),idface, FACE);
//		}
//
//		//id pour toutes les aretes exterieurs des faces quadrangulees
//		unsigned int idedge = map.getEdgeId(*nvol);
//		map.setEdgeId(map.phi2(*nvol), idedge, DART);
//		map.setEdgeId( map.phi2(*(nvol+1)), idedge, DART);
//	}
//
//	//manque id pour les aretes interieurs : 6
//	DartMarker mne(map);
//	for(std::vector<Dart>::iterator it = newEdges.begin() ; it != newEdges.end() ; ++it)
//	{
//		if(!mne.isMarked(*it))
//		{
//			unsigned int idedge = map.getNewEdgeId();
//			map.setEdgeId(*it, idedge, EDGE);
//			mne.markOrbit(EDGE,*it);
//			std::cout << "plop" << std::endl;
//		}
//	}
//	std::cout << std::endl;

//	map.setCurrentLevel(cur) ;
}





} //namespace IHM

} //namespace Algo

} //namespace CGoGN



