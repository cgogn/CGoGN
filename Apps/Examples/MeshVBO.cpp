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
* Web site: https://iggservis.u-strasbg.fr/CGoGN/                              *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include <iostream>
#include "Utils/os_spec.h"

#include "GLSLShader.h"
#include "glutwin.h"

#include "Algo/Render/vbo_MapRender.h"
#include "Algo/Render/vbo_pointsCB.hpp"

#include "Algo/Render/topo_vboRender.h"

#include "Topology/map/emap2.h"

#include "Algo/Render/map_glRender.h"

#include "point4d.h"
#include "Decimation/simplifMesh.h"

#include <gmtl/Output.h>

using namespace CGoGN;

//#define RENDERKIND VBORender::VBO_TRIANGLE_STRIPS
#define RENDERKIND Algo::Render::VBO::VBO_TRIANGLES
//#define RENDERKIND VBORender::VBO_EXPLODED

struct PFP {
	typedef DartObj<DP::MAPD2_V0U1> DART;
	typedef e0map2<DART> MAP;
	typedef Emb::Point4D EMB;
	static const unsigned id1=1;
};


PFP::MAP myMap;

typedef Algo::Render::VBO::mapRender<PFP> VBORenderType;


int getFileType(char* filename)
{
	std::string fstr(filename);
	if (fstr.rfind(".trianbgz")!=std::string::npos)
		return 6;
	if (fstr.rfind(".trianb")!=std::string::npos)
		return 5;
	if (fstr.rfind(".trian")!=std::string::npos)
		return 1;
	if (fstr.rfind(".ply")!=std::string::npos)
		return 2;
	if (fstr.rfind(".off")!=std::string::npos)
		return 3;
	if (fstr.rfind(".obj")!=std::string::npos)
		return 4;

	return -1;
}


class myGlutWin: public Utils::SimpleGlutWin
{
public:
	gmtl::Vec4f colDif;

	gmtl::Vec4f colSpec;

	gmtl::Vec4f colClear;

	gmtl::Vec4f colNormal;

	float shininess;

	Algo::Decimation::SimplifTrian<PFP, Algo::Decimation::EdgeCrit_LengthMiddle<PFP> >* m_simplif;

	/**
	 * position of object
	 */
	gmtl::Vec3f gPosObj;

	/**
	 * width of object
	 */
	float gWidthObj;

	/**
	 * factor to apply to normal drawing
	 */
	float normalScaleFactor;

	/**
	 * mesh normals
	 */
	std::vector<gmtl::Vec3f> vnormals;

	/**
	 * redraw CB
	 */
	void myRedraw();

	/**
	 * keyboard CB
	 */
	void myKeyboard(unsigned char keycode, int x, int y);

	/**
	 * Display list init
	 */
	void initDL();


	/**
	 * Display list init only for normal drawing
	 */
	void initDLNormals(void);


	/**
	 * Display list init only for lines drawing
	 */
	void initDLLines(void);

	/**
	 * inverse the normal when computing normal
	 */
	bool invertedNormals;

	/**
	 * inverse object for culling
	 */
	bool invertedObject;

	/**
	 * rendering normals ?
	 */
	bool renderNormal;

	/**
	 * rendering lines ?
	 */
	bool renderLines;

	/**
	 * style of rendering
	 */
	int renderStyle;

	/**
	 * Display List for object
	 */
	GLuint dl_obj;

	VBORenderType *vbor;

//	Algo::Render::VBO::dataRender* vbodr;

	Algo::Render::VBO::topo_MD2_VBORender<PFP>* vbor_topo;

	/**
	 * Display List for object (second)
	 */
	GLuint dl_obj2;

	/**
	 * Display List for normal
	 */
	GLuint dl_norm;


	/**
	 * render mode enum
	 */
	enum { CLEAR=1, LINE, FLAT, GOURAUD, PHONG, NORMAL };

	myGlutWin(	int* argc, char **argv, int winX, int winY) :
				SimpleGlutWin(argc,argv,winX,winY),
				m_simplif(NULL),
				invertedNormals(true),
				invertedObject(false),
				renderNormal(false),
				renderLines(false),
				renderStyle(PHONG),
				dl_obj(-1),
				vbor(NULL),
				vbor_topo(NULL),
				dl_obj2(-1),
				dl_norm(-1) {
		if (this->shaderOk) shaders[0].loadShaders("phong_vs.txt","phong_ps.txt");
	}
};

void myGlutWin::initDL(void)
{
	GLint t1 = glutGet(GLUT_ELAPSED_TIME);

	if (vbor != NULL)
		delete vbor;

	//std::vector<gmtl::Vec3f> points;

	//for (float f=80.0f;f<300.0f; f+=1.0f)
	//{
	//	points.push_back(gmtl::Vec3f(f,f,f));
	//	points.push_back(gmtl::Vec3f(f-1.0,f-2.0,f-7.0f));
	//}

	//vbodr = new VBORender::datavboRender();
	//vbodr->createPoints(points);


	SelectorTrue<PFP::MAP::Dart> allDarts;
	Algo::Render::VBO::VBO_Points_CB<PFP>* my_vbo_cb = new Algo::Render::VBO::VBO_Points_CB<PFP>();

	vbor = new VBORenderType(myMap,allDarts, my_vbo_cb);
 	vbor->initData(0,invertedNormals);
 	vbor->initPrimitives<PFP>(myMap, good, Algo::Render::VBO::VBO_TRIANGLES);
	vbor->initPrimitives<PFP>(myMap, good, Algo::Render::VBO::VBO_LINES);


//	vbor->initEmb_Flat_Exploded(myMap.getNbDarts() , true, invertedNormals,0.8f);
	GLint t2 = glutGet(GLUT_ELAPSED_TIME);
	GLfloat seconds = (t2 - t1) / 1000.0f;
	std::cout << "VBOistisation: "<< seconds << "sec" << std::endl;


	if (vbor_topo!= NULL)
		delete vbor_topo;
	vbor_topo = new Algo::Render::VBO::topo_MD2_VBORender<PFP>(myMap,allDarts);
	vbor_topo->initEmb(0.9,0.9,true);

	GLint t3 = glutGet(GLUT_ELAPSED_TIME);
	seconds = (t3 - t2) / 1000.0f;
	std::cout << "VBOistisation topo: " << seconds << "sec" << std::endl;
}

void myGlutWin::initDLNormals(void)
{
//	vbor->initNormals(normalScaleFactor);
}

void myGlutWin::initDLLines(void)
{
	//if (vbor == NULL)
	//{
	//	SelectorTrue<PFP::MAP::Dart> allDarts;
	//	vbor = new VBORenderType(myMap,allDarts);
	//	vbor->initEmb(0, true, invertedNormals);
	//}
	//vbor->initPrimitives(VBORender::VBO_LINES);
	//std::cout << "VBO_OPT_LINES"<<std::endl;

}


void myGlutWin::myRedraw(void)
{
// 	GLfloat black[4]= {0.0f,0.0f,0.0f,1.0f};
// 	GLfloat amb[4]= {0.2f,0.1f,0.1f,1.0f};

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	float sc = 50./gWidthObj;
	glScalef(sc,sc,sc);
	glTranslatef(-gPosObj[0],-gPosObj[1],-gPosObj[2]);


	if (renderLines)
	{
		glDisable(GL_LIGHTING);
		glColor3f(0.0f,0.0f,0.0f);
		vbor->draw(Algo::Render::VBO::VBO_LINES);

		glEnable( GL_POLYGON_OFFSET_FILL );
		glPolygonOffset( 1.0f, 1.0f );
	}

// 	if (vbor_topo!= NULL)
// 	{
// 		glColor3f(1.0f,1.0f,1.0f);
// 		glPointSize(4.0f);
// 		glDisable(GL_LIGHTING);
// 		vbor_topo->drawDarts();
// 		glColor3f(1.0f,0.0f,0.0f);
// 		vbor_topo->drawPhi2();
//  		glColor3f(0.0f,1.0f,1.0f);
//  		vbor_topo->drawPhi1();
//
// 		glPointSize(6.0f);
// 		glColor3f(1.0f,1.0f,0.0f);
// 		vbor_topo->drawVertices();
// 		glEnable(GL_LIGHTING);
// 	}

	glEnable( GL_POLYGON_OFFSET_FILL );
	glPolygonOffset( 1.0f, 1.0f );

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);
	glColor3f(0.7f,0.8f,1.0f);

	glMaterialfv(GL_FRONT,GL_SPECULAR,colSpec.getData());
	glMaterialf( GL_FRONT, GL_SHININESS, shininess );

	glEnable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT,GL_FILL);
	glShadeModel(GL_SMOOTH);

	if (invertedObject)
		glFrontFace(GL_CW);
	else
		glFrontFace(GL_CCW);

	shaders[0].bind();

	vbor->draw(RENDERKIND);

	shaders[0].unbind();

	glDisable( GL_POLYGON_OFFSET_FILL );

	//glEnable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT,GL_FILL);
	//glDisable(GL_LIGHTING);
	//glColor3f(1.0f,0.0f,0.0f);
	//vbor->draw(VBORender::VBO_TRIANGLES);

	if (renderLines)
	{
		glDisable( GL_POLYGON_OFFSET_FILL );
	}

	if (renderNormal)
	{
		glDisable(GL_LIGHTING);
		glColor3f(1.0f,1.0f,0.0f);
//		vbor->drawNormals();
	}


	glPopMatrix();

}

void myGlutWin::myKeyboard(unsigned char keycode, int, int)
{
//	GLint t1,t2;
//	GLfloat seconds;
	switch(keycode)
	{
	case 'v':
		{
		GLint t1 = glutGet(GLUT_ELAPSED_TIME);

		for (int i=0; i<20; ++i)
		{
			myRedraw();
			glutSwapBuffers();
		}

		GLint t2 = glutGet(GLUT_ELAPSED_TIME);
		GLfloat seconds = (t2 - t1) ;
		std::cout << "speed = "<< 20000.0f/seconds << " fps" << std::endl;
		}
		break;

	case 'S':
		normalScaleFactor *= 1.1f;
		initDLNormals();
		glutPostRedisplay();
		break;

	case 's':
		normalScaleFactor /= 1.1f;
		initDLNormals();
		glutPostRedisplay();
		break;

	case 'u':
		if (m_simplif ==NULL)
		{
			std::cout << "Darts init: "<< myMap.getNbDarts()<< std::endl;
			GLint t1 = glutGet(GLUT_ELAPSED_TIME);
			m_simplif = new Algo::Decimation::SimplifTrian<PFP, Algo::Decimation::EdgeCrit_LengthMiddle<PFP> >(myMap);
			GLint t2 = glutGet(GLUT_ELAPSED_TIME);
			GLfloat seconds = (t2 - t1) / 1000.0f;
			std::cout << "init simplif: "<< seconds<<"sec"<<std::endl;
		}
		{
		GLint t1 = glutGet(GLUT_ELAPSED_TIME);
		m_simplif->simplifUntil( myMap.getNbDarts()/myMap.getDartsPerTriangle()/2);
		GLint t2 = glutGet(GLUT_ELAPSED_TIME);
		GLfloat seconds = (t2 - t1) / 1000.0f;
		std::cout << "simplif: "<< seconds << "sec" << std::endl;
		std::cout << "Darts: "<< myMap.getNbDarts() << std::endl;
		}
		initDL();
		glutPostRedisplay();
		break;


	case 'd':
		setFoc(getFoc()/1.05f);
		reshape(-1,-1);
//		recalcModelView();

		glutPostRedisplay();
		break;
	case 'D':
		setFoc(getFoc()*1.05f);
		reshape(-1,-1);
//		recalcModelView();

		glutPostRedisplay();
		break;

	case 'i':
		invertedNormals = !invertedNormals;
		initDL();
		glutPostRedisplay();
		break;

	case 'o':
		invertedObject = !invertedObject;
		initDL();
		glutPostRedisplay();
		break;

	case 'O':
		invertedObject = !invertedObject;
		invertedNormals = !invertedNormals;
		initDL();
		glutPostRedisplay();
		break;

	case 'n':
		renderNormal = ! renderNormal;
		if (renderNormal)
			initDLNormals();
		glutPostRedisplay();
		break;

	case 'f':
		renderStyle = FLAT;
		initDL();
		glutPostRedisplay();
		break;

	case 'g':
		renderStyle = GOURAUD;
		initDL();
		glutPostRedisplay();
		break;

	case 'p':
		renderStyle = PHONG;
		initDL();
		glutPostRedisplay();
		break;

	case 'b':
		renderStyle = CLEAR;
		initDL();
		glutPostRedisplay();
		break;

	case 'l':
		renderLines = !renderLines;

		//if (renderLines)
		//	initDLLines();
		//else
		//{
		//	delete vborl;
		//	vborl=NULL;
		//	if (glIsList(dl_obj2))
		//		glDeleteLists(dl_obj2,1);
		//}

		glutPostRedisplay();
		break;

	case 'z':
		shininess /= 1.1f;
		glutPostRedisplay();
		break;
	case 'Z':
		shininess *= 1.1f;
		glutPostRedisplay();
		break;

	}
}



int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "syntax: "<< argv[0]<<" filename"<<std::endl;
		return 1;
	}

	char* filename= argv[1];

	Marker mi;
	bool success = myMap.importMesh<PFP::EMB>(filename,PFP::DART::getVertexEmbId(),mi);
	if (!success)
		return 1;

	myMap.unmarkAll(mi);
	myMap.releaseMarker(mi);

	std::cout<<  "objet importe"<<std::endl;
	myMap.check();

	gmtl::Vec3f gMax;
	gmtl::Vec3f gMin;

	Algo::Render::computeBoundingBox<PFP>(myMap,gMin,gMax);

	// compute width and position of object for centering


	float tailleX = gMax[0] - gMin[0];
	float tailleY = gMax[1] - gMin[1];
	float tailleZ = gMax[2] - gMin[2];

	myGlutWin mgw(&argc,argv,800,800);

	mgw.gPosObj = (gMax+gMin)/2.0f;

	mgw.gWidthObj = std::max<float>( std::max<float>(tailleX,tailleY),tailleZ);
	mgw.normalScaleFactor = std::min<float>( std::min<float>(tailleX,tailleY),tailleZ) / 50.0f;

	//mgw.gWidthObj = max( max(tailleX,tailleY),tailleZ);
	//mgw.normalScaleFactor = min( min(tailleX,tailleY),tailleZ) / 50.0f;

	mgw.colClear = gmtl::Vec4f(0.2f,0.2f,0.2f,0.1);
	mgw.colDif = gmtl::Vec4f(0.8f,0.9f,0.7f,1.0f);
	mgw.colSpec = gmtl::Vec4f(0.9f,0.9f,0.9f,1.0f);
	mgw.colNormal = gmtl::Vec4f(1.0f,0.0f,0.0f,1.0f);
	mgw.shininess=80.0f;

	mgw.initDL();

	mgw.mainLoop();

	return 0;
}



