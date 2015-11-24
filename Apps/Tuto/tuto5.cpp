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

#include "tuto5.h"
#include <iostream>

#include "Algo/Tiling/Volume/cubic.h"
#include "Algo/Modelisation/polyhedron.h"
#include "Algo/Modelisation/subdivision.h"
#include "Algo/Modelisation/subdivision3.h"

#include "Algo/Render/GL2/topo3Render.h"
#include "Algo/Render/SVG/mapSVGRender.h"

#include <QTimer>

MAP myMap;

VertexAttribute<VEC3, MAP> position ;
Dart dglobal;

void MyQT::balls_onoff(bool /*x*/)
{
	render_balls = !render_balls;
	updateGL();
	CGoGNerr << " balls_onoff  "<< CGoGNendl;
}

void MyQT::vectors_onoff(bool /*x*/)
{
	render_vectors = !render_vectors;
	updateGL();
	CGoGNerr << " vectors_onoff  "<< CGoGNflush;
}

void MyQT::text_onoff(bool /*x*/)
{
	render_text = !render_text;
	updateGL();
	CGoGNerr << " text_onoff  " << CGoGNflush;
}


void MyQT::topo_onoff(bool /*x*/)
{
	render_topo = !render_topo;
	updateGL();
	CGoGNerr << " topo_onoff  " << CGoGNflush;
}

void MyQT::slider_balls(int x)
{
	m_sprite->setSize(0.05f*x);
	updateGL();
}

void MyQT::slider_vectors(int x)
{
	m_lines->setScale(0.02*x);
	updateGL();
}

void MyQT::slider_text(int x)
{
	m_strings->setScale(0.02f*x);
	updateGL();
}

void MyQT::animate()
{
//	transfoMatrix() = glm::rotate(transfoMatrix(), 0.5f, glm::vec3(0.5773f,0.5773f,0.5773f));
	transfoRotate( 0.5f, 0.5773f,0.5773f,0.5773f);
	updateGLMatrices();
	updateGL();
}

void MyQT::storeVerticesInfo()
{
	CellMarker<MAP, VERTEX> mv(myMap);
	for (Dart d = myMap.begin(); d != myMap.end(); myMap.next(d))
	{
		if (!mv.isMarked(d))
		{
			mv.mark(d);
			std::stringstream ss;
			ss << d << " : "<< position[d];
			m_strings->addString(ss.str(), position[d]);
		}
	}
}

void MyQT::cb_initGL()
{
	// create the render
	m_render = new Algo::Render::GL2::MapRender();

	m_render_topo = new Algo::Render::GL2::Topo3RenderMap<PFP>();

 	// create VBO for position
	m_positionVBO = new Utils::VBO();
	m_positionVBO->updateData(position);

	// using simple shader with color
	m_shader = new Utils::ShaderSimpleColor();
	m_shader->setAttributePosition(m_positionVBO);
	m_shader->setColor(Geom::Vec4f(0.,1.,0.,0.));

	m_sprite = new Utils::PointSprite();
	m_sprite->setAttributePosition(m_positionVBO);
	m_sprite->setColor(Geom::Vec4f(1.0f, 0.0f , 0.0f, 1.0f));

	m_strings = new Utils::Strings3D(true, Geom::Vec3f(0.1f,0.0f,0.3f));
	storeVerticesInfo();
	m_strings->sendToVBO();

    // copy de contenu de VBO a la creation
	m_dataVBO = new Utils::VBO(*m_positionVBO);

	m_lines = new Utils::ShaderVectorPerVertex();
	m_lines->setAttributePosition(m_positionVBO);
	m_lines->setAttributeVector(m_dataVBO);
	m_lines->setScale(0.2f);
	m_lines->setColor(Geom::Vec4f(0.0f, 1.0f, 0.2f, 0.0f));

	// accede au buffer du VBO pour modification
	PFP::VEC3* data = static_cast<PFP::VEC3*>(m_dataVBO->lockPtr());
	for (unsigned int i=0; i< m_dataVBO->nbElts(); ++i)
	{
		data[i].normalize();
	}
	m_dataVBO->releasePtr();

	registerShader(m_shader);
	registerShader(m_strings);
	registerShader(m_sprite);
	registerShader(m_lines);

	m_render->initPrimitives<PFP>(myMap, Algo::Render::GL2::TRIANGLES);
	m_render->initPrimitives<PFP>(myMap, Algo::Render::GL2::LINES);
	m_render->initPrimitives<PFP>(myMap, Algo::Render::GL2::POINTS);

//	SelectorDartNoBoundary<PFP::MAP> nb(myMap);
	m_render_topo->updateData(myMap, position,  0.9f, 0.9f, 0.9f); // nb

	// timer example for animation
	m_timer = new QTimer( this );
	connect( m_timer, SIGNAL(timeout()), SLOT(animate()) );
}

void MyQT::cb_redraw()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_LIGHTING);

	m_render->draw(m_shader, Algo::Render::GL2::POINTS);

	glLineWidth(2.0f);
	m_shader->setColor(Geom::Vec4f(1.,1.,0.,0.));
	m_render->draw(m_shader, Algo::Render::GL2::LINES);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);

	if (render_topo)
		m_render_topo->drawTopo();

/*	Dart d = myMap.phi2(myMap.begin());
	m_render_topo->overdrawDart(d, 5, 1.0f, 0.0f, 1.0f);
	 d = myMap.phi1(myMap.begin());
	m_render_topo->overdrawDart(d, 5, 1.0f, 0.0f, 1.0f);
*/	
	m_render_topo->overdrawDart(m_selected, 5, 1.0f, 0.0f, 1.0f);
	
	glDisable(GL_POLYGON_OFFSET_FILL);

	if (render_text)
		m_strings->drawAll(Geom::Vec3f(0.0f, 1.0f, 1.0f));

	if (render_balls)
	{
		m_render->draw(m_sprite, Algo::Render::GL2::POINTS);
	}

	if (render_vectors)
	{
		glLineWidth(1.0f);
		m_render->draw(m_lines, Algo::Render::GL2::POINTS);
	}
}

void MyQT::cb_mousePress(int /*button*/, int x, int y)
{
	if (Shift())
	{
//		SelectorDartNoBoundary<PFP::MAP> nb(myMap);
		Dart d = m_render_topo->picking(myMap, x,y); // nb
		if (d != Dart::nil())
		{
			CGoGNout << "Dart "<< d << " clicked" << CGoGNendl;
			m_selected = d;
		}
		else
		{
			statusMsg("");
		}
		updateGL();
	}
}

void MyQT::cb_keyPress(int code)
{
	if (code  == 's')
	{
		std::string filename = selectFileSave("Export SVG file ",".","(*.svg)");
		Utils::SVG::SVGOut svg(filename, modelViewMatrix(), projectionMatrix());
//		svg.setWidth(1.0f);
//		svg.setColor(Geom::Vec3f(0.0f,0.0f,0.5f));
		Algo::Render::SVG::renderEdges<PFP>(svg, myMap, position);
//		svg.setColor(Geom::Vec3f(0.0f,0.8f,0.0f));
//		svg.setWidth(5.0f);
		Algo::Render::SVG::renderVertices<PFP>(svg, myMap, position);
	//	svg.setColor(Geom::Vec3f(1.0f,0.0f,0.0f));
		m_strings->toSVG(svg);
		//svg destruction close the file
	}
	if (code  == 't')
	{
		if (m_timer->isActive())
			m_timer->stop();
		else
			m_timer->start(1000/30); // 30 fps
	}

	if(code == 'c')
	{
		//SelectorDartNoBoundary<PFP::MAP> nb(myMap);
		Algo::Volume::Modelisation::catmullClarkVol<PFP>(myMap, position);

		m_positionVBO->updateData(position);
		m_render->initPrimitives<PFP>(myMap, Algo::Render::GL2::TRIANGLES);
		m_render->initPrimitives<PFP>(myMap, Algo::Render::GL2::LINES);
		m_render->initPrimitives<PFP>(myMap, Algo::Render::GL2::POINTS);

		m_render_topo->updateData(myMap, position, 0.9f, 0.9f, 0.9f);
	}
}

int main(int argc, char **argv)
{
	position = myMap.addAttribute<VEC3, VERTEX, MAP>("position");

	CGoGNout << 5.34 << " toto "<< Geom::Vec3f(2.5f, 2.2f, 4.3f) << CGoGNendl;
	CGoGNout << 3 << " tutu "<< 4 << CGoGNendl;

	int nb=3;
	if (argc>1)
		nb = atoi(argv[1]);
    Algo::Volume::Tilings::Cubic::Grid<PFP> cubic(myMap, nb, nb, nb);
    cubic.embedIntoGrid(position, 1.0f, 1.0f, 1.0f);
    dglobal = NIL;

    // un peu d'interface
	QApplication app(argc, argv);
	MyQT sqt;

	// interface de tuto5.ui
    Utils::QT::uiDockInterface dock;
    sqt.setDock(&dock);

 	// message d'aide
	sqt.setHelpMsg("Enter pour dock on/off\nShift Enter pour console on/off\nShift Click gauche pour selectionner un brin");

	CGoGNout.toStatusBar(&sqt);
	CGoGNout << "CGoGNOut StatusBar" << Geom::Vec3f(2.5f, 2.2f, 4.3f) << CGoGNendl;

	CGoGNout.toConsole(&sqt);

	CGoGNout << "CGoGNOut dans la console" << Geom::Vec3f(2.5f, 2.2f, 4.3f) << CGoGNendl;

	CGoGNout.noStatusBar();

	//  bounding box
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
    float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
    Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

    // envoit info BB a l'interface
	sqt.setParamObject(lWidthObj, lPosObj.data());

	sqt.setCallBack( dock.checkBox_balls, SIGNAL(toggled(bool)), SLOT(balls_onoff(bool)) );
	sqt.setCallBack( dock.checkBox_vectors, SIGNAL(toggled(bool)), SLOT(vectors_onoff(bool)) );
	sqt.setCallBack( dock.checkBox_text, SIGNAL(toggled(bool)), SLOT(text_onoff(bool)) );
	sqt.setCallBack( dock.checkBox_topo, SIGNAL(toggled(bool)), SLOT(topo_onoff(bool)) );

	sqt.setCallBack( dock.slider_balls, SIGNAL(valueChanged(int)), SLOT(slider_balls(int)) );
	sqt.setCallBack( dock.slider_vectors, SIGNAL(valueChanged(int)), SLOT(slider_vectors(int)) );
	sqt.setCallBack( dock.slider_text, SIGNAL(valueChanged(int)), SLOT(slider_text(int)) );

	sqt.m_selected = myMap.begin();

	sqt.setGeometry(100,100,1024,1024);
	sqt.show();

	sqt.slider_balls(50);
	sqt.slider_vectors(50);
	sqt.slider_text(50);

	GLint texSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);
	CGoGNdbg.toConsole(&sqt);
	CGoGNerr.toConsole(&sqt);
	CGoGNdbg << " TextureSize " <<  texSize << CGoGNendl;
	CGoGNerr << " test ERROR  " <<  5*7 << CGoGNflush;

	// et on attend la fin.
	return app.exec();
}
