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
#include <QWidget>
#include <QtGui>

#include "Utils/shaderPhong.h"
using namespace CGoGN ;


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
		std::cout <<"Color " << color.red()<<","<< color.green()<<","<<color.blue()<< std::endl;
		shader1->setDiffuse(Geom::Vec4f(float(color.red())/255.0f,float(color.green())/255.0f,float(color.blue())/255.0f,0.0f));
		updateGL();
	}
	else
	{
		std::cout << "Cancel"<< std::endl;
	}
}


MyQT::MyQT(): m_render(NULL)
{
//	currentShader = NULL;
}


MyQT::~MyQT()
{ }



void MyQT::cb_initGL()
{
	Utils::GLSLShader::setCurrentOGLVersion(3);
	// instanciation of the renderer (here using VBOs)
	m_render = new Algo::Render::GL2::MapRender();

	positionVBO = new Utils::VBO();
	normalVBO = new Utils::VBO();

	shader1 = new Utils::ShaderPhong();

	shader1->setAttributePosition(*positionVBO);
	shader1->setAttributeNormal(*normalVBO);

	currentShader = shader1;
}

void MyQT::cb_updateMatrix()
{
	if (currentShader)
	{
		currentShader->bind();
		currentShader->updateMatrices(m_projection_matrix, m_modelView_matrix);
	}
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


//void MyGlutWin::myKeyboard(unsigned char keycode, int x, int y)
//{
//	switch(keycode)
//	{
//	case 'f':
//		focale-=0.1f;
//		setFoc(focale);
//		break;
//	case 'F':
//		focale+=0.1f;
//		setFoc(focale);
//		break;
//	}
////
////	switch(keycode)
////	{
////	case 'c':
////		rt=0;
////		current_shader = &shaders[0];
////		setCurrentShader(current_shader);
////		std::cout << "Color"<< std::endl;
////		break;
////	case 'p':
////		rt=1;
////		current_shader = &shaders[1];
////		setCurrentShader(current_shader);
////		std::cout << "Phong"<< std::endl;
////		break;
////	}
////	glutPostRedisplay();
//
//
//	switch(keycode)
//	{
//	case 'r':
//		shader1.setDiffuse(Geom::Vec4f(1.,0.,0.,0.));
//		break;
//	case 'v':
//		shader1.setDiffuse(Geom::Vec4f(0.,1.,0.,0.));
//		break;
//	case 'j':
//		shader1.setDiffuse(Geom::Vec4f(1.,1.,0.,0.));
//		break;
//
//	case 'w':
//		shader1.setAttributeColor(normalVBO);
//		break;
//	case 'x':
//		shader1.unsetAttributeColor();
//		break;
//
//	}
//	glutPostRedisplay();
//}


int main(int argc, char **argv)
{
	// declaration of the map
	PFP::MAP myMap;

	AttributeHandler<PFP::VEC3> position = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "position");
	AttributeHandler<PFP::VEC3> normal = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "normal");

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

	// on tasse  vers le haut
	layout->addItem(new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Expanding));

	// et met tout ça dans le dock
	interface_gb->setLayout(layout);
	dock->setWidget(interface_gb);

	sqt.windowTitle("OGL3 QT");
	sqt.dockTitle("interface");

	// connection widget callback
	sqt.setCallBack( slider_foc, SIGNAL(valueChanged(int)), SLOT(sliderFocale_cb(int)) );
	sqt.setCallBack( buttonColor, SIGNAL(clicked()), SLOT(color_cb()) );

	// computation of the bounding box
	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
	float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
	Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

	sqt.setParamObject(lWidthObj,lPosObj.data());

	// first show to init GL Context before initializing VBO
	sqt.show();

	sqt.positionVBO->updateData(position);
	sqt.normalVBO->updateData(normal);

	// update the renderer (primitives and geometry)
	SelectorTrue allDarts;
	sqt.m_render->initPrimitives<PFP>(myMap, allDarts,Algo::Render::GL2::TRIANGLES);
	sqt.m_render->initPrimitives<PFP>(myMap, allDarts,Algo::Render::GL2::LINES);
	sqt.m_render->initPrimitives<PFP>(myMap, allDarts,Algo::Render::GL2::POINTS);

	// second show to have a good initial redraw
	sqt.show();

	return app.exec();
	//    return 0;
}
