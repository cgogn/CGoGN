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

#include "Utils/glutwin.h"

#include "Topology/map/map2.h"
#include "Topology/generic/mapBrowser.h"

#include "Geometry/matrix.h"
#include "Geometry/vector_gen.h"
#include "Algo/Import/import.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Render/map_glRender.h"
#include "Algo/Render/vbo_MapRender.h"


using namespace CGoGN ;

struct PFP {
	// definition de la carte
	typedef Map2 MAP;

	// definition du type de reel utilise
	typedef float REAL;
	// definition du type de vecteur (point) utilise
	typedef Geom::Vector<3,REAL> VEC3;
	// definition du type de matrice 3x3 utilise
	typedef Geom::Matrix<3,3,REAL> MATRIX33;
	// definition du type de matrice 4x4 utilise
	typedef Geom::Matrix<4,4,REAL> MATRIX44;

	// definition du type du AttributeHandler de vecteur 3D
	typedef AttributeHandler<VEC3> TVEC3;
};


PFP::MAP myMap;

SelectorTrue allDarts;


PFP::TVEC3 position ;
PFP::TVEC3 normal ;
AttributeHandler<Geom::Vec4f> color ;

class myGlutWin: public Utils::SimpleGlutWin
{
public:

     void myRedraw();

     float gWidthObj;
     Geom::Vec3f gPosObj;

     Algo::Render::VBO::MapRender_VBO<PFP>* m_render;

 	myGlutWin(	int* argc, char **argv, int winX, int winY):SimpleGlutWin(argc,argv,winX,winY) {}
};

void myGlutWin::myRedraw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	// center the object
	float sc = 50.0f / gWidthObj;
	glScalef(sc, sc, sc);
	glTranslatef(-gPosObj[0], -gPosObj[1], -gPosObj[2]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_LIGHTING);

	// draw the lines
	glColor3f(1.0f, 1.0f, 0.0f);
	m_render->draw(Algo::Render::VBO::LINES);

	// draw the faces
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	glColor3f(0.0f, 0.5f, 0.0f);
	m_render->draw(Algo::Render::VBO::TRIANGLES);
	glDisable(GL_POLYGON_OFFSET_FILL);

	glPopMatrix();
}


// Algorithme qui parcours une carte et affiche l'attribut position de chaque brin
//
template<typename PFP>
void TestDeParcoursAFF(typename PFP::MAP& m, MapBrowser& mb, const typename PFP::TVEC3& pos)
{
	for (Dart d = mb.begin(); d != mb.end(); mb.next(d))
	{
		typename PFP::VEC3 P = pos[d];
		std::cout << "P "<< P << std::endl;
	}
}


int main(int argc, char **argv)
{
	/// Utilisation des MapBrowsers

	/// on reprend la carte de tuto1

	Dart d2 = myMap.newOrientedFace(3);
	Dart d3 = myMap.newOrientedFace(4);
	myMap.sewFaces(d2,d3);

	position = myMap.addAttribute<Geom::Vec3f>(VERTEX_ORBIT, "position");

	position[d2] = PFP::VEC3(0.0f, 0.0f, 0.0f);
	d2 = myMap.phi1(d2);
	position[d2] = PFP::VEC3(2.0f, 0.0f, 0.0f);
	d2 = myMap.phi1(d2);
	position[d2] = PFP::VEC3(1.0f, 3.0f, 0.0f);
	d2 = myMap.phi1(d2);
	d3 = myMap.phi<11>(d3);
	position[d3] = PFP::VEC3(0.0f, -2.0f, 0.0f);
	d3 = myMap.phi1(d3);
	position[d3] = PFP::VEC3(2.0f, -2.0f, 0.0f);
	d3 = myMap.phi1(d3);

	// MapBrowser: la carte elle meme
	std::cout << "Parcours avec la carte"<<std::endl;
	TestDeParcoursAFF<PFP>(myMap, myMap, position);



	// MapBrowserLinkedAuto
	// sous-carte dans une liste avec attribut gere par le browser
	std::cout << "Parcours avec le browser (les sommets)"<<std::endl;

	// creation d'un browser avec attribut cree a la volee
	MapBrowserLinkedAuto<PFP::MAP>mbl(myMap);

	// on ajoute un brin par sommet dans le browser
	myMap.foreach_orbit(VERTEX_ORBIT, mbl);

	// et on parcours la sous-carte avec ce browser
	TestDeParcoursAFF<PFP>(myMap, mbl, position);



	// MapBrowserLinkedAttr
	// sous-carte dans une liste avec attribut gere par l'appelant
	std::cout << "Parcours avec le browser (le triangle)"<<std::endl;

	// on cree un attribut Dart pour la liste
	AutoAttributeHandler<Dart> tableLink(myMap, DART_ORBIT);
	// le browser
	MapBrowserLinkedAttr<PFP::MAP>mbl2(tableLink);
	// que l'on remplit a la main
	Dart d = d2;
	mbl2.add(d);
	d =  myMap.phi1(d);
	mbl2.add(d);
	d =  myMap.phi1(d);
	mbl2.add(d);

	// et on parcours la sous-carte avec ce browser
	TestDeParcoursAFF<PFP>(myMap, mbl2, position);



	// MapBrowserSelector
	// sous-carte  avec des brins marques (equivalent de l'utilisation de good dans les algos)
	std::cout << "Parcours avec le browser selector"<<std::endl;

	// on marque 2 brins pour le test
	DartMarker mk(myMap);
	mk.mark(d2);
	mk.mark(d3);

	// le selector qui selectionne les "bons" brins (les autres sont reconduits a la frontiere ;)
	SelectorMarked selector(mk);

//	// le browser
//	MapBrowserSelector<PFP::MAP>mbsel(myMap,selector);

//	// et on parcours la sous-carte avec ce browser
//	TestDeParcoursAFF<PFP>(myMap,mbsel,position);


    // un peu d'interface
	myGlutWin mgw(&argc,argv,800,800);

    // calcul de la bounding box
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
    // pour l'affichage
    mgw.gWidthObj = std::max<float>(std::max<float>(bb.size(0),bb.size(1)),bb.size(2));
    mgw.gPosObj =  (bb.min() +  bb.max()) /2.0f;

    // allocation des objets necessaires pour le rendu
    mgw.m_render = new Algo::Render::VBO::MapRender_VBO<PFP>(myMap, allDarts);

    // maj des donnees de position
    mgw.m_render->updateData(Algo::Render::VBO::POSITIONS, position);
    // creation des primitives de rendu a partir de la carte
    mgw.m_render->initPrimitives(Algo::Render::VBO::TRIANGLES);
    mgw.m_render->initPrimitives(Algo::Render::VBO::LINES);

    mgw.mainLoop();

    delete mgw.m_render;

    return 0;
}
