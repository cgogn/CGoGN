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
#include "Algo/Tiling/Volume/cubic.h"
#include "Algo/Modelisation/subdivision3.h"

SimpleMap3::SimpleMap3()
{
	position = myMap.addAttribute<VEC3, VERTEX>("position");


      Algo::Volume::Tilings::Cubic::Grid<PFP> cubic(myMap, 1, 1, 1);
      cubic.embedIntoGrid(position, 1.0f, 1.0f, 1.0f);
      myMap.check();


//    Dart dp = Algo::Surface::Modelisation::createQuadrangularPyramid<PFP>(myMap);

//    position[dp] = typename PFP::VEC3(0.5,0.5,-0.5);
//    position[myMap.phi1(dp)] = typename PFP::VEC3(0.5,0.5,0.5);
//    position[myMap.phi1(myMap.phi1(dp))] = typename PFP::VEC3(0.5,-0.5,0.5);
//    position[myMap.phi_1(dp)] = typename PFP::VEC3(0.5,-0.5,-0.5);
//    position[myMap.phi_1(myMap.phi2(dp))] = typename PFP::VEC3(1.5f, 0.0f, 0.0f);

//    Dart dtemp = myMap.phi_1(myMap.phi2(myMap.phi_1(myMap.phi_1(myMap.phi2(d)))));

//    myMap.sewVolumes(dtemp,dp);

//      Algo::Volume::Tilings::Cubic::Grid<PFP> cubic(myMap, 2, 1, 1);
//      cubic.embedIntoGrid(position, 1.0f, 1.0f, 1.0f);
//      myMap.check();

//	unsigned int nb=0;
//	for(unsigned int i = position.begin(); i!=position.end(); position.next(i))
//		nb++;

//	std::cout << "Nb vertices (equals 12) : " << nb << std::endl;
//	assert(nb==12);


//	d = myMap.phi2(myMap.phi1(myMap.phi1(myMap.phi2(d))));

//	Dart dd = myMap.phi3(d);

//	myMap.unsewVolumes(d);

//	myMap.check();

//	nb=0;
//	for(unsigned int i = position.begin(); i!=position.end(); position.next(i))
//		nb++;

//	std::cout << "Nb vertices after unsew (equals 16) : " << nb << std::endl;
//	assert(nb==16);

//	myMap.sewVolumes(d,dd);

//	myMap.check();

//	nb=0;
//	for(unsigned int i = position.begin(); i!=position.end(); position.next(i))
//		nb++;

//	std::cout << "Nb vertices after resew (equals 12) : " << nb << std::endl;
//	assert(nb==12);

}

void SimpleMap3::initGUI()
{
}

void SimpleMap3::cb_initGL()
{
	Utils::GLSLShader::setCurrentOGLVersion(2) ;

	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position) ;
	VEC3 gPosObj = bb.center() ;
	float tailleX = bb.size(0) ;
	float tailleY = bb.size(1) ;
	float tailleZ = bb.size(2) ;
	float gWidthObj = std::max<float>(std::max<float>(tailleX, tailleY), tailleZ) ;
	setParamObject(gWidthObj, gPosObj.data());

	m_render_topo = new Algo::Render::GL2::Topo3RenderMap<PFP>();
	m_render_topo->setDartWidth(2.0f);
	m_render_topo->setInitialDartsColor(1.0f,1.0f,1.0f);
	m_render_topo->updateData(myMap, position, 0.9f,0.9f,0.8f);

	m_render_topo_boundary = new Algo::Render::GL2::TopoRenderMap<PFP>();
	m_render_topo_boundary->setDartWidth(2.0f);
	m_render_topo_boundary->setInitialDartsColor(0.4f,0.8f,0.4f);
	m_render_topo_boundary->updateDataBoundary(myMap, position, 0.9f,0.9f,bb.maxSize()/50.0f);

	m_render_topo_primal = new Algo::Render::GL2::Topo3PrimalRender<PFP>();
    m_render_topo_primal->setDartWidth(2.0f);
    m_render_topo_primal->setInitialDartsColor(1.0f,1.0f,1.0f);
	m_render_topo_primal->updateData(myMap, position, 0.95f,0.85f);
}

void SimpleMap3::cb_keyPress(int code)
{
    switch(code)
    {
        case 'e':
        {
            time_t rawtime;
            struct tm * timeinfo;
            char buffer[80];

            time (&rawtime);
            timeinfo = localtime (&rawtime);

            strftime (buffer,80,".%F.%H:%M:%S",timeinfo);

            std::string filename = std::string("topo_screenshot") + std::string(buffer) + std::string(".svg");
            m_render_topo->svgout2D(filename, modelViewMatrix(), projectionMatrix());
            break;
        }
        case 'E':
        {
            time_t rawtime;
            struct tm * timeinfo;
            char buffer[80];

            time (&rawtime);
            timeinfo = localtime (&rawtime);

            strftime (buffer,80,".%F.%H:%M:%S",timeinfo);

            std::string filename = std::string("topo_screenshot") + std::string(buffer) + std::string(".svg");
            m_render_topo_primal->svgout2D(filename, modelViewMatrix(), projectionMatrix());
            break;
        }
    }
}


void SimpleMap3::cb_redraw()
{
	glDisable(GL_LIGHTING);
	glLineWidth(1.0f);
    m_render_topo->drawTopo();
    //m_render_topo_boundary->drawTopo();
    //m_render_topo_primal->drawTopo();
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
