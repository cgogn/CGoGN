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
#include "Topology/map/embeddedMap2.h"
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
0.145434f,0.126469f, 0.141475f,0.096252f, 0.133621f,0.0730446f, 0.120846f,0.0558913f,
0.0764256f,0.0359233f,  0.0f,0.028701f, 0.0f,0.0215257f, 0.0f,0.0143506f, 0.0f,0.00717529f, 0.0f,0.0f,

0.0560801f, 0.0f, 0.11216f,0.0f, 0.16824f,0.0f,  0.22432f,0.0f, 0.2804f,0.0f,  0.33648f,0.0f, 0.39256f,0.0f,
0.448641f,0.0f, 0.448641f,0.00717529f, 0.448641f,0.0143506f, 0.448641f,0.0215257f, 0.448641f,0.028701f,
0.406716f,0.0313208f, 0.373254f,0.0363483f, 0.347367f,0.0446329f, 0.328172f,0.0570242f,
0.314784f,0.0743722f, 0.306316f,0.0975264f, 0.301885f,0.127337f, 0.300604f,0.164653f,
0.300604f,0.248489f, 0.300604f,0.332326f, 0.300604f,0.416163f, 0.300604f,0.5f,
0.300604f,0.583837f, 0.300604f,0.667674f, 0.300604f,0.751511f, 0.300604f,0.835347f,
0.301761f,0.872601f, 0.305891f,0.902261f, 0.313987f,0.925229f, 0.327039f,0.942409f,
0.371979f,0.963015f,  0.448641f,0.971299f, 0.448641f,0.978474f, 0.448641f,0.98565f,
 0.448641f,0.992825f, 0.448641f,1.0f, 0.39256f,1.0f, 0.33648f,1.0f, 0.2804f,1.0f, 0.22432f,1.0f,
0.11216f,1.0f,  0.0f,1.0f,  0.0f,0.992825f, 0.0f,0.98565f, 0.0f,0.978474f, 0.0f,0.971299f,
0.0431605f,0.967874f, 0.0770629f,0.96259f, 0.102787f,0.954438f, 0.121412f,0.942409f,
0.134019f,0.925495f, 0.141687f,0.902686f, 0.145496f,0.872973f, 0.146526f,0.835347f,
0.146526f,0.751511f, 0.146526f,0.667674f, 0.146526f,0.583837f, 0.146526f,0.5f,
0.146526f,0.416163f, 0.146526f,0.332326f, 0.146526f,0.248489f, 0.146526f,0.164653f};


float Gfont[174*2]={
0.934964f,0.533333f, 0.888768f,0.533333f, 0.842572f,0.533333f, 0.796377f,0.533333f,
 0.703986f,0.533333f,  0.611594f,0.533333f,
 0.611594f,0.526812f, 0.611594f,0.52029f,
0.611594f,0.513768f, 0.611594f,0.507246f,
0.632945f,0.505602f, 0.650611f,0.503872f, 0.665118f,0.501973f, 0.676993f,0.499819f,
0.686761f,0.497325f, 0.69495f,0.494407f, 0.702086f,0.490979f, 0.708696f,0.486957f,
0.716245f,0.481097f, 0.722622f,0.473234f, 0.727879f,0.463264f, 0.732065f,0.451087f,
0.735233f,0.4366f, 0.737432f,0.419701f, 0.738714f,0.400289f, 0.73913f,0.378261f,
0.73913f,0.348913f, 0.73913f,0.319565f, 0.73913f,0.290217f, 0.73913f,0.26087f,
0.73913f,0.231522f, 0.73913f,0.202174f, 0.73913f,0.172826f, 0.73913f,0.143478f,
0.735128f,0.126489f, 0.723686f,0.110553f, 0.705656f,0.0960427f, 0.681884f,0.0833333f,
0.653221f,0.0727978f, 0.620516f,0.0648098f, 0.584618f,0.0597429f, 0.546377f,0.057971f,
0.462175f,0.0655967f, 0.38716f,0.0879982f, 0.321963f,0.124462f, 0.26721f,0.174275f,
0.223531f,0.236725f, 0.191553f,0.311096f, 0.171906f,0.396677f, 0.165217f,0.492754f,
0.167085f,0.543526f, 0.172554f,0.593229f, 0.18142f,0.641199f, 0.193478f,0.686775f,
0.208526f,0.729294f, 0.226359f,0.768093f, 0.246773f,0.802511f, 0.269565f,0.831884f,
0.294667f,0.857006f, 0.322011f,0.879053f, 0.351393f,0.897942f, 0.382609f,0.913587f,
0.415455f,0.925903f, 0.449728f,0.934805f, 0.485224f,0.940209f, 0.521739f,0.942029f,
0.55169f,0.940744f, 0.580639f,0.936911f, 0.608534f,0.930565f, 0.635326f,0.921739f,
0.660963f,0.910468f, 0.685394f,0.896784f, 0.708568f,0.880723f, 0.730435f,0.862319f,
0.745853f,0.84707f, 0.75976f,0.831363f, 0.772546f,0.814603f, 0.784601f,0.796196f,
0.796317f,0.775546f, 0.808084f,0.752061f, 0.820293f,0.725144f, 0.833333f,0.694203f,
 0.841667f,0.694203f,  0.85f,0.694203f,
 0.858333f,0.694203f,  0.866667f,0.694203f,
0.865217f,0.732428f, 0.863768f,0.770652f, 0.862319f,0.808877f, 0.86087f,0.847101f,
0.85942f,0.885326f, 0.857971f,0.923551f, 0.856522f,0.961775f, 0.855072f,1.0f,
 0.847101f,1.0f,  0.83913f,1.0f,
 0.831159f,1.0f,  0.823188f,1.0f,
0.819138f,0.990039f, 0.813609f,0.98091f, 0.806757f,0.972767f, 0.798732f,0.965761f,
0.789688f,0.960046f, 0.779778f,0.955774f, 0.769155f,0.953099f, 0.757971f,0.952174f,
0.75214f,0.95238f, 0.745562f,0.953012f, 0.738304f,0.954085f, 0.730435f,0.955616f,
0.722022f,0.957623f, 0.713134f,0.960122f, 0.703838f,0.963131f, 0.694203f,0.966667f,
0.669684f,0.97434f, 0.645063f,0.981046f, 0.620375f,0.986767f, 0.595652f,0.991485f,
0.570929f,0.995185f, 0.546241f,0.997849f, 0.52162f,0.999459f, 0.497101f,1.0f,
0.392522f,0.990268f, 0.297147f,0.962092f, 0.212438f,0.917001f, 0.139855f,0.856522f,
0.0808595f,0.782184f, 0.0369112f,0.695516f, 0.00947117f,0.598047f, 0.0f,0.491304f,
0.00183978f,0.439586f, 0.00738215f,0.390874f, 0.016661f,0.345049f, 0.0297101f,0.301993f,
0.0465636f,0.261586f, 0.0672554f,0.223709f, 0.0918195f,0.188244f, 0.12029f,0.155072f,
0.15863f,0.119698f, 0.200657f,0.0886549f, 0.246114f,0.0620613f, 0.294746f,0.0400362f,
0.3463f,0.0226987f, 0.400521f,0.0101675f, 0.457153f,0.00256171f, 0.515942f,0.0f,
0.56498f,0.00227868f, 0.61721f,0.00871821f, 0.670527f,0.0187244f, 0.722826f,0.0317028f,
0.771999f,0.047059f, 0.815942f,0.0641983f, 0.852548f,0.0825265f, 0.87971f,0.101449f,
0.87971f,0.138225f, 0.87971f,0.175f, 0.87971f,0.211775f, 0.87971f,0.248551f,
0.87971f,0.285326f, 0.87971f,0.322101f, 0.87971f,0.358877f, 0.87971f,0.395652f,
0.880621f,0.4247f, 0.883741f,0.448143f, 0.889646f,0.466627f, 0.898913f,0.480797f,
0.912121f,0.491299f, 0.929846f,0.498777f, 0.952666f,0.503878f, 0.981159f,0.507246f,
 0.981159f,0.513768f, 0.981159f,0.52029f,
 0.981159f,0.526812f, 0.981159f,0.533333f};

/**
 * Struct that contains some informations about the types of the manipulated objects
 * Mainly here to be used by the algorithms that are parameterized by it
 */
struct PFP: public PFP_STANDARD
{
	// definition of the type of the map
	typedef EmbeddedMap2 MAP;
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
