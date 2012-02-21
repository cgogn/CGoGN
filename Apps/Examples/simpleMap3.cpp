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

#include "simpleMap3.h"
#include "Utils/GLSLShader.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Modelisation/primitives3d.h"
#include "Algo/Modelisation/subdivision3.h"


SimpleMap3::SimpleMap3()
{
	position = myMap.addAttribute<PFP::VEC3>(VERTEX, "position");

	Algo::Modelisation::Primitive3D<PFP> primCat(myMap,position);
	Dart d = primCat.hexaGrid_topo(2,1,1);
	primCat.embedHexaGrid(1,1,1);

	unsigned int nb=0;
	for(unsigned int i = position.begin(); i!=position.end(); position.next(i))
		nb++;

	std::cout << "Nb vertices (equals 12) : " << nb << std::endl;
	assert(nb==12);

	d = myMap.phi2(myMap.phi1(myMap.phi1(myMap.phi2(d))));

	Dart dd = myMap.phi3(d);

	myMap.unsewVolumes(d);

	myMap.check();

	nb=0;
	for(unsigned int i = position.begin(); i!=position.end(); position.next(i))
		nb++;

	std::cout << "Nb vertices after unsew (equals 16) : " << nb << std::endl;
	assert(nb==16);

	myMap.sewVolumes(d,dd);

	myMap.check();

	nb=0;
	for(unsigned int i = position.begin(); i!=position.end(); position.next(i))
		nb++;

	std::cout << "Nb vertices after resew (equals 12) : " << nb << std::endl;
	assert(nb==12);
}

void SimpleMap3::initGUI()
{

}

void SimpleMap3::cb_initGL()
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

void SimpleMap3::cb_redraw()
{
	glDisable(GL_LIGHTING);
	glLineWidth(1.0f);
	Algo::Render::GL1::renderTopoMD3<PFP>(myMap, position, true, true, true, 0.9f, 0.9f, 0.9f);
}


/**********************************************************************************************
 *                                      MAIN FUNCTION                                         *
 **********************************************************************************************/

int main(int argc, char **argv)
{
	QApplication app(argc, argv) ;

	SimpleMap3 sqt ;
	sqt.setGeometry(0, 0, 1000, 800) ;
 	sqt.show() ;

	sqt.initGUI() ;

	return app.exec() ;
}

