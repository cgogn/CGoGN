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

#include "tuto1.h"
#include "Algo/Geometry/boundingbox.h"

using namespace CGoGN ;

int main(int argc, char **argv)
{
	//	// interface
	QApplication app(argc, argv);
	MyQT sqt;

	// example code itself
	sqt.createMap();

	// set help message in menu
	sqt.setHelpMsg("First Tuto: \nCreate two faces\nsew them\nand affect positions");
	// final show for redraw
	sqt.show();
	// and wait for the end

//	QGLViewer* ptr = sqt.getQGLWidget();
	return app.exec();
}

void MyQT::cb_keyPress(int keycode)
{
	switch(keycode)
	{
		case 'c' :
			getQGLWidget()->camera()->centerScene();
			updateGL();
			break;
		default:
			break;
	}
}

void MyQT::createMap()
{
	// creation of a new attribute on vertices of type 3D vector for position.
	// a handler to this attribute is returned
	position = myMap.addAttribute<VEC3, VERTEX>("position");

	// creation of 2 new faces: 1 triangle and 1 square
	Dart d1 = myMap.newFace(3);
	Dart d2 = myMap.newFace(4);

	// sew these faces along one of their edge
	myMap.sewFaces(d1, d2);

	// affect position by moving in the map
	position[d1] = VEC3(0, 0, 0);
	position[PHI1(d1)] = VEC3(2, 0, 0);
	position[PHI_1(d1)] = VEC3(1, 2, 0);
	position[PHI<11>(d2)] = VEC3(0, -2, 0);
	position[PHI_1(d2)] = VEC3(2, -2, 0);

    //  bounding box of scene
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
    float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
    Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

	show();
    // send BB info to interface for centering on GL screen
	setParamObject(lWidthObj, lPosObj.data());

	// first show for be sure that GL context is binded
	show();

	// render the topo of the map without boundary darts
	m_render_topo->setInitialBoundaryDartsColor(0,1,0);
	m_render_topo->updateData(myMap, position, 0.9f, 0.9f,true);
}

// initialization GL callback
void MyQT::cb_initGL()
{
	m_render_topo = new Algo::Render::GL2::TopoRenderMap<PFP>() ;
}

// redraw GL callback (clear and swap already done)
void MyQT::cb_redraw()
{
	m_render_topo->drawTopo();
}
