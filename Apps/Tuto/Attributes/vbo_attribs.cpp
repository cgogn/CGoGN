/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2012, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include "vbo_attribs.h"
#include "Algo/Geometry/boundingbox.h"

using namespace CGoGN ;

int main(int argc, char **argv)
{
	//	// interface
	QApplication app(argc, argv);
	MyQT sqt;
	// copy output tout Qt console of application (shift enter)
	CGoGNout.toConsole(&sqt);

	// example code itself
	sqt.createMap();
	// set help message in menu
	sqt.setHelpMsg("Tuto 2: \nCreate and use multiple attributes\nrender with multiple shaders");

	// final show for redraw
	sqt.show();
	// and wait for the end
	return app.exec();
}

void MyQT::createMap()
{
	// creation of 2 new faces: 1 triangle and 1 square, sew and embed (see tuto1 for details)
	Dart d1 = myMap.newFace(3);
	Dart d2 = myMap.newFace(4);
	myMap.sewFaces(d1, d2);
	VertexAttribute<VEC3, MAP> position = myMap.addAttribute<PFP::VEC3, VERTEX, MAP>("position");
	position[d1] = PFP::VEC3(0, 0, 0);
	position[PHI1(d1)] = PFP::VEC3(2, 0, 0);
	position[PHI_1(d1)] = PFP::VEC3(1, 2, 0);
	position[PHI<11>(d2)] = PFP::VEC3(0, -2, 0);
	position[PHI_1(d2)] = PFP::VEC3(2, -2, 0);

	// create another attribute on vertices (for faces drawing)
	VertexAttribute<VEC3, MAP> colorF = myMap.addAttribute<PFP::VEC3, VERTEX, MAP>("colorF");

	colorF[d1] = Geom::Vec3f(1.0f,0.0f,0.0f);
	colorF[PHI1(d1)] = Geom::Vec3f(0.0f,1.0f,0.0f);
	colorF[PHI_1(d1)] = Geom::Vec3f(0.0f,0.0f,1.0f);
	colorF[PHI<11>(d2)] = Geom::Vec3f(1.0f,0.0f,1.0f);
	colorF[PHI_1(d2)] = Geom::Vec3f(0.0f,1.0f,1.0f);

	// create another attribute on vertices (for edges drawing)
	VertexAttribute<int, MAP> colorE = myMap.addAttribute<int, VERTEX, MAP>("colorE");

	colorE[d1] = 152;
	colorE[PHI1(d1)] = 255;
	colorE[PHI_1(d1)] = 64;
	colorE[PHI<11>(d2)] = 127;
	colorE[PHI_1(d2)] = 192;


    //  bounding box of scene
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
    float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
    Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

    // send BB info to interface for centering on GL screen
	setParamObject(lWidthObj, lPosObj.data());

	// first show for be sure that GL context is binded
	show();


	// HERE VBO conversion on the fly

	// update of position VBO  with on the fly conversion from double to float (automatic)
	m_positionVBO->updateData(position);

	// update color edge with on the fly computation of RGB from int
	// code writen in lambda
//	m_colorVBO1->updateDataConversion<int,3>(colorE, [](const float& x)
//	{
//		return Geom::Vec3f(float(x)/255.0f,float(x)/255.0f,float(x)/255.0f);
//	});
	// or with furnished operator of conversion
	DataConversion::functorScalarToRGBf<int> conv2col(0,255);
	m_colorVBO1->updateDataConversion<int,3>(colorE,conv2col);


	// update color face with on the fly inversion of RGB
	m_colorVBO2->updateDataConversion<PFP::VEC3,3>(colorF, [](const PFP::VEC3& c)
	{
		return Geom::Vec3f(float(1.0-c[2]),float(1.0-c[1]),float(1.0-c[0]));
	});



	// construct rendering primities
	m_render->initPrimitives<PFP>(myMap, Algo::Render::GL2::TRIANGLES);
	m_render->initPrimitives<PFP>(myMap, Algo::Render::GL2::LINES);
	m_render->initPrimitives<PFP>(myMap, Algo::Render::GL2::POINTS);

}

// initialization GL callback
void MyQT::cb_initGL()
{
	// create the render
	m_render = new Algo::Render::GL2::MapRender();

	// create VBO for position
	m_positionVBO = new Utils::VBO();
	// and color
	m_colorVBO1 = new Utils::VBO();
	m_colorVBO2 = new Utils::VBO();

	m_scalarVBO = new Utils::VBO();

	// using simple shader with color
	m_shader = new Utils::ShaderSimpleColor();
	m_shader->setAttributePosition(m_positionVBO);
	m_shader->setColor(Geom::Vec4f(0.0f, 1.0f, 0.0f, 0.0f));
	// each shader must be registred to allow Qt interface to update matrices uniforms
	registerShader(m_shader);

	m_shader2 = new Utils::ShaderColorPerVertex();
	m_shader2->setAttributePosition(m_positionVBO);
	// each shader must be registred to allow Qt interface to update matrices uniforms
	registerShader(m_shader2);
}

// redraw GL callback (clear and swap already done)
void MyQT::cb_redraw()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);

	glLineWidth(4.0f);
	m_shader2->setAttributeColor(m_colorVBO1);
	m_render->draw(m_shader2, Algo::Render::GL2::LINES);

	// draw white points
	glPointSize(7.0f);
	m_shader->setColor(Geom::Vec4f(1.,1.,1.,0.));
	m_render->draw(m_shader, Algo::Render::GL2::POINTS);

	// use offset for nice drawing
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);

	// draw faces with pervertex color rendering
	m_shader2->setAttributeColor(m_colorVBO2);
	m_render->draw(m_shader2, Algo::Render::GL2::TRIANGLES);

	glDisable(GL_POLYGON_OFFSET_FILL);
}
