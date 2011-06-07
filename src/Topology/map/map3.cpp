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

#include <list>
#include <set>
#include <map>
#include <vector>

#include "Topology/map/map3.h"

namespace CGoGN
{

/*! @name Generator and Deletor
 *  To generate or delete volumes in a 3-map
 *************************************************************************/
//ok
void Map3::deleteOrientedVolume(Dart d)
{
	DartMarkerStore mark(*this);		// Lock a marker

	std::vector<Dart> visitedFaces;		// Faces that are traversed
	visitedFaces.reserve(16);
	visitedFaces.push_back(d);			// Start with the face of d

//	// For every face added to the list
//	for (face = visitedFaces.begin(); !found && face != visitedFaces.end(); ++face)
//	{
//		if (!mark.isMarked(*face))		// Face has not been visited yet
//		{
//			unsewVolumes(*face);
//
//			Dart dNext = *face ;
//			do
//			{
//				mark.mark(dNext);					// Mark
//				Dart adj = phi2(dNext);				// Get adjacent face
//				if (adj != dNext && !mark.isMarked(adj))
//					visitedFaces.push_back(adj);	// Add it
//				dNext = phi1(dNext);
//			} while(dNext != *face);
//		}
//	}

	mark.markOrbit(FACE, d) ;

	for(std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;

		unsewVolumes(e);

		do	// add all face neighbours to the table
		{
			Dart ee = phi2(e) ;
			if(!mark.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mark.markOrbit(FACE, ee) ;
			}
			e = phi1(e) ;
		} while(e != *face) ;
	}


	// delete every visited face
	for (std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
		Map1::deleteOrientedFace(*face);
}

/*! @name Topological Operators
 *  Topological operations on 3-maps
 *************************************************************************/
void Map3::sewVolumes(Dart d, Dart e)
{
	assert(faceDegree(d) == faceDegree(e));

	Dart fitD = d ;
	Dart fitE = e ;
	do
	{
		phi3sew(fitD,fitE);
		fitD = phi1(fitD) ;
		fitE = phi_1(fitE) ;
	} while(fitD != d) ;
}

void Map3::unsewVolumes(Dart d)
{
	Dart fitD = d;
	do
	{
		phi3unsew(fitD);
		fitD = phi1(fitD);
	} while(fitD != d);
}

bool Map3::mergeVolumes(Dart d)
{
	Dart e = phi3(d) ;
	if(e != d)
	{
		unsewVolumes(d);
		Map2::mergeVolumes(d, e); // merge the two volumes along common face
		return true ;
	}
	return false ;
}

void Map3::splitFace(Dart d, Dart e)
{
	Map2::splitFace(d,e);

	if (phi3(d) != d)
	{
		Dart dd = phi1(phi3(d));
		Dart ee = phi1(phi3(e));

		Map2::splitFace(dd,ee);

		phi3sew(phi_1(d), phi_1(ee));
		phi3sew(phi_1(e), phi_1(dd));
	}
}

void Map3::cutEdge(Dart d)
{
	if(phi3(d) == d)
		d = phi2(d);

	Dart prev = d;
	Dart dd = alpha2(d);
	Map2::cutEdge(d);

	while (dd!=d)
	{
		prev = dd;
		dd = alpha2(dd);

		Map2::cutEdge(prev);

		if (phi3(prev) != prev)
		{
			Dart d3 = phi3(prev);
			phi3unsew(prev);
			phi3sew(prev, phi1(d3));
			phi3sew(d3, phi1(prev));
		}
	}

	if (phi3(d) != d)
	{
		Dart d3 = phi3(d);
		phi3unsew(d);
		phi3sew(d, phi1(d3));
		phi3sew(d3, phi1(d));
	}
}

void Map3::uncutEdge(Dart d)
{
	if(phi3(d) == d)
		d = phi_1(phi2(d));

	Dart prev = d;
	Dart dd = alpha2(d);

	Map2::uncutEdge(prev);

	if(phi3(dd) != dd)
		phi3sew(dd,phi2(prev));

	while (dd!=d)
	{
		prev = dd;
		dd = alpha2(dd);

		Map2::uncutEdge(prev);

		phi3sew(dd, phi2(prev));
	}

}

bool Map3::deleteVertex(Dart d)
{
	DartMarkerStore mv(*this);	// Lock a marker

	std::list<Dart> darts_list;			//Darts that are traversed
	darts_list.push_back(d);			//Start with the dart d
	std::list<Dart>::iterator darts;

	mv.mark(d);

	std::list<Dart> unique_darts_list;
	//unique_darts_list.reserve(30);
	unique_darts_list.push_back(d);


	bool boundary = false;
	if(isBoundaryVertex(d))
		boundary = true;

	for(darts = darts_list.begin(); darts != darts_list.end() ; ++darts)
	{
		Dart dc = *darts;

		//add phi21 and phi23 successor if they are not marked yet
		Dart d2 = phi2(dc);
		Dart d21 = phi1(d2); // turn in volume
		Dart d23 = phi3(d2); // change volume

		if(!mv.isMarked(d21))
		{
			darts_list.push_back(d21);
			mv.mark(d21);
		}

		if((d23!=d2) && !mv.isMarked(d23))
		{
			darts_list.push_back(d23);
			unique_darts_list.push_back(d23);
			mv.mark(d23);
		}
	}

	for(darts = unique_darts_list.begin(); darts != unique_darts_list.end() ; ++darts)
	{
		mergeVolumes(*darts);
	}

	if(boundary)
	{
		Dart vit = d ;
			do
			{
				Dart f = phi_1(phi2(vit)) ;
				phi1sew(vit, f) ;
				vit = phi2(phi_1((vit))) ;
			} while(vit != d) ;
			Map1::deleteFace(d) ;
	}

	return true;
}

//TODO
//bool Map3::flipEdge(Dart d)
//{
//	if(phi3(d) == d)
//		d = phi2(d);
//
//	Dart e = alpha2(d);
//	Dart e2 = phi2(e);
//
//	// Test if an opposite
//	if (e != d)
//	{
//		Dart dPrev = phi_1(d);
//		Dart dNext = phi1(d);
//		Dart e2Prev = phi_1(e2);
//		Dart e2Next = phi1(e2);
//
//		phi1sew(d, e2Prev);		// Detach the two
//		phi1sew(e2, dPrev);		// vertices of the edge
//		phi1sew(d, dNext);		// Insert the edge in its
//		phi1sew(e2, e2Next);	// new vertices after flip
//		return true;
//	}
//	else // flip a border edge with Map2 flipEdge
//		return Map2::flipEdge(d);
//}

//TODO
//bool Map3::flipBackEdge(Dart d)
//{
//	return false;
//}

////TODO
//bool Map3::flipFace(Dart d)
//{
//	//prevoir de refaire un linkFace et de ne pas
//	//faire inserFace si flipEdge renvoie faux
//
//	//save a dart from a non-modifed-face of one tetrahedron
//	Dart r = phi2(d);
//
//	//detach common face from tetrahedron from the rest of the faces
//	//unlinkFace(d);
//	Map3::mergeVolumes(d);
//
//	//flip the common edge
//	Map2::flipEdge(r);
//
//	//insert the old face in the new flipped edge
//	//Map3::insertFace(r,d);
//	Map3::splitFace(r,d);
//
//	return true;
//}

//void Map3::insertFace(Dart d, Dart e)
//{
//	assert(faceDegree(d) == faceDegree(e)); //les faces ont la meme longueur
//
//    Dart dd = d;
//    Dart nFd = e;
//
//    do {
//    	//sewFace(dd,nFd);
//
//    	Dart d2 = phi2(dd);
//        unsewFaces(dd);
//        sewFaces(d2,phi3(nFd));
//        sewFaces(dd,nFd);
//
//        dd = phi_1(phi2(phi_1(dd)));
//        nFd = phi1(nFd);
//    } while (nFd != e);
//
//}


int Map3::collapseEdge(Dart d, bool delDegenerateFaces,
		bool delDegenerateVolumes)
{
	Dart e = d;
	int i = 0;

	//stocke un brin par volume autour de l'arete
	std::list<Dart> tmp;
	do
	{
		tmp.push_back(e);
		e = alpha2(e);
		i++;
	} while (e != d);

	for (std::list<Dart>::iterator it = tmp.begin(); it != tmp.end(); ++it)
	{
		Dart e = phi2(*it);

		cutSpike(e);

		Dart t1=e,t2=e;
		//si les faces opposées ont un tetraedre cousu
		if(phi3(phi2(phi1(e))) != phi2(phi1(e))) {
			t1 = phi3(phi2(phi1(e)));
			unsewVolumes(t1);
		}

		if(phi3(phi2(phi_1(e))) != phi2(phi_1(e))) {
			t2 = phi3(phi2(phi_1(e)));
			unsewVolumes(t2);
		}

		if(t1 != e && t2 != e) {
			sewVolumes(t1,t2);
		}

		//unsewVolumes(e);
		//unsewVolumes(*it);

		deleteOrientedVolume(*it);
	}

	return i;
}

//	CGoGNout << "coll topo" << CGoGNendl;
//	Dart e = d;
//
//	//stocke un brin par volume autour de l'arete
//	std::list<Dart> tmp;
//	do
//	{
//		tmp.push_back(e);
//		e = alpha2(e);
//	} while (e != d);
//
//	//contraction de la 2 carte de chaque 2-arete
//	for (std::list<Dart>::iterator it = tmp.begin(); it != tmp.end(); ++it)
//	{
//		//un brin d'une face adjacente a l'arrete contracte
//		Dart d = phi2(phi_1(*it));
//		Map2::collapseEdge(*it, delDegenerateFaces);
//
//		//test de la degeneresence
//		//impossible d'avoir un volume de moins de 4 faces sans avoir de phi2 en points fixe donc on les vire
//		if(delDegenerateVolumes && Map2::volumeDegree(d) < 4)
//		{
//			CGoGNout << "del vol" << CGoGNendl;
//			Dart e = d;
//			//pour tous les brins de la face adjacente
//
//			do
//			{
//				Dart ee = phi3(e);
//				Dart ff = phi3(phi2(e));
//
//				//si les brins ont un voisin par phi3
//				if(ee != e)
//					phi3unsew(ee);
//
//				if(ff != phi2(e))
//					phi3unsew(ff);
//
//				//si les deux en ont un, il faut les coudres ensemble
//				if(ee != e && ff != phi2(e))
//					phi3sew(ee, ff);
//
//				//on peut supprimer les brins de cette arete
//				deleteDart(e);
//				deleteDart(phi2(e));
//				e = phi1(e);
//
//			} while (e != d);
//		}
//	}

//TODO
void Map3::collapseFace(Dart d, bool delDegenerateFaces,
		bool delDegenerateVolumes)
{
	Dart e = d;
	std::list<Dart> tmp;

	//save a dart from the edge for all neighbors
	do
	{
		//if(phi3(phi2(e)) != phi2(e))
		//		tmp.push_back(phi3(phi2(e)));
		tmp.push_back(phi3(phi2(e)));
		e = phi1(e);
	}while(e != d);

	//del the last one (n-1 edge collapse)
	tmp.pop_back();

	//CGoGNout << "#voisin=" << tmp.size() << CGoGNendl;

	//collapse all the edges in the list
	for(std::list<Dart>::iterator it = tmp.begin() ; it != tmp.end() ; ++it)
	{
		Dart d = *it;
		//CGoGNout << "collapseEdge" << CGoGNendl;
		//collapseEdge(*it, delDegenerateFaces, delDegenerateVolumes);
		//stocke un brin par volume autour de l'arete

		Dart e = d;

		//stocke un brin par volume autour de l'arete
		std::list<Dart> tmpedge;
		do
		{
			tmpedge.push_back(e);
			e = alpha2(e);
		} while (e != d);

		for (std::list<Dart>::iterator it = tmpedge.begin(); it != tmpedge.end(); ++it)
		{
			Dart e = phi2(*it);

			cutSpike(e);

			Dart t1=e,t2=e;
			//si les faces opposées ont un tetraedre cousu
			if(phi3(phi2(phi1(e))) != phi2(phi1(e))) {
				t1 = phi3(phi2(phi1(e)));
				unsewVolumes(t1);
			}

			if(phi3(phi2(phi_1(e))) != phi2(phi_1(e))) {
				t2 = phi3(phi2(phi_1(e)));
				unsewVolumes(t2);
			}

			if(t1 != e && t2 != e) {
				sewVolumes(t1,t2);
			}

			//deleteOrientedVolume(*it);
		}
	}
//	for(std::list<Dart>::iterator it = tmp.begin() ; it != tmp.end() ; ++it)
//	{
//		deleteOrientedVolume(*it);
//	}
}

//TODO
void Map3::collapseVolume(Dart d, bool delDegenerateFaces,
		bool delDegenerateVolumes)
{
	//pour toutes les faces du volume
	//sauvegarder un brin du voisin s'il existe
	std::vector<Dart> neighborsVol;
	neighborsVol.reserve(16);

	DartMarkerStore mark(*this);		// Lock a marker

	std::vector<Dart> visitedFaces;		// Faces that are traversed
	visitedFaces.reserve(16);
	visitedFaces.push_back(d);			// Start with the face of d
	std::vector<Dart>::iterator face;

	// For every face added to the list
	for (face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart dsave = *face ;
		if(phi3(dsave) != dsave)
			neighborsVol.push_back(dsave);


	}

	// delete every visited face
	for (face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
		Map1::deleteOrientedFace(*face);

	//enlever le dernier (n-1) face collapse


	//contracter toutes les faces de la liste
}

//TODO
void Map3::mergeFaces(Dart d, Dart e)
{

}

Dart Map3::cutSpike(Dart d)
{
  Dart e=d;
  int nb=0;
  Dart dNew;
  int tet=0;

  //CGoGNout << "cut" << CGoGNendl;

  //count the valence of the vertex
  do {
    nb++;
    e=phi1(phi2(e));
  } while (e!=d);

  if(nb<3)
  {
	CGoGNout << "Warning : cannot cut 2 volumes without creating a degenerated face " << CGoGNendl;
	return d;
  }
  else
  {
	 //triangulate around the vertex
	do {
		if(phi1(phi1(phi1(e)))!=e)
		{
			splitFace(phi_1(e),phi1(e));
			//CGoGNout << "split" << CGoGNendl;
		}
		else
			tet++;

		e=phi1(phi2(e));
	} while (e!=d);

//	CGoGNout << "#tet= " << tet << CGoGNendl;
//	CGoGNout << "#nb= " << nb << CGoGNendl;

	//si toute ces faces ne sont pas triangulaires (on insere une face)
	if(tet != nb) {
		//CGoGNout << "new face" << CGoGNendl;
		dNew=newFace(nb);
		Dart d3 = newFace(nb);
		sewVolumes(dNew,d3);

		//sew a face following the triangles
		Dart dTurn=dNew;
		do {
			Dart d1 = phi1(e);
			Dart dSym = phi2(d1);
			phi2unsew(d1);
			phi2sew(dTurn,d1);
			phi2sew(phi3(dTurn),dSym);
			dTurn = phi1(dTurn);
			e=phi1(phi2(e));
		}while(e!=d);
	}
	else
		dNew = d;
  }

  return dNew;
}

int Map3::edgeDegree(Dart d)
{
	int deg = 0;
	Dart e = d;

	do
	{
		deg++;
		e = alpha2(e);
	} while(e != d);

	return deg;
}

unsigned int Map3::vertexDegree(Dart d)
{
	int count = 0;
	DartMarkerStore mv(*this);	// Lock a marker

	std::list<Dart> darts_list;			//Darts that are traversed
	darts_list.push_back(d);			//Start with the dart d
	std::list<Dart>::iterator darts;

	mv.mark(d);

	for(darts = darts_list.begin(); darts != darts_list.end() ; ++darts)
	{
		Dart dc = *darts;

		//add phi21 and phi23 successor if they are not marked yet
		Dart d2 = phi2(dc);
		Dart d21 = phi1(d2); // turn in volume
		Dart d23 = phi3(d2); // change volume

		if(!mv.isMarked(d21))
		{
			darts_list.push_back(d21);
			mv.mark(d21);
		}

		if((d23!=d2) && !mv.isMarked(d23))
		{
			darts_list.push_back(d23);
			mv.mark(d23);
		}
	}

	std::cout << "#darts = " << darts_list.size() << std::endl;

	DartMarkerStore me(*this);

	for(darts = darts_list.begin(); darts != darts_list.end() ; ++darts)
	{
		if(!me.isMarked(*darts))
		{
			++count;
			me.markOrbit(EDGE, *darts);
		}
	}

	return count;
}

bool Map3::isBoundaryVolume(Dart d)
{
	bool isBoundary = false;
	DartMarkerStore mark(*this);		// Lock a marker

	std::vector<Dart> visitedFaces ;
	visitedFaces.reserve(100) ;
	visitedFaces.push_back(d) ;
	mark.markOrbit(FACE, d) ;

	for(std::vector<Dart>::iterator face = visitedFaces.begin(); !isBoundary && face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;

		//if this dart is not phi3-linked
		if(phi3(e) == e)
			isBoundary = true;

		if(!isBoundary)
		{

			do	// add all face neighbours to the table
			{
				Dart ee = phi2(e) ;
				if(!mark.isMarked(ee)) // not already marked
				{
					visitedFaces.push_back(ee) ;
					mark.markOrbit(FACE, ee) ;
				}
				e = phi1(e) ;
			} while(e != *face) ;
		}
	}

	return isBoundary;
}

bool Map3::isBoundaryVertex(Dart d)
{
	DartMarkerStore mv(*this);			// Lock a marker
	bool found = false;					// Last functor return value

	std::list<Dart> darts_list;			//Darts that are traversed
	darts_list.push_back(d);			//Start with the dart d
	std::list<Dart>::iterator darts;

	mv.mark(d);

	for(darts = darts_list.begin(); !found && darts != darts_list.end() ; ++darts)
	{
		Dart dc = *darts;

		//add phi21 and phi23 successor if they are not marked yet
		Dart d2 = phi2(dc);
		Dart d21 = phi1(d2); // turn in volume
		Dart d23 = phi3(d2); // change volume

		if(!mv.isMarked(d21))
		{
			darts_list.push_back(d21);
			mv.mark(d21);
		}

		if((d23!=d2) && !mv.isMarked(d23))
		{
			darts_list.push_back(d23);
			mv.mark(d23);
		}

		found = phi3(dc) == dc;
	}

	return found;
}

/*! @name Cell Functors
 *  Apply functors to all darts of a cell
 *************************************************************************/

bool Map3::foreach_dart_of_vertex(Dart d, FunctorType& f, unsigned int thread)
{
	DartMarkerStore mv(*this,thread);	// Lock a marker
	bool found = false;					// Last functor return value

	std::list<Dart> darts_list;			//Darts that are traversed
	darts_list.push_back(d);			//Start with the dart d
	std::list<Dart>::iterator darts;

	mv.mark(d);

	for(darts = darts_list.begin(); !found && darts != darts_list.end() ; ++darts)
	{
		Dart dc = *darts;

		//add phi21 and phi23 successor if they are not marked yet
		Dart d2 = phi2(dc);
		Dart d21 = phi1(d2); // turn in volume
		Dart d23 = phi3(d2); // change volume

		if(!mv.isMarked(d21))
		{
			darts_list.push_back(d21);
			mv.mark(d21);
		}

		if((d23!=d2) && !mv.isMarked(d23))
		{
			darts_list.push_back(d23);
			mv.mark(d23);
		}

		found = f(dc);
	}

	return found;
}

bool Map3::foreach_dart_of_edge(Dart d, FunctorType& f, unsigned int thread)
{
	Dart dNext = d;
	do {
		if (Map2::foreach_dart_of_edge(dNext,f,thread)) return true;
		dNext = alpha2(dNext);
	} while (dNext != d);
	return false;
}

bool Map3::foreach_dart_of_open_edge(Dart d, FunctorType& f, unsigned int thread)
{

	DartMarkerStore mv(*this,thread);	// Lock a marker
	bool found = false;					// Last functor return value

	std::list<Dart> darts_list;			//Darts that are traversed
	darts_list.push_back(d);			//Start with the dart d
	std::list<Dart>::iterator darts;

	mv.mark(d);

	for(darts = darts_list.begin(); !found && darts != darts_list.end() ; ++darts)
	{
		Dart d1 = *darts;

		Dart d3 = phi2(d1); // change face
		Dart d4 = phi3(d1); // change volume

		if (!mv.isMarked(d3))
		{
			darts_list.push_back(d3);
			mv.mark(d3);
		}
		if (!mv.isMarked(d4))
		{
			darts_list.push_back(d4);
			mv.mark(d4);
		}

		found = f(d1);
	}


	return found;
}

bool Map3::foreach_dart_of_face(Dart d, FunctorType& f, unsigned int thread)
{
	if (foreach_dart_of_oriented_face(d,f,thread)) return true;

	Dart d3 = phi3(d);
	if (d3 != d) return foreach_dart_of_oriented_face(d3,f,thread);
	return false;
}

bool Map3::foreach_dart_of_cc(Dart d, FunctorType& f, unsigned int thread)
{
	DartMarkerStore mv(*this,thread);			// Lock a marker
	bool found = false;					// Last functor return value

	std::list<Dart> darts_list;			//Darts that are traversed
	darts_list.push_back(d);			//Start with the dart d
	std::list<Dart>::iterator darts;

	mv.mark(d);


	for(darts = darts_list.begin(); !found && darts != darts_list.end() ; ++darts)
	{
		Dart d1 = *darts;

		// add all successors if they are not marked yet
		Dart d2 = phi1(d1); // turn in face
		Dart d3 = phi2(d1); // change volume
		Dart d4 = phi3(d1); // change volume

		if (!mv.isMarked(d2))
		{
			darts_list.push_back(d2);
			mv.mark(d2);
		}
		if (!mv.isMarked(d3))
		{
			darts_list.push_back(d2);
			mv.mark(d2);
		}
		if (!mv.isMarked(d4))
		{
			darts_list.push_back(d4);
			mv.mark(d4);
		}

		found = f(d1);	// functor say finish
	}


	return found;
}

//bool Map3::foreach_connex_volume(Dart d, int degree, FunctorType& f, FunctorSelect<Dart>& s)
//{
//	Marker m = this->getNewMarker();
//	bool found = false;
//	std::list<Dart> darts_list;
//	std::list<Dart> neighbours_list;
//	darts_list.push_back(d);
//	this->markOrbit(DART,d,m);
//
//	std::list<Dart>::iterator prem = darts_list.begin();
//
//	while (!found && prem != darts_list.end()) {
//		Dart d1 = *prem;
//		Dart dd;
//		switch(degree) {
//			case 0 : //vertex connexity
//				{
//					dd=d1;
//					std::list<Dart> darts_list2;
//					darts_list2.push_back(dd);
//					this->markOrbit(DART,dd,m);
//					neighbours_list.push_back(dd);
//					std::list<Dart>::iterator prem2 = darts_list2.begin();
//
//					while (!found && prem2 != darts_list2.end()) {
//						Dart dd1 = *prem2;
//						Dart d3 = phi2(dd1);
//						Dart d2 = phi1(d3); // turn in volume
//						Dart d4 = phi3(d3); // change volume
//
//						if(s(dd1))
//							found =  f(dd1);
//
//						if(!this->isMarkedDart(d2,m)) {
//							darts_list2.push_back(d2);
//							markOrbit(DART,d2,m);
//						}
//
//						if(!this->isMarkedDart(d4,m)) {
//							darts_list2.push_back(d4);
//							markOrbit(DART,d4,m);
//						}
//
//						++prem2;
//					}
//				}
//				break;
//			case 1 : //edge connexity
//				dd=d1;
//				neighbours_list.push_back(dd);
//				do {
//					if(!this->isMarkedDart(dd,m)) {
//						markOrbit(DART,dd,m);
//						if(s(dd))
//							found =  f(dd);
//					}
//					dd = alpha2(dd);
//				} while(!found && dd!=d1);
//				break;
//			default : //face connexity
//				dd = phi3(d1);
//				if(!this->isMarkedDart(dd,m)) {
//					neighbours_list.push_back(dd);
//					markOrbit(2,dd,m);
//					if(s(dd)) {
//						found =  f(dd);
//					}
//				}
//		} //end switch
//
//		//test rest of the volume
//		// add phi1 and phi2 successor of they are not yet marked
//		Dart d2 = phi1(d1); // turn in face
//		Dart d3 = phi2(d1); // change volume
//
//		if (!this->isMarkedDart(d2,m)) {
//			darts_list.push_back(d2);
//			this->markOrbit(DART,d2,m);
//		}
//		if (!this->isMarkedDart(d3,m)) {
//			darts_list.push_back(d3);
//			this->markOrbit(DART,d3,m);
//		}
//
//		++prem;
//	}
//
//	//unmark current volume
//	for (std::list<Dart>::iterator it = darts_list.begin(); it != darts_list.end(); ++it) {
//		this->unmarkOrbit(DART,(*it),m);
//	}
//
//	//unmark connex volumes checked
//	FunctorUnmark<Map3 > fum(*this,m);
//	for (std::list<Dart>::iterator it = neighbours_list.begin(); it != neighbours_list.end(); ++it) {
//		switch(degree) {
//			case 0 :
//				foreach_dart_of_vertex((*it),fum);
//				break;
//			case 1 :
//				foreach_dart_of_edge((*it),fum);
//				break;
//			default :
//				foreach_dart_of_face((*it),fum);
//		}
//	}
//	this->releaseMarker(DART,m);
//	return found;
//}

//// template <typename DART>
//// void Map3::foreach_volume(FunctorType<Dart>* funct)
//// {
//// 	// lock a marker
//// 	int markOV = this->getMarkerIndex();
//// 	for(Dart d = this->begin(); d != this->end(); this->next(d))
//// 	{
//// 		if (!this->isMarkedDart(d,markOV))  // if not yet treated
//// 		{
//// 			(*funct)(d);			// call the functor and
//// 			this->markVolume(d,markOV);  // mark all dart of the vol
//// 		}
//// 	}
//// 	this->releaseMarker(DART,markOV);
//// }




void Map3::closeMap(DartMarker& marker)
{
	std::list<Dart> dartList;

// step 1: double,mark & store darts that have fixed point phi3
	for(Dart d = this->begin(); d != this->end(); this->next(d))
	{
		if (phi3(d) == d)
		{
			Dart e = this->newDart();
			marker.markOrbit(DART,e);
			phi3sew(d,e);
			dartList.push_back(e);
		}
	}

// step 2: update phi1 of new darts
	for (std::list<Dart>::iterator it=dartList.begin(); it!=dartList.end(); ++it)
	{
		Dart d = *it;		// external dart
		Dart e = phi3(d);	// internal dart

		// update phi1 if not already sewn (the last dart of the dart is automatiquely sewn)
		// test à faire dans sewPhi ?
		//if (phi1(d) != phi3(phi_1(e))) phi1sew(d, phi3(phi_1(e)));
		if (phi1(d) == d) {
			Dart eNext = phi1(e);
			do {
				phi1sew(d, phi3(eNext));
				eNext = phi1(eNext);
			} while (eNext != e);
		}
		// search the dart for phi2:
		e = phi3(phi2(e));
		while (!marker.isMarked(e))
		{
			e = phi3(phi2(e));
		}
		//update phi2 if not already sewn (if e have previously been sewn with d)
		//test à faire dans sewPhi ?
		if (phi2(d) != e) phi2sew(d,e);
	}

}

//void Map3::reverseOrientation()
//{
//	Marker mf2 = this->getNewMarker();
//	Marker mf3 = this->getNewMarker();
//
//	// reverse all faces (only phi1 is modified)
//	for (Dart d= this->begin(); d != this->end(); this->next(d))
//	{
//		if (!isMarkedDart(d,mf2))
//		{
//			reverseFace(d);
//
//			Dart e=d;
//			do
//			{
//				markOrbit(DART,e,mf2);
//				markOrbit(DART,e,mf3);
//				e=phi1(e);
//			}
//			while (e!=d);
//		}
//	}
//
//	// store all new phi2 and phi3
//	std::vector<Dart> vdphi2;
//	std::vector<Dart> vdphi3;
//	vdphi2.reserve(this->getNbDarts());
//	vdphi3.reserve(this->getNbDarts());
//	for (Dart d= this->begin(); d != this->end(); this->next(d))
//	{
//		Dart e = phi_1(phi2(phi1(d)));
//		vdphi2.push_back(e);
//		Dart f = phi_1(phi3(phi1(d)));
//		vdphi3.push_back(f);
//	}
//
//	// apply the phi2sew with stored phi2 on all darts
//	std::vector<Dart>::iterator id2 = vdphi2.begin();
//	std::vector<Dart>::iterator id3 = vdphi3.begin();
//	for (Dart d= this->begin(); d != this->end(); this->next(d),++id2,++id3)
//	{
//		if (isMarkedDart(d,mf2))
//		{
//			unmarkOrbit(DART,d,mf2); // unmark the two darts
//			unmarkOrbit(DART,*id2,mf2);
//
//			if (phi2(d) != d)
//				phi2unsew(d);	// unsew the two darts if necessary
//			if (phi2(*id2) != *id2)
//				phi2unsew(*id2);
//			phi2sew(d,*id2); // sew the darts
//		}
//
//		if (isMarkedDart(d,mf3))
//		{
//			unmarkOrbit(DART,d,mf3); // unmark the two darts
//			unmarkOrbit(DART,*id3,mf3);
//
//			if (phi3(d) != d)
//				phi3unsew(d);	// unsew the two darts if necessary
//			if (phi3(*id3) != *id3)
//				phi3unsew(*id3);
//			phi3sew(d,*id3); // sew the darts
//		}
//	}
//
//	// no need to clear marker second pass do it
//	this->releaseMarker(DART,mf2);
//	this->releaseMarker(DART,mf3);
//}
//
//bool Map3::check()
//{
//    CGoGNout << "Check: topology begin" << CGoGNendl;
//    Marker m = this->getNewMarker();
//    for(Dart d = this->begin(); d != this->end(); this->next(d))
//    {
//        Dart d3 = phi3(d);
//        if (phi3(d3) != d) // phi3 involution ?
//		{
//            CGoGNout << "Check: phi3 is not an involution" << CGoGNendl;
//            return false;
//        }
//
//        Dart d2 = phi2(d);
//        if (phi2(d2) != d) // phi2 involution ?
//		{
//            CGoGNout << "Check: phi2 is not an involution" << CGoGNendl;
//            return false;
//        }
//
//        Dart d1 = phi1(d);
//        if (phi_1(d1) != d) // phi1 a une image correcte ?
//		{
//            CGoGNout << "Check: unconsistent phi_1 link" << CGoGNendl;
//            return false;
//        }
//
//        if (isMarkedDart(d1,m)) // phi1 a un seul antécédent ?
//		{
//            CGoGNout << "Check: dart with two phi1 predecessors" << CGoGNendl;
//            return false;
//        }
//        markOrbit(DART,d1,m);
//
//        if (d1 == d)
//            CGoGNout << "Check: (warning) face loop (one edge)" << CGoGNendl;
//
//        if (phi1(d1) == d)
//            CGoGNout << "Check: (warning) face with only two edges" << CGoGNendl;
//
//        if (phi2(d1) == d)
//            CGoGNout << "Check: (warning) dandling edge (phi2)" << CGoGNendl;
//
//        if (phi3(d1) == d)
//            CGoGNout << "Check: (warning) dandling edge (phi3)" << CGoGNendl;
//    }
//
//    for(Dart d = this->begin(); d != this->end(); this->next(d))
//    {
//        if (!isMarkedDart(d,m)) // phi1 a au moins un antécédent ?
//		{
//            CGoGNout << "Check: dart with no phi1 predecessor" << CGoGNendl;
//            return false;
//        }
//    }
//    this->unmarkAll(DART,m);
//    this->releaseMarker(DART,m);
//    CGoGNout << "Check: topology ok" << CGoGNendl;
//    return true;
//}







//void Map3::deleteEdge(Dart d)
//{
//	Dart e = d;
//	std::list<Dart> tmp;
//	do
//	{
//		tmp.push_back(e);
//		e=phi3(phi2(e));
//	}
//	while (e!=d);
//
//	for (std::list<Dart>::iterator it=tmp.begin();it!=tmp.end();++it)
//	{
//		Map2::deleteEdge(*it);
//	}
//
//}
//
//
//void Map3::removeEdge(Dart d)
//{
//
//	Dart e = d;
//	std::list<Dart> tmp;
//	do
//	{
//		tmp.push_back(e);
//		e=phi3(phi2(e));
//	}
//	while (e!=d);
//
//	for (std::list<Dart>::iterator it=tmp.begin();it!=tmp.end();++it)
//	{
//		removeFace(*it);
//	}
//}
//
//
//void Map3::removeVertex(Dart d)
//{
//	std::vector<Dart> store;
//	FunctorStore<Dart> fs(store);
//	foreach_dart_of_vertex(d,fs);
//
//	Marker toMerge = this->getNewMarker();
//
//	for (std::vector<Dart>::iterator it = store.begin() ; it!=store.end() ; ++it)
//	{
//		if (!this->isMarkedDart(*it,toMerge))
//		{
//			this->markOrbit(DART,this->phi3(this->phi_1(*it)),toMerge);
//		}
//		else
//		{
//			this->removeFace(*it);
//		}
//	}
//	this->releaseMarker(DART,toMerge);
//}
//
//
//
//

//
//Dart Map3::trianguleFace(Dart d0)
//{
//	Dart d1 = phi1(d0);		// Begin with d1 to avoid looking for the dart before d0
//	Dart d = d1;			// Dart d is used to turn around the face
//
//	if (d1 == d0)
//		CGoGNout << "Warning: triangulation of a face with only one edge" << CGoGNendl;
//
//	if (phi1(d1) == d0)
//		CGoGNout << "Warning: triangulation of a face with only two edges" << CGoGNendl;
//
//	Dart n = newEdge(2); 	// Create the first edge with n in the central vertex
//	Dart first = phi2(n); 	// Store the opposite of the first edge
//	Dart prec = phi3(n);
//	Dart next = phi1(d); 	// Get the next edge in the face of d
//	phi1sew(n,d); 			// Insert the edge in the face of d (between d and next)
//
//	AttributeHandler<Marker> dmarkers( VERTEX<<24 ,*this); // a modifier pour virer <<24
//
//	dmarkers[n] = dmarkers[d]; //	n->setMarkerVal(d->getMarkerVal());
//	dmarkers[phi2(n)] = dmarkers[d]; //	phi2(n)->setMarkerVal(d->getMarkerVal());
//
//	phi1sew(phi1(phi3(n)),phi3(next));
//
//	dmarkers[phi3(n)] = dmarkers[phi3(d)]; //	phi3(n)->setMarkerVal(phi3(d)->getMarkerVal());
//	dmarkers[phi1(phi3(n))] = dmarkers[phi3(d)]; //	phi1(phi3(n))->setMarkerVal(phi3(d)->getMarkerVal());
//
//	d = next; 				// Go to the next edge
//
//	while (d != d1)
//	{
//		n = newEdge(2); 		// Create an edge
//		next = phi1(d); 	// Get the next edge in the face of d
//		phi1sew(n,d); 		// Insert the edge in the face of d (between d and next)
//
//		dmarkers[n] = dmarkers[d];//	n->setMarkerVal(d->getMarkerVal());
//		dmarkers[phi2(n)] = dmarkers[d]; //	phi2(n)->setMarkerVal(d->getMarkerVal());
//
//		phi1sew(phi1(phi3(n)),phi3(next));
//
//		dmarkers[phi3(n)] = dmarkers[phi3(d)]; //	phi3(n)->setMarkerVal(phi3(d)->getMarkerVal());
//		dmarkers[phi1(phi3(n))] = dmarkers[phi3(d)]; //	phi1(phi3(n))->setMarkerVal(phi3(d)->getMarkerVal());
//
//		phi1sew(phi2(n),first); // Sew the edge with the first one around central vertex
//		phi1sew(prec,phi3(n));
//		prec = phi3(n);
//		d = next; 			// Go to next edge
//	}
//
//	return n; // Return the last created edge
//}
//
//
//
//
//
//Dart Map3::cutSpike(Dart d)
//{
//  Dart e=d;
//  int nb=0;
//  Dart dNew;
//
//  //count the valence of the vertex
//  do {
//    nb++;
//    e=phi1(phi2(e));
//  } while (e!=d);
//
//  if(nb<3)
//  {
//	CGoGNout << "Warning : cannot cut 2 volumes without creating a degenerated face " << CGoGNendl;
//	return d;
//  }
//  else
//  {
//	//triangulate around the vertex
//	do {
//		if(phi1(phi1(phi1(e)))!=e)
// 			cutFace(phi_1(e),phi1(e));
//		e=phi1(phi2(e));
//	} while (e!=d);
//
//	dNew=newFace(nb);
//
//	//sew a face following the triangles
//	Dart dTurn=dNew;
//	do {
//		Dart d1 = phi1(e);
//		Dart dSym = phi2(d1);
//		phi2unsew(d1);
//		phi2sew(dTurn,d1);
//		phi2sew(phi3(dTurn),dSym);
//		dTurn = phi1(dTurn);
//		e=phi1(phi2(e));
//	}while(e!=d);
//  }
//
//  return dNew;
//}
//
//
//Dart Map3::tetrahedrizeVolume(Dart d)
//{
//	// store all the dart of the volume
//	std::vector<Dart> vStore;
//	FunctorStore<Dart> fs(vStore);
//	foreach_dart_of_volume(d,fs);
//
//	//get a new marker
//	Marker traite = this->getNewMarker();
//
//	//the dart that will be returned
//	Dart ret;
//	//for each dart of the volume
//	for (std::vector<Dart>::iterator it = vStore.begin() ; it != vStore.end() ; ++it )
//	{
//		Dart dc=*it;
//		//if not processed
//		if (!isMarkedDart(dc,traite))
//		{
//			Dart dc2 = phi2(dc);
//
//			//mark the dart
//			markOrbit(DART,dc,traite);
//			markOrbit(DART,dc2,traite);
//
//			//create the new triangles
//			Dart dd=this->newFace(3);
//			Dart ee=phi3(dd);
//
//			//and sew them
//			phi2unsew(dc);
//			phi2unsew(dc2);
//			phi2sew(dc,dd);
//			phi2sew(dc2,ee);
//
//			//prepare the returned dart
//			ret=phi_1(dd);
//		}
//	}
//
//	//for each dart
//	for (std::vector<Dart>::iterator it = vStore.begin() ; it != vStore.end() ; ++it)
//	{
//		Dart dc=*it;
//		//if processed
//		if (isMarkedDart(dc,traite))
//		{
//			//get the previous dart in the face
//			Dart dc2 = phi_1(dc);
//
//			//unmark them
//			unmarkOrbit(DART,dc,traite);
//
//			//and sew them to create the tetra
//			phi2sew(phi1(phi2(dc)),phi_1(phi2(dc2)));
//		}
//	}
//
//	this->unmarkAll(DART,traite);
//	this->releaseMarker(DART,traite);
//
//	return ret;
//
//}
//
//

} // namespace CGoGN
