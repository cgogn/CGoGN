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

#include <list>

namespace CGoGN
{

namespace Algo
{

namespace Modelisation
{

namespace Tetrahedron
{
/************************************************************************************************
 * 																		Tetrahedron functions															   *
 ************************************************************************************************/

template <typename PFP>
bool isTetrahedron(typename PFP::MAP& the_map, Dart d)
{
	DartMarkerStore mark(*the_map);			// Lock a marker
	bool isTetrahedron = true;				// Last return value

	std::list<Dart> visitedFaces;			// Faces that are traversed
	visitedFaces.push_back(d);
	std::list<Dart>::iterator faces;

	int nbFaces = 0;						// Count the faces

	//Test the number of faces end its valency
	for(faces = visitedFaces.begin() ; isTetrahedron && faces != visitedFaces.end() ; ++faces)
	{
		Dart dc = *faces;

		//if this dart is not marked
		if(!mark.isMarked(dc))
		{
			//test the valency of this face
			if(dc != the_map.phi1(the_map.phi1(the_map.phi1(dc))))
				isTetrahedron = false;
			else
			{
				//mark them
				mark.markOrbit(DART_ORBIT,dc);
				//if phi2 not marked
				if(!mark.markOrbit(DART_ORBIT, the_map.phi2(dc)))
					visitedFaces.push_back(the_map.phi2(dc));

				//increase the number of faces
				nbFaces++;

				//too much faces
				if(nbFaces > 4)
					isTetrahedron = false;
				// or count the size of the face
				else
				{
					mark.markOrbit(DART_ORBIT, the_map.phi1(dc));
					//if phi12 not marked
					if(!mark.markOrbit(DART_ORBIT, the_map.phi2(the_map.phi1(dc))))
						visitedFaces.push_back(the_map.phi2(the_map.phi1(dc)));
					mark.markOrbit(DART_ORBIT, the_map.phi_1(dc));
					//if phi_12 not marked
					if(!mark.markOrbit(DART_ORBIT, the_map.phi2(the_map.phi_1(dc))))
						visitedFaces.push_back(the_map.phi2(the_map.phi_1(dc)));
				}
			}
		}
	}

	//nettoyage
	mark.unmarkAll();

	return isTetrahedron;
}

/************************************************************************************************
 * 																		Topological functions															   *
 ************************************************************************************************/

//sew a face into the edge
template <typename PFP>
Dart linkIntoEdge(typename PFP::MAP& map, Dart d, Dart e)
{
	Dart e2 = map.phi2(e);
	Dart d3 = map.phi3(d);

	//Decoud les 2 brins
	map.unsewFaces(e);

	//Coudre la nouvelle face au milieu de l'ancienne arête
	map.sewFaces(e2,d3);
	map.sewFaces(e,d);

	map.setDartEmbedding(VERTEX_ORBIT, d, map.getEmbedding(e2,VERTEX_ORBIT)) ;
	map.setDartEmbedding(VERTEX_ORBIT, d3, map.getEmbedding(e,VERTEX_ORBIT)) ;

	return e2;
}

//unsew a face from the edge
template <typename PFP>
void unlinkFromEdge(typename PFP::MAP& map, Dart d)
{
	Dart d3 = map.phi3(d);

	if(map.isOrbitEmbedded(VERTEX_ORBIT))
	{
		//Si la face n'est pas libre en phi2
		if(map.phi2(d) != d && map.phi2(d3) != d3)
		{
			unsigned int dVEmb = map.getDartEmbedding(VERTEX_ORBIT, d) ;
			if(dVEmb != EMBNULL)
			{
				map.embedOrbit(VERTEX_ORBIT, d, dVEmb) ;
				map.setDartEmbedding(VERTEX_ORBIT, d, EMBNULL) ;
			}

			unsigned int d3VEmb = map.getDartEmbedding(VERTEX_ORBIT, d3) ;
			if(d3VEmb != EMBNULL)
			{
				map.embedOrbit(VERTEX_ORBIT, d3, d3VEmb) ;
				map.setDartEmbedding(VERTEX_ORBIT, d3, EMBNULL) ;
			}
		}
		//Si la face est libre en phi2
		else
		{

		}
	}

	Dart e2 = map.phi2(d3);
	Dart d2 = map.phi2(d);
	map.unsewFaces(e2);
	map.unsewFaces(d2);
	map.sewFaces(d2,e2);
}

template <typename PFP>
void unlinkFace(typename PFP::MAP& map, Dart d)
{
	Dart e = d;
	do
	{
		unlinkFromEdge<PFP>(map, e);
		e = map.phi1(e);
	}
	while (e != d);
}

template <typename PFP>
void insertFace(typename PFP::MAP& map, Dart d, Dart nF)
{
	Dart dd = d;
	Dart nFd = nF;

	do {
		//TODO linkIntoEdge

		Dart d2 = map.phi2(dd);
		map.unsewFaces(dd);
		map.sewFaces(dd,nFd);
		map.sewFaces(d2,map.phi3(nFd));

		map.setDartEmbedding(VERTEX_ORBIT, nFd, map.getEmbedding(d2,VERTEX_ORBIT)) ;
		map.setDartEmbedding(VERTEX_ORBIT, map.phi3(nFd), map.getEmbedding(dd,VERTEX_ORBIT)) ;

		dd = map.phi_1(map.phi2(map.phi_1(dd)));
		nFd = map.phi1(nFd);

	} while (nFd != nF);
}

/***********************************************************************************************
 * 										swap functions										   *
 ***********************************************************************************************/
//ok
template <typename PFP>
void swap2To2(typename PFP::MAP& map, Dart d)
{
	//save a dart from a non-modifed-face of one tetrahedron
	Dart r = map.phi2(d);

	//detach common face from tetrahedron from the rest of the faces
	unlinkFace<PFP>(map, d);

	//flip the middle edge
	map.flipEdge(r);
	Dart e = map.phi2(r);

	unsigned int dVEmb = map.getDartEmbedding(VERTEX_ORBIT, r) ;
	if(dVEmb != EMBNULL)
	{
		map.setDartEmbedding(VERTEX_ORBIT, map.phi_1(r), dVEmb) ;
		map.setDartEmbedding(VERTEX_ORBIT, r, EMBNULL) ;
	}

	unsigned int eVEmb = map.getDartEmbedding(VERTEX_ORBIT, e) ;
	if(eVEmb != EMBNULL)
	{
		map.setDartEmbedding(VERTEX_ORBIT, map.phi_1(e), eVEmb) ;
		map.setDartEmbedding(VERTEX_ORBIT, e, EMBNULL) ;
	}

	//insert the face in the flipped edge
	insertFace<PFP>(map, r, d);

	Dart dd = d;
	do {
		Dart e = map.phi2(dd);
		Dart e2= map.phi2(map.phi3(dd));

		map.setDartEmbedding(VERTEX_ORBIT, dd, map.getEmbedding(e2,VERTEX_ORBIT)) ;
		map.setDartEmbedding(VERTEX_ORBIT, map.phi3(dd), map.getEmbedding(e,VERTEX_ORBIT)) ;


		dd = map.phi1(dd);
	} while( dd!=d);

}

//ok
template <typename PFP>
void swap4To4(typename PFP::MAP& map, Dart d)
{
	//!! 4 decouture inutile, seule l'intersection du centre doit etre decousu puis recousu

	Dart e = map.phi2(map.phi3(d));
	Dart dd = map.phi2(d);

	//unsew middle crossing darts
	map.unsewVolumes(d);
	map.unsewVolumes(map.phi2(map.phi3(dd)));

	Algo::Modelisation::Tetrahedron::swap2To2<PFP>(map, dd);
	Algo::Modelisation::Tetrahedron::swap2To2<PFP>(map, e);

	//sew middle darts so that they do not cross
	map.sewVolumes(d,map.phi2(map.phi3(e)));
	map.sewVolumes(map.phi2(map.phi3(dd)),map.phi2(e));
}

template <typename PFP>
void swap3To2(typename PFP::MAP& map, Dart d)
{
	Dart en1 = map.phi_1(map.phi2(d));
	Dart en2 = map.phi1(d);
	Dart fi = map.phi2(en1);

	//Decouture de la premiere face
	unlinkFromEdge<PFP>(map, map.phi2(en1));
	unlinkFromEdge<PFP>(map, map.phi2(en2));

	//Decouture de la seconde face
	en1 = map.phi1(map.phi2(en1));
	en2 = map.phi_1(map.phi2(en2));
	unlinkFromEdge<PFP>(map, map.phi2(en1));
	unlinkFromEdge<PFP>(map, map.phi2(en2));

	//Decouture de la troisieme face
	en1 = map.phi1(map.phi2(en1));
	en2 = map.phi_1(map.phi2(en2));
	unlinkFromEdge<PFP>(map, map.phi2(en1));
	unlinkFromEdge<PFP>(map, map.phi2(en2));

	//Faces interieurs
	Dart fi2 = map.phi2(map.phi1(fi));
	Dart fi3 = map.phi2(map.phi3(map.phi1(fi)));

	map.deleteFace(fi);
	map.deleteFace(fi2);
	map.deleteFace(fi3);

	    //Couture de cette face au milieu des 2 tetraedres
	    Dart f = map.newFace(3);
	    Dart fprim = map.newFace(3);
	    map.sewVolumes(f,fprim);
	//     Dart en = linkIntoEdge(f,d);

	    Dart ff=f;
	    Dart en= d;
	    do {
	        Dart e2 = map.phi2(en);
	        Dart d3 = map.phi3(ff);
	        map.unsewFaces(en);
	        map.sewFaces(e2,d3);
	        map.sewFaces(en,ff);

	        en = map.phi_1(map.phi2(map.phi_1(en)));

	        ff = map.phi1(ff);
	    } while(ff!=f);


		Dart dd = d;

		do {
			Dart e = map.phi2(map.phi3(map.phi2(dd)));

			unsigned int eVEmb = map.getDartEmbedding(VERTEX_ORBIT, e) ;
			unsigned int ddVEmb = map.getDartEmbedding(VERTEX_ORBIT, dd) ;

			map.setDartEmbedding(VERTEX_ORBIT, map.phi2(dd), eVEmb) ;
			map.setDartEmbedding(VERTEX_ORBIT, map.phi2(e), ddVEmb) ;

			dd = map.phi1(map.phi2(map.phi1(dd)));
		} while( dd!=d);

}



//[precond] le brin doit venir d'une face partagé par 2 tetraèdres
// renvoie un brin de l'ancienne couture entre les 2 tetras qui est devenu une arête
template <typename PFP>
void swap2To3(typename PFP::MAP& map, Dart d)
{
	Dart e = map.phi1(map.phi2(map.phi3(d)));

	unsigned int p1 = map.getDartEmbedding(VERTEX_ORBIT, map.phi_1(map.phi2(d))) ;
	unsigned int p2 = map.getDartEmbedding(VERTEX_ORBIT, map.phi2(map.phi1(map.phi2(map.phi3(d))))) ;

	//détachement des demi-faces du milieu
	//garde la relation volumique qui les lies
	//relie les faces de bords des 2 tetraèdres
	//renvoie le brin de base d'un des 2 tétraèdres
	unlinkFace<PFP>(map, d);

	//Couture de la premiere face
	Dart en1 = linkIntoEdge<PFP>(map,d,e);
	Dart en2 = linkIntoEdge<PFP>(map, map.phi1(d), map.phi_1(map.phi2(map.phi_1(e))));
	map.setDartEmbedding(VERTEX_ORBIT, map.phi_1(d), p1);
	map.setDartEmbedding(VERTEX_ORBIT, map.phi1(map.phi3(d)), p2);

	///Couture de la seconde face
	en1 = map.phi1(map.phi1(en1));
	en2 = map.phi_1(map.phi_1(en2));

	Dart f1 = map.newFace(3);
	Dart f1prim = map.newFace(3);
	map.sewVolumes(f1,f1prim);
	en1 = linkIntoEdge<PFP>(map, f1,en1);
	en2 = linkIntoEdge<PFP>(map, map.phi1(f1),en2);

	map.setDartEmbedding(VERTEX_ORBIT, map.phi_1(f1), p1);
	map.setDartEmbedding(VERTEX_ORBIT, map.phi1(map.phi3(f1)), p2);

	///Couture de la troisieme face
	en1 = map.phi1(map.phi1(en1));
	en2 = map.phi_1(map.phi_1(en2));

	Dart f2 = map.newFace(3);
	Dart f2prim = map.newFace(3);
	map.sewVolumes(f2,f2prim);
	en1 = linkIntoEdge<PFP>(map, f2,en1);
	en2 = linkIntoEdge<PFP>(map, map.phi1(f2),en2);

	//couture des 3 faces du milieu
	map.sewFaces(map.phi_1(d), map.phi1(map.phi3(f2)));
	map.sewFaces(map.phi1(map.phi3(d)), map.phi_1(f1));
	map.sewFaces(map.phi1(map.phi3(f1)), map.phi_1(f2));

	map.setDartEmbedding(VERTEX_ORBIT, map.phi_1(f2), p1);
	map.setDartEmbedding(VERTEX_ORBIT, map.phi1(map.phi3(f2)), p2);

}

template <typename PFP>
void swap5To4(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& positions)
{


}

}//end namespace Tetrahedron
}//end namespace Modelisation
}//end namespace Algo
}//end namespace CGoGN


