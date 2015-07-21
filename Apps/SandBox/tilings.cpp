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

#include "tilings.h"

#include "Geometry/vector_gen.h"

#include "Algo/Import/import.h"
#include "Algo/Geometry/boundingbox.h"
#include "Utils/GLSLShader.h"
//#include "Algo/Geometry/area.h"
#include "Algo/Geometry/normal.h"
#include "Geometry/matrix.h"
#include "Geometry/transfo.h"

#include "Utils/cgognStream.h"
#include "Utils/chrono.h"

using namespace CGoGN ;

/**
 * Struct that contains some informations about the types of the manipulated objects
 * Mainly here to be used by the algorithms that are parameterized by it
 */
struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef EmbeddedMap2 MAP;
};

typedef PFP::MAP MAP;
typedef PFP::VEC3 VEC3;

PFP::MAP myMap;

VertexAttribute<VEC3, MAP> position;
VertexAttribute<VEC3, MAP> position2;
VertexAttribute<VEC3, MAP> normal;

void MyQT::cb_initGL()
{
	// create the render
	m_render = new Algo::Render::GL2::MapRender();

	// create VBO for position
	m_positionVBO = new Utils::VBO();
	m_positionVBO->updateData(position);

	m_normalVBO =  new Utils::VBO();

	m_shader = new Utils::ShaderSimpleColor();
	m_shader->setAttributePosition(m_positionVBO);
	m_shader->setColor(Geom::Vec4f(1.,1.,0.,0.));

	m_lines = new Utils::ShaderVectorPerVertex();
	m_lines->setAttributePosition(m_positionVBO);
	m_lines->setAttributeVector(m_normalVBO);
	m_lines->setScale(0.2f);
	m_lines->setColor(Geom::Vec4f(0.0f, 1.0f, 0.2f, 0.0f));

	Algo::Surface::Geometry::computeNormalVertices<PFP>(myMap, position, normal) ;

	m_render->initPrimitives<PFP>(myMap, Algo::Render::GL2::LINES);
	m_render->initPrimitives<PFP>(myMap, Algo::Render::GL2::POINTS);

	registerShader(m_shader);
	registerShader(m_lines);

	m_normalVBO->updateData(normal);
}

void MyQT::cb_redraw()
{
	m_render->draw(m_shader, Algo::Render::GL2::LINES);
	m_render->draw(m_lines, Algo::Render::GL2::POINTS);
}

void MyQT::squareTiling(int code)
{
	switch(code)
	{
		case 1:
		{
			std::cout << "square grid tiling" << std::endl;
			Algo::Surface::Tilings::Square::Grid<PFP> g(myMap, 10, 10, true);
			g.embedIntoGrid(position, 1.0, 1.0, 0.0);

			Geom::Matrix44f trf;
			trf.identity();
			Geom::translate<float>(0.5, 0.5, 0.0, trf);
			g.transform(position, trf);

			g.exportPositions(position, "grid.bs");

			break;
		}
		case 2:
		{
			std::cout << "square grid twisted strip tiling" << std::endl;
			Algo::Surface::Tilings::Square::Grid<PFP> g(myMap, 10, 10, true);
			g.embedIntoTwistedStrip(position, 0.2f, 0.7f, 5);

			Geom::Matrix44f trf;
			trf.identity();
			Geom::translate<float>(0.5, 0.5, 0.0, trf);
			g.transform(position, trf);


			g.exportPositions(position, "gridtwisted.bs");

			break;
		}
		case 3:
		{
			std::cout << "square grid helicoid tiling" << std::endl;
			Algo::Surface::Tilings::Square::Grid<PFP> g(myMap,20,20,true);
			g.embedIntoHelicoid(position, 0.2f,  0.3f, 0.8f, 2.0f);

//			Geom::Matrix44f trf;
//			trf.identity();
//			Geom::translate<float>(0.5,0.5,0.0,trf);
//			g.transform(position, trf);


			g.exportPositions(position, "gridhelicoid.bs");

			break;
		}
		case 4:
		{
			std::cout << "square cylinder tiling" << std::endl;
			Algo::Surface::Tilings::Square::Cylinder<PFP> c(myMap,20,20);
			c.embedIntoCylinder(position,0.5f,0.7f,1.0f);

			Geom::Matrix44f trf;
			trf.identity();
			Geom::translate<float>(0.5f,0.5f,0.0f,trf);
			c.transform(position, trf);


			c.exportPositions(position, "cylinder.bs");

			break;
		}
		case 5:
		{
			std::cout << "square cylinder tiling" << std::endl;
			Algo::Surface::Tilings::Square::Cylinder<PFP> c(myMap,20,20);
			c.triangleBottom();
			c.triangleTop();
			c.embedIntoCylinder(position,0.5f,0.7f,1.0f);

			break;
		}
		case 6:
		{
			std::cout << "square cylinder sphere tiling" << std::endl;
			Algo::Surface::Tilings::Square::Cylinder<PFP> c(myMap,20,20);
			c.triangleTop();
			c.triangleBottom();
			c.embedIntoSphere(position,0.5);

			break;
		}
		case 7:
		{
			std::cout << "square cylinder cone tiling" << std::endl;
			Algo::Surface::Tilings::Square::Cylinder<PFP> c(myMap,20,20);
			c.triangleTop();
			c.triangleBottom();
			c.embedIntoCone(position,0.5, 1.0);

			break;
		}
		case 8:
		{
            std::cout << "square cube tiling" << std::endl;
            Algo::Surface::Tilings::Square::Cube<PFP> c(myMap,1,1,2);
			c.embedIntoCube(position,1.0, 1.0, 1.0);

			Geom::Matrix44f trf;
			trf.identity();
			Geom::translate<float>(0.5,0.5,0.5,trf);
			c.transform(position, trf);

			c.exportPositions(position, "cube.bs");

			break;
		}
		case 9:
		{
			std::cout << "square tore tiling" << std::endl;
			Algo::Surface::Tilings::Square::Tore<PFP> c(myMap,20,10);
			c.embedIntoTore(position, 0.9f, 0.5f);

			c.exportPositions(position, "tore.bs");
			break;
		}

		default:
		{
			break;
		}
	}

	myMap.check();
}

// mouse picking
void MyQT::triangularTiling(int code)
{
	switch(code)
	{
		case 1:
		{
			std::cout << "triangle grid tiling" << std::endl;
            Algo::Surface::Tilings::Triangular::Grid<PFP> g(myMap,1,5,true);
			g.embedIntoGrid(position,1,1);

			break;
		}
		case 2:
		{
			std::cout << "triangle grid twisted strip tiling" << std::endl;
			Algo::Surface::Tilings::Triangular::Grid<PFP> g(myMap,10,10,true);
			g.embedIntoTwistedStrip(position, 0.3f, 0.8f, 5);

			break;
		}
		case 3:
		{
			std::cout << "triangle grid helocoid tiling" << std::endl;
			Algo::Surface::Tilings::Triangular::Grid<PFP> g(myMap,20,20,true);
			g.embedIntoHelicoid(position, 0.3f,  0.8f, 5.0f, 2.0f);

			break;
		}
		case 4:
		{
			std::cout << "triangle cylinder tiling" << std::endl;
			Algo::Surface::Tilings::Triangular::Cylinder<PFP> c(myMap,20,20);
			c.embedIntoCylinder(position,0.5f,0.7f,5.0f);

			break;
		}
		case 5:
		{
			std::cout << "triangle cylinder tiling" << std::endl;
			Algo::Surface::Tilings::Triangular::Cylinder<PFP> c(myMap,20,20);
			c.triangleBottom();
			c.triangleTop();
			c.embedIntoCylinder(position,0.5f,0.7f,5.0f);

			break;
		}
		case 6:
		{
			std::cout << "triangle cylinder sphere tiling" << std::endl;
			Algo::Surface::Tilings::Triangular::Cylinder<PFP> c(myMap,20,20);
			c.triangleTop();
			c.triangleBottom();
			c.embedIntoSphere(position,0.5f);

			break;
		}
		case 7:
		{
			std::cout << "triangle cylinder cone tiling" << std::endl;
			Algo::Surface::Tilings::Triangular::Cylinder<PFP> c(myMap,20,20);
			c.triangleTop();
			c.triangleBottom();
			c.embedIntoCone(position,0.5f, 5.0f);

			break;
		}
		case 8:
		{
            std::cout << "triangle cube tiling" << std::endl;
            Algo::Surface::Tilings::Triangular::Cube<PFP> c(myMap,4,4,4);
            c.embedIntoCube(position,5.0f,5.0f, 5.0f);

			break;
		}
		case 9:
		{
            std::cout << "triangle tore tiling" << std::endl;
            Algo::Surface::Tilings::Triangular::Tore<PFP> c(myMap,20,10);
            c.embedIntoTore(position,5.0f,2.0f);

			break;
		}

		default:
		{
			break;
		}
	}
}


void MyQT::hexagonalTiling(int code)
{
    switch(code)
    {
        case 1:
        {
            std::cout << "hexagonal grid tiling" << std::endl;
            Algo::Surface::Tilings::Hexagonal::Grid<PFP> g(myMap,10,6,true);
            g.embedIntoGrid(position,1,1);

            break;
        }
        case 2:
        {
//            std::cout << "hexagonal grid twisted strip tiling" << std::endl;
//            Algo::Surface::Tilings::Triangular::Grid<PFP> g(myMap,10,10,true);
//            g.embedIntoTwistedStrip(position, 0.3, 0.8, 5);

            break;
        }
        case 3:
        {
//            std::cout << "hexagonal grid helocoid tiling" << std::endl;
//            Algo::Surface::Tilings::Triangular::Grid<PFP> g(myMap,20,20,true);
//            g.embedIntoHelicoid(position, 0.3,  0.8, 5.0, 2.0);

            break;
        }
        case 4:
        {
//            std::cout << "hexagonal cylinder tiling" << std::endl;
//            Algo::Surface::Tilings::Triangular::Cylinder<PFP> c(myMap,20,20);
//            c.embedIntoCylinder(position,0.5,0.7,5.0);

            break;
        }
        case 5:
        {
//            std::cout << "hexagonal cylinder tiling" << std::endl;
//            Algo::Surface::Tilings::Triangular::Cylinder<PFP> c(myMap,20,20);
//            c.triangleBottom();
//            c.triangleTop();
//            c.embedIntoCylinder(position,0.5,0.7,5.0);

            break;
        }
        case 6:
        {
//			std::cout << "hexagonal cylinder sphere tiling" << std::endl;
//			Algo::Surface::Tilings::Triangular::Cylinder<PFP> c(myMap,20,20);
//			c.triangleTop();
//			c.triangleBottom();
//			c.embedIntoSphere(position,0.5);

            break;
        }
        case 7:
        {
//			std::cout << "hexagonal cylinder cone tiling" << std::endl;
//			Algo::Surface::Tilings::Triangular::Cylinder<PFP> c(myMap,20,20);
//			c.triangleTop();
//			c.triangleBottom();
//			c.embedIntoCone(position,0.5, 5.0);

            break;
        }
        case 8:
        {
//            std::cout << "hexagonal cube tiling" << std::endl;
//            Algo::Surface::Tilings::Triangular::Cube<PFP> c(myMap,4,4,4);
//            c.embedIntoCube(position,5.0,5.0, 5.0);

            break;
        }
        case 9:
        {
//            std::cout << "hexagonal tore tiling" << std::endl;
//            Algo::Surface::Tilings::Triangular::Tore<PFP> c(myMap,20,10);
//            c.embedIntoTore(position,5.0,2.0);

            break;
        }

        default:
        {
            break;
        }
    }
}
int main(int argc, char **argv)
{
	// interface:
	QApplication app(argc, argv);
	MyQT sqt;

	sqt.statusMsg("Neww to create a sphere or Load for a mesh file");
	CGoGNStream::allToConsole(&sqt);

	if (!position.isValid())
		position = myMap.addAttribute<PFP::VEC3, VERTEX, MAP>("position");

	if (!normal.isValid())
		normal = myMap.addAttribute<PFP::VEC3, VERTEX, MAP>("normal");

	if(argc == 3)
	{
		if(argv[1][0] == 'S')
			sqt.squareTiling(atoi(argv[2]));
		else if(argv[1][0] == 'T')
			sqt.triangularTiling(atoi(argv[2]));
        else if(argv[1][0] == 'H')
            sqt.hexagonalTiling(atoi(argv[2]));
        else
            return -1;
	}
	myMap.check();

	//  bounding box
	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
	float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
	Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);
	CGoGNout << "lPosObj=" << lPosObj << CGoGNendl;
	CGoGNout << "lWidthObj=" << lWidthObj << CGoGNendl;
	sqt.setParamObject(lWidthObj,lPosObj.data());

	//	myMap.enableQuickTraversal<EDGE>() ;
	//	myMap.enableQuickTraversal<VERTEX>() ;

	sqt.show();

	return app.exec();
}
