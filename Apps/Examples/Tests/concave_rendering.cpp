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

#include "concave_rendering.h"

#include <iostream>

#include "Topology/generic/parameters.h"
#include "Topology/map/map2.h"
#include "Topology/generic/embeddedMap2.h"
#include "Geometry/vector_gen.h"

#include "Algo/Import/import.h"
#include "Algo/Geometry/boundingbox.h"

#include "Algo/Render/GL2/mapRender.h"
#include "Utils/Shaders/shaderSimpleColor.h"

#include <glm/gtc/type_ptr.hpp>

#include "Algo/Render/SVG/mapSVGRender.h"


#include "Algo/Modelisation/triangulation.h"

using namespace CGoGN ;



float Ifont[74*2]={
0.145434,0.126469, 0.141475,0.096252, 0.133621,0.0730446, 0.120846,0.0558913,
0.0764256,0.0359233,  0,0.028701, 0,0.0215257, 0,0.0143506, 0,0.00717529, 0,0,

0.0560801,0, 0.11216,0, 0.16824,0,  0.22432,0, 0.2804,0,  0.33648,0, 0.39256,0,
0.448641,0, 0.448641,0.00717529, 0.448641,0.0143506, 0.448641,0.0215257, 0.448641,0.028701,
0.406716,0.0313208, 0.373254,0.0363483, 0.347367,0.0446329, 0.328172,0.0570242,
0.314784,0.0743722, 0.306316,0.0975264, 0.301885,0.127337, 0.300604,0.164653,
0.300604,0.248489, 0.300604,0.332326, 0.300604,0.416163, 0.300604,0.5,
0.300604,0.583837, 0.300604,0.667674, 0.300604,0.751511, 0.300604,0.835347,
0.301761,0.872601, 0.305891,0.902261, 0.313987,0.925229, 0.327039,0.942409,
0.371979,0.963015,  0.448641,0.971299, 0.448641,0.978474, 0.448641,0.98565,
 0.448641,0.992825, 0.448641,1, 0.39256,1, 0.33648,1, 0.2804,1, 0.22432,1,
0.11216,1,  0,1,  0,0.992825, 0,0.98565, 0,0.978474, 0,0.971299,
0.0431605,0.967874, 0.0770629,0.96259, 0.102787,0.954438, 0.121412,0.942409,
0.134019,0.925495, 0.141687,0.902686, 0.145496,0.872973, 0.146526,0.835347,
0.146526,0.751511, 0.146526,0.667674, 0.146526,0.583837, 0.146526,0.5,
0.146526,0.416163, 0.146526,0.332326, 0.146526,0.248489, 0.146526,0.164653};


float Gfont[174*2]={
0.934964,0.533333, 0.888768,0.533333, 0.842572,0.533333, 0.796377,0.533333,
 0.703986,0.533333,  0.611594,0.533333,
 0.611594,0.526812, 0.611594,0.52029,
0.611594,0.513768, 0.611594,0.507246,
0.632945,0.505602, 0.650611,0.503872, 0.665118,0.501973, 0.676993,0.499819,
0.686761,0.497325, 0.69495,0.494407, 0.702086,0.490979, 0.708696,0.486957,
0.716245,0.481097, 0.722622,0.473234, 0.727879,0.463264, 0.732065,0.451087,
0.735233,0.4366, 0.737432,0.419701, 0.738714,0.400289, 0.73913,0.378261,
0.73913,0.348913, 0.73913,0.319565, 0.73913,0.290217, 0.73913,0.26087,
0.73913,0.231522, 0.73913,0.202174, 0.73913,0.172826, 0.73913,0.143478,
0.735128,0.126489, 0.723686,0.110553, 0.705656,0.0960427, 0.681884,0.0833333,
0.653221,0.0727978, 0.620516,0.0648098, 0.584618,0.0597429, 0.546377,0.057971,
0.462175,0.0655967, 0.38716,0.0879982, 0.321963,0.124462, 0.26721,0.174275,
0.223531,0.236725, 0.191553,0.311096, 0.171906,0.396677, 0.165217,0.492754,
0.167085,0.543526, 0.172554,0.593229, 0.18142,0.641199, 0.193478,0.686775,
0.208526,0.729294, 0.226359,0.768093, 0.246773,0.802511, 0.269565,0.831884,
0.294667,0.857006, 0.322011,0.879053, 0.351393,0.897942, 0.382609,0.913587,
0.415455,0.925903, 0.449728,0.934805, 0.485224,0.940209, 0.521739,0.942029,
0.55169,0.940744, 0.580639,0.936911, 0.608534,0.930565, 0.635326,0.921739,
0.660963,0.910468, 0.685394,0.896784, 0.708568,0.880723, 0.730435,0.862319,
0.745853,0.84707, 0.75976,0.831363, 0.772546,0.814603, 0.784601,0.796196,
0.796317,0.775546, 0.808084,0.752061, 0.820293,0.725144, 0.833333,0.694203,
 0.841667,0.694203,  0.85,0.694203,
 0.858333,0.694203,  0.866667,0.694203,
0.865217,0.732428, 0.863768,0.770652, 0.862319,0.808877, 0.86087,0.847101,
0.85942,0.885326, 0.857971,0.923551, 0.856522,0.961775, 0.855072,1,
 0.847101,1,  0.83913,1,
 0.831159,1,  0.823188,1,
0.819138,0.990039, 0.813609,0.98091, 0.806757,0.972767, 0.798732,0.965761,
0.789688,0.960046, 0.779778,0.955774, 0.769155,0.953099, 0.757971,0.952174,
0.75214,0.95238, 0.745562,0.953012, 0.738304,0.954085, 0.730435,0.955616,
0.722022,0.957623, 0.713134,0.960122, 0.703838,0.963131, 0.694203,0.966667,
0.669684,0.97434, 0.645063,0.981046, 0.620375,0.986767, 0.595652,0.991485,
0.570929,0.995185, 0.546241,0.997849, 0.52162,0.999459, 0.497101,1,
0.392522,0.990268, 0.297147,0.962092, 0.212438,0.917001, 0.139855,0.856522,
0.0808595,0.782184, 0.0369112,0.695516, 0.00947117,0.598047, 0,0.491304,
0.00183978,0.439586, 0.00738215,0.390874, 0.016661,0.345049, 0.0297101,0.301993,
0.0465636,0.261586, 0.0672554,0.223709, 0.0918195,0.188244, 0.12029,0.155072,
0.15863,0.119698, 0.200657,0.0886549, 0.246114,0.0620613, 0.294746,0.0400362,
0.3463,0.0226987, 0.400521,0.0101675, 0.457153,0.00256171, 0.515942,0,
0.56498,0.00227868, 0.61721,0.00871821, 0.670527,0.0187244, 0.722826,0.0317028,
0.771999,0.047059, 0.815942,0.0641983, 0.852548,0.0825265, 0.87971,0.101449,
0.87971,0.138225, 0.87971,0.175, 0.87971,0.211775, 0.87971,0.248551,
0.87971,0.285326, 0.87971,0.322101, 0.87971,0.358877, 0.87971,0.395652,
0.880621,0.4247, 0.883741,0.448143, 0.889646,0.466627, 0.898913,0.480797,
0.912121,0.491299, 0.929846,0.498777, 0.952666,0.503878, 0.981159,0.507246,
 0.981159,0.513768, 0.981159,0.52029,
 0.981159,0.526812, 0.981159,0.533333};

/**
 * Struct that contains some informations about the types of the manipulated objects
 * Mainly here to be used by the algorithms that are parameterized by it
 */
struct PFP: public PFP_STANDARD
{
	// definition of the type of the map
	typedef EmbeddedMap2<Map2> MAP;
};

// declaration of the map
PFP::MAP myMap;
// and attribute of position
AttributeHandler<PFP::VEC3> position;

unsigned int nb_ears;

void MyQT::cb_initGL()
{
	// choose to use GL version 2
	Utils::GLSLShader::setCurrentOGLVersion(2);

	// create the render
	m_render = new Algo::Render::GL2::MapRender();

	// create VBO for position
	m_positionVBO = new Utils::VBO();

	// using simple shader with color
	m_shader = new Utils::ShaderSimpleColor();
	m_shader->setAttributePosition(m_positionVBO);
	m_shader->setColor(Geom::Vec4f(0.0f, 1.0f, 0.0f, 0.0f));
	registerShader(m_shader);

	nb_ears = 0;
}

void MyQT::cb_redraw()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	if (m_shader)
	{
		glPointSize(3.0f);
		m_shader->setColor(Geom::Vec4f(0.,1.,1.,0.));
		m_render->draw(m_shader, Algo::Render::GL2::POINTS);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glLineWidth(2.0f);
		glPolygonOffset(0.6f, 0.6f);
		m_shader->setColor(Geom::Vec4f(1.,1.,0.,0.));
		m_render->draw(m_shader, Algo::Render::GL2::LINES);

		glPolygonOffset(0.8f, 0.8f);
		m_shader->setColor(Geom::Vec4f(0.0,0.,0.,0.));
		glLineWidth(1.0f);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		m_render->drawSub(m_shader, Algo::Render::GL2::TRIANGLES, nb_ears);

		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		m_shader->setColor(Geom::Vec4f(0.,1.,0.,0.));
		nb_ears = m_render->drawSub(m_shader, Algo::Render::GL2::TRIANGLES, nb_ears);

		glDisable(GL_POLYGON_OFFSET_FILL);
	}
}

void MyQT::cb_keyPress(int code)
{
	switch(code)
	{
	case '+':
		nb_ears++;
		// born sup dans drawing
		updateGL();
		break;
	case '-':
		if (nb_ears>=1)
			nb_ears--;
		updateGL();
		break;
	case 'n':
		nb_ears=0;
		updateGL();
		break;

	case 'a':
		nb_ears=99999999;
		updateGL();
		break;
	case 't':
		{

			Algo::Modelisation::EarTriangulation<PFP> triangulation(myMap);
			triangulation.triangule();

			SelectorTrue allDarts;
			m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::TRIANGLES);
			m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::LINES);
			updateGL();
		}
		break;
//	case 'u':
//		{
//			Dart d=myMap.begin();
//			while (d != myMap.end())
//			{
//				if (myMap.phi2(d) != d)
//				{
//					Dart e = d;
//					myMap.next(d);
//					if (d== myMap.phi2(e))
//						myMap.next(d);
//					myMap.mergeFaces(e);
//				}
//				else
//					myMap.next(d);
//			}
//		}
//		break;
	}
}

int main(int argc, char **argv)
{

	position = myMap.addAttribute<PFP::VEC3>(VERTEX, "position");

	Dart d0 = myMap.newFace(12);
	position[d0] = PFP::VEC3(0, 20, 0);
	d0 = myMap.phi1(d0);
	position[d0] = PFP::VEC3(10, 20, 0);
	d0 = myMap.phi1(d0);
	position[d0] = PFP::VEC3(10, 30, 0);
	Dart dx = myMap.phi1(d0);
	d0 = myMap.phi<11>(dx);
	position[d0] = PFP::VEC3(8, 27, 0);
	d0 = myMap.phi1(d0);
	position[d0] = PFP::VEC3(8, 22, 0);
	d0 = myMap.phi1(d0);
	position[d0] = PFP::VEC3(2, 22, 0);
	d0 = myMap.phi1(d0);
	position[d0] = PFP::VEC3(2, 27, 0);
	d0 = myMap.phi1(d0);
	myMap.sewFaces(d0,dx);
	position[d0] = PFP::VEC3(5, 27, 0);
	d0 = myMap.phi1(d0);
	position[d0] = PFP::VEC3(5, 30, 0);
	d0 = myMap.phi1(d0);
	position[d0] = PFP::VEC3(0, 30, 0);



	d0 = myMap.newFace(4);
	position[d0] = PFP::VEC3(-5, 14, -5);
	d0 = myMap.phi1(d0);
	position[d0] = PFP::VEC3(0, 18, -5);
	d0 = myMap.phi1(d0);
	position[d0] = PFP::VEC3(5, 14, -5);
	d0 = myMap.phi1(d0);
	position[d0] = PFP::VEC3(0, 20, -5);
	d0 = myMap.phi1(d0);



	Dart d1 = myMap.newFace(10);

	position[d1] = PFP::VEC3(0, 0, 0);
	d1 = myMap.phi1(d1);

	position[d1] = PFP::VEC3(2, 4, 0);
	d1 = myMap.phi1(d1);
	position[d1] = PFP::VEC3(4, 0, 0);
	d1 = myMap.phi1(d1);

	position[d1] = PFP::VEC3(10, 0, 0);
	d1 = myMap.phi1(d1);

	position[d1] = PFP::VEC3(4, 2, 0);
	d1 = myMap.phi1(d1);
	position[d1] = PFP::VEC3(14, 6, 0);
	d1 = myMap.phi1(d1);
	position[d1] = PFP::VEC3(6, 16, 0);
	d1 = myMap.phi1(d1);

	position[d1] = PFP::VEC3(8, 8, 0);
	d1 = myMap.phi1(d1);
	position[d1] = PFP::VEC3(4, 4, 0);
	d1 = myMap.phi1(d1);
	position[d1] = PFP::VEC3(0, 8, 0);


	Dart d2 = myMap.newFace(12);

	position[d2] = PFP::VEC3(0, -20, 0);
	d2 = myMap.phi1(d2);
	position[d2] = PFP::VEC3(4, -20, 0);
	d2 = myMap.phi1(d2);
	position[d2] = PFP::VEC3(8, -20, 0);
	d2 = myMap.phi1(d2);
	position[d2] = PFP::VEC3(12, -20, 0);
	d2 = myMap.phi1(d2);
	position[d2] = PFP::VEC3(12, -16, 0);
	d2 = myMap.phi1(d2);
	position[d2] = PFP::VEC3(12, -12, 0);
	d2 = myMap.phi1(d2);
	position[d2] = PFP::VEC3(12, -8, 0);
	d2 = myMap.phi1(d2);
	position[d2] = PFP::VEC3(8, -8, 0);
	d2 = myMap.phi1(d2);
	position[d2] = PFP::VEC3(4, -8, 0);
	d2 = myMap.phi1(d2);
	position[d2] = PFP::VEC3(0, -8, 0);
	d2 = myMap.phi1(d2);
	position[d2] = PFP::VEC3(0, -12, 0);
	d2 = myMap.phi1(d2);
	position[d2] = PFP::VEC3(0, -16, 0);
	d2 = myMap.phi1(d2);



#define NB 32

	//SPIRAL

	Dart d3 = myMap.newFace(NB*2);

	for (int i = 0; i<NB; ++i)
	{
		float z = 3.0f*float(rand()-RAND_MAX/2)/float(RAND_MAX);
		float alpha = (4.0f*6.283f / NB)*i;
		float radius = 1.2f*(NB-i);
		position[d3] = PFP::VEC3(radius*cos(alpha) -  2*NB+8, radius*sin(alpha), z);
		d3 = myMap.phi1(d3);
	}

	for (int i = NB-1; i>=0; --i)
	{
		float z = 3.0f*float(rand()-RAND_MAX/2)/float(RAND_MAX);
		float alpha = (4.0f*6.283f / NB)*i;
		float radius = (NB-i);
		position[d3] = PFP::VEC3(radius*cos(alpha) -  2*NB+8, radius*sin(alpha), z);
		d3 = myMap.phi1(d3);
	}


	//CIRCLE

	Dart d6 = myMap.newFace(NB);
	for (int i = 0; i<NB; ++i)
	{
		float z = 3.0f*float(rand()-RAND_MAX/2)/float(RAND_MAX);
		float alpha = (6.283f / NB)*i;
		float radius = NB;
		position[d6] = PFP::VEC3(radius*cos(alpha) + 2*NB+8, radius*sin(alpha), z);
		d6 = myMap.phi1(d6);
	}

// pour comparer les 2 versions (oreille et basique)
//#define NBB 100
//	for (int j = 0; j<10000; ++j)
//	{
//		Dart d6 = myMap.newFace(NBB);
//		for (int i = 0; i<NBB; ++i)
//		{
//			float alpha = (6.283f / NB)*i;
//			float radius = NB;
//			position[d6] = PFP::VEC3(radius*cos(alpha) + 2*NB+8, radius*sin(alpha), 0.1f*j);
//			d6 = myMap.phi1(d6);
//		}
//	}

	Geom::Vec3f V1(3,3,3);
	V1.normalize();
	Geom::Vec3f V2 = V1 ^ Geom::Vec3f(0,0,-1);
	Geom::Vec3f V3 = V1 ^ V2;

	V1 *= 50.0f;
	V2 *= 50.0f;

	Dart d5 = myMap.newFace(74);
	for (int i=0; i<74;++i)
	{
		float a = float(rand()-RAND_MAX/2)/float(RAND_MAX) * 0.25f;
		position[d5] = PFP::VEC3(0.0,60.0,0.0f) + Ifont[2*i] * V1 + Ifont[2*i+1]*V2 + a*V3;
		d5 = myMap.phi1(d5);
	}
	Dart d9 = myMap.newFace(174);
	for (int i=0; i<174;++i)
	{
		float a = float(rand()-RAND_MAX/2)/float(RAND_MAX) * 0.25f;
		position[d9] = PFP::VEC3(60.0,60.0,0.0f) + Gfont[2*i] * V1 + Gfont[2*i+1]*V2 + a*V3;
		d9 = myMap.phi1(d9);
	}


	// interface:
	QApplication app(argc, argv);
	MyQT sqt;


	// message d'aide
	sqt.setHelpMsg("Concave face rendering (ears method):\n"
			"a show all trianglesfaces\n"
			"a show none trianglesfaces\n"
			"+ / - show trinagles order rendering (ears creation)");

    //  bounding box
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
    float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
    Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

    // envoit info BB a l'interface
	sqt.setParamObject(lWidthObj, lPosObj.data());

	// show 1 pour GL context
	sqt.show();

	// update du VBO position (context GL necessaire)
	sqt.m_positionVBO->updateData(position);

	// update des primitives du renderer
	SelectorTrue allDarts;
	sqt.m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::TRIANGLES);
	sqt.m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::LINES);
	sqt.m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::POINTS);


//	 show final pour premier redraw
	sqt.show();

	// et on attend la fin.
	return app.exec();
}
