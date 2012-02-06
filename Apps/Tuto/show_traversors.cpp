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


#include "show_traversors.h"
#include <iostream>


#include "Algo/Modelisation/primitives3d.h"
#include "Algo/Modelisation/polyhedron.h"
#include "Algo/Modelisation/subdivision.h"

#include "Algo/Render/GL2/topo3Render.h"
#include "Algo/Render/SVG/mapSVGRender.h"

#include "Topology/generic/traversor3.h"
#include "Topology/generic/traversor2.h"

#include "Algo/Render/GL2/drawerCells.h"


PFP::MAP myMap;
PFP::TVEC3 position ;
Dart dglobal;



void MyQT::cb_checkTopo(bool b)
{
	m_showTopo = b;
	updateGL();
}

void MyQT::cb_combo1(int x)
{
	m_val_combo1 = x+1;
	if (m_val_combo1!=m_val_combo3)
		traverse3();
	else
		CGoGNerr <<"undefined traversor" << CGoGNendl;
}

void MyQT::cb_combo2(int x)
{
	m_val_combo2 = x+1;
	if (m_val_combo1!=m_val_combo3)
		traverse3();
	else
		CGoGNerr <<"undefined traversor" << CGoGNendl;
}

void MyQT::cb_combo3(int x)
{
	m_val_combo3 = x+1;
	if (m_val_combo1!=m_val_combo3)
		traverse3();
	else
		CGoGNerr <<"undefined traversor" << CGoGNendl;
}

void MyQT::cb_combo4(int x)
{
	m_val_combo4 = x+1;
	if (m_val_combo4!=m_val_combo6)
		traverse2();
	else
		CGoGNerr <<"undefined traversor" << CGoGNendl;
}

void MyQT::cb_combo5(int x)
{
	m_val_combo5 = x+1;
	if (m_val_combo4!=m_val_combo6)
		traverse2();
	else
		CGoGNerr <<"undefined traversor" << CGoGNendl;
}

void MyQT::cb_combo6(int x)
{
	m_val_combo6 = x+1;
	if (m_val_combo4!=m_val_combo6)
		traverse2();
	else
		CGoGNerr <<"undefined traversor" << CGoGNendl;
}


void MyQT::cb_initGL()
{
	// choose to use GL version 2
	Utils::GLSLShader::setCurrentOGLVersion(2);

    m_render_topo = new Algo::Render::GL2::Topo3Render();

	SelectorDartNoBoundary<PFP::MAP> nb(myMap);
	m_render_topo->updateData<PFP>(myMap, position,  0.95f, 0.9f, 0.8f, nb);

}

void MyQT::cb_redraw()
{
	if (m_showTopo)
	{
		m_render_topo->drawTopo();

		if (m_selected != NIL)
			m_render_topo->overdrawDart(m_selected, 6, 1.0f, 0.0f, 1.0f);
	
		for (std::vector<Dart>::iterator it=m_affDarts.begin(); it!=m_affDarts.end(); ++it)
		{
			m_render_topo->overdrawDart(*it, 6, 1.0f, 1.0f, 1.0f);
		}
	}

	m_drawer.callList();

}

void MyQT::cb_mousePress(int button, int x, int y)
{
	if (Shift())
	{
		SelectorDartNoBoundary<PFP::MAP> nb(myMap);	
		Dart d = m_render_topo->picking<PFP>(myMap, x,y, nb);
		if (d != Dart::nil())
		{
			CGoGNout << "Dart "<< d << " clicked" << CGoGNendl;
			m_selected = d;
		}
		updateGL();
	}
}


void MyQT::colorizeCell(Dart d, unsigned int orbit, float r,float g, float b)
{
	TraversorDartsOfOrbit<PFP::MAP>doo (myMap,orbit,d);
	for (Dart e = doo.begin(); e!=doo.end(); e = doo.next())
		m_render_topo->setDartColor(e,r,g,b);
}


void MyQT::traverse2()
{
	if (m_selected == NIL)
		return;

	int code = (m_val_combo5)*100+m_val_combo4*10+m_val_combo6;

	SelectorDartNoBoundary<PFP::MAP> nb(myMap);

	m_drawer.newList(GL_COMPILE);
	m_drawer.lineWidth(3.0f);
	m_drawer.pointSize(7.0f);
	m_drawer.color3f(1.0f,1.0f,0.0f);

	m_affDarts.clear();
	switch(code)
	{
	// 1YX incient
	case 113:
	{
		Algo::Render::drawerFace<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor2FV<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerVertices<PFP>(m_drawer, myMap,m_affDarts,position);
		break;
	}
	case 123:
	{
		Algo::Render::drawerFace<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor2FE<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerEdges<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}

	case 112:
	{
		Algo::Render::drawerEdge<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor2EV<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerVertices<PFP>(m_drawer, myMap,m_affDarts,position);
		break;
	}
	case 132:
	{
		Algo::Render::drawerEdge<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor2EF<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerFaces<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}
	case 121:
	{
		Algo::Render::drawerVertex<PFP>(m_drawer,myMap,m_selected,position);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor2VE<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerEdges<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}
	case 131:
	{
		Algo::Render::drawerVertex<PFP>(m_drawer,myMap,m_selected,position);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor2VF<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerFaces<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}
	// 2XY adjacent
	case 212:
	{
		Algo::Render::drawerVertex<PFP>(m_drawer,myMap,m_selected,position);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor2VVaE<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerVertices<PFP>(m_drawer, myMap,m_affDarts,position);
		break;
	}
	case 213:
	{
		Algo::Render::drawerVertex<PFP>(m_drawer,myMap,m_selected,position);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor2VVaF<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerVertices<PFP>(m_drawer, myMap,m_affDarts,position);
		break;
	}
	case 221:
	{
		Algo::Render::drawerEdge<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor2EEaV<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerEdges<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}
	case 223:
	{
		Algo::Render::drawerEdge<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor2EEaF<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerEdges<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}
	case 231:
	{
		Algo::Render::drawerFace<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor2FFaV<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerFaces<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}
	case 232:
	{
		Algo::Render::drawerFace<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor2FFaE<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerFaces<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}

	default:
		CGoGNerr <<"Not implemented" << CGoGNendl;
		break;
	}
	m_drawer.endList();
	updateGL();
}



void MyQT::traverse3()
{
	int code = (m_val_combo2)*100+m_val_combo1*10+m_val_combo3;
//	std::cout << "CODE="<< code << std::endl;
	if (m_selected == NIL)
		return;
	SelectorDartNoBoundary<PFP::MAP> nb(myMap);

	m_affDarts.clear();
	m_drawer.newList(GL_COMPILE);
	m_drawer.lineWidth(3.0f);
	m_drawer.pointSize(7.0f);
	m_drawer.color3f(1.0f,1.0f,0.0f);
	switch(code)
	{
	// 1YX incient
	case 114:
	{

		Algo::Render::drawerVolume<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);

		Traversor3WV<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerVertices<PFP>(m_drawer, myMap,m_affDarts,position);
		break;
	}
	case 124:
	{
		Algo::Render::drawerVolume<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor3WE<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerEdges<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}
	case 134:
	{
		Algo::Render::drawerVolume<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor3WF<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerFaces<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}
	case 113:
	{
		Algo::Render::drawerFace<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor3FV<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerVertices<PFP>(m_drawer, myMap,m_affDarts,position);
		break;
	}
	case 123:
	{
		Algo::Render::drawerFace<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor3FE<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerEdges<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}
	case 143:
	{
		Algo::Render::drawerFace<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor3FW<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerVolumes<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}

	case 112:
	{
		Algo::Render::drawerEdge<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor3EV<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerVertices<PFP>(m_drawer, myMap,m_affDarts,position);
		break;
	}
	case 132:
	{
		Algo::Render::drawerEdge<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor3EF<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerFaces<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}
	case 142:
	{
		Algo::Render::drawerEdge<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor3EW<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerVolumes<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}

	case 121:
	{
		Algo::Render::drawerVertex<PFP>(m_drawer,myMap,m_selected,position);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor3VE<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerEdges<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}
	case 131:
	{
		Algo::Render::drawerVertex<PFP>(m_drawer,myMap,m_selected,position);
		Traversor3VF<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerFaces<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}
	case 141:
	{
		Algo::Render::drawerVertex<PFP>(m_drawer,myMap,m_selected,position);
		Traversor3VW<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerVolumes<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}

	// 2XY adjacent
	case 212:
	{
		Algo::Render::drawerVertex<PFP>(m_drawer,myMap,m_selected,position);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor3VVaE<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerVertices<PFP>(m_drawer, myMap,m_affDarts,position);
		break;
	}
	case 213:
	{
		Algo::Render::drawerVertex<PFP>(m_drawer,myMap,m_selected,position);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor3VVaF<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerVertices<PFP>(m_drawer, myMap,m_affDarts,position);
		break;
	}
	case 214:
	{
		Algo::Render::drawerVertex<PFP>(m_drawer,myMap,m_selected,position);
		m_drawer.color3f(1.0f,0.0f,0.0f);

		Traversor3VVaW<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerVertices<PFP>(m_drawer, myMap,m_affDarts,position);
		break;
	}
	case 221:
	{
		Algo::Render::drawerEdge<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor3EEaV<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerEdges<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}
	case 223:
	{
		Algo::Render::drawerEdge<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor3EEaF<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerEdges<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}
	case 224:
	{
		Algo::Render::drawerEdge<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor3EEaW<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerEdges<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}
	case 231:
	{
		Algo::Render::drawerFace<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor3FFaV<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerFaces<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}
	case 232:
	{
		Algo::Render::drawerFace<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor3FFaE<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerFaces<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}
	case 234:
	{
		Algo::Render::drawerFace<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor3FFaW<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerFaces<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}
	case 241:
	{
		Algo::Render::drawerVolume<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor3WWaV<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerVolumes<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}
	case 242:
	{
		Algo::Render::drawerVolume<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor3WWaE<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerVolumes<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}
	case 243:
	{
		Algo::Render::drawerVolume<PFP>(m_drawer,myMap,m_selected,position,0.7f);
		m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor3WWaF<PFP::MAP> tra(myMap,m_selected);
		for (Dart d=tra.begin(); d != tra.end(); d= tra.next())
			m_affDarts.push_back(d);
		Algo::Render::drawerVolumes<PFP>(m_drawer, myMap,m_affDarts,position,0.7f);
		break;
	}

	default:
		CGoGNerr <<"Not implemented" << CGoGNendl;
		break;
	}
	m_drawer.endList();
	updateGL();
}



int main(int argc, char **argv)
{
	position = myMap.addAttribute<PFP::VEC3>(VERTEX, "position");

	Algo::Modelisation::Primitive3D<PFP> prim(myMap, position);
	dglobal = prim.hexaGrid_topo(4,4,4);
	prim.embedHexaGrid(1.0f,1.0f,1.0f);

    // un peu d'interface
	QApplication app(argc, argv);
	MyQT sqt;

	// interface de tuto5.ui
    Utils::QT::uiDockInterface dock;
    sqt.setDock(&dock);

 	// message d'aide
	sqt.setHelpMsg("shit click to select a dart\nand select a traversor\nif keyboard focus problem\nundock");



	//  bounding box
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
    float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
    Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

    // envoit info BB a l'interface
	sqt.setParamObject(lWidthObj, lPosObj.data());

	sqt.setCallBack( dock.combo1, SIGNAL(	activated(int)), SLOT(cb_combo1(int)) );
	sqt.setCallBack( dock.combo2, SIGNAL(	activated(int)), SLOT(cb_combo2(int)) );
	sqt.setCallBack( dock.combo3, SIGNAL(	activated(int)), SLOT(cb_combo3(int)) );

	sqt.setCallBack( dock.combo4, SIGNAL(	activated(int)), SLOT(cb_combo4(int)) );
	sqt.setCallBack( dock.combo5, SIGNAL(	activated(int)), SLOT(cb_combo5(int)) );
	sqt.setCallBack( dock.combo6, SIGNAL(	activated(int)), SLOT(cb_combo6(int)) );

	sqt.setCallBack( dock.checkTopo, SIGNAL(	clicked(bool)), SLOT(cb_checkTopo(bool)) );

	sqt.m_selected = NIL;

	sqt.show();


	// et on attend la fin.
	return app.exec();
}
