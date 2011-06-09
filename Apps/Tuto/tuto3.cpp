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

#include "Topology/generic/parameters.h"
#include "Topology/map/map2.h"
#include "Topology/generic/cellmarker.h"

#include "Geometry/vector_gen.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Render/GL2/mapRender.h"
#include "Utils/Shaders/shaderSimpleColor.h"
#include "Utils/Shaders/shaderColorPerVertex.h"


#include "tuto3.h"

MyQT* sqt1_ptr;
MyQT* sqt2_ptr;

using namespace CGoGN ;

struct PFP: public PFP_STANDARD
{
	// definition de la carte
	typedef Map2 MAP;
};

void MyQT::cb_initGL()
{
	// choose to use GL version 2
	Utils::GLSLShader::setCurrentOGLVersion(2);

	// create the render
	m_render = new Algo::Render::GL2::MapRender();

	// create VBO for position
	m_positionVBO = new Utils::VBO();
	m_colorVBO = new Utils::VBO();

	// using simple shader with color
	m_shader = new Utils::ShaderSimpleColor();
	m_shader->setAttributePosition(m_positionVBO);


	m_shader2 = new Utils::ShaderColorPerVertex();
	m_shader2->setAttributePosition(m_positionVBO);
	m_shader2->setAttributeColor(m_colorVBO);

	registerShader(m_shader);
	registerShader(m_shader2);
}

void MyQT::cb_redraw()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glLineWidth(2.0f);
	m_shader->setColor(Geom::Vec4f(1.,1.,0.,0.));
	m_render->draw(m_shader, Algo::Render::GL2::LINES);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	m_render->draw(m_shader2, Algo::Render::GL2::TRIANGLES);
	glDisable(GL_POLYGON_OFFSET_FILL);
}

void MyQT::cb_keyPress(int code)
{
	switch(code)
	{
	case 's':
		if (this == sqt2_ptr)
			sqt1_ptr->synchronize(sqt2_ptr);
		if (this == sqt1_ptr)
			sqt2_ptr->synchronize(sqt1_ptr);
		break;
	}
}

int main(int argc, char **argv)
{
	PFP::MAP myMap;
	SelectorTrue allDarts;

	PFP::TVEC3 position ;
	PFP::TVEC3 normal ;
	AttributeHandler<Geom::Vec3f> color ;

	Dart d2 = myMap.newOrientedFace(3);
	Dart d3 = myMap.newOrientedFace(4);
	myMap.sewFaces(d2,d3);

	position = myMap.addAttribute<PFP::VEC3>(VERTEX, "position");
	color = myMap.addAttribute<PFP::VEC3>(VERTEX, "couleur");

	position[d2] = PFP::VEC3(0.0f, 0.0f, 0.0f);
	color[d2] = PFP::VEC3(1.0f, 0.0f, 0.0f);
	d2 = myMap.phi1(d2);
	position[d2] = PFP::VEC3(2.0f, 0.0f, 0.0f);
	color[d2] = PFP::VEC3(0.0f, 1.0f, 0.0f);
	d2 = myMap.phi1(d2);
	position[d2] = PFP::VEC3(1.0f, 3.0f, 0.0f);
	color[d2] = PFP::VEC3(0.0f, 0.0f, 1.0f);
	d2 = myMap.phi1(d2);
	d3 = myMap.phi<11>(d3);
	position[d3] = PFP::VEC3(0.0f, -2.0f, 0.0f);
	color[d3] = PFP::VEC3(1.0f, 0.0f, 1.0f);
	d3 = myMap.phi1(d3);
	position[d3] = PFP::VEC3(2.0f, -2.0f, 0.0f);
	color[d3] = PFP::VEC3(0.0f, 1.0f, 1.0f);
	d3 = myMap.phi1(d3);

	//MARK !!!

	// on reserve des marqueur de brins
	DartMarkerStore mf(myMap);
	DartMarkerStore me(myMap);

	// avec lesquels on peut marquer des orbits
	mf.markOrbit(FACE, d2);
	me.markOrbit(EDGE, d2);

	// les sommet sont plonges, on peut utiliser un marqueur de cellule sommet
	CellMarker cm(myMap, VERTEX);
	cm.mark(d3);

	for (Dart d = myMap.begin(); d!= myMap.end(); myMap.next(d))
	{
		if (me.isMarked(d))
			CGoGNout << "Dart "<< d.label() << " marque par me"<< CGoGNendl;
		if (mf.isMarked(d))
			CGoGNout << "Dart "<< d.label() << " marque par mf"<< CGoGNendl;
		if (cm.isMarked(d))
			CGoGNout << "Sommet de dart "<< d.label() << " marque par mcv"<< CGoGNendl;
	}

	//nettoyage
	cm.unmarkAll();

	// interface:
	QApplication app(argc, argv);

	MyQT sqt;
	sqt1_ptr = &sqt;

	MyQT sqt2;
	sqt2_ptr = &sqt2;

	// message d'aide
	sqt.setHelpMsg("Tuto3:\n"
			"marker tuto\n"
			"using two VBO & two shader");

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
	// update du VBO color
	sqt.m_colorVBO->updateData(color);

	// update des primitives du renderer
	sqt.m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::TRIANGLES);
	sqt.m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::LINES);

	// show final pour premier redraw
	sqt.show();

	// et pour le fun une deuxieme carte et une deuxieme interface:

	PFP::MAP myMap2;
	PFP::TVEC3 position2 ;
	AttributeHandler<Geom::Vec3f> color2 ;

	Dart dx = myMap2.newOrientedFace(4);

	position2 = myMap2.addAttribute<PFP::VEC3>(VERTEX, "position");
	color2 = myMap2.addAttribute<PFP::VEC3>(VERTEX, "couleur");

	position2[dx] = PFP::VEC3(0.0f, 0.0f, 0.0f);
	color2[dx] = PFP::VEC3(1.0f, 1.0f, 0.0f);
	dx = myMap.phi1(dx);
	position2[dx] = PFP::VEC3(2.0f, 0.0f, 0.0f);
	color2[dx] = PFP::VEC3(0.0f, 1.0f, 0.0f);
	dx = myMap.phi1(dx);
	position2[dx] = PFP::VEC3(2.0f, 2.0f, 0.0f);
	color2[dx] = PFP::VEC3(1.0f, 0.0f, 1.0f);
	dx = myMap.phi1(dx);
	position2[dx] = PFP::VEC3(0.0f, 2.0f, 0.0f);
	color2[dx] = PFP::VEC3(0.0f, 1.0f, 1.0f);

	sqt2.setHelpMsg("Fenetre 2!!");

	sqt2.setParamObject(lWidthObj,lPosObj.data());

	// show 1 pour GL context
	sqt2.show();

	// update du VBO position (context GL necessaire)
	sqt2.m_positionVBO->updateData(position2);
	// update du VBO color
	sqt2.m_colorVBO->updateData(color2);

	// update des primitives du renderer
	sqt2.m_render->initPrimitives<PFP>(myMap2, allDarts, Algo::Render::GL2::TRIANGLES);
	sqt2.m_render->initPrimitives<PFP>(myMap2, allDarts, Algo::Render::GL2::LINES);

	// et on attend la fin.
	return app.exec();
}
