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


#include "tuto_orbits.h"
#include <iostream>


#include "Algo/Modelisation/primitives3d.h"
#include "Algo/Modelisation/polyhedron.h"
#include "Algo/Modelisation/subdivision.h"

#include "Algo/Render/GL2/topo3Render.h"
#include "Algo/Render/SVG/mapSVGRender.h"

#include "Algo/Import/import.h"

MAP myMap;
VertexAttribute<VEC3> position ;
AttributeHandler<VEC3, DART> middleDarts;


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
	current_orbit = x;
	unsigned int orbs[9] = {VERTEX,EDGE,FACE,VOLUME,PFP::MAP::ORBIT_IN_PARENT(VERTEX),PFP::MAP::ORBIT_IN_PARENT(EDGE),PFP::MAP::ORBIT_IN_PARENT(FACE),PFP::MAP::ORBIT_IN_PARENT2(VERTEX),PFP::MAP::ORBIT_IN_PARENT2(EDGE)};
	storeVerticesInfoGen(orbs[current_orbit], m_att_orbits[x]);

	if (m_clicked != Dart::nil())
	{

		m_selected.clear();

		// easy way to traverse darts of orbit
		Traversor<MAP>* tra = Traversor<MAP>::createDartsOfOrbits(myMap,m_clicked,orbs[current_orbit]);
		for (Dart e = tra->begin(); e != tra->end(); e = tra->next())
			m_selected.push_back(e);
	}

	//	m_selected.clear();
	updateGL();
}

template< unsigned int ORBIT>
void MyQT::storeVerticesInfo(const AttributeHandler<int, ORBIT>* attrib)
{
	SelectorDartNoBoundary<PFP::MAP> nb(myMap);
	m_render_topo->computeDartMiddlePositions<PFP>(myMap, middleDarts, nb);

	m_strings->clear();
	for (Dart d = myMap.begin(); d != myMap.end(); myMap.next(d))
	{
		if (nb(d))
		{
			std::stringstream ss;
			ss << (*attrib)[d];
			m_strings->addString(ss.str(), middleDarts[d]);
		}
	}
    m_strings->sendToVBO();
}






void MyQT::storeVerticesInfoGen(unsigned int orb, const AttributeHandlerGen* attrib)
{
	switch(orb)
	{
	case VERTEX:
		storeVerticesInfo<VERTEX>(static_cast< const AttributeHandler<int, VERTEX>* >(attrib));
		break;
	case EDGE:
		storeVerticesInfo<EDGE>(static_cast< const AttributeHandler<int, EDGE>* >(attrib));
		break;
	case FACE:
		storeVerticesInfo<FACE>(static_cast< const AttributeHandler<int, FACE>* >(attrib));
		break;
	case VOLUME:
		storeVerticesInfo<VOLUME>(static_cast< const AttributeHandler<int, VOLUME>* >(attrib));
		break;
	case PFP::MAP::VERTEX_OF_PARENT:
		storeVerticesInfo<PFP::MAP::VERTEX_OF_PARENT>(static_cast< const AttributeHandler<int, PFP::MAP::VERTEX_OF_PARENT>* >(attrib));
		break;
	case PFP::MAP::EDGE_OF_PARENT:
		storeVerticesInfo<PFP::MAP::EDGE_OF_PARENT>(static_cast< const AttributeHandler<int, PFP::MAP::EDGE_OF_PARENT>* >(attrib));
		break;
	case PFP::MAP::FACE_OF_PARENT:
		storeVerticesInfo<PFP::MAP::FACE_OF_PARENT>(static_cast< const AttributeHandler<int, PFP::MAP::FACE_OF_PARENT>* >(attrib));
		break;
	case PFP::MAP::VERTEX_OF_PARENT2:
		storeVerticesInfo<PFP::MAP::VERTEX_OF_PARENT2>(static_cast< const AttributeHandler<int, PFP::MAP::VERTEX_OF_PARENT2>* >(attrib));
		break;
	case PFP::MAP::EDGE_OF_PARENT2:
		storeVerticesInfo<PFP::MAP::EDGE_OF_PARENT2>(static_cast< const AttributeHandler<int, PFP::MAP::EDGE_OF_PARENT2>* >(attrib));
		break;
	}
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

    storeVerticesInfoGen(VERTEX, m_att_orbits[0]);
}

void MyQT::cb_redraw()
{
	m_render_topo->drawTopo();

	for (unsigned int i=0; i< m_selected.size(); ++i)
		m_render_topo->overdrawDart(m_selected[i], 7, 1.0f, 0.0f, 1.0f);

	if (render_text)
		m_strings->drawAll(Geom::Vec3f(0.0f, 1.0f, 1.0f));

}

void MyQT::cb_mousePress(int button, int x, int y)
{
	if (Shift())
	{
		SelectorDartNoBoundary<PFP::MAP> nb(myMap);	
		m_clicked = m_render_topo->picking<PFP>(myMap, x,y, nb);
		if (m_clicked != Dart::nil())
		{
			unsigned int orbs[9] = {VERTEX,EDGE,FACE,VOLUME,PFP::MAP::ORBIT_IN_PARENT(VERTEX),PFP::MAP::ORBIT_IN_PARENT(EDGE),PFP::MAP::ORBIT_IN_PARENT(FACE),PFP::MAP::ORBIT_IN_PARENT2(VERTEX),PFP::MAP::ORBIT_IN_PARENT2(EDGE)};
			m_selected.clear();

			// easy way to traverse darts of orbit
			Traversor<MAP>* tra = Traversor<MAP>::createDartsOfOrbits(myMap,m_clicked,orbs[current_orbit]);
			for (Dart e = tra->begin(); e != tra->end(); e = tra->next())
				m_selected.push_back(e);
		}
		updateGL();
	}
}


template <unsigned int ORB>
void MyQT::init_att_orb(AttributeHandlerGen* attg)
{
	int i=0;
	TraversorCell<MAP,ORB> tra(myMap);

//	AttributeHandler<int,ORB>* attx = reinterpret_cast< AttributeHandler<int,ORB>* >(attg);
	AttributeHandler<int,ORB>* att = static_cast< AttributeHandler<int,ORB>* >(attg);

	for (Dart d = tra.begin(); d != tra.end(); d = tra.next())
	{
		(*att)[d] = i++;
	}
}

void MyQT::initMap()
{
	std::cout << "INIT MAP"<< std::endl;

	position = myMap.addAttribute<VEC3, VERTEX>("position");
	Algo::Modelisation::Primitive3D<PFP> prim(myMap, position);
	int nb=2;
	prim.hexaGrid_topo(nb,nb,nb);
	prim.embedHexaGrid(1.0f,1.0f,1.0f);

	AttributeHandler<int, VERTEX> att0  = myMap.addAttribute<int, VERTEX>("vertex");
	m_att_orbits[0] = new AttributeHandler<int, VERTEX>(att0);
	init_att_orb<VERTEX>(m_att_orbits[0]);

	m_att_orbits[1] = new AttributeHandler<int, EDGE>(myMap.addAttribute<int, EDGE>("edge"));
	m_att_orbits[2] = new AttributeHandler<int, FACE>(myMap.addAttribute<int, FACE>("face"));
	m_att_orbits[3] = new AttributeHandler<int, VOLUME>(myMap.addAttribute<int, VOLUME>("volume"));
	m_att_orbits[4] = new AttributeHandler<int, VERTEX + PFP::MAP::IN_PARENT>(myMap.addAttribute<int, VERTEX + PFP::MAP::IN_PARENT>("vertex2"));
	m_att_orbits[5] = new AttributeHandler<int, EDGE + PFP::MAP::IN_PARENT>(myMap.addAttribute<int, EDGE + PFP::MAP::IN_PARENT>("edge2"));
	m_att_orbits[6] = new AttributeHandler<int, FACE + PFP::MAP::IN_PARENT>(myMap.addAttribute<int, FACE + PFP::MAP::IN_PARENT>("face2"));
	m_att_orbits[7] = new AttributeHandler<int, VERTEX + PFP::MAP::IN_PARENT2>(myMap.addAttribute<int, VERTEX + PFP::MAP::IN_PARENT2>("vertex1"));
	m_att_orbits[8] = new AttributeHandler<int, EDGE + PFP::MAP::IN_PARENT2>(myMap.addAttribute<int, EDGE + PFP::MAP::IN_PARENT2>("face1"));



	init_att_orb<EDGE>(m_att_orbits[1]);

	init_att_orb<FACE>(m_att_orbits[2]);

	init_att_orb<VOLUME>(m_att_orbits[3]);

	init_att_orb<PFP::MAP::VERTEX_OF_PARENT>(m_att_orbits[4]);

	init_att_orb<PFP::MAP::EDGE_OF_PARENT>(m_att_orbits[5]);

	init_att_orb<PFP::MAP::FACE_OF_PARENT>(m_att_orbits[6]);

	init_att_orb<PFP::MAP::VERTEX_OF_PARENT2>(m_att_orbits[7]);

	init_att_orb<PFP::MAP::EDGE_OF_PARENT2>(m_att_orbits[8]);

	middleDarts = myMap.addAttribute<VEC3, DART>("middle");
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
	sqt.setHelpMsg("Select an orbit then\nshift click left to select a dart");

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
