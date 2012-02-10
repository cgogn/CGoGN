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
* Web site: http://cgogn.unistra.fr/                                  *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include "simpleGMap2.h"
#include "Utils/GLSLShader.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Modelisation/polyhedron.h"

SimpleGMap2::SimpleGMap2()
{
	 position = myMap.addAttribute<PFP::VEC3>(VERTEX, "position");

     Dart d = Algo::Modelisation::createTetrahedron<PFP>(myMap);
     position[d] = VEC3(0,0,0);
     position[myMap.phi1(d)] = VEC3(10,0,15);
     position[myMap.phi_1(d)] = VEC3(10,20,15);
     position[myMap.phi_1(myMap.phi2(d))] = VEC3(0,0,30);

     VEC3 mid = (position[d] + position[myMap.phi1(d)]) / 2.0f;
     myMap.cutEdge(d);
     position[myMap.phi1(d)] = mid;

     Algo::Modelisation::Polyhedron<PFP> poly(myMap, position);

     d = poly.cylinder_topo(5, 1, false, false);

     poly.embedCylinder(10, 10, 5);

     d = myMap.phi1(d);
     Dart dd = myMap.beta2(d);
     myMap.unsewFaces(d);
     myMap.sewFaces(d, dd);

     position[d][1] += 3.0f;
}

void SimpleGMap2::initGUI()
{

}

void SimpleGMap2::cb_initGL()
{
	Utils::GLSLShader::setCurrentOGLVersion(1) ;

	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position) ;
	VEC3 gPosObj = bb.center() ;
	float tailleX = bb.size(0) ;
	float tailleY = bb.size(1) ;
	float tailleZ = bb.size(2) ;
	float gWidthObj = std::max<float>(std::max<float>(tailleX, tailleY), tailleZ) ;
	setParamObject(gWidthObj, gPosObj.data());
}

void SimpleGMap2::cb_redraw()
{
	glDisable(GL_LIGHTING);
	glLineWidth(1.0f);
	Algo::Render::GL1::renderTopoGMD2<PFP>(myMap, position, true, true, true, 0.9f, 0.9f, 0.9f);
}


/**********************************************************************************************
 *                                      MAIN FUNCTION                                         *
 **********************************************************************************************/

int main(int argc, char **argv)
{
	QApplication app(argc, argv) ;

	SimpleGMap2 sqt ;
	sqt.setGeometry(0, 0, 1000, 800) ;
 	sqt.show() ;

	sqt.initGUI() ;

	return app.exec() ;
}

