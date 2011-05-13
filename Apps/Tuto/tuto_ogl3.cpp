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

#include "tuto_ogl3.h"
#include "Utils/vbo.h"

using namespace CGoGN ;

// declarations globales (plus simple)
PFP::MAP myMap;
AttributeHandler<PFP::VEC3> position;
AttributeHandler<PFP::VEC3> normal;

void MyQT::sliderFocale_cb(int x)
{
	m_glWidget->setFocal(0.01f*x);
	updateGL();
}

void MyQT::color_cb()
{
	QColor color = QColorDialog::getColor ();
	if (color.isValid())
	{
		CGoGNout <<"Color " << color.red()<<","<< color.green()<<","<<color.blue()<< CGoGNendl;
		shader1->setDiffuse(Geom::Vec4f(float(color.red())/255.0f,float(color.green())/255.0f,float(color.blue())/255.0f,0.0f));
		updateGL();
	}
	else
	{
		CGoGNout << "Cancel"<< CGoGNendl;
	}
}

void MyQT::pervertex_cb(bool val)
{
	if (val)
	{
		shader1->setAttributeColor(colorVBO);
		CGoGNout << "color per vertex"<< CGoGNendl;
	}
	else
	{
		shader1->unsetAttributeColor();
		CGoGNout << "global color"<< CGoGNendl;
	}
	updateGLMatrices();
}

MyQT::MyQT(): m_render(NULL)
{}

MyQT::~MyQT()
{}

void MyQT::cb_initGL()
{
	Utils::GLSLShader::setCurrentOGLVersion(3);
	// instanciation of the renderer (here using VBOs)
	m_render = new Algo::Render::GL2::MapRender();

	positionVBO = new Utils::VBO();
	normalVBO = new Utils::VBO();

	positionVBO->updateData(position);
	normalVBO->updateData(normal);

	// create colors from normal (remap [-1,1] to [0,1]
	colorVBO = new Utils::VBO(*normalVBO);
	Geom::Vec3f* colors= reinterpret_cast<Geom::Vec3f*>(colorVBO->lockPtr());
	for (unsigned int i =0 ; i<colorVBO->nbElts();++i)
	{
		*colors = (Geom::Vec3f(1.,1.,1.)+ *colors)/2.0f;
		colors++;
	}
	colorVBO->releasePtr();

	// update the renderer (primitives and geometry)
	SelectorTrue allDarts;
	m_render->initPrimitives<PFP>(myMap, allDarts,Algo::Render::GL2::TRIANGLES);
	m_render->initPrimitives<PFP>(myMap, allDarts,Algo::Render::GL2::LINES);
	m_render->initPrimitives<PFP>(myMap, allDarts,Algo::Render::GL2::POINTS);

	shader1 = new Utils::ShaderPhong();
	shader1->setAttributePosition(positionVBO);
	shader1->setAttributeNormal(normalVBO);

	currentShader = shader1;
}

void MyQT::cb_redraw()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_LIGHTING);
	if (currentShader)
	{
		m_render->draw(currentShader, Algo::Render::GL2::TRIANGLES);
	}
}

int main(int argc, char **argv)
{
	position = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "position");
	normal = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "normal");

	Algo::Modelisation::Polyhedron<PFP> prim3(myMap, position);
	prim3.tore_topo(32, 64);
	prim3.embedTore(1.0f,0.3f);

	Algo::Geometry::computeNormalVertices<PFP>(myMap,position,normal);

	QApplication app(argc, argv);
	MyQT sqt;

	// 	Qt "a la main"

	QDockWidget* dock = sqt.addEmptyDock();

	// group principal
	QGroupBox *interface_gb = new QGroupBox();
	QVBoxLayout *layout = new QVBoxLayout;

	layout->addWidget(new QLabel("Focale"));

	QSlider *slider_foc = new QSlider(Qt::Horizontal);
	slider_foc->setRange(50, 300);
	slider_foc->setSliderPosition(200);
	layout->addWidget(slider_foc);

	QPushButton* buttonColor = new QPushButton("Color");
	layout->addWidget(buttonColor);

	QCheckBox* perVertexColor = new QCheckBox("PerVertex");
	layout->addWidget(perVertexColor);

	// on tasse  vers le haut
	layout->addItem(new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Expanding));

	// et met tout ça dans le dock
	interface_gb->setLayout(layout);
	dock->setWidget(interface_gb);

	sqt.windowTitle("OGL3 QT");
	sqt.dockTitle("interface");

	sqt.setDock(dock) ;

	// connection widget callback
	sqt.setCallBack( slider_foc, SIGNAL(valueChanged(int)), SLOT(sliderFocale_cb(int)) );
	sqt.setCallBack( buttonColor, SIGNAL(clicked()), SLOT(color_cb()) );
	sqt.setCallBack( perVertexColor, SIGNAL(toggled(bool)), SLOT(pervertex_cb(bool)) );

	// computation of the bounding box
	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
	float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
	Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

	sqt.setParamObject(lWidthObj,lPosObj.data());

	// first show to init GL Context before initializing VBO
	sqt.show();

	CGoGNStream::allToConsole(&sqt);
	CGoGNStream::allToStd(false);

	// second show to have a good initial redraw
	sqt.show();

	return app.exec();
	//    return 0;
}
