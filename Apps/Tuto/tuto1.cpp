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
struct PFP
{
	// definition of the type of the map
	typedef EmbeddedMap2<Map2> MAP;

	// definition of the type of real value
	typedef float REAL;
	// definition of the type of 3D vector
	typedef Geom::Vector<3,REAL> VEC3;

	// definition of the type of 3D vector attribute handler
	typedef AttributeHandler<VEC3> TVEC3;
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

     Algo::Render::VBO::MapRender_VBO<PFP>* m_render;

 	MyGlutWin(int* argc, char **argv, int winX, int winY) : SimpleGlutWin(argc, argv, winX, winY) {}
 	~MyGlutWin()
 	{
 		delete m_render ;
 	}
};

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
	m_render->draw(Algo::Render::VBO::LINES);

	// draw the faces
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	glColor3f(0.0f, 0.5f, 0.0f);
	m_render->draw(Algo::Render::VBO::TRIANGLES);
	glDisable(GL_POLYGON_OFFSET_FILL);

	glPopMatrix();
}

int main(int argc, char **argv)
{
	// declaration of the map
	PFP::MAP myMap;

	// this selector is going to select all the darts
	SelectorTrue allDarts;

	// creation of 2 new faces: 1 triangle and 1 square
	Dart d1 = myMap.newFace(3);
	Dart d2 = myMap.newFace(4);

	// sew these faces along one of their edge
	myMap.sewFaces(d1, d2);

	// creation of a new attribute on vertices of type 3D vector
	// a handler to this attribute is returned
	AttributeHandler<PFP::VEC3> position = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "position");

	// affect a position to the vertices of the mesh
	position[d1] = PFP::VEC3(0, 0, 0);
	position[myMap.phi1(d1)] = PFP::VEC3(2, 0, 0);
	position[myMap.phi_1(d1)] = PFP::VEC3(1, 2, 0);

	position[myMap.phi<11>(d2)] = PFP::VEC3(0, -2, 0);
	position[myMap.phi_1(d2)] = PFP::VEC3(2, -2, 0);

	// add another triangle
	Dart d3 = myMap.newOrientedFace(3);

	position[d3] = PFP::VEC3(4, 0, 0);
	position[myMap.phi1(d3)] = PFP::VEC3(8, 0, 0);
	position[myMap.phi_1(d3)] = PFP::VEC3(6, 3, 0);

    // instanciation of the interface
	MyGlutWin mgw(&argc, argv, 800, 800);

    // computation of the bounding box
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
    mgw.gWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
    mgw.gPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

    // instanciation of the renderer (here using VBOs)
    mgw.m_render = new Algo::Render::VBO::MapRender_VBO<PFP>(myMap, allDarts);

    // update the renderer (primitives and geometry)
    mgw.m_render->initPrimitives(Algo::Render::VBO::TRIANGLES);
    mgw.m_render->initPrimitives(Algo::Render::VBO::LINES);
    mgw.m_render->updateData(Algo::Render::VBO::POSITIONS, position);

    mgw.mainLoop();

    return 0;
}
