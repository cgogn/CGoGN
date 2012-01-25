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


#include "tuto_orbits.h"
#include <iostream>


#include "Algo/Modelisation/primitives3d.h"
#include "Algo/Modelisation/polyhedron.h"
#include "Algo/Modelisation/subdivision.h"

#include "Algo/Render/GL2/topo3Render.h"
#include "Algo/Render/SVG/mapSVGRender.h"

#include "Algo/Import/import.h"

PFP::MAP myMap;
PFP::TVEC3 position ;
PFP::TVEC3 middleDarts;



void MyQT::text_onoff(bool x)
{
	render_text = !render_text;
	updateGL();
	CGoGNerr << " text_onoff  " << CGoGNflush;
}


void MyQT::slider_text(int x)
{
	m_strings->setScale(0.02f*x);
	updateGL();
}


void MyQT::orbit_list(int x)
{
	storeVerticesInfo<int>(m_att_orbits[x]);
	current_orbit = x;
	updateGL();
}


template<typename T>
void MyQT::storeVerticesInfo(const AttributeHandler<T>& attrib)
{
	SelectorDartNoBoundary<PFP::MAP> nb(myMap);
	m_render_topo->computeDartMiddlePositions<PFP>(myMap,middleDarts,nb);

	m_strings->clear();
	for (Dart d=myMap.begin(); d!=myMap.end(); myMap.next(d))
	{
		if (nb(d))
		{
			std::stringstream ss;
			ss << attrib[d];
			m_strings->addString(ss.str(),middleDarts[d]);
		}
	}
    m_strings->sendToVBO();
}

void MyQT::cb_initGL()
{
	// choose to use GL version 2
	Utils::GLSLShader::setCurrentOGLVersion(2);

    m_render_topo = new Algo::Render::GL2::Topo3Render();
	SelectorDartNoBoundary<PFP::MAP> nb(myMap);
	m_render_topo->updateData<PFP>(myMap, position,  0.9f, 0.8f, 0.8f, nb);

    m_strings = new Utils::Strings3D(true, Geom::Vec3f(0.1f,0.0f,0.3f));
	registerShader(m_strings);


    storeVerticesInfo<int>(m_att_orbits[0]);


}

void MyQT::cb_redraw()
{
	m_render_topo->drawTopo();

	for (unsigned int i=0; i< m_selected.size(); ++i)
		m_render_topo->overdrawDart(m_selected[i], 5, 1.0f, 0.0f, 1.0f);

	if (render_text)
		m_strings->drawAll(Geom::Vec3f(0.0f, 1.0f, 1.0f));

}

void MyQT::cb_mousePress(int button, int x, int y)
{
	if (Shift())
	{
		SelectorDartNoBoundary<PFP::MAP> nb(myMap);	
		Dart d = m_render_topo->picking<PFP>(myMap, x,y, nb);
		if (d != Dart::nil())
		{
			unsigned int orbs[9] ={VERTEX,EDGE,FACE,VOLUME,PFP::MAP::ORBIT_IN_PARENT(VERTEX),PFP::MAP::ORBIT_IN_PARENT(EDGE),PFP::MAP::ORBIT_IN_PARENT(FACE),PFP::MAP::ORBIT_IN_PARENT2(VERTEX),PFP::MAP::ORBIT_IN_PARENT2(EDGE)};
			m_selected.clear();

			// easy way to traverse darts of orbit
			TraversorDartsOfOrbit<PFP::MAP> tra(myMap,orbs[current_orbit],d);
			for (Dart e = tra.begin(); e != tra.end(); e = tra.next())
				m_selected.push_back(e);
		}
		updateGL();
	}
}


void MyQT::initMap()
{
	std::cout << "INIT MAP"<< std::endl;

	position = myMap.addAttribute<PFP::VEC3>(VERTEX, "position");
	Algo::Modelisation::Primitive3D<PFP> prim(myMap, position);
	int nb=2;
	prim.hexaGrid_topo(nb,nb,nb);
	prim.embedHexaGrid(1.0f,1.0f,1.0f);

	m_att_orbits[0] = myMap.addAttribute<int>(VERTEX,"vertex");
	m_att_orbits[1] = myMap.addAttribute<int>(EDGE,"edge");
	m_att_orbits[2] = myMap.addAttribute<int>(FACE,"face");
	m_att_orbits[3] = myMap.addAttribute<int>(VOLUME,"volume");
	m_att_orbits[4] = myMap.addAttribute<int>(PFP::MAP::ORBIT_IN_PARENT(VERTEX),"vertex2");
	m_att_orbits[5] = myMap.addAttribute<int>(PFP::MAP::ORBIT_IN_PARENT(EDGE),"edge2");
	m_att_orbits[6] = myMap.addAttribute<int>(PFP::MAP::ORBIT_IN_PARENT(FACE),"face2");
	m_att_orbits[7] = myMap.addAttribute<int>(PFP::MAP::ORBIT_IN_PARENT2(VERTEX),"vertex1");
	m_att_orbits[8] = myMap.addAttribute<int>(PFP::MAP::ORBIT_IN_PARENT2(EDGE),"face1");

	int i=0;
	TraversorCell<PFP::MAP> tra0(myMap, VERTEX);
	for (Dart d = tra0.begin(); d != tra0.end(); d = tra0.next())
	{
		m_att_orbits[0][d] = i++;
	}

	i=0;
	TraversorCell<PFP::MAP> tra1(myMap, EDGE);
	for (Dart d = tra1.begin(); d != tra1.end(); d = tra1.next())
	{
		m_att_orbits[1][d] = i++;
	}

	i=0;
	TraversorCell<PFP::MAP> tra2(myMap, FACE);
	for (Dart d = tra2.begin(); d != tra2.end(); d = tra2.next())
	{
		m_att_orbits[2][d] = i++;
	}

	i=0;
	TraversorCell<PFP::MAP> tra3(myMap, VOLUME);
	for (Dart d = tra3.begin(); d != tra3.end(); d = tra3.next())
	{
		m_att_orbits[3][d] = i++;
	}

	i=0;
	TraversorCell<PFP::MAP> tra02(myMap, PFP::MAP::ORBIT_IN_PARENT(VERTEX));
	for (Dart d = tra02.begin(); d != tra02.end(); d = tra02.next())
	{
		m_att_orbits[4][d] = i++;
	}

	i=0;
	TraversorCell<PFP::MAP> tra12(myMap, PFP::MAP::ORBIT_IN_PARENT(EDGE));
	for (Dart d = tra12.begin(); d != tra12.end(); d = tra12.next())
	{
		m_att_orbits[5][d] = i++;
	}

	i=0;
	TraversorCell<PFP::MAP> tra22(myMap, PFP::MAP::ORBIT_IN_PARENT(FACE));
	for (Dart d = tra22.begin(); d != tra22.end(); d = tra22.next())
	{
		m_att_orbits[6][d] = i++;
	}

	i=0;
	TraversorCell<PFP::MAP> tra01(myMap, PFP::MAP::ORBIT_IN_PARENT2(VERTEX));
	for (Dart d = tra01.begin(); d != tra01.end(); d = tra01.next())
	{
		m_att_orbits[7][d] = i++;
	}

	i=0;
	TraversorCell<PFP::MAP> tra11(myMap, PFP::MAP::ORBIT_IN_PARENT2(EDGE));
	for (Dart d = tra11.begin(); d != tra11.end(); d = tra11.next())
	{
		m_att_orbits[8][d] = i++;
	}

	middleDarts = myMap.addAttribute<PFP::VEC3>(DART, "middle");

}

int main(int argc, char **argv)
{

    // un peu d'interface
	QApplication app(argc, argv);
	MyQT sqt;

	// interface de tuto5.ui
    Utils::QT::uiDockInterface dock;
    sqt.setDock(&dock);

 	// message d'aide
	sqt.setHelpMsg("Enter pour dock on/off\nShift Enter pour console on/off\nShift Click gauche pour selectionner un brin");

	sqt.initMap();

	//  bounding box
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
    float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
    Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

    // envoit info BB a l'interface
	sqt.setParamObject(lWidthObj, lPosObj.data());

	sqt.setCallBack( dock.checkBox_text, SIGNAL(toggled(bool)), SLOT(text_onoff(bool)) );
	sqt.setCallBack( dock.slider_text, SIGNAL(valueChanged(int)), SLOT(slider_text(int)) );
	sqt.setCallBack( dock.Orbits, SIGNAL(	currentIndexChanged(int)), SLOT(orbit_list(int)) );

	sqt.show();
	sqt.slider_text(50);

	// et on attend la fin.
	return app.exec();
}
