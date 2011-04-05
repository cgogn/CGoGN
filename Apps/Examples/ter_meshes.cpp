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
void fonction_exemple(typename PFP::MAP& map, const AttributeHandler<typename PFP::VEC3>& position, Dart d)
{
	// affiche les Sommets d'une face:
	Dart e=d;
	do
	{
		std::cout << "Position brin"<<e.index<< " = "<< position[e]<< std::endl;
		e = map.phi1(e); // dans l'autre sens myMap.phi_1(e)
	}while (e!=d);
}




int main(int argc, char **argv)
{
	// declaration of the map
	PFP::MAP myMap;

	// this selector is going to select all the darts
	SelectorTrue allDarts;


	std::vector<std::string> attrNames ;
	if(!Algo::Import::importMesh<PFP>(myMap, argv[1], attrNames))
	{
		std::cerr << "could not import " << argv[1] << std::endl ;
		return 1 ;
	}
	// cree un handler d'attribut pour la position des points (créé lors de l'import)
	AttributeHandler<PFP::VEC3> position = myMap.getAttribute<PFP::VEC3>(VERTEX_ORBIT, attrNames[0]) ;
	// cree un handler pour les normales aux sommets
	AttributeHandler<PFP::VEC3> normal = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "normal");


//	// parcours de tous les brins de la carte:
//	for (Dart d = myMap.begin(); d != myMap.end(); myMap.next(d))
//		fonction_exemple<PFP>(myMap, position, d);

	// parcours de toutes les faces de la carte:
	DartMarker mf(myMap);
	for (Dart d = myMap.begin(); d != myMap.end(); myMap.next(d))
	{
		if (! mf.isMarked(d)) // si d non marque:
		{
			fonction_exemple<PFP>(myMap, position, d);
			// marque tous les brins de la face de d
			mf.markOrbit(FACE_ORBIT, d);
		}
	}

	// pas obligatoire (fait dans le destructeur)
	mf.unmarkAll();


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
