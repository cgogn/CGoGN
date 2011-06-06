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

#include "Utils/GLSLShader.h"
#include "Utils/glutwin.h"

#include "Topology/generic/parameters.h"
#include "Topology/gmap/gmap2.h"
#include "Topology/generic/embeddedMap2.h"

#include "Geometry/matrix.h"
#include "Geometry/vector_gen.h"
#include "Algo/Import/import.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Render/map_glRender.h"
#include "Algo/Render/topo_vboRender.h"

using namespace CGoGN;


// definition des parametres de la carte
struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef EmbeddedMap2<GMap2> MAP;
};

PFP::MAP myMap;

// interface
class myGlutWin: public Utils::SimpleGlutWin
{
public:
        Geom::Vec4f colDif;

        Geom::Vec4f colSpec;

        Geom::Vec4f colClear;

        Geom::Vec4f colNormal;

        float shininess;

    	PFP::TVEC3 position ;
    	PFP::TVEC3 normal ;

        /**
         * position of object
         */
        PFP::VEC3 gPosObj;

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
        std::vector<PFP::VEC3> vnormals;

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
         * GL (old school) rendering function (called by init DL)
         */
        void render(int renderMode);

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
        * aide affichee
        */
        bool aff_help;

        /**
         * style of rendering
         */
        int renderStyle;

        /**
         * Display List for object
         */
        GLuint dl_obj;

        /**
         * Display List for object (second)
         */
        GLuint dl_obj2;

        /**
         * Display List for normal
         */
        GLuint dl_norm;

        /**
         * render topology
         */
        Algo::Render::GL2::topo_VBORenderGMap* m_render_topo;

        /**
         * render mode enum
         */
        enum { CLEAR=1, LINE, FLAT, GOURAUD, PHONG, NORMAL,NONE };

	myGlutWin(	int* argc, char **argv, int winX, int winY) :
		SimpleGlutWin(argc,argv,winX,winY),
		invertedNormals(true),
		invertedObject(true),
		renderNormal(false),
		renderLines(true),
		aff_help(false),
		renderStyle(FLAT),
		dl_obj(-1),
		dl_obj2(-1),
		dl_norm(-1)
    {
		if (this->shaderOk) shaders[0].loadShaders("phong_vs.txt","phong_ps.txt");
		m_render_topo = new Algo::Render::GL2::topo_VBORenderGMap();
	}
};

void myGlutWin::render(int renderMode)
{
	switch (renderMode)
	{
	case FLAT:
			glEnable(GL_LIGHTING);
			glDisable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT,GL_FILL);
			glShadeModel(GL_FLAT);
			Algo::Geometry::computeNormalVertices<PFP>(myMap, position, normal) ;

			if (invertedObject)
					glFrontFace(GL_CW);
			else
					glFrontFace(GL_CCW);

			Algo::Render::Direct::renderTriQuadPoly<PFP>(myMap, Algo::Render::Direct::FLAT, 1.0f, position, normal);
			break;

	case LINE:
			glDisable(GL_LIGHTING);
			glEnable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT,GL_LINE);

			if (invertedObject)
					glFrontFace(GL_CW);
			else
					glFrontFace(GL_CCW);

			if (renderStyle!=CLEAR)
					glColor3f(1.0f,1.0f,0.0f);
			else
					glColor4fv(colDif.data());

			Algo::Render::Direct::renderTriQuadPoly<PFP>(myMap, Algo::Render::Direct::LINE, 1.0f, position, normal);

			break;

	case NONE:
			break;

	default:
			break;
	}
}

void myGlutWin::initDL(void)
{
	if (glIsList(dl_obj))
			glDeleteLists(dl_obj,1);

	dl_obj = glGenLists(1);

	glNewList(dl_obj,GL_COMPILE);

	if (renderStyle==FLAT)
			render(FLAT);

	glEndList();
}

void myGlutWin::initDLLines(void)
{
	std::vector<PFP::VEC3> normals;

	if (glIsList(dl_obj2))
			glDeleteLists(dl_obj2,1);

	dl_obj2 = glGenLists(1);

	glNewList(dl_obj2,GL_COMPILE);

	render(LINE);

	glEndList();
}

void myGlutWin::myRedraw(void)
{
	GLfloat black[4]= {0.0f,0.0f,0.0f,1.0f};
	GLfloat amb[4]= {0.2f,0.1f,0.1f,1.0f};

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	float sc = 50./gWidthObj;
	glScalef(sc,sc,sc);
	glTranslatef(-gPosObj[0],-gPosObj[1],-gPosObj[2]);

	if (glIsList(dl_obj2))
			glCallList(dl_obj2);

	m_render_topo->drawTopo();

	glEnable( GL_POLYGON_OFFSET_FILL );
	glPolygonOffset( 1.0f, 1.0f );

	if (glIsList(dl_obj))
	{
			glMaterialfv(GL_FRONT, GL_DIFFUSE, colDif.data());
			glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
			glMaterialfv(GL_FRONT,GL_SPECULAR,black);
			glMaterialf( GL_FRONT, GL_SHININESS, 1.0f );
			glCallList(dl_obj);
	}

	glDisable( GL_POLYGON_OFFSET_FILL );


	glPopMatrix();
}

void myGlutWin::myKeyboard(unsigned char keycode, int, int)
{
//	GLint t1,t2;
//	GLfloat seconds;
	switch (keycode) {

	case 'd':
			setFoc(getFoc()/1.05f);
			reshape(-1,-1);

			glutPostRedisplay();
			break;
	case 'D':
			setFoc(getFoc()*1.05f);
			reshape(-1,-1);

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

	case 'f':
			renderStyle = FLAT;
			initDL();
			glutPostRedisplay();
			break;


	case 'l':
			renderLines = !renderLines;

			if (renderLines)
					initDLLines();
			else {
					if (glIsList(dl_obj2))
							glDeleteLists(dl_obj2,1);
			}
			initDL();
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
	myGlutWin mgw(&argc,argv,800,800);

	char* filename = argv[1] ;

	PFP::VEC3 gMax;
	PFP::VEC3 gMin;

	if(argc < 2)
	{
		mgw.position = myMap.addAttribute<PFP::VEC3>(VERTEX, "position") ;

		Dart d = myMap.newFace(4);
		Dart e = myMap.newFace(3);

		myMap.sewFaces(d, e);

		// plonge les 4 sommets du carré
		mgw.position[d] = PFP::VEC3(-1,1,0);

		d = myMap.phi1(d);
		mgw.position[d] = PFP::VEC3(1,1,0);

		d = myMap.phi1(d);
		mgw.position[d] = PFP::VEC3(1,-1,0);

		d = myMap.phi1(d);
		mgw.position[d] = PFP::VEC3(-1,-1,0);

		// plonge le dernier sommet du triangle
		d = myMap.phi_1(e);
		mgw.position[d] = PFP::VEC3(0,2,0);
	}
	else
	{
		std::vector<std::string> attrNames ;
		if(!Algo::Import::importMesh<PFP>(myMap, filename, attrNames))
		{
			CGoGNerr <<"Import fail"<< CGoGNendl;
			exit(1);
		}
		mgw.position = myMap.getAttribute<PFP::VEC3>(VERTEX, attrNames[0]) ;
	}

	mgw.normal = myMap.addAttribute<PFP::VEC3>(VERTEX, "normal") ;

	// pour le rendu
	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, mgw.position) ;

	mgw.gPosObj = bb.center() ;
	float tailleX = bb.size(0) ;
	float tailleY = bb.size(1) ;
	float tailleZ = bb.size(2) ;

	mgw.gWidthObj = std::max<float>(std::max<float>(tailleX, tailleY), tailleZ) ;
	mgw.normalScaleFactor = std::min<float>(std::min<float>(tailleX,tailleY),tailleZ) / 50.0f ;

	mgw.colClear = Geom::Vec4f(0.2f,0.2f,0.2f,0.1);
	mgw.colDif = Geom::Vec4f(0.8f,0.9f,0.7f,1.0f);
	mgw.colSpec = Geom::Vec4f(0.9f,0.9f,0.9f,1.0f);
	mgw.colNormal = Geom::Vec4f(1.0f,0.0f,0.0f,1.0f);
	mgw.shininess=80.0f;

	mgw.m_render_topo->updateData<PFP>(myMap,mgw.position,0.9f, 0.9f);

	mgw.initDL();
	mgw.initDLLines();

	mgw.mainLoop();

	return 0;
}
