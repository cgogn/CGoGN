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

#define RENDU_COULEUR 1

#include "glutwin.h"

#include "Algo/Import/import.h"
#include "Algo/Import/export.h"
#include "map_dual/mapd2_xpt.h"
#include "Algo/Render/map_glRender.h"

#include "point3d_col.h"

//#include "point6d.h"
//#include "pointBDF.h"


#include <gmtl/Output.h>

gmtl::Vec3f gMax;
gmtl::Vec3f gMin;

struct PFP {
	typedef DartObj<DP::MAPD2_V0U1> DART;
	typedef e0mapd2<DART> MAP;
	typedef Emb::Point3D_Col EMB;
//	typedef Emb::Point6D EMB;		// Pour fichier fred avec couleur, tout en float:  #include "point6d.h"
//	typedef Emb::Point_BRDF EMB;	// Pour fichier fred avec donnee, pos + 9 float + 16 float: #include "pointBDF.h"
	static const unsigned id0=0;
};

PFP::MAP myMap;

class myGlutWin: public SimpleGlutWin
{
public:
	gmtl::Vec3f gPosObj;
	float gWidthObj;
	GLuint dl;
	void myRedraw();
	void myKeyboard(unsigned char keycode, int x, int y);
	void initDL();
	bool useDL;
	std::vector<gmtl::Vec3f> vnormals;
};


void myGlutWin::initDL(void)
{
	if (glIsList(dl))
		glDeleteLists(dl,1);

	dl = glGenLists(1);



	glNewList(dl,GL_COMPILE);

		glEnable(GL_NORMALIZE);
		glFrontFace(GL_CCW);
		glEnable(GL_CULL_FACE);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

		SelectorTrue<PFP::MAP::Dart> allDarts;
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glColor3f(1.,0.5,0.);
		MapGLRender::renderTriQuadPoly<PFP>(myMap, MapGLRender::FLAT, 0.8f, allDarts, vnormals, false);

	glEndList();

	std::cout <<"Using GL display list"<<std::endl;


	useDL=true;
}

void myGlutWin::myRedraw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	float sc = 8./gWidthObj;
	glScalef(sc,sc,sc);
	glTranslatef(-gPosObj[0],-gPosObj[1],-gPosObj[2]);

	if (useDL)
		glCallList(dl);

	glPopMatrix();

}

void myGlutWin::myKeyboard(unsigned char keycode, int x, int y)
{

}


int main(int argc, char **argv)
{
	myGlutWin mgw;
	mgw.init(argc,argv,500,500);


	gmtl::Vec3f gMax;
	gmtl::Vec3f gMin;

	char* filename = argv[1];

	// import
	Import::importPlyV2<PFP>(myMap,filename);
	std::cout<<  "objet importe"<<std::endl;


	GLint t1 = glutGet(GLUT_ELAPSED_TIME);

	// calcule les voisins (phi2)
	Import::computeNeighbourOptimized<PFP>(myMap,gMin,gMax);

	// ferme la carte en cas de surface ouverte
	myMap.setBoundaryMarkerValue(myMap.closeMap());

	GLint t2 = glutGet(GLUT_ELAPSED_TIME);
	GLfloat seconds = (t2 - t1) / 1000.0f;
	cout << "phi2isation et fermeture: "<< seconds<<"sec"<<endl;

	MapGLRender::computeBoundingBox<PFP>(myMap,gMin,gMax);

	mgw.gPosObj[0] = (gMax[0]+gMin[0])/2;
	mgw.gPosObj[1] = (gMax[1]+gMin[1])/2;
	mgw.gPosObj[2] = (gMax[2]+gMin[2])/2;


	float tailleX = gMax[0] - gMin[0];
	float tailleY = gMax[1] - gMin[1];
	float tailleZ = gMax[2] - gMin[2];
	mgw.gWidthObj = std::max( std::max(tailleX,tailleY) ,tailleZ);


	mgw.initDL();

	mgw.mainLoop();

	return 0;
}



