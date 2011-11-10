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

#include "tuto4.h"

#include "Topology/generic/parameters.h"
#include "Topology/map/map2.h"

#include "Geometry/matrix.h"
#include "Geometry/vector_gen.h"
#include "Algo/Import/import.h"
#include "Algo/Geometry/boundingbox.h"
#include "Topology/generic/cellmarker.h"

#include "Algo/Render/GL2/mapRender.h"
#include "Algo/Render/GL2/topoRender.h"
#include "Utils/Shaders/shaderSimpleColor.h"
#include "Utils/Shaders/shaderFlat.h"

#include "Utils/cgognStream.h"

using namespace CGoGN ;

struct PFP: public PFP_STANDARD
{
	// definition de la carte
	typedef Map2 MAP;
};

PFP::MAP myMap;

PFP::TVEC3 position ;
PFP::TVEC3 normal ;
AttributeHandler<Geom::Vec4f> color ;

void MyQT::cb_initGL()
{
	// choose to use GL version 2
	Utils::GLSLShader::setCurrentOGLVersion(2);

	// create the render
	m_render = new Algo::Render::GL2::MapRender();

	m_render_topo = new Algo::Render::GL2::TopoRenderMapD();

	// create VBO for position
	m_positionVBO = new Utils::VBO();

	// using simple shader with color
	m_shader = new Utils::ShaderSimpleColor();
	m_shader->setAttributePosition(m_positionVBO);
	m_shader->setColor(Geom::Vec4f(0.,1.,0.,0.));

	m_shader2 = new Utils::ShaderFlat();
	m_shader2->setAttributePosition(m_positionVBO);

	registerShader(m_shader);
	registerShader(m_shader2);
}

void MyQT::cb_redraw()
{
	if(render_topo)
	{
		m_render_topo->drawTopo();
	}

	if(render_obj)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glEnable( GL_POLYGON_OFFSET_FILL );
		glPolygonOffset( 1.0f, 1.0f );
		m_render->draw(m_shader2, Algo::Render::GL2::TRIANGLES);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
}


void MyQT::cb_keyPress(int code)
{
	switch(code)
	{
	case 'o':
		render_obj = !render_obj;
		updateGL();
		break;

	case 't':
		render_topo = !render_topo;
		updateGL();
		break;
	}
}

void MyQT::button_compile()
{
	QString st1 = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->vertexEdit->toPlainText();
	QString st2 = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->fragmentEdit->toPlainText();
	QString st3 = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->geometryEdit->toPlainText();

	m_shader2->reloadVertexShaderFromMemory(st1.toStdString().c_str());
	m_shader2->reloadFragmentShaderFromMemory(st2.toStdString().c_str());
	m_shader2->reloadGeometryShaderFromMemory(st3.toStdString().c_str());

	m_shader2->recompile();
	updateGLMatrices();
}

void MyQT::slider_explode(int x)
{
	m_shader2->setExplode(0.01*x);
	updateGL();
}

int main(int argc, char **argv)
{
	/// Utilisation des Marker
	position = myMap.addAttribute<Geom::Vec3f>(VERTEX, "position");

	Algo::Modelisation::Polyhedron<PFP> prim3(myMap, position);
	Dart d2 = prim3.tore_topo(16, 24);
	prim3.embedTore(1.0f,0.3f);

//	position[d2] = PFP::VEC3(0.0f, 0.0f, 0.0f);
//	d2 = myMap.phi1(d2);
//	position[d2] = PFP::VEC3(2.0f, 0.0f, 0.0f);
//	d2 = myMap.phi1(d2);
//	position[d2] = PFP::VEC3(1.0f, 3.0f, 0.0f);
//	d2 = myMap.phi1(d2);
//	d3 = myMap.phi<11>(d3);
//	position[d3] = PFP::VEC3(0.0f, -2.0f, 0.0f);
//	d3 = myMap.phi1(d3);
//	position[d3] = PFP::VEC3(2.0f, -2.0f, 0.0f);
//	d3 = myMap.phi1(d3);

//	unsigned int idAttV2 = myMap.addAttribute<float>(VERTEX,"reel");
//	PFP::AttributeHandler<float> tableReels(idAttV2,VERTEX,myMap);
//
//	tableReels[d2] = 3.5f;
//	tableReels[myMap.phi1(d2)] = 3.7f;
//
//	// Attention ici on cree un attribut de FACE donc les face sont maintenant plangees
//	// l'attribut cree se detruira a la fin de la portee de l'objet
//	PFP::MAP::AutoAttributeHandler<Geom::Vec3f> tableRGB(myMap, FACE, "RGB"); // "RGB" optionnel
//
//	tableRGB[d3] = Geom::Vec3f(1.0f,2.0f,3.0f);
//
//	// acces par les brins
//	for (Dart d = myMap.begin(); d!= myMap.end(); myMap.next(d))
//	{
//		CGoGNout << "Brin "<<d.label()<< " reel="<<tableReels[d]<< "  RGB="<<tableRGB[d]<<CGoGNendl;
//	}
//
//	//acces direct par balayge du tableau
//	for (unsigned int id = tableRGB.begin(); id != tableRGB.end(); tableRGB.next(id))
//	{
//		CGoGNout << "RGB["<<id<<"] = "<<tableRGB.at(id)<<CGoGNendl;
//	}

	CellMarkerStore cm(myMap, VERTEX);

	Dart d3 = myMap.phi1(d2);
	cm.mark(d2);
	cm.mark(d3);
	cm.unmarkAll();

	// interface:
	QApplication app(argc, argv);
	MyQT sqt;

	// interface de tuto5.ui
    Utils::QT::uiDockInterface dock;
    sqt.setDock(&dock);

	// message d'aide
	sqt.setHelpMsg("Tuto4");

    //  bounding box
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
    float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
    Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

    // envoit info BB a l'interface
	sqt.setParamObject(lWidthObj,lPosObj.data());

	sqt.setCallBack( dock.compileButton, SIGNAL(clicked()), SLOT(button_compile()) );
	sqt.setCallBack( dock.explodeSlider, SIGNAL(valueChanged(int)), SLOT(slider_explode(int)) );

	// show 1 pour GL context
	sqt.show();

	// update du VBO position (context GL necessaire)
	sqt.m_positionVBO->updateData(position);

	// update des primitives du renderer
	sqt.m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::TRIANGLES);
	sqt.m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::LINES);

	sqt.m_render_topo->updateData<PFP>(myMap, position, 0.9f, 0.9f);

	dock.vertexEdit->setPlainText(QString(sqt.m_shader2->getVertexShaderSrc()));
	dock.fragmentEdit->setPlainText(QString(sqt.m_shader2->getFragmentShaderSrc()));
	dock.geometryEdit->setPlainText(QString(sqt.m_shader2->getGeometryShaderSrc()));
	// show final pour premier redraw
	sqt.show();

	CGoGNout.toConsole(&sqt);
	CGoGNerr.toConsole(&sqt);

	// et on attend la fin.
	return app.exec();
}
