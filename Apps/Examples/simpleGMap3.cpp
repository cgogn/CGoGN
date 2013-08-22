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
    volume = myMap.addAttribute<VEC3, VOLUME>("volume");

    //	CellMarker<EDGE> mE(myMap);

    Algo::Volume::Modelisation::Primitive3D<PFP> primCat(myMap,position);
    Dart d = primCat.hexaGrid_topo(1,1,1);
    primCat.embedHexaGrid(1,1,1);
    myMap.check();

    Dart dp = Algo::Surface::Modelisation::createQuadrangularPyramid<PFP>(myMap);

    position[dp] = typename PFP::VEC3(0.5,0.5,-0.5);
    position[myMap.phi1(dp)] = typename PFP::VEC3(0.5,0.5,0.5);
    position[myMap.phi1(myMap.phi1(dp))] = typename PFP::VEC3(0.5,-0.5,0.5);
    position[myMap.phi_1(dp)] = typename PFP::VEC3(0.5,-0.5,-0.5);
    position[myMap.phi_1(myMap.phi2(dp))] = typename PFP::VEC3(1.5f, 0.0f, 0.0f);

    Dart dtemp = myMap.beta1(myMap.beta0(myMap.beta1(myMap.beta2(myMap.beta1(myMap.beta0(myMap.beta1(myMap.beta2(d))))))));

    myMap.sewVolumes(dtemp,dp);

    //	Geom::Plane3D<PFP::REAL> pl(VEC3(-1,-0.5,-0.5),VEC3(-1,-0.5,0.5),VEC3(1,0.5,0.5));
    //	Algo::Volume::Modelisation::sliceConvexVolume<PFP>(myMap, position, d, pl);

    //	myMap.check();

    //	for(unsigned int i = position.begin() ; i != position.end() ; position.next(i))
    //		position[i] += VEC3(2,0,0);

    //	Algo::Volume::Modelisation::Primitive3D<PFP> prim(myMap, position);
    //	d = prim.hexaGrid_topo(2,2,1);
    //	prim.embedHexaGrid(1,1,1);

    //	Dart d1 = myMap.phi1(myMap.phi1(myMap.phi2(myMap.phi1(myMap.phi1(d)))));
    //	VEC3 mid0 = (position[d1] + position[myMap.phi1(d1)]) / 2.0f;
    //	myMap.cutEdge(d1);
    //	position[myMap.phi1(d1)] = mid0;

    //	VEC3 mid1 = (position[d] + position[myMap.phi1(d)]) / 2.0f;
    //	myMap.cutEdge(d);
    //	position[myMap.phi1(d)] = mid1;

    //	d = myMap.phi1(myMap.phi1(myMap.phi2(myMap.phi1(myMap.phi1(d)))));
    //	VEC3 mid = (position[d] + position[myMap.phi1(d)]) / 2.0f;
    //	myMap.cutEdge(d);
    //	position[myMap.phi1(d)] = mid;

    //	myMap.splitFace(d,myMap.phi1(myMap.phi1(myMap.phi1(d))));

    //	myMap.check();

    //	for(unsigned int i = position.begin() ; i != position.end() ; position.next(i))
    //		position[i] += VEC3(0,2,0);

    //	Algo::Volume::Modelisation::Primitive3D<PFP> prim2(myMap,position);
    //	d = prim2.hexaGrid_topo(2,1,1);
    //	prim2.embedHexaGrid(1,1,1);

    //	d = myMap.phi2(myMap.phi1(myMap.phi1(myMap.phi2(d))));
    //	myMap.unsewVolumes(d);

    //	myMap.check();
}

void SimpleGMap3::initGUI()
{
}

void SimpleGMap3::cb_keyPress(int code)
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
    }
}

void SimpleGMap3::cb_initGL()
{
    Utils::GLSLShader::setCurrentOGLVersion(2) ;

    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position) ;
    VEC3 gPosObj = bb.center() ;
    float tailleX = bb.size(0) ;
    float tailleY = bb.size(1) ;
    float tailleZ = bb.size(2) ;
    float gWidthObj = std::max<float>(std::max<float>(tailleX, tailleY), tailleZ) ;
    setParamObject(gWidthObj, gPosObj.data());

    m_render_topo = new Algo::Render::GL2::Topo3Render();
    m_render_topo->setDartWidth(2.0f);
    m_render_topo->setInitialDartsColor(1.0f,1.0f,1.0f);
    m_render_topo->updateData<PFP>(myMap, position, 0.9f,0.9f,0.8f);
}

void SimpleGMap3::cb_redraw()
{
    glDisable(GL_LIGHTING);
    glLineWidth(1.0f);
    m_render_topo->drawTopo();
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
