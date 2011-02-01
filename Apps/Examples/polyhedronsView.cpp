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
#include "Geometry/matrix.h"
#include "Geometry/transfo.h"
#include "Algo/Geometry/boundingbox.h"

#include "Algo/Render/vbo_MapRender.h"
#include "Algo/Modelisation/polyhedron.h"
#include "Topology/generic/parameters.h"

using namespace CGoGN;



struct PFP: public PFP_STANDARD
{
	// definition de la carte
	typedef EmbeddedMap2<Map2> MAP;
};


PFP::MAP myMap;


class myGlutWin: public Utils::SimpleGlutWin
{
public:
	/**
	 * position of object
	 */
	Geom::Vec3f gPosObj;

	/**
	 * width of object
	 */
	float gWidthObj;

	/**
	 * redraw CB
	 */
	void myRedraw();

	/**
	 * keyboard CB
	 */
	void myKeyboard(unsigned char keycode, int x, int y);

	/**
	 * Update VBO buffers for map
	 */
	void updateRender();

	/**
	 * GL initialization
	 */
	void init();

	// position handler
	PFP::TVEC3 position ;


	/**
	 * render object
	 */
	Algo::Render::VBO::MapRender_VBO* m_render;



	myGlutWin(	int* argc, char **argv, int winX, int winY) :
				SimpleGlutWin(argc,argv,winX,winY),
				m_render(NULL)
				 {	}

	~myGlutWin();
};

myGlutWin::~myGlutWin()
{
	if (m_render)
		delete m_render;
}

void myGlutWin::init()
{

	glClearColor(0.1f,0.1f,0.1f,0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightZeroPosition);
	glEnable(GL_LIGHT0);
}


void myGlutWin::updateRender()
{
	SelectorTrue allDarts;

	// create the renderer (first call only)
	if (m_render == NULL)
		m_render = new Algo::Render::VBO::MapRender_VBO() ;

	// update buffer op vertices positions
	m_render->updateData(Algo::Render::VBO::POSITIONS, position);
	// update flat faces primtives (warning need position buffer)
	m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::VBO::FLAT_TRIANGLES);
	// update lines primitives
	m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::VBO::LINES);

}


void myGlutWin::myRedraw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	// center the scene
	float sc = 50./gWidthObj;
	glScalef(sc,sc,sc);
	glTranslatef(-gPosObj[0],-gPosObj[1],-gPosObj[2]);

	// DRAW LINES OF OBJECTS
	glDisable(GL_LIGHTING);
	glColor3f(1.0f,1.0f,0.0f);
	m_render->draw(Algo::Render::VBO::LINES) ;

	//shit for nice line rendering
	glEnable( GL_POLYGON_OFFSET_FILL );
	glPolygonOffset( 1.0f, 1.0f );

	// DRAW FACES OF OBJECTS
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);
	glColor3f(0.0f,0.0f,0.9f);
	m_render->draw(Algo::Render::VBO::FLAT_TRIANGLES) ;

	glDisable( GL_POLYGON_OFFSET_FILL );

	glPopMatrix();
}

void myGlutWin::myKeyboard(unsigned char keycode, int x, int y)
{
	switch(keycode)
	{
	case 27:
		exit(0);
	default:
		break;
	}
}


int main(int argc, char **argv)
{
	/// init glut interface and
	myGlutWin mgw(&argc,argv,800,800);
	mgw.init();

	mgw.position = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "position") ;


	// create a cylinder
	Algo::Modelisation::Polyhedron<PFP> prim1(myMap, mgw.position);
	prim1.cylinder_topo(16,8, true, false);
	prim1.embedCylinder(30.0f, 10.0f, 70.0f);

	// create a sphere
	Algo::Modelisation::Polyhedron<PFP> prim2(myMap, mgw.position);
	prim2.cylinder_topo(16,16, true, true); // topo of sphere is a closed cylinder
	prim2.embedSphere(40.0f);

	// create a geometric transfomation (homogeneous matrix) and apply it to primitive
	Geom::Matrix44f trf;
	trf.identity();
	Geom::scale<float>(1.0f,1.0f,1.5f,trf);
	Geom::translate<float>(90.0f,0.0,0.0,trf);
	prim2.transform(trf);

	// create a tore
	Algo::Modelisation::Polyhedron<PFP> prim3(myMap, mgw.position);
	prim3.tore_topo(16, 12);
	prim3.embedTore(30.0f,10.0f);
	// transformation
	trf.identity();
	Geom::rotateX<float>(0.6f,trf);
	Geom::rotateY<float>(0.4f,trf);
	Geom::translate<float>(180.0f,0.0,0.0,trf);
	prim3.transform(trf);

	// create a subdivided cube
	Algo::Modelisation::Polyhedron<PFP> prim4(myMap, mgw.position);
	prim4.cube_topo(4, 5, 6);
	prim4.embedCube(50.0f,50.0f, 50.0f);

	//transform
	trf.identity();
	Geom::rotate<float>(1.0f,1.0f,1.0f,0.4f,trf);
	Geom::translate<float>(270.0f,0.0,0.0,trf);
	prim4.transform(trf);

	// update renderer
	mgw.updateRender();


	// compute BB and store it for object positionning in screen
	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap,mgw.position) ;
	mgw.gWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
	mgw.gPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

	mgw.mainLoop();

	return 0;
}


