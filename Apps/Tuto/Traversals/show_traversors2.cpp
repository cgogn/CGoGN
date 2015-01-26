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

#include "show_traversors2.h"
#include <iostream>

#include "Algo/Tiling/Surface/square.h"
#include "Algo/Modelisation/polyhedron.h"
#include "Algo/Modelisation/subdivision.h"
#include "Algo/Import/import.h"

#include "Algo/Render/GL2/topo3Render.h"
#include "Algo/Render/SVG/mapSVGRender.h"

#include "Topology/generic/traversor/traversorFactory.h"

#include "Algo/Render/GL2/drawerCells.h"


void MyQT::cb_checkTopo(bool b)
{
    m_showTopo = b;
    updateGL();
}



void MyQT::cb_combo4(int x)
{
    m_first2 = x;
    if (m_first2!=m_second2)
        traverse2();
    else
        CGoGNerr <<"undefined traversor" << CGoGNendl;
}

void MyQT::cb_combo5(int x)
{
    m_ajd_or_inci2 = x;
    if (m_first2!=m_second2)
        traverse2();
    else
        CGoGNerr <<"undefined traversor" << CGoGNendl;
}

void MyQT::cb_combo6(int x)
{
    m_second2 = x;
    if (m_first2!=m_second2)
        traverse2();
    else
        CGoGNerr <<"undefined traversor" << CGoGNendl;
}

void MyQT::cb_explode(int x)
{
    m_expl = float (x) /100.0f;
	if (m_last == 2)
        traverse2();
}

void MyQT::cb_initGL()
{
	m_render_topo = new Algo::Render::GL2::TopoRender();
	m_render_topo->updateData<PFP>(myMap, position,  0.95f, 0.9f, 0.8f);
	m_dm_topo = new DartMarker<MAP>(myMap);
}

void MyQT::cb_redraw()
{
    if (m_showTopo)
    {
		glDepthFunc(GL_LESS);
        m_render_topo->drawTopo();

		glDepthFunc(GL_LEQUAL);
        if (m_selected != NIL)
			m_render_topo->overdrawDart(myMap, m_selected, 7, 1.0f, 0.0f, 1.0f);

        for (std::vector<Dart>::iterator it=m_affDarts.begin(); it!=m_affDarts.end(); ++it)
        {
			m_render_topo->overdrawDart(myMap, *it, 6, 1.0f, 1.0f, 1.0f);
        }
    }

    m_drawer.callList();
}

void MyQT::cb_mousePress(int /*button*/, int x, int y)
{
    if (Shift())
    {
		Dart d = m_render_topo->picking(myMap, x, y);
        if (d != Dart::nil())
        {
            CGoGNout << "Dart " << d << " clicked" << CGoGNendl;
            m_selected = d;
        }
        updateGL();
    }
}

void  MyQT::cb_Save()
{
    std::string filename = selectFileSave("Export SVG file ",".","(*.svg)");

    std::string filename1 = filename + std::string("Drawer") + std::string(".svg");
    std::string filename2 = filename + std::string("Topo") + std::string(".svg");

    Utils::SVG::SVGOut svg1(filename1, modelViewMatrix(), projectionMatrix());
    m_drawer.toSVG(svg1);
    svg1.write();

    Utils::SVG::SVGOut svg2(filename2, modelViewMatrix(), projectionMatrix());
    m_render_topo->toSVG(svg2);
    svg2.write();
}


void MyQT::traverse2()
{
    if (m_selected == NIL)
        return;
    m_last=2;
    //	int code = (m_ajd_or_inci2)*100+m_first2*10+m_second2;

    m_drawer.newList(GL_COMPILE);
    m_drawer.lineWidth(7.0f);
    m_drawer.pointSize(9.0f);
    m_drawer.color3f(0.0f,0.7f,0.0f);

    m_affDarts.clear();

    if (m_ajd_or_inci2 == 0) // incident
    {
        Algo::Render::drawerCell<PFP>(VERTEX+m_second2, m_drawer, myMap, m_selected, position, m_expl);
        m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor* tra = TraversorFactory<PFP::MAP>::createIncident(myMap, m_selected, 2, VERTEX+m_second2, VERTEX+m_first2);
        for (Dart d=tra->begin(); d != tra->end(); d= tra->next())
            m_affDarts.push_back(d);
        Algo::Render::drawerCells<PFP>(VERTEX+m_first2, m_drawer, myMap, m_affDarts, position, m_expl);
    }
    else	// adjacent
    {
        Algo::Render::drawerCell<PFP>(VERTEX+m_first2, m_drawer, myMap, m_selected, position, m_expl);
        m_drawer.color3f(1.0f,0.0f,0.0f);
		Traversor* tra = TraversorFactory<PFP::MAP>::createAdjacent(myMap, m_selected, 2, VERTEX+m_first2, VERTEX+m_second2);
        for (Dart d = tra->begin(); d != tra->end(); d = tra->next())
            m_affDarts.push_back(d);
        Algo::Render::drawerCells<PFP>(VERTEX+m_first2, m_drawer, myMap, m_affDarts, position, m_expl);
    }

    m_drawer.endList();

    //	SelectorMarked sm(*m_dm_topo);
	m_render_topo->updateData<PFP>(myMap, position, 0.95f, 0.9f, 0.8f);

    updateGL();
}

void MyQT::dynamicMarkOrbit(unsigned int orb)
{
    switch(orb)
    {
        case VERTEX:
            m_dm_topo->markOrbit<VERTEX>(m_selected);
            break;
        case EDGE:
            m_dm_topo->markOrbit<EDGE>(m_selected);
            break;
        case FACE:
            m_dm_topo->markOrbit<FACE>(m_selected);
            break;
        case VOLUME:
            m_dm_topo->markOrbit<VOLUME>(m_selected);
            break;
        default:
            break;
    }
}

void MyQT::init(char *fname)
{
	if(fname == NULL)
	{
		position = myMap.addAttribute<VEC3, VERTEX, MAP>( "position");

		Algo::Surface::Tilings::Square::Grid<PFP> grid(myMap, 5, 5, true);
		grid.embedIntoGrid(position, 1.,1.,0.);
		dglobal = NIL;
	}
	else
	{
		std::string filename(fname);
		size_t pos = filename.rfind(".");    // position of "." in filename
		std::string extension = filename.substr(pos);
		std::vector<std::string> attrNames ;

		if(extension == std::string(".map"))
		{
			myMap.clear(true);

			if(!myMap.loadMapBin(filename))
			{
				std::cout << "could not load file" << std::endl;
			}

			//myMap.dumpAttributesAndMarkers();

			position = myMap.getAttribute<VEC3, VERTEX, MAP>("position");
		}
		else if(!Algo::Surface::Import::importMesh<PFP>(myMap, filename, attrNames))
		{
			std::cerr << "could not import " << filename << std::endl ;
			exit(1);
		}
		position = myMap.getAttribute<VEC3, VERTEX, MAP>(attrNames[0]) ;
	}

	Geom::BoundingBox<VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
	float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
	Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

	// envoit info BB a l'interface
	setParamObject(lWidthObj, lPosObj.data());
	m_selected = NIL;
}

int main(int argc, char **argv)
{
	// un peu d'interface
	QApplication app(argc, argv);
	MyQT sqt;

    if(argc == 1)
    {
		sqt.init(NULL);
    }
    else
    {
		sqt.init(argv[1]);
    }

    // interface de tuto5.ui
    Utils::QT::uiDockInterface dock;
    sqt.setDock(&dock);

    // message d'aide
    sqt.setHelpMsg("shit click to select a dart\nand select a traversor\nif keyboard focus problem\nundock");

    //  bounding box
//    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
//    float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
//    Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

//    // envoit info BB a l'interface
//    sqt.setParamObject(lWidthObj, lPosObj.data());

    sqt.setCallBack( dock.combo4, SIGNAL(	activated(int)), SLOT(cb_combo4(int)) );
    sqt.setCallBack( dock.combo5, SIGNAL(	activated(int)), SLOT(cb_combo5(int)) );
    sqt.setCallBack( dock.combo6, SIGNAL(	activated(int)), SLOT(cb_combo6(int)) );
    sqt.setCallBack( dock.explodeSlider, SIGNAL(valueChanged(int)), SLOT(cb_explode(int)) );

    sqt.setCallBack( dock.checkTopo, SIGNAL(	clicked(bool)), SLOT(cb_checkTopo(bool)) );

//    sqt.m_selected = NIL;

    sqt.show();

    // et on attend la fin.
    return app.exec();
}
