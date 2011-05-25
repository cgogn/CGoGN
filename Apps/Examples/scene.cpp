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

#include <iostream>

#include "Utils/glutwin.h"

#include "Topology/generic/parameters.h"
#include "Topology/map/map2.h"
#include "Topology/generic/embeddedMap2.h"

#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"
#include "Geometry/transfo.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Geometry/normal.h"

#include "Algo/Render/vbo_MapRender.h"
#include "Algo/Modelisation/polyhedron.h"
#include "Topology/generic/cellmarker.h"

#include "Utils/scene_graph.h"

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

	PFP::TVEC3 normal ;

	Utils::SceneGraph::Group_Node* root;

	/**
	 * render object
	 */
	Algo::Render::GL2::MapRender_VBO* m_render;



	myGlutWin(	int* argc, char **argv, int winX, int winY) :
				SimpleGlutWin(argc,argv,winX,winY),
				m_render(NULL)
				 {	}

	void exitCallback();

	~myGlutWin() {}

};

void myGlutWin::exitCallback()
{
	CGoGNout << "DESTRUCTOR"<< CGoGNendl;
	eraseGraph(root);
//	if (m_render)
//		delete m_render;

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
		m_render = new Algo::Render::GL2::MapRender_VBO() ;

	// update buffer op vertices positions
	m_render->updateData(Algo::Render::GL2::POSITIONS, position);
	// update flat faces primtives (warning need position buffer)
	m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::FLAT_TRIANGLES);
	// update smooth faces primtives
	m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::TRIANGLES);
	// update lines primitives
	m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::LINES);

	Algo::Geometry::computeNormalVertices<PFP>(myMap, position, normal) ;
	m_render->updateData(Algo::Render::GL2::NORMALS, normal);

}


void myGlutWin::myRedraw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	// center the scene
	float sc = 50./gWidthObj;
	glScalef(sc,sc,sc);
	glTranslatef(-gPosObj[0],-gPosObj[1],-gPosObj[2]);

//	// DRAW LINES OF OBJECTS
//	glDisable(GL_LIGHTING);
//	glColor3f(1.0f,1.0f,0.0f);
//	m_render->draw(Algo::Render::GL2::LINES) ;
//
//	//shit for nice line rendering
//	glEnable( GL_POLYGON_OFFSET_FILL );
//	glPolygonOffset( 1.0f, 1.0f );
//
//	// DRAW FACES OF OBJECTS
//	glEnable(GL_LIGHTING);
//	glEnable(GL_COLOR_MATERIAL);
//	glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);
//	glColor3f(0.0f,0.0f,0.9f);
//	m_render->draw(Algo::Render::GL2::TRIANGLES) ;
//
//	glDisable( GL_POLYGON_OFFSET_FILL );

	Utils::SceneGraph::renderGraph(root);

	glPopMatrix();
}

void myGlutWin::myKeyboard(unsigned char keycode, int x, int y)
{
	switch(keycode)
	{
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
	mgw.normal = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "normal") ;

	glewInit();
	if (! Utils::GLSLShader::areShadersSupported())
	{
		CGoGNerr << "Shaders not supported"<< CGoGNendl;
		exit(1);
	}

	Utils::GLSLShader* phong = new Utils::GLSLShader() ;
	phong->loadShaders("phong_vs.txt", "phong_ps.txt") ;


	Algo::Modelisation::Polyhedron<PFP> prim1(myMap, mgw.position);
	prim1.cylinder_topo(16,15, true, true);
	prim1.embedCylinder(30.0f,20.0f,50.0f);

	CellMarker mark1(myMap,VERTEX_CELL);
	prim1.mark(mark1);

	// create a sphere
	Algo::Modelisation::Polyhedron<PFP> prim2(myMap, mgw.position);
	prim2.cylinder_topo(16,16, true, true);
	prim2.embedSphere(40.0f);

	CellMarker mark2(myMap,VERTEX_CELL);
	prim2.mark(mark2);

	// create a sphere
	Algo::Modelisation::Polyhedron<PFP> prim3(myMap, mgw.position);
	prim3.tore_topo(12,48);
	prim3.embedTore(50.0f,10.0f);

	CellMarker mark3(myMap,VERTEX_CELL);
	prim3.mark(mark3);



	Algo::Geometry::computeNormalVertices<PFP>(myMap, mgw.position, mgw.normal) ;

	Algo::Render::GL2::MapRender_VBO* render1 = new Algo::Render::GL2::MapRender_VBO() ;
	render1->updateData(Algo::Render::GL2::POSITIONS, mgw.position);
	render1->updateData(Algo::Render::GL2::NORMALS, mgw.normal);

	SelectorCellMarked cm1(mark1);
	render1->initPrimitives<PFP>(myMap, cm1, Algo::Render::GL2::TRIANGLES);
	render1->initPrimitives<PFP>(myMap, cm1, Algo::Render::GL2::FLAT_TRIANGLES);
	render1->initPrimitives<PFP>(myMap, cm1, Algo::Render::GL2::LINES);


	Algo::Render::GL2::MapRender_VBO* render2 = new Algo::Render::GL2::MapRender_VBO(*render1) ;

	SelectorCellMarked cm2(mark2);
	render2->initPrimitives<PFP>(myMap, cm2, Algo::Render::GL2::TRIANGLES);
	render2->initPrimitives<PFP>(myMap, cm2, Algo::Render::GL2::FLAT_TRIANGLES);
	render2->initPrimitives<PFP>(myMap, cm2, Algo::Render::GL2::POINTS);

	Algo::Render::GL2::MapRender_VBO* render3 = new Algo::Render::GL2::MapRender_VBO(*render1) ;
	SelectorCellMarked cm3(mark3);
	render3->initPrimitives<PFP>(myMap, cm3, Algo::Render::GL2::FLAT_TRIANGLES);
	render3->initPrimitives<PFP>(myMap, cm3, Algo::Render::GL2::TRIANGLES);
	render3->initPrimitives<PFP>(myMap, cm3, Algo::Render::GL2::LINES);


	mgw.root = new Utils::SceneGraph::Group_Node();

	Utils::SceneGraph::Material_Node* mater = new Utils::SceneGraph::Material_Node();
	mater->setDiffuse(Geom::Vec4f(0.3f, 1.0f, 0.0f, 1.0f));
	mater->setAmbient(Geom::Vec4f(0.1f, 0.1f, 0.1f, 1.0f));
	mater->setSpecular(Geom::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
	mater->setShininess(100.0f);
	mater->setColor(Geom::Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
	mater->setShader(phong);

	mgw.root->setMaterial(mater);

	Utils::SceneGraph::GL2_Node* vbon = new Utils::SceneGraph::GL2_Node(render1);
	vbon->setPrimitives(Algo::Render::GL2::FLAT_TRIANGLES);
	vbon->setPrimitives(Algo::Render::GL2::LINES);

	mgw.root->addChild(vbon);

	Utils::SceneGraph::Group_Node* group1 = new Utils::SceneGraph::Group_Node();
	Geom::Matrix44f mat;
	mat.identity();
	Geom::translate(80.0f, 0.0f, 0.0f,mat);
	group1->setMatrixTransfo(mat);

	mgw.root->addChild(group1);

	Utils::SceneGraph::GL2_Node* vbon2 = new Utils::SceneGraph::GL2_Node(render2);
	vbon2->setPrimitives(Algo::Render::GL2::TRIANGLES);
	vbon2->setPrimitives(Algo::Render::GL2::POINTS);
	group1->addChild(vbon2);


	Utils::SceneGraph::Group_Node* group2 = new Utils::SceneGraph::Group_Node();
	mat.identity();
	Geom::translate(100.0f, 50.0f, 20.0f,mat);
	group2->setMatrixTransfo(mat);

	mgw.root->addChild(group2);


	mater = new Utils::SceneGraph::Material_Node();
	mater->setDiffuse(Geom::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));
	mater->setAmbient(Geom::Vec4f(0.1f, 0.1f, 0.1f, 1.0f));
	mater->setSpecular(Geom::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
	mater->setShininess(100.0f);
	mater->setColor(Geom::Vec4f(1.0f, 1.0f, 0.0f, 1.0f));
	mater->setNoShader();
	group2->setMaterial(mater);

	vbon = new Utils::SceneGraph::GL2_Node(render3);
	vbon->setPrimitives(Algo::Render::GL2::TRIANGLES);
	vbon->setPrimitives(Algo::Render::GL2::LINES);

	group2->addChild(vbon);


// not possible here to compute a BB !!
	mgw.gWidthObj = 200.0f;
	mgw.gPosObj = Geom::Vec3f(100.0f,0.0f,0.0f);

	mgw.mainLoop();

	return 0;
}


