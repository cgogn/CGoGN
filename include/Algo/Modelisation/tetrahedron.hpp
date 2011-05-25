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

namespace CGoGN
{

namespace Algo
{

namespace Modelisation
{

namespace Tetrahedron
{
/***********************************************************************************************
 * 										Tetrahedron functions								   *
 ***********************************************************************************************/

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


/***********************************************************************************************
 * 										swap functions										   *
 ***********************************************************************************************/
//ok
template <typename PFP>
void swap2To2(typename PFP::MAP& the_map, Dart d, typename PFP::TVEC3& positions)
{
	unsigned int p1 = EMBNULL;
	unsigned int p2 = EMBNULL;

	Dart r = the_map.phi2(d);
	Dart f = the_map.phi2(the_map.phi3(d));

	Dart s1 = the_map.phi_1(r);
	Dart s2 = the_map.phi_1(f);

	p1 = the_map.getEmbedding(the_map.phi_1(r), VERTEX_ORBIT);
	p2 = the_map.getEmbedding(the_map.phi_1(f), VERTEX_ORBIT);

	// Topological swap
	Dart e = d;
	do
	{
		//the_map.unsewFace(e);
		e = the_map.phi1(e);
	}
	while (e != d);

	the_map.flipEdge(r);
	the_map.insertFace(r,d);


	the_map.setDartEmbedding(VERTEX_ORBIT,r,p2);
	the_map.setDartEmbedding(VERTEX_ORBIT,f,p1);

	Dart dd = d;
	do {
		Dart e = the_map.phi2(dd);
		Dart e2 = the_map.template phi<32>(dd);
		the_map.setDartEmbedding(VERTEX_ORBIT, dd, the_map.getEmbedding(e2,VERTEX_ORBIT)) ;
		the_map.setDartEmbedding(VERTEX_ORBIT, the_map.phi3(dd), the_map.getEmbedding(e,VERTEX_ORBIT)) ;
		dd = the_map.phi1(dd);
	}while(dd!=d);


}

//ok
template <typename PFP>
void swap4To4(typename PFP::MAP& the_map, Dart d, typename PFP::TVEC3& positions)
{
	//!! 4 decouture inutile, seule l'intersection du centre doit etre decousu puis recousu

	Dart e = the_map.phi2(the_map.phi3(d));
	Dart dd = the_map.phi2(d);

	//unsew middle crossing darts
	the_map.unsewVolumes(d);
	the_map.unsewVolumes(the_map.phi2(the_map.phi3(dd)));

	Algo::Modelisation::Tetrahedron::swap2To2<PFP>(the_map, dd, positions);
	Algo::Modelisation::Tetrahedron::swap2To2<PFP>(the_map, e, positions);

	//sew middle darts so that they do not cross
	the_map.sewVolumes(d,the_map.phi2(the_map.phi3(e)));
	the_map.sewVolumes(the_map.phi2(the_map.phi3(dd)),the_map.phi2(e));
}

template <typename PFP>
void swap3To2(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& positions)
{
    Dart en1 = map.phi_1(map.phi2(d));
    Dart en2 = map.phi1(d);
    Dart fi = map.phi2(en1);

    //Decouture de la premiere face
//    map.unsewFace(map.phi2(en1));
//    map.unsewFace(map.phi2(en2));
//
//    //Decouture de la seconde face
//    en1 = map.phi1(map.phi2(en1));
//    en2 = map.phi_1(map.phi2(en2));
//    map.unsewFace(map.phi2(en1));
//    map.unsewFace(map.phi2(en2));
//
//    //Decouture de la troisieme face
//    en1 = map.phi1(map.phi2(en1));
//    en2 = map.phi_1(map.phi2(en2));
//    map.unsewFace(map.phi2(en1));
//    map.unsewFace(map.phi2(en2));

    //Faces interieurs
    Dart fi2 = map.phi2(map.phi1(fi));
    Dart fi3 = map.phi2(map.phi3(map.phi1(fi)));

    map.deleteFace(fi);
    map.deleteFace(fi2);
    map.deleteFace(fi3);

    //Couture de cette face au milieu des 2 tetraedres
    Dart f = map.newFace(3);
//    Dart g = map.newFace(3);
//    map.sewVolumes(f,g);
//
//    map.sewFace(f,d);

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

	//tMap3<DART>::flipTetra3To2(d);

	Dart dd = d;

	do {
		Dart e = map.phi2(map.phi3(map.phi2(dd)));
		map.setDartEmbedding(VERTEX_ORBIT, map.phi2(dd), map.getEmbedding(e,VERTEX_ORBIT)) ;
		map.setDartEmbedding(VERTEX_ORBIT, map.phi2(e), map.getEmbedding(dd,VERTEX_ORBIT)) ;

		dd = map.phi1(map.phi2(map.phi1(dd)));
	} while( dd!=d);
}



//[precond] le brin doit venir d'une face partagé par 2 tetraèdres
// renvoie un brin de l'ancienne couture entre les 2 tetras qui est devenu une arête
template <typename PFP>
void swap2To3(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& attributs)
{
	Dart e = map.phi2(d);
	map.mergeVolumes(d);

	Dart f = map.phi1(e);
	map.Map2::cutEdge(e);
	Dart g = map.phi1(e);
	attributs[g] = attributs[e];
	attributs[g] += attributs[f];
	attributs[g] *= 0.5;

	map.splitFace(f, map.phi1(map.phi1(f)));

//		unsigned int p1 = EMBNULL;
//		unsigned int p2 = EMBNULL;
//
//		Dart t1 = the_map.phi1(the_map.phi2(the_map.phi3(d)));
//		Dart t2 = the_map.phi_1(the_map.phi2(d));
//
//		p1 = the_map.getEmbedding(the_map.phi2(t1), VERTEX_ORBIT);
//		p2 = the_map.getEmbedding(t2, VERTEX_ORBIT);
//
//		//unsew the middle face
//		Dart temp = d;
//		do{
//			the_map.unsewFace(temp);
//			temp = the_map.phi1(temp);
//		}while(temp != d);
//
//		//the_map.deleteFace(d);
//		//Dart f = the_map.newFace(3);
//
//		//sew a first face
//		the_map.sewFace(t1, d);
//		the_map.sewFace(t2,the_map.phi1(d));
//
//		positions[the_map.phi_1(d)] = positions[t2];
//		positions[the_map.phi1(the_map.phi3(d))] = positions[the_map.phi2(t1)];
//		//the_map.setDartEmbedding(VERTEX_ORBIT, the_map.phi_1(d), p2) ;
//		//the_map.setDartEmbedding(VERTEX_ORBIT, the_map.phi1(the_map.phi3(d)), p1) ;
//
//
//		//Couture de la seconde face
//		t1 = the_map.phi2(the_map.phi1(t1));
//		t2 = the_map.phi2(the_map.phi1(the_map.phi1(t2)));
//
//		Dart f1 = the_map.newFace(3);
//		the_map.sewFace(t1,f1);
//		the_map.sewFace(t2, the_map.phi1(f1));
//
//		positions[the_map.phi_1(f1)] = positions[t2];
//		positions[the_map.phi1(the_map.phi3(f1))] = positions[the_map.phi2(t1)];


//		//the_map.setDartEmbedding(VERTEX_ORBIT, the_map.phi_1(f1), p2) ;
//		//the_map.setDartEmbedding(VERTEX_ORBIT, the_map.phi1(the_map.phi3(f1)), p1) ;
//		the_map.embedOrbit(VERTEX_ORBIT, the_map.phi_1(f1), p2);
//		the_map.embedOrbit(VERTEX_ORBIT, the_map.phi1(the_map.phi3(f1)), p1);
//		the_map.embedOrbit(VERTEX_ORBIT, f1, the_map.getEmbedding(t1 ,VERTEX_ORBIT));
//		the_map.embedOrbit(VERTEX_ORBIT, the_map.phi1(f1), the_map.getEmbedding(the_map.phi1(t2) ,VERTEX_ORBIT));


		///Couture de la troisieme face
//		t1 = the_map.phi2(the_map.phi1(t1));
//		t2 = the_map.phi2(the_map.phi1(the_map.phi1(t2)));
//
//		Dart f2 = the_map.newFace(3);
//		the_map.sewFace(t1,f2);
//		the_map.sewFace(t2,the_map.phi1(f2));
//
////		the_map.setDartEmbedding(VERTEX_ORBIT, the_map.phi_1(f2), p2) ;
////		the_map.setDartEmbedding(VERTEX_ORBIT, the_map.phi1(the_map.phi3(f2)), p1) ;
//		the_map.embedOrbit(VERTEX_ORBIT, the_map.phi_1(f2), p2);
//		the_map.embedOrbit(VERTEX_ORBIT, the_map.phi1(the_map.phi3(f2)), p1);


		//couture des 3 faces du milieu
//		the_map.sewFaces(the_map.phi_1(d),the_map.phi1(the_map.phi3(f2)));
//		the_map.sewFaces(the_map.phi1(the_map.phi3(d)),the_map.phi_1(f1));
//		the_map.sewFaces(the_map.phi1(the_map.phi3(f1)),the_map.phi_1(f2));

//		the_map.embedOrbit(VERTEX_ORBIT, d, p1);
//		the_map.embedOrbit(VERTEX_ORBIT, the_map.phi1(the_map.phi3(d)), p2);
}

template <typename PFP>
void swap5To4(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& positions)
{


}

}//end namespace Tetrahedron
}//end namespace Modelisation
}//end namespace Algo
}//end namespace CGoGN


