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

#include "tuto4.h"
#include "Algo/Geometry/boundingbox.h"
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


	CGoGNout << "You can pick darts dans see incident/dajacent cells (one dart) with click/shift click/ctrl click"<< CGoGNendl;

	// and wait for the end
	return app.exec();
}


// example of usage of traversor for local traverse
void MyQT::cb_mouseClick(int button, int x, int y)
{

	if (button == Qt::LeftButton)
	{
		Dart  d = m_render_topo->picking<PFP>(myMap,x,y);
		if (d != NIL)
		{
			dart_selected.clear();
			// EDGES INCIDENT TO VERTEX
			if (Shift())
			{
				Traversor2VE<PFP::MAP> tra(myMap,d);
				for (Dart e=tra.begin(); e!=tra.end(); e=tra.next())
					dart_selected.push_back(e);
				CGoGNout << "traverse edges incident to a vertex" << CGoGNendl;
				color = Geom::Vec3f(1,0,0);
			}
			// EDGES INCIDENT TO FACES
			else if (Control())
			{
				Traversor2FE<PFP::MAP> tra(myMap,d);
				for (Dart e=tra.begin(); e!=tra.end(); e=tra.next())
					dart_selected.push_back(e);
				CGoGNout << "traverse edges incident to a face (its boundary)" << CGoGNendl;
				color = Geom::Vec3f(0,1,0);
			}
			// VERTICES ADJACENT TO VERTEX BY A FACE
			else
			{
				Traversor2VVaF<PFP::MAP> tra(myMap,d);
				for (Dart e=tra.begin(); e!=tra.end(); e=tra.next())
					dart_selected.push_back(e);
				CGoGNout << "traverse vertices adjacent to vertex by a face " << CGoGNendl;
				color = Geom::Vec3f(1,1,0);

			}
		}
		updateGL();
	}
}


void MyQT::traverseMap()
{
	//traverse cells with traversor

	CGoGNout << "Traverse with Vertex Traversor:  "<< CGoGNendl;
	TraversorV<PFP::MAP> traV(myMap);
	for (Dart d=traV.begin(); d!=traV.end(); d=traV.next())
		CGoGNout << "Vertex of dart "<<d<< CGoGNendl;

	CGoGNout << "Traverse with Edge Traversor:  "<< CGoGNendl;
	TraversorE<PFP::MAP> traE(myMap);
	for (Dart d=traE.begin(); d!=traE.end(); d=traE.next())
		CGoGNout << "Edge of dart "<<d<< CGoGNendl;

	CGoGNout << "Traverse with Face Traversor:  "<< CGoGNendl;
	TraversorF<PFP::MAP> traF(myMap);
	for (Dart d=traF.begin(); d!=traF.end(); d=traF.next())
		CGoGNout << "Face of dart "<<d<< CGoGNendl;

}




void MyQT::createMap()
{

	Dart d1 = Algo::Modelisation::Polyhedron<PFP>::createHexa(myMap);

	Dart d2 = d1;

	position = myMap.addAttribute<PFP::VEC3>(VERTEX, "position");

	position[d2] = PFP::VEC3(1, 0, 0);
	d2 = PHI1(d2);
	position[d2] = PFP::VEC3(0, 0, 0);
	d2 = PHI1(d2);
	position[d2] = PFP::VEC3(0, 1, 0);
	d2 = PHI1(d2);
	position[d2] = PFP::VEC3(1, 1, 0);
	d2 = PHI<2112>(d2);
	position[d2] = PFP::VEC3(1, 0, 1);
	d2 = PHI1(d2);
	position[d2] = PFP::VEC3(1, 1, 1);
	d2 = PHI1(d2);
	position[d2] = PFP::VEC3(0, 1, 1);
	d2 = PHI1(d2);
	position[d2] = PFP::VEC3(0, 0, 1);

    //  bounding box of scene
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
    float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
    Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

    // send BB info to interface for centering on GL screen
	setParamObject(lWidthObj, lPosObj.data());

	// first show for be sure that GL context is binded
	show();
	m_render_topo->updateData<PFP>(myMap, position, 0.9f, 0.9f);
}


// initialization GL callback
void MyQT::cb_initGL()
{
	m_render_topo = new Algo::Render::GL2::TopoRender() ;
}

// redraw GL callback (clear and swap already done)
void MyQT::cb_redraw()
{
	if (!dart_selected.empty())
	for (std::vector<Dart>::iterator it = dart_selected.begin(); it != dart_selected.end(); ++it)
	{
		m_render_topo->overdrawDart(*it, 5, color[0],color[1],color[2]);
	}
	m_render_topo->drawTopo();

}





