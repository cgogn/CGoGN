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

#include "tuto1.h"

#include <iostream>

#include "Topology/generic/parameters.h"
#include "Topology/map/map2.h"
#include "Topology/generic/embeddedMap2.h"
#include "Geometry/vector_gen.h"

#include "Algo/Import/import.h"
#include "Algo/Geometry/boundingbox.h"

#include "Algo/Render/GL2/mapRender.h"
#include "Utils/shaderSimpleColor.h"

#include <glm/gtc/type_ptr.hpp>

#include "Algo/Render/SVG/mapSVGRender.h"


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

// declaration of the map
PFP::MAP myMap;
// and attribute of position
AttributeHandler<PFP::VEC3> position;

void MyQT::cb_initGL()
{
	// choose to use GL version 2
	Utils::GLSLShader::setCurrentOGLVersion(2);

	// create the render
	m_render = new Algo::Render::GL2::MapRender();

	// create VBO for position
	m_positionVBO = new Utils::VBO();

	// using simple shader with color
	m_shader = new Utils::ShaderSimpleColor();
	m_shader->setAttributePosition(m_positionVBO);
	m_shader->setColor(Geom::Vec4f(0.0f, 1.0f, 0.0f, 0.0f));
	registerShader(m_shader);
}

void MyQT::cb_redraw()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	if (m_shader)
	{
		glLineWidth(2.0f);
		m_shader->setColor(Geom::Vec4f(1.,1.,0.,0.));
		m_render->draw(m_shader, Algo::Render::GL2::LINES);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f, 1.0f);

		m_shader->setColor(Geom::Vec4f(0.,1.,0.,0.));
		m_render->draw(m_shader, Algo::Render::GL2::TRIANGLES);

		glDisable(GL_POLYGON_OFFSET_FILL);
	}
}

void MyQT::cb_keyPress(int code)
{
	if ((code >65) && (code< 123 ))
		CGoGNout << " key char " << char(code) << "pressed"<< CGoGNendl;

	if ((code >'0') && (code<='9'))
		CGoGNout << " key num " << code-'0' << "pressed"<< CGoGNendl;

	if (code  == 's')
	{
		std::string filename = selectFileSave("Export SVG file ");
		CGoGNout << "Exporting "<<filename<<CGoGNendl;
		Algo::Render::SVG::SVGOut svg(filename,modelViewMatrix(),projectionMatrix());
		svg.renderLinesToSVG<PFP>(myMap,position);
		svg.setColor(Geom::Vec3f(0.7f,0.0f,0.4f));
		svg.renderFacesToSVG<PFP>(myMap,position,0.8f);
		//svg destruction close the file
	}
}

int main(int argc, char **argv)
{

	// creation of 2 new faces: 1 triangle and 1 square
	Dart d1 = myMap.newFace(3);
	Dart d2 = myMap.newFace(4);

	// sew these faces along one of their edge
	myMap.sewFaces(d1, d2);

	// creation of a new attribute on vertices of type 3D vector
	// a handler to this attribute is returned
	position = myMap.addAttribute<PFP::VEC3>(VERTEX, "position");

	// affect a position to the vertices of the mesh
	position[d1] = PFP::VEC3(0, 0, 0);
	position[myMap.phi1(d1)] = PFP::VEC3(2, 0, 0);
	position[myMap.phi_1(d1)] = PFP::VEC3(1, 2, 0);

	position[myMap.phi<11>(d2)] = PFP::VEC3(0, -2, 0);
	position[myMap.phi_1(d2)] = PFP::VEC3(2, -2, 0);

	// interface:
	QApplication app(argc, argv);
	MyQT sqt;

	// ajout entree dans le menu application
	sqt.add_menu_entry("entree1", SLOT(menu_slot1()));

	// message d'aide
	sqt.setHelpMsg("First Tuto:\n"
			"create 2 faces\n"
			"and sew them \n"
			"simple interface in Qt");

    //  bounding box
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
    float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
    Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

    // envoit info BB a l'interface
	sqt.setParamObject(lWidthObj, lPosObj.data());

	// show 1 pour GL context
	sqt.show();

	// update du VBO position (context GL necessaire)
	sqt.m_positionVBO->updateData(position);

	// update des primitives du renderer
	SelectorTrue allDarts;
	sqt.m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::TRIANGLES);
	sqt.m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::LINES);

	// show final pour premier redraw
	sqt.show();

	// et on attend la fin.
	return app.exec();
}
