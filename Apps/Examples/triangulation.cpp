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


#include "Topology/generic/parameters.h"
#include "Topology/map/map2.h"
#include "Topology/generic/embeddedMap2.h"

#include "Geometry/vector_gen.h"

#include "Algo/Import/import.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Render/vbo_MapRender.h"


using namespace CGoGN ;


/**
 * Struct that contains some informations about the types of the manipulated objects
 * Mainly here to be used by the algorithms that are parameterized by it
 */
struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef EmbeddedMap2<Map2> MAP;
};

/**
 * A class for a little interface and rendering
 */
class MyGlutWin: public Utils::SimpleGlutWin
{
public:
     void myRedraw();

     PFP::REAL gWidthObj;
     PFP::VEC3 gPosObj;

     Algo::Render::GL2::MapRender_VBO* m_render;

 	MyGlutWin(int* argc, char **argv, int winX, int winY) : SimpleGlutWin(argc, argv, winX, winY) {}
 	~MyGlutWin()
 	{
 		delete m_render ;
 	}
};

// Routine d'affichage
void MyGlutWin::myRedraw()
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
	m_render->draw(Algo::Render::GL2::LINES);

	// draw the faces
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	glColor3f(0.0f, 0.5f, 0.0f);
	m_render->draw(Algo::Render::GL2::TRIANGLES);
	glDisable(GL_POLYGON_OFFSET_FILL);

	glPopMatrix();
}

template<typename PFP>
void TriangulateFromIndices(const AttributeHandler<typename PFP::VEC3>& position, const std::vector<unsigned int> indices, std::vector<unsigned int>& triangles)
{
	// pour eviter le typename ...
	typedef typename PFP::VEC3 VEC3F;

	// combien de sommets ?
	CGoGNout << "Nombre de sommets: "<< indices.size()<< CGoGNendl;

	//juste une boucle pour les afficher
	unsigned int num=0;
	for (std::vector<unsigned int>::const_iterator it = indices.begin(); it != indices.end(); ++it)
	{
		VEC3F P = position[*it];
		CGoGNout << "Sommet num "<< num++ << " : indice "<< *it << " = "<< P << CGoGNendl;
	}

}

template<typename PFP>
void TriangulateFromDart(typename PFP::MAP& map, const AttributeHandler<typename PFP::VEC3>& position, Dart d, std::vector<unsigned int>& triangles)
{
	std::vector<unsigned int> indices;

	// fait le tour de la face et stock les indices des points
	Dart e = d;
	do
	{
		indices.push_back( map.getEmbedding(e,VERTEX) ); // recupere l'indice du point dans le handler
		e = map.phi1(e); // brin suivant dans la face
	} while (e != d); // tq par revenu au debut

	// appel à la fonction independante de la carte
	TriangulateFromIndices<PFP>(position,indices, triangles);
}






int main(int argc, char **argv)
{
	// declaration of the map
	PFP::MAP myMap;

	// this selector is going to select all the darts
	SelectorTrue allDarts;

	// creation of a new attribute on vertices of type 3D vector
	// a handler to this attribute is returned
	AttributeHandler<PFP::VEC3> position = myMap.addAttribute<PFP::VEC3>(VERTEX, "position");

	// creation face 1 (convexe)
	Dart d1 = myMap.newFace(5);

	// affecte les positions aux sommets
	Dart d = d1;

	position[d] = PFP::VEC3(-5, 0, 0);
	d = myMap.phi1(d);
	position[d] = PFP::VEC3(-1, 0, 0.3);
	d = myMap.phi1(d);
	position[d] = PFP::VEC3(-1, 3, 0.1);
	d = myMap.phi1(d);
	position[d] = PFP::VEC3(-3, 6, 0.7);
	d = myMap.phi1(d);
	position[d] = PFP::VEC3(-5, 4, 0.2);
	d = myMap.phi1(d);

	// seconde face (concave)
	Dart d2 = myMap.newFace(5);
	d = d2;
	position[d] = PFP::VEC3(1, 0, 0);
	d = myMap.phi1(d);
	position[d] = PFP::VEC3(5, 0, 0.4);
	d = myMap.phi1(d);
	position[d] = PFP::VEC3(5, 4, 0.1);
	d = myMap.phi1(d);
	position[d] = PFP::VEC3(2, 2, 0);
	d = myMap.phi1(d);
	position[d] = PFP::VEC3(1, 3, 0.2);
	d = myMap.phi1(d);

	// vecteur d'indices pour les triangles
	std::vector<unsigned int> triangles;

	// triangule la deuxieme face
	TriangulateFromDart<PFP>(myMap, position, d2, triangles);

    // instanciation of the interface
	MyGlutWin mgw(&argc, argv, 800, 800);

    // computation of the bounding box
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
    mgw.gWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
    mgw.gPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

    // instanciation of the renderer (here using VBOs)
    mgw.m_render = new Algo::Render::GL2::MapRender_VBO();

    // update the renderer (primitives and geometry)
    mgw.m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::TRIANGLES);
    mgw.m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::LINES);
    mgw.m_render->updateData(Algo::Render::GL2::POSITIONS, position);

    mgw.mainLoop();

    return 0;
}
