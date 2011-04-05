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


#include "Algo/Geometry/boundingbox.h"
#include "Algo/Render/GL2/mapRender.h"

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
class MyGlutWin: public Utils::SimpleGlutWin
{
public:

	void myRedraw();

    PFP::REAL gWidthObj;
    PFP::VEC3 gPosObj;

    Algo::Render::GL2::MapRender_VBO* m_render;

    Utils::GLSLShader shaders[4];

    Geom::Vec4f colSpec;
	float shininess;

    int rt;

 	MyGlutWin(int* argc, char **argv, int winX, int winY) : SimpleGlutWin(argc, argv, winX, winY),rt(0)
    {
 		shaders[0].loadShaders("phong_gl2.vert","phong_gl2.frag");

 	    colSpec = Geom::Vec4f(0.9f, 0.9f, 0.9f, 1.0f) ;
 	    shininess = 80.0f ;
    }
 	~MyGlutWin()
 	{
 		delete m_render ;
 	}
};




void MyGlutWin::myRedraw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	// center the scene
	float sc = 50./gWidthObj;
	glScalef(sc,sc,sc);
	glTranslatef(-gPosObj[0],-gPosObj[1],-gPosObj[2]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	// some materials
	glMaterialfv(GL_FRONT, GL_SPECULAR, colSpec.data()) ;
	glMaterialf(GL_FRONT, GL_SHININESS, shininess) ;

	// bin shader
	shaders[0].bind();

	// draw triangles
	m_render->draw(Algo::Render::GL2::TRIANGLES) ;

	// unbind (in case of other rendering wihtout shader)
	shaders[0].unbind();

	glPopMatrix();


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

	// compute the normal of mesh
	Algo::Geometry::computeNormalVertices<PFP>(myMap,position,normal);

	mgw.init();

	glClearColor(0.1,0.1,0.1,0.0);
	glEnable(GL_DEPTH_TEST);

    // computation of the bounding box
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
    mgw.gWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
    mgw.gPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);


    // instanciation of the vbo renderer
    mgw.m_render = new Algo::Render::GL2::MapRender_VBO();

    // vertex attribute declaration (max 12)
    mgw.m_render->useVertexAttributeName("ColorPerVertex",mgw.shaders[0]);

    // must relink the shader after binding attributes
    mgw.shaders[0].link();

    // copy data from map attributes to vbo for  vertex attribute
    mgw.m_render->updateVAData("ColorPerVertex", normal);

    // copy data from map attributes to vbo
    mgw.m_render->updateData(Algo::Render::GL2::POSITIONS, position);
    mgw.m_render->updateData(Algo::Render::GL2::NORMALS, normal);

    // update the renderer (primitives)
	SelectorTrue allDarts;
    mgw.m_render->initPrimitives<PFP>(myMap, allDarts,Algo::Render::GL2::TRIANGLES);
//    mgw.m_render->initPrimitives<PFP>(myMap, allDarts,Algo::Render::GL2::LINES);
//    mgw.m_render->initPrimitives<PFP>(myMap, allDarts,Algo::Render::GL2::POINTS);


    mgw.mainLoop();

    return 0;
}
