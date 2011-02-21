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

#include "Utils/glutwingl3.h"

#include "Topology/generic/parameters.h"
#include "Topology/map/map2.h"
#include "Topology/generic/embeddedMap2.h"


#include "Algo/Geometry/boundingbox.h"
#include "Algo/Render/gl3mapRender.h"

#include "Algo/Modelisation/polyhedron.h"
#include "Geometry/vector_gen.h"
#include "Algo/Geometry/normal.h"

using namespace CGoGN ;


/**
 * Struct that contains some informations about the types of the manipulated objects
 * Mainly here to be used by the algorithms that are parameterized by it
 */
struct PFP: public PFP_STANDARD
{
	// definition of the type of the map
	typedef EmbeddedMap2<Map2> MAP;
};


/**
 * A class for a little interface and rendering
 */
class MyGlutWin: public Utils::SimpleGlutWinGL3
{
public:

	void myRedraw();

    void myKeyboard(unsigned char keycode, int x, int y);

    void renderColor();

    void renderPhong();

    void initPhongMaterial();

    PFP::REAL gWidthObj;
    PFP::VEC3 gPosObj;

    Algo::Render::GL3::MapRender* m_render;

    Utils::GLSLShader* current_shader;
    Utils::GLSLShader shaders[4];

    int rt;

 	MyGlutWin(int* argc, char **argv, int winX, int winY) : SimpleGlutWinGL3(argc, argv, winX, winY),rt(0)
    {
 		shaders[0].loadShaders("color_gl3.vert","color_gl3.frag");
 		shaders[1].loadShaders("phong_gl3.vert","phong_gl3.frag");
 		current_shader = &shaders[0];
 		setCurrentShader(current_shader);
    }
 	~MyGlutWin()
 	{
 		delete m_render ;
 	}
};


void MyGlutWin::initPhongMaterial()
{
	float diffuse[] = {0.0f,1.0f,0.0f,0.0f};
	float specular[] = {0.3f,0.3f,1.0f,0.0f};
	float ambient[] = {0.1f,0.03f,0.03f,0.0f};
	float shininess = 100.0f;
	float lightPos[] = {100.0,300.0,1000.0f};

	shaders[1].bind();

	shaders[1].setuniformf<4>("materialDiffuse",diffuse);
	shaders[1].setuniformf<4>("materialSpecular",specular);
	shaders[1].setuniformf<4>("materialAmbient",ambient);
	shaders[1].setuniformf<1>("shininess",&shininess);

	shaders[1].setuniformf<3>("LightPosition",lightPos);

}


void MyGlutWin::renderColor()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_LIGHTING);

	current_shader->bind();


	// draw the lines
	glLineWidth(2.0f);
	float colorLine[] = {1.0f,1.0f,0.0f,0.0f};
	current_shader->setuniformf<4>("Color",colorLine);
	m_render->draw(Algo::Render::GL3::LINES);

	// draw the faces
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);

	float colorFace[] = {0.1f,0.1f,0.3f,0.0f};
	current_shader->setuniformf<4>("Color",colorFace);
	m_render->draw(Algo::Render::GL3::TRIANGLES);

	glDisable(GL_POLYGON_OFFSET_FILL);

}

void MyGlutWin::renderPhong()
{

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_LIGHTING);

	current_shader->bind();

	m_render->draw(Algo::Render::GL3::TRIANGLES);
}


void MyGlutWin::myRedraw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	oglPushModelViewMatrix();

	switch(rt)
	{
	case 0:
		renderColor();
		break;
	case 1:
		renderPhong();
		break;
	}

//	oglPopModelViewMatrix();
}


void MyGlutWin::myKeyboard(unsigned char keycode, int x, int y)
{

	switch(keycode)
	{
	case 'c':
		rt=0;
		current_shader = &shaders[0];
		setCurrentShader(current_shader);
		std::cout << "Color"<< std::endl;
		break;
	case 'p':
		rt=1;
		current_shader = &shaders[1];
		setCurrentShader(current_shader);
		std::cout << "Phong"<< std::endl;
		break;
	}
	glutPostRedisplay();

}


int main(int argc, char **argv)
{

    // instanciation of the interface
	MyGlutWin mgw(&argc, argv, 800, 800);

	// declaration of the map
	PFP::MAP myMap;

	AttributeHandler<PFP::VEC3> position = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "position");
	AttributeHandler<PFP::VEC3> normal = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "normal");

	Algo::Modelisation::Polyhedron<PFP> prim3(myMap, position);
	prim3.tore_topo(16, 32);
	prim3.embedTore(30.0f,10.0f);

	Algo::Geometry::computeNormalVertices<PFP>(myMap,position,normal);

	mgw.init();

	glClearColor(0.1,0.1,0.3,0.0);

    // computation of the bounding box
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
    mgw.gWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
    mgw.gPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

    mgw.setParamObject(mgw.gWidthObj,mgw.gPosObj.data());

    // instanciation of the renderer (here using VBOs)
    mgw.m_render = new Algo::Render::GL3::MapRender();


	// declare names attributes of shaders
    mgw.m_render->useVertexAttributeName("VertexPosition",mgw.shaders[0]);
    mgw.m_render->useVertexAttributeName("VertexPosition",mgw.shaders[1]);
    unsigned int attNormalId = mgw.m_render->useVertexAttributeName("VertexNormal",mgw.shaders[1]);

    // send data to gc
    mgw.m_render->updateData("VertexPosition", position);	// with name
    mgw.m_render->updateData(attNormalId, normal);			//with Id

    // update the renderer (primitives and geometry)
	SelectorTrue allDarts;
    mgw.m_render->initPrimitives<PFP>(myMap, allDarts,Algo::Render::GL3::TRIANGLES);
    mgw.m_render->initPrimitives<PFP>(myMap, allDarts,Algo::Render::GL3::LINES);
    mgw.m_render->initPrimitives<PFP>(myMap, allDarts,Algo::Render::GL3::POINTS);

    mgw.initPhongMaterial();

    mgw.mainLoop();

    return 0;
}
