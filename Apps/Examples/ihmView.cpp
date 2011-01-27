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

#include "Utils/glutwin_atb.h"

#include "Topology/map/map2.h"
#include "Topology/generic/embeddedMap2.h"
#include "Algo/ImplicitHierarchicalMesh/ihm.h"
#include "Algo/ImplicitHierarchicalMesh/subdivision.h"

#include "Geometry/matrix.h"
#include "Geometry/vector_gen.h"

#include "Algo/Import/import.h"
#include "Algo/Export/export.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Render/map_glRender.h"
#include "Algo/Render/vbo_MapRender.h"
#include "Topology/generic/ecell.h"
#include "Algo/Render/topo_vboRender.h"

using namespace CGoGN ;

struct PFP
{
	// definition de la carte
	typedef Algo::IHM::ImplicitHierarchicalMap MAP;

	// definition du type de reel utilise
	typedef float REAL;
	typedef Geom::Vector<3,REAL> VEC3;
	typedef Geom::Vector<6,REAL> VEC6;
	typedef Geom::Matrix<3,3,REAL> MATRIX33;
	typedef Geom::Matrix<4,4,REAL> MATRIX44;
	typedef Geom::Matrix<3,6,REAL> MATRIX36;

	typedef Algo::IHM::AttributeHandler_IHM<VEC3> TVEC3;
	typedef Algo::IHM::AttributeHandler_IHM<REAL> TREAL;
	typedef Algo::IHM::AttributeHandler_IHM<MATRIX33> TFRAME;
	typedef Algo::IHM::AttributeHandler_IHM<MATRIX36> TRGBFUNCS;
};

INIT_STATICS_MAP() ;

PFP::MAP myMap ;
SelectorTrue allDarts ;


class MyGlutWin : public Utils::GlutWin_ATB
{
public:
	TwBar* viewer_bar ;

	enum renderMode { FLAT, GOURAUD, PHONG, LINES, NORMALS, TOPO } ;

	Geom::Vec4f colDif ;
	Geom::Vec4f colSpec ;
	Geom::Vec4f colClear ;
	Geom::Vec4f colNormal ;

	float shininess ;

	PFP::VEC3 gPosObj ;
	float gWidthObj ;
	float normalBaseSize ;
	float normalScaleFactor ;
	float faceShrinkage ;

	int renderStyle ;
	bool renderObject ;
	bool renderLines ;
	bool renderPoints ;
	bool renderNormals ;
	bool renderTopo ;

	unsigned int m_nbVertices ;

	PFP::TVEC3 position ;
	PFP::TVEC3 normal ;

	Algo::Render::VBO::MapRender_VBO<PFP>* vbo_render ;

	Algo::Render::VBO::topo_VBORenderMapD* topo_render ;

	GLuint dl_norm ;

	MyGlutWin(int* argc, char **argv, int winX, int winY) ;

	void init() ;

	void initGUI() ;

	void myRedraw() ;

	void myKeyboard(unsigned char keycode, int x, int y) ;

	void updateVBOprimitives(int upType) ;

	void updateVBOdata(int upType, bool recompute = true) ;

	void initDLNormals() ;

	void render(int renderMode) ;
} ;


/**********************************************************************************************
 *                                 AntTweakBar CALLBACKS                                      *
 **********************************************************************************************/

void TW_CALL setRenderStyleCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->renderStyle = *(GLenum*)value ;
	if(mgw->renderStyle == MyGlutWin::FLAT)
		TwDefine("Viewer/FaceShrinkage visible=true") ;
	else
		TwDefine("Viewer/FaceShrinkage visible=false") ;
	glutPostRedisplay() ;
}
void TW_CALL getRenderStyleCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(GLenum*)value = mgw->renderStyle ;
}

void TW_CALL setShininessCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->shininess = *(float*)value ;
	glutPostRedisplay() ;
}
void TW_CALL getShininessCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(float*)value = mgw->shininess ;
}

void TW_CALL setFocalCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->setFocal(*(float*)value) ;
	mgw->reshape(-1, -1) ;
	glutPostRedisplay() ;
}
void TW_CALL getFocalCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(float*)value = mgw->getFocal() ;
}

void TW_CALL setRenderObjectCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->renderObject = *(GLboolean*)value ;
	glutPostRedisplay() ;
}
void TW_CALL getRenderObjectCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(GLboolean*)value = mgw->renderObject ;
}

void TW_CALL setRenderLinesCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->renderLines = *(GLboolean*)value ;
	glutPostRedisplay() ;
}
void TW_CALL getRenderLinesCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(GLboolean*)value = mgw->renderLines ;
}

void TW_CALL setRenderPointsCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->renderPoints = *(GLboolean*)value ;
	glutPostRedisplay() ;
}
void TW_CALL getRenderPointsCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(GLboolean*)value = mgw->renderPoints ;
}

void TW_CALL setRenderNormalsCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->renderNormals = *(GLboolean*)value ;
	if(mgw->renderNormals)
	{
		mgw->initDLNormals() ;
		TwDefine("Viewer/NormalScaleFactor visible=true") ;
	}
	else
		TwDefine("Viewer/NormalScaleFactor visible=false") ;
	glutPostRedisplay() ;
}
void TW_CALL getRenderNormalsCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(GLboolean*)value = mgw->renderNormals ;
}

void TW_CALL setRenderTopoCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->renderTopo = *(GLboolean*)value ;
	glutPostRedisplay()  ;
}
void TW_CALL getRenderTopoCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(GLboolean*)value = mgw->renderTopo ;
}

void TW_CALL setNormalScaleFactorCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->normalScaleFactor = *(float*)value ;
	if(mgw->renderNormals) mgw->initDLNormals() ;
	glutPostRedisplay() ;
}
void TW_CALL getNormalScaleFactorCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(float*)value = mgw->normalScaleFactor ;
}

void TW_CALL setFaceShrinkageCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->faceShrinkage = *(float*)value ;
	glutPostRedisplay() ;
}
void TW_CALL getFaceShrinkageCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(float*)value = mgw->faceShrinkage ;
}


/**********************************************************************************************
 *                                 MyGlutWin IMPLEMENTATION                                   *
 **********************************************************************************************/


MyGlutWin::MyGlutWin(int* argc, char **argv, int winX, int winY) :
	GlutWin_ATB(argc, argv, winX, winY),
	renderStyle(FLAT),
	renderObject(true),
	renderLines(false),
	renderPoints(false),
	renderNormals(false),
	renderTopo(false),
	vbo_render(NULL),
	topo_render(NULL)
{
	normalScaleFactor = 1.0f ;
	faceShrinkage = 0.0f ;

	colClear = Geom::Vec4f(0.2f, 0.2f, 0.2f, 0.1f) ;
	colDif = Geom::Vec4f(0.8f, 0.9f, 0.7f, 1.0f) ;
	colSpec = Geom::Vec4f(0.9f, 0.9f, 0.9f, 1.0f) ;
	colNormal = Geom::Vec4f(1.0f, 0.0f, 0.0f, 1.0f) ;
	shininess = 80.0f ;

	setFocal(5.0f) ;

	dl_norm = glGenLists(1) ;
	if(this->shaderOk)
	{
		shaders[0].loadShaders("phong_vs.txt", "phong_ps.txt") ;
		shaders[1].loadShaders("flat_vs.txt", "flat_ps.txt", "flat_gs.txt") ;
	}
}

void MyGlutWin::init()
{
	vbo_render = new Algo::Render::VBO::MapRender_VBO<PFP>(myMap, allDarts) ;
	updateVBOprimitives(Algo::Render::VBO::TRIANGLES | Algo::Render::VBO::LINES | Algo::Render::VBO::POINTS) ;
	updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS) ;

	topo_render = new Algo::Render::VBO::topo_VBORenderMapD() ;
	topo_render->updateData<PFP>(myMap, position, 0.9f, 0.9f) ;
}

void MyGlutWin::initGUI()
{
	viewer_bar = TwNewBar("Viewer") ;
	TwDefine("Viewer position='16 16' size='220 450' valueswidth='85'") ;

	TwType tw_render_style = TwDefineEnum("Shading","Flat,Gouraud,Phong") ;
	TwAddVarCB(viewer_bar, "RenderStyle", tw_render_style, setRenderStyleCB, getRenderStyleCB, this, "") ;
	TwAddVarCB(viewer_bar, "Shininess", TW_TYPE_FLOAT, setShininessCB, getShininessCB, this, "min=1 max=255 step=1") ;
	TwAddVarCB(viewer_bar, "Focal", TW_TYPE_FLOAT, setFocalCB, getFocalCB, this, "min=0.6 max=5.0 step=0.1") ;
	TwAddVarCB(viewer_bar, "Show Object", TW_TYPE_BOOL8, setRenderObjectCB, getRenderObjectCB, this, "") ;
	TwAddVarCB(viewer_bar, "Show Lines", TW_TYPE_BOOL8, setRenderLinesCB, getRenderLinesCB, this, "") ;
	TwAddVarCB(viewer_bar, "Show Points", TW_TYPE_BOOL8, setRenderPointsCB, getRenderPointsCB, this, "") ;
	TwAddVarCB(viewer_bar, "Show Normals", TW_TYPE_BOOL8, setRenderNormalsCB, getRenderNormalsCB, this, "") ;
	TwAddVarCB(viewer_bar, "Show Topo", TW_TYPE_BOOL8, setRenderTopoCB, getRenderTopoCB, this, "") ;
	TwAddVarCB(viewer_bar, "NormalScaleFactor", TW_TYPE_FLOAT, setNormalScaleFactorCB, getNormalScaleFactorCB, this, "min=0.1 max=5.0 step=0.1") ;
	TwDefine("Viewer/NormalScaleFactor visible=false") ;
	TwAddVarCB(viewer_bar, "FaceShrinkage", TW_TYPE_FLOAT, setFaceShrinkageCB, getFaceShrinkageCB, this, "min=0.0 max=0.9 step=0.01") ;
}

void MyGlutWin::myRedraw()
{
	GLfloat black[4] = { 0.0f, 0.0f, 0.0f, 1.0f } ;
	GLfloat amb[4] = { 0.2f, 0.1f, 0.1f, 1.0f } ;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;

	glPushMatrix() ;

	float sc = 50. / gWidthObj ;
	glScalef(sc, sc, sc) ;
	glTranslatef(-gPosObj[0], -gPosObj[1], -gPosObj[2]) ;

	if (renderTopo)
	{
		glDisable(GL_LIGHTING);
		topo_render->drawTopo();
	}

	if(renderLines)
	{
		glDisable(GL_LIGHTING);
		glColor3f(0.0f,0.0f,0.0f);
		vbo_render->draw(Algo::Render::VBO::LINES) ;
	}

	if(renderPoints)
	{
		glPointSize(2.0f) ;
		glDisable(GL_LIGHTING);
		glColor3f(0.3f,0.3f,0.8f);
		vbo_render->draw(Algo::Render::VBO::POINTS) ;
	}

	if(renderNormals)
		glCallList(dl_norm) ;

	glEnable(GL_POLYGON_OFFSET_FILL) ;
	glPolygonOffset(1.0f, 1.0f) ;

	if(renderObject)
	{
		glMaterialfv(GL_FRONT, GL_DIFFUSE, colDif.data()) ;
		glMaterialfv(GL_FRONT, GL_AMBIENT, amb) ;
		if(renderStyle != PHONG)
		{
			glMaterialfv(GL_FRONT, GL_SPECULAR, black) ;
			glMaterialf(GL_FRONT, GL_SHININESS, 1.0f) ;
		}
		else
		{
			glMaterialfv(GL_FRONT, GL_SPECULAR, colSpec.data()) ;
			glMaterialf(GL_FRONT, GL_SHININESS, shininess) ;
		}

		glEnable(GL_LIGHTING) ;
		glEnable(GL_CULL_FACE) ;
		glPolygonMode(GL_FRONT, GL_FILL) ;
		if(renderStyle == FLAT)
		{
			glShadeModel(GL_FLAT) ;
			shaders[1].bind() ;
			glUniform1fARB(glGetUniformLocationARB(shaders[1].program_handler(),"explode"), faceShrinkage) ;
			vbo_render->draw(Algo::Render::VBO::TRIANGLES) ;
			shaders[1].unbind() ;
		}
		else
		{
			glShadeModel(GL_SMOOTH) ;
			if(renderStyle == PHONG)
				shaders[0].bind() ;
			vbo_render->draw(Algo::Render::VBO::TRIANGLES) ;
			if(renderStyle == PHONG)
				shaders[0].unbind() ;
		}
	}

	glDisable(GL_POLYGON_OFFSET_FILL) ;

	glPopMatrix() ;
}

void MyGlutWin::myKeyboard(unsigned char keycode, int x, int y)
{
	switch(keycode)
	{
		case 'q':
		{
			for(Dart d = myMap.begin(); d != myMap.end(); myMap.next(d))
			{
				Dart dd = myMap.phi2(d) ;
				if(myMap.faceDegree(d) == 3 && myMap.faceDegree(dd) == 3)
				{
					if(Algo::Geometry::faceNormal<PFP>(myMap, d, position) * Algo::Geometry::faceNormal<PFP>(myMap, dd, position) > 0.99)
						myMap.mergeFaces(d) ;
				}
			}

			updateVBOprimitives(Algo::Render::VBO::TRIANGLES | Algo::Render::VBO::LINES | Algo::Render::VBO::POINTS) ;
			updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS, true) ;
			topo_render->updateData<PFP>(myMap, position, 0.9f, 0.9f) ;

			glutPostRedisplay() ;
			break ;
		}

		case 's':
		{
			srand(time(NULL)) ;

			unsigned int max = myMap.getMaxLevel() ;
			myMap.setCurrentLevel(max) ;

			unsigned int nbF = myMap.getNbOrbits(FACE_ORBIT) ;
			unsigned int nbDarts = myMap.getNbDarts() ;

			for(unsigned int i = 0; i < nbF / 2; ++i)
			{
				unsigned int r = rand() % nbDarts ;
				Dart d(r) ;
				if(myMap.isDartValid(d) && myMap.getDartLevel(d) <= max && !myMap.faceIsSubdivided(d))
					Algo::IHM::subdivideFace<PFP>(myMap, d, position) ;
			}

			myMap.setCurrentLevel(myMap.getMaxLevel()) ;

			updateVBOprimitives(Algo::Render::VBO::TRIANGLES | Algo::Render::VBO::LINES | Algo::Render::VBO::POINTS) ;
			updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS) ;
			topo_render->updateData<PFP>(myMap, position, 0.9f, 0.9f) ;

			glutPostRedisplay() ;
			break ;
		}

		case '0':
		{
			unsigned int cur = myMap.getCurrentLevel() ;
			if(cur > 0)
			{
				myMap.setCurrentLevel(0) ;

				updateVBOprimitives(Algo::Render::VBO::TRIANGLES | Algo::Render::VBO::LINES | Algo::Render::VBO::POINTS) ;
				updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS) ;
				topo_render->updateData<PFP>(myMap, position, 0.9f, 0.9f) ;

				glutPostRedisplay() ;
			}
			break ;
		}

		case '+':
		{
			unsigned int cur = myMap.getCurrentLevel() ;
			if(cur < myMap.getMaxLevel())
			{
				myMap.setCurrentLevel(cur + 1) ;

				updateVBOprimitives(Algo::Render::VBO::TRIANGLES | Algo::Render::VBO::LINES | Algo::Render::VBO::POINTS) ;
				updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS) ;
				topo_render->updateData<PFP>(myMap, position, 0.9f, 0.9f) ;

				glutPostRedisplay() ;
			}
			break ;
		}

		case '-':
		{
			unsigned int cur = myMap.getCurrentLevel() ;
			if(cur > 0)
			{
				myMap.setCurrentLevel(cur - 1) ;

				updateVBOprimitives(Algo::Render::VBO::TRIANGLES | Algo::Render::VBO::LINES | Algo::Render::VBO::POINTS) ;
				updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS) ;
				topo_render->updateData<PFP>(myMap, position, 0.9f, 0.9f) ;

				glutPostRedisplay() ;
			}
			break ;
		}
	}
}

void MyGlutWin::updateVBOprimitives(int upType)
{
	if(upType & Algo::Render::VBO::TRIANGLES)
	{
		vbo_render->initPrimitives(Algo::Render::VBO::TRIANGLES) ;
	}

	if(upType & Algo::Render::VBO::LINES)
	{
		vbo_render->initPrimitives(Algo::Render::VBO::LINES) ;
	}

	if(upType & Algo::Render::VBO::POINTS)
	{
		vbo_render->initPrimitives(Algo::Render::VBO::POINTS) ;
	}
}

void MyGlutWin::updateVBOdata(int upType, bool recompute)
{
	if(upType & Algo::Render::VBO::POSITIONS)
		vbo_render->updateData(Algo::Render::VBO::POSITIONS, position) ;

	if(upType & Algo::Render::VBO::NORMALS)
	{
		if(recompute)
			Algo::Geometry::computeNormalVertices<PFP>(myMap, position, normal) ;
		vbo_render->updateData(Algo::Render::VBO::NORMALS, normal) ;
		if(renderNormals)
			initDLNormals() ;
	}
}

void MyGlutWin::initDLNormals()
{
	glNewList(dl_norm, GL_COMPILE) ;
	render(NORMALS) ;
	glEndList() ;
}

void MyGlutWin::render(int renderMode)
{
	switch(renderMode)
	{
		case NORMALS : {
			glLineWidth(1.0f) ;
			glDisable(GL_LIGHTING) ;
			glColor3fv(colNormal.data()) ;
			Algo::Render::Direct::renderNormalVertices<PFP>(myMap, position, normal, normalScaleFactor * normalBaseSize) ;
			break ;
		}
	}
}

/**********************************************************************************************
 *                                      MAIN FUNCTION                                         *
 **********************************************************************************************/

int main(int argc, char** argv)
{
	MyGlutWin* mgw = new MyGlutWin(&argc, argv, 1200, 800) ;

	mgw->position = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "position") ;
	mgw->normal = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "normal") ;
	std::cout << "position -> " << mgw->position.isValid() << std::endl ;
	std::cout << "normal -> " << mgw->normal.isValid() << std::endl ;

	char* filename = argv[1] ;

	GLint t1 = glutGet(GLUT_ELAPSED_TIME) ;
	bool success = Algo::Import::importMesh<PFP>(myMap, filename, mgw->position, Algo::Import::ImportSurfacique::UNKNOWNSURFACE) ;
	if(!success)
	{
		std::cerr << "could not import "<< filename << std::endl ;
		return 1 ;
	}
	GLint t2 = glutGet(GLUT_ELAPSED_TIME) ;
	GLfloat seconds = (t2 - t1) / 1000.0f ;
	std::cout << "import: " << seconds << " sec" << std::endl ;

	myMap.init() ;

	// compute width and position of object for centering
	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, mgw->position) ;
	std::cout << bb << std::endl ;

	mgw->gPosObj = bb.center() ;
	float tailleX = bb.size(0) ;
	float tailleY = bb.size(1) ;
	float tailleZ = bb.size(2) ;

	mgw->gWidthObj = std::max<float>(std::max<float>(tailleX, tailleY), tailleZ) ;
	mgw->normalBaseSize = std::min<float>(std::min<float>(tailleX,tailleY),tailleZ) / 50.0f ;

	mgw->init() ;
	mgw->initGUI() ;
	mgw->mainLoop() ;

	return 0 ;
}
