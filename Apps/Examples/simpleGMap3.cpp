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

#include "simpleGMap3.h"
#include "Utils/GLSLShader.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Modelisation/primitives3d.h"
#include "Algo/Modelisation/subdivision3.h"

SimpleGMap3::SimpleGMap3()
{
	position = myMap.addAttribute<VEC3, VERTEX>("position");
	normal = myMap.addAttribute<VEC3, VERTEX>("normal");
	volume = myMap.addAttribute<VEC3, VOLUME>("volume");

	Algo::Modelisation::Primitive3D<PFP> primCat(myMap,position);
	Dart d = primCat.hexaGrid_topo(3,1,1);
	primCat.embedHexaGrid(2,1,1);
	std::cout << "AAA"<< std::endl;
	myMap.check();
	std::cout << "AAA"<< std::endl;
	DartMarker markOrient(myMap);
	std::vector<Dart> orient;
	FunctorStore fs(orient);
	
	d = 49;
	myMap.foreach_dart_of_oriented_volume(d, fs);

	std::cout << "AAA"<< std::endl;

	for(std::vector<Dart>::iterator it = orient.begin() ; it != orient.end() ; ++it)
		markOrient.mark(*it);

	SelectorMarked sm(markOrient);
	std::cout << "AAA"<< std::endl;
	//Algo::Modelisation::catmullClarkVol<PFP,PFP::TVEC3,PFP::VEC3>(myMap, position, sm);

	for(unsigned int i = position.begin() ; i != position.end() ; position.next(i))
		position[i] += VEC3(2,0,0);

	Algo::Modelisation::Primitive3D<PFP> prim(myMap, position);
	d = prim.hexaGrid_topo(2,2,1);
	prim.embedHexaGrid(1,1,1);

	Dart d1 = myMap.phi1(myMap.phi1(myMap.phi2(myMap.phi1(myMap.phi1(d)))));
	VEC3 mid0 = (position[d1] + position[myMap.phi1(d1)]) / 2.0f;
	myMap.cutEdge(d1);
	position[myMap.phi1(d1)] = mid0;

	VEC3 mid1 = (position[d] + position[myMap.phi1(d)]) / 2.0f;
	myMap.cutEdge(d);
	position[myMap.phi1(d)] = mid1;

	d = myMap.phi1(myMap.phi1(myMap.phi2(myMap.phi1(myMap.phi1(d)))));
	VEC3 mid = (position[d] + position[myMap.phi1(d)]) / 2.0f;
	myMap.cutEdge(d);
	position[myMap.phi1(d)] = mid;

	for(unsigned int i = position.begin() ; i != position.end() ; position.next(i))
		position[i] += VEC3(0,2,0);

	Algo::Modelisation::Primitive3D<PFP> prim2(myMap,position);
	d = prim2.hexaGrid_topo(2,1,1);
	prim2.embedHexaGrid(1,1,1);

	d = myMap.phi2(myMap.phi1(myMap.phi1(myMap.phi2(d))));
	myMap.unsewVolumes(d);

	myMap.check();
}

void SimpleGMap3::initGUI()
{
}

void SimpleGMap3::cb_initGL()
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

void SimpleGMap3::cb_redraw()
{
	glDisable(GL_LIGHTING);
	glLineWidth(1.0f);
	Algo::Render::GL1::renderTopoGMD3<PFP>(myMap, position, true, true, true, true, 0.9f, 0.9f, 0.9f, 0.9f);

	glDisable(GL_LIGHTING);
	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	Algo::Render::GL1::renderTriQuadPoly<PFP>(myMap, Algo::Render::GL1::LINE, 1.0,position, normal);
}

/**********************************************************************************************
 *                                      MAIN FUNCTION                                         *
 **********************************************************************************************/

int main(int argc, char **argv)
{
	QApplication app(argc, argv) ;

	SimpleGMap3 sqt ;
	sqt.setGeometry(0, 0, 1000, 800) ;
 	sqt.show() ;

	sqt.initGUI() ;

	return app.exec() ;
}
