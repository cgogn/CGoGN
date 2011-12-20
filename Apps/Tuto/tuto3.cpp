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

#include "tuto3.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Selection/raySelector.h"
#include "Algo/Modelisation/polyhedron.h"

using namespace CGoGN ;


int main(int argc, char **argv)
{
	//	// interface
	QApplication app(argc, argv);
	MyQT sqt;
	// copy output tout Qt console of application (shift enter)
	CGoGNout.toConsole(&sqt);

	// example code itself
	sqt.createMap();

	sqt.traverseMap();

	// set help message in menu
	sqt.setHelpMsg("Tuto 3: \nUsage of DartMarker and CellMarker\nPick of dart with mouse");
	// final show for redraw
	sqt.show();


	CGoGNout << "You can pick darts dans see it's id with left mouse button"<< CGoGNendl;

	// and wait for the end
	return app.exec();
}


void MyQT::traverseMap()
{

	//traverse cells of map using topological markers on darts

	CGoGNout << "Traverse with DartMarkers:"<< CGoGNendl;
	DartMarker dmV(myMap);
	DartMarker dmE(myMap);
	DartMarker dmF(myMap);
	for (Dart d = myMap.begin(); d != myMap.end(); myMap.next(d))
	{
		if (!dmV.isMarked(d))
		{
			CGoGNout << "Vertex of dart "<<d<<CGoGNendl;
			dmV.markOrbit(VERTEX,d);
		}
		if (!dmE.isMarked(d))
		{
			CGoGNout << "Edgee of dart "<<d<<CGoGNendl;
			dmE.markOrbit(EDGE,d);
		}
		if (!dmF.isMarked(d))
		{
			CGoGNout << "Face of dart "<<d<<CGoGNendl;
			dmF.markOrbit(FACE,d);
		}
	}


	// traverses cells of map with markers on embedded cells.
	// More efficients but more memory costly if cells are not already embedded.
	// Avoid using construction of objects not ell embedded

	CGoGNout << "========================="<< CGoGNendl;
	CGoGNout << "Traverse with CellMarkers:"<< CGoGNendl;
	CellMarker cmV(myMap,VERTEX);
	CellMarker cmE(myMap,EDGE);
	CellMarker cmF(myMap,FACE);
	for (Dart d = myMap.begin(); d != myMap.end(); myMap.next(d))
	{
		if (!cmV.isMarked(d))
		{
			CGoGNout << "Vertex of dart "<<d<<CGoGNendl;
			cmV.mark(d);
		}
		if (!cmE.isMarked(d))
		{
			CGoGNout << "Edgee of dart "<<d<<CGoGNendl;
			cmE.mark(d);
		}
		if (!cmF.isMarked(d))
		{
			CGoGNout << "Face of dart "<<d<<CGoGNendl;
			cmF.mark(d);
		}
	}

	// markers are cleaned and released at destruction of DartMarkers & CellMarkers
	// DartMarkerStore should be used if few darts are traversed
	// DartMarkerNoUnmark can be use if you want to manage unmarking yourself

}


void MyQT::createMap()
{

	Dart d1 = Algo::Modelisation::Polyhedron<PFP>::createTetra(myMap);

	Dart d2 = d1;

	position = myMap.addAttribute<PFP::VEC3>(VERTEX, "position");

	position[d2] = PFP::VEC3(1, 0, 0);
	d2 = PHI1(d2);
	position[d2] = PFP::VEC3(-1, 0, 0);
	d2 = PHI1(d2);
	position[d2] = PFP::VEC3(0, 2, 0);
	d2 = PHI<211>(d2);
	position[d2] = PFP::VEC3(0, 1, 2);

    //  bounding box of scene
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
    float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
    Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

    // send BB info to interface for centering on GL screen
	setParamObject(lWidthObj, lPosObj.data());

	// first show for be sure that GL context is binded
	show();

	// render the topo of the map without boundary darts
	SelectorDartNoBoundary<PFP::MAP> nb(myMap);
	m_render_topo->updateData<PFP>(myMap, position, 0.9f, 0.9f,nb);
}


// initialization GL callback
void MyQT::cb_initGL()
{
	m_render_topo = new Algo::Render::GL2::TopoRenderMapD() ;
}

// redraw GL callback (clear and swap already done)
void MyQT::cb_redraw()
{
	if (dart_selected != NIL)
		m_render_topo->overdrawDart(dart_selected, 5, 1.0f,0.0f,0.0f);
	m_render_topo->drawTopo();

}

// mouse picking
void MyQT::cb_mouseClick(int button, int x, int y)
{
	if (button == Qt::LeftButton)
	{
		Dart  d = m_render_topo->picking<PFP>(myMap,allDarts,x,y);
		if (d != NIL)

			CGoGNout << "Dart "<< d <<  CGoGNendl;
		dart_selected=d;
		updateGL();
	}

}
