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

#include <iostream>

#include "Utils/glutwin_atb.h"

#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"

#include "Geometry/matrix.h"
#include "Geometry/vector_gen.h"

#include "Algo/Import/import.h"
#include "Algo/Export/export.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Render/map_glRender.h"
#include "Algo/Render/vbo_MapRender.h"
#include "Algo/Render/topo_vboRender.h"

#include "Algo/Geometry/laplacian.h"
#include "Algo/Modelisation/subdivision.h"
#include "Algo/Decimation/decimation.h"
#include "Algo/Remeshing/pliant.h"
#include "Algo/Geometry/feature.h"

using namespace CGoGN ;

struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef EmbeddedMap2 MAP;
};

typedef PFP::MAP MAP;

MAP myMap ;

class MyGlutWin : public Utils::GlutWin_ATB
{
public:
	TwBar* viewer_bar ;

	enum renderMode { FLAT, GOURAUD, PHONG, LINES, NORMALS, LAPLACIANS, TOPO } ;

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
	bool renderTopo;
	bool renderLaplacians ;

	unsigned int m_nbVertices ;

	PFP::TVEC3 position ;
	PFP::TVEC3 normal ;
	PFP::TVEC3 laplacian ;

	Algo::Render::GL2::MapRender_VBO* vbo_render ;
	Algo::Render::GL2::topo_VBORenderMapD* topo_render ;
	GLuint dl_norm, dl_lapl ;

	MyGlutWin(int* argc, char **argv, int winX, int winY) ;

	void init() ;

	void initGUI() ;

	void myRedraw() ;

	void myKeyboard(unsigned char keycode, int x, int y) ;

	void updateVBOprimitives(int upType) ;

	void updateVBOdata(int upType, bool recompute = true) ;

	void initDL(int renderMode) ;
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
		mgw->initDL(MyGlutWin::NORMALS) ;
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

void TW_CALL setRenderLaplaciansCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->renderLaplacians = *(GLboolean*)value ;
	if(mgw->renderLaplacians)
	{
		mgw->initDL(MyGlutWin::LAPLACIANS) ;
		TwDefine("Viewer/NormalScaleFactor visible=true") ;
	}
	else
		TwDefine("Viewer/NormalScaleFactor visible=false") ;
	glutPostRedisplay() ;
}
void TW_CALL getRenderLaplaciansCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(GLboolean*)value = mgw->renderLaplacians ;
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
	if(mgw->renderNormals) mgw->initDL(MyGlutWin::NORMALS) ;
	if(mgw->renderLaplacians) mgw->initDL(MyGlutWin::LAPLACIANS) ;
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
	renderLaplacians(false),
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
	dl_lapl = glGenLists(1) ;
	if(this->shaderOk)
	{
		shaders[0].loadShaders("phong_vs.txt", "phong_ps.txt") ;
		shaders[1].loadShaders("flat_vs.txt", "flat_ps.txt", "flat_gs.txt") ;
	}
}

void MyGlutWin::init()
{
	normal = myMap.addAttribute<PFP::VEC3>(VERTEX, "normal") ;
	laplacian = myMap.addAttribute<PFP::VEC3>(VERTEX, "laplacian") ;

	vbo_render = new Algo::Render::GL2::MapRender_VBO() ;
	updateVBOdata(Algo::Render::GL2::POSITIONS | Algo::Render::GL2::NORMALS) ;
	updateVBOprimitives(Algo::Render::GL2::TRIANGLES | Algo::Render::GL2::LINES | Algo::Render::GL2::POINTS) ;

	topo_render = new Algo::Render::GL2::topo_VBORenderMapD() ;
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
	TwAddVarCB(viewer_bar, "Show Laplacians", TW_TYPE_BOOL8, setRenderLaplaciansCB, getRenderLaplaciansCB, this, "") ;
	TwAddVarCB(viewer_bar, "NormalScaleFactor", TW_TYPE_FLOAT, setNormalScaleFactorCB, getNormalScaleFactorCB, this, "min=0.1 max=5.0 step=0.1") ;
	TwDefine("Viewer/NormalScaleFactor visible=false") ;
	TwAddVarCB(viewer_bar, "Show Topo", TW_TYPE_BOOL8, setRenderTopoCB, getRenderTopoCB, this, "") ;
	TwAddVarCB(viewer_bar, "FaceShrinkage", TW_TYPE_FLOAT, setFaceShrinkageCB, getFaceShrinkageCB, this, "min=0.0 max=0.9 step=0.01") ;
}

void MyGlutWin::myRedraw()
{
	GLfloat black[4] = { 0.0f, 0.0f, 0.0f, 1.0f } ;
	GLfloat amb[4] = { 0.2f, 0.1f, 0.1f, 1.0f } ;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;

	glPushMatrix() ;

	float sc = 50.0f / gWidthObj ;
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
		glLineWidth(1.0f) ;
		glColor3f(0.0f,0.0f,0.0f);
		vbo_render->draw(Algo::Render::GL2::LINES) ;
	}

	if(renderPoints)
	{
		glPointSize(2.0f) ;
		glDisable(GL_LIGHTING);
		glColor3f(0.3f,0.3f,0.8f);
		vbo_render->draw(Algo::Render::GL2::POINTS) ;
	}

	if(renderNormals)
		glCallList(dl_norm) ;

	if(renderLaplacians)
		glCallList(dl_lapl) ;

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
			vbo_render->draw(Algo::Render::GL2::TRIANGLES) ;
			shaders[1].unbind() ;
		}
		else
		{
			glShadeModel(GL_SMOOTH) ;
			if(renderStyle == PHONG)
				shaders[0].bind() ;
			vbo_render->draw(Algo::Render::GL2::TRIANGLES) ;
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
		case 'l':
		{
			GLint t1 = glutGet(GLUT_ELAPSED_TIME) ;

			Algo::Modelisation::LoopSubdivision<PFP>(myMap, position) ;

			GLint t2 = glutGet(GLUT_ELAPSED_TIME) ;
			GLfloat seconds = (t2 - t1) / 1000.0f ;
			CGoGNout << "loop: "<< seconds << "sec" << CGoGNendl ;

			t1 = glutGet(GLUT_ELAPSED_TIME) ;

			updateVBOprimitives(Algo::Render::GL2::TRIANGLES | Algo::Render::GL2::LINES | Algo::Render::GL2::POINTS) ;
			updateVBOdata(Algo::Render::GL2::POSITIONS | Algo::Render::GL2::NORMALS) ;
			topo_render->updateData<PFP>(myMap, position, 0.9f, 0.9f) ;

			t2 = glutGet(GLUT_ELAPSED_TIME) ;
			seconds = (t2 - t1) / 1000.0f ;
			CGoGNout << "display update: "<< seconds << "sec" << CGoGNendl ;

			glutPostRedisplay() ;
			break ;
		}

		case 'c':
		{
			GLint t1 = glutGet(GLUT_ELAPSED_TIME) ;

			Algo::Modelisation::CatmullClarkSubdivision<PFP>(myMap, position) ;

			GLint t2 = glutGet(GLUT_ELAPSED_TIME) ;
			GLfloat seconds = (t2 - t1) / 1000.0f ;
			CGoGNout << "catmull-clark: "<< seconds << "sec" << CGoGNendl ;

			t1 = glutGet(GLUT_ELAPSED_TIME) ;

			updateVBOprimitives(Algo::Render::GL2::TRIANGLES | Algo::Render::GL2::LINES | Algo::Render::GL2::POINTS) ;
			updateVBOdata(Algo::Render::GL2::POSITIONS | Algo::Render::GL2::NORMALS) ;
			topo_render->updateData<PFP>(myMap, position, 0.9f, 0.9f) ;

			t2 = glutGet(GLUT_ELAPSED_TIME) ;
			seconds = (t2 - t1) / 1000.0f ;
			CGoGNout << "display update: "<< seconds << "sec" << CGoGNendl ;

			glutPostRedisplay() ;
			break ;
		}

		case '3':
		{
			GLint t1 = glutGet(GLUT_ELAPSED_TIME) ;

			Algo::Modelisation::Sqrt3Subdivision<PFP>(myMap, position) ;

			GLint t2 = glutGet(GLUT_ELAPSED_TIME) ;
			GLfloat seconds = (t2 - t1) / 1000.0f ;
			CGoGNout << "sqrt3: "<< seconds << "sec" << CGoGNendl ;

			normal = myMap.getAttribute<PFP::VEC3>(VERTEX, "normal") ;
			if(!normal.isValid())
				normal = myMap.addAttribute<PFP::VEC3>(VERTEX, "normal") ;
			laplacian = myMap.getAttribute<PFP::VEC3>(VERTEX, "laplacian") ;
			if(!laplacian.isValid())
				laplacian = myMap.addAttribute<PFP::VEC3>(VERTEX, "laplacian") ;

			t1 = glutGet(GLUT_ELAPSED_TIME) ;

			updateVBOprimitives(Algo::Render::GL2::TRIANGLES | Algo::Render::GL2::LINES | Algo::Render::GL2::POINTS) ;
			updateVBOdata(Algo::Render::GL2::POSITIONS | Algo::Render::GL2::NORMALS) ;
			topo_render->updateData<PFP>(myMap, position, 0.9f, 0.9f) ;

			t2 = glutGet(GLUT_ELAPSED_TIME) ;
			seconds = (t2 - t1) / 1000.0f ;
			CGoGNout << "display update: "<< seconds << "sec" << CGoGNendl ;

			glutPostRedisplay() ;
			break ;
		}

		case 't':
		{
			GLint t1 = glutGet(GLUT_ELAPSED_TIME) ;

			Algo::Modelisation::trianguleFaces<PFP>(myMap, position) ;

			GLint t2 = glutGet(GLUT_ELAPSED_TIME) ;
			GLfloat seconds = (t2 - t1) / 1000.0f ;
			CGoGNout << "triangulation: "<< seconds << "sec" << CGoGNendl ;

			t1 = glutGet(GLUT_ELAPSED_TIME) ;

			updateVBOprimitives(Algo::Render::GL2::TRIANGLES | Algo::Render::GL2::LINES | Algo::Render::GL2::POINTS) ;
			updateVBOdata(Algo::Render::GL2::POSITIONS | Algo::Render::GL2::NORMALS) ;
			topo_render->updateData<PFP>(myMap, position, 0.9f, 0.9f) ;

			t2 = glutGet(GLUT_ELAPSED_TIME) ;
			seconds = (t2 - t1) / 1000.0f ;
			CGoGNout << "display update: "<< seconds << "sec" << CGoGNendl ;

			glutPostRedisplay() ;
			break ;
		}

		case 'q':
		{
			GLint t1 = glutGet(GLUT_ELAPSED_TIME) ;

			Algo::Modelisation::quadranguleFaces<PFP>(myMap, position) ;

			GLint t2 = glutGet(GLUT_ELAPSED_TIME) ;
			GLfloat seconds = (t2 - t1) / 1000.0f ;
			CGoGNout << "quadrangulation: "<< seconds << "sec" << CGoGNendl ;

			t1 = glutGet(GLUT_ELAPSED_TIME) ;

			updateVBOprimitives(Algo::Render::GL2::TRIANGLES | Algo::Render::GL2::LINES | Algo::Render::GL2::POINTS) ;
			updateVBOdata(Algo::Render::GL2::POSITIONS | Algo::Render::GL2::NORMALS) ;
			topo_render->updateData<PFP>(myMap, position, 0.9f, 0.9f) ;

			t2 = glutGet(GLUT_ELAPSED_TIME) ;
			seconds = (t2 - t1) / 1000.0f ;
			CGoGNout << "display update: "<< seconds << "sec" << CGoGNendl ;

			glutPostRedisplay() ;
			break ;
		}

		case 's':
		{
			unsigned int nbVertices = myMap.getNbOrbits(VERTEX) ;

			GLint t1 = glutGet(GLUT_ELAPSED_TIME) ;

			Algo::Decimation::decimate<PFP>(myMap, Algo::Decimation::S_QEM, Algo::Decimation::A_QEM, position, nbVertices * 0.75) ;

			GLint t2 = glutGet(GLUT_ELAPSED_TIME) ;
			GLfloat seconds = (t2 - t1) / 1000.0f ;
			CGoGNout << "decimation: "<< seconds << "sec" << CGoGNendl ;

			t1 = glutGet(GLUT_ELAPSED_TIME) ;

			updateVBOprimitives(Algo::Render::GL2::TRIANGLES | Algo::Render::GL2::LINES | Algo::Render::GL2::POINTS) ;
			updateVBOdata(Algo::Render::GL2::POSITIONS | Algo::Render::GL2::NORMALS) ;
			topo_render->updateData<PFP>(myMap, position, 0.9f, 0.9f) ;

			t2 = glutGet(GLUT_ELAPSED_TIME) ;
			seconds = (t2 - t1) / 1000.0f ;
			CGoGNout << "display update: "<< seconds << "sec" << CGoGNendl ;

			glutPostRedisplay() ;
			break ;
		}

		case 'd':
		{
			AttributeHandler<PFP::VEC3> positionF = myMap.getAttribute<PFP::VEC3>(FACE, "position") ;
			if(!positionF.isValid())
				positionF = myMap.addAttribute<PFP::VEC3>(FACE, "position") ;
			Algo::Geometry::computeCentroidFaces<PFP>(myMap, position, positionF) ;

			GLint t1 = glutGet(GLUT_ELAPSED_TIME) ;

			Algo::Modelisation::computeDual<PFP>(myMap) ;

			GLint t2 = glutGet(GLUT_ELAPSED_TIME) ;
			GLfloat seconds = (t2 - t1) / 1000.0f ;
			CGoGNout << "dual computation: "<< seconds << "sec" << CGoGNendl ;

			position = positionF ;

			normal = myMap.getAttribute<PFP::VEC3>(VERTEX, "normal") ;
			if(!normal.isValid())
				normal = myMap.addAttribute<PFP::VEC3>(VERTEX, "normal") ;
			laplacian = myMap.getAttribute<PFP::VEC3>(VERTEX, "laplacian") ;
			if(!laplacian.isValid())
				laplacian = myMap.addAttribute<PFP::VEC3>(VERTEX, "laplacian") ;

			t1 = glutGet(GLUT_ELAPSED_TIME) ;

			updateVBOprimitives(Algo::Render::GL2::TRIANGLES | Algo::Render::GL2::LINES | Algo::Render::GL2::POINTS) ;
			updateVBOdata(Algo::Render::GL2::POSITIONS | Algo::Render::GL2::NORMALS) ;
			topo_render->updateData<PFP>(myMap, position, 0.9f, 0.9f) ;

			t2 = glutGet(GLUT_ELAPSED_TIME) ;
			seconds = (t2 - t1) / 1000.0f ;
			CGoGNout << "display update: "<< seconds << "sec" << CGoGNendl ;

			glutPostRedisplay() ;
			break ;
		}

		case 'r':
		{

			GLint t1 = glutGet(GLUT_ELAPSED_TIME) ;

			Algo::Remeshing::pliantRemeshing<PFP>(myMap, position, normal) ;

			GLint t2 = glutGet(GLUT_ELAPSED_TIME) ;
			GLfloat seconds = (t2 - t1) / 1000.0f ;
			CGoGNout << "pliant remeshing: "<< seconds << "sec" << CGoGNendl ;

			t1 = glutGet(GLUT_ELAPSED_TIME) ;

			updateVBOprimitives(Algo::Render::GL2::TRIANGLES | Algo::Render::GL2::LINES | Algo::Render::GL2::POINTS) ;
			updateVBOdata(Algo::Render::GL2::POSITIONS | Algo::Render::GL2::NORMALS) ;
			topo_render->updateData<PFP>(myMap, position, 0.9f, 0.9f) ;

			t2 = glutGet(GLUT_ELAPSED_TIME) ;
			seconds = (t2 - t1) / 1000.0f ;
			CGoGNout << "display update: "<< seconds << "sec" << CGoGNendl ;

			glutPostRedisplay() ;
			break ;
		}

		case '9':
		{
			CellMarker markVisit(myMap, VERTEX) ;

			CellMarker markNoDelete(myMap, VERTEX) ;

			std::vector<Dart> visited ;
			visited.reserve(myMap.getNbCells(VERTEX)) ;

			visited.push_back(myMap.begin()) ;
			markVisit.mark(myMap.begin()) ;

			for(unsigned int i = 0; i < visited.size(); ++i)
			{
				bool deleteV = true ;
				if(markNoDelete.isMarked(visited[i]))
					deleteV = false ;

				Dart vit = visited[i] ;
				do	// add all vertex neighbors to the table
				{
					Dart nv = myMap.phi1(vit) ;
					if(!markVisit.isMarked(nv))
					{
						visited.push_back(nv) ;
						markVisit.mark(nv) ;
					}
					if(deleteV)
						markNoDelete.mark(nv) ;
					vit = myMap.alpha1(vit) ;
				} while(vit != visited[i]) ;

				if(deleteV)
					myMap.deleteVertex(visited[i]) ;
			}

//			for(Dart d = myMap.begin(); d != myMap.end(); myMap.next(d))
//			{
//				if(!markVisit.isMarked(d))
//				{
//					Dart dd = myMap.phi1(d) ;
//					myMap.deleteVertex(d) ;
//					Dart fit = dd ;
//					do
//					{
//						markVisit.mark(fit) ;
//						fit = myMap.phi1(fit) ;
//					} while(fit != dd) ;
//				}
//			}

			updateVBOprimitives(Algo::Render::GL2::TRIANGLES | Algo::Render::GL2::LINES | Algo::Render::GL2::POINTS) ;
			updateVBOdata(Algo::Render::GL2::POSITIONS | Algo::Render::GL2::NORMALS) ;
			topo_render->updateData<PFP>(myMap, position, 0.9f, 0.9f) ;

			glutPostRedisplay() ;
			break ;
		}

		case 'a':
		{
			Algo::Geometry::computeLaplacianVertices<PFP>(myMap, Algo::Geometry::TOPOLOGICAL, position, laplacian) ;
			break ;
		}

		case 'x':
		{
			Algo::Export::exportOFF<PFP>(myMap, position, "export.off") ;
			break ;
		}

		case 'n':
		{
			myMap.saveMapBin("plop");
		}
	}
}

void MyGlutWin::updateVBOprimitives(int upType)
{
	if(upType & Algo::Render::GL2::TRIANGLES)
	{
		vbo_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::TRIANGLES) ;
	}

	if(upType & Algo::Render::GL2::LINES)
	{
		vbo_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::LINES) ;
	}

	if(upType & Algo::Render::GL2::POINTS)
	{
		vbo_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::POINTS) ;
	}
}

void MyGlutWin::updateVBOdata(int upType, bool recompute)
{
	if(upType & Algo::Render::GL2::POSITIONS)
		vbo_render->updateData(Algo::Render::GL2::POSITIONS, position) ;

	if(upType & Algo::Render::GL2::NORMALS)
	{
		if(recompute)
			Algo::Geometry::computeNormalVertices<PFP>(myMap, position, normal) ;
		vbo_render->updateData(Algo::Render::GL2::NORMALS, normal) ;
		if(renderNormals)
			initDL(MyGlutWin::NORMALS) ;
	}
}

void MyGlutWin::initDL(int renderMode)
{
	switch(renderMode)
	{
		case NORMALS : {
			glNewList(dl_norm, GL_COMPILE) ;
			glLineWidth(1.0f) ;
			glDisable(GL_LIGHTING) ;
			glColor3fv(colNormal.data()) ;
			Algo::Render::Direct::renderNormalVertices<PFP>(myMap, position, normal, normalScaleFactor * normalBaseSize) ;
			glEndList() ;
			break ;
		}

		case LAPLACIANS : {
			glNewList(dl_lapl, GL_COMPILE) ;
			glLineWidth(1.0f) ;
			glDisable(GL_LIGHTING) ;
			glColor3f(0.0f, 1.0f, 0.0f) ;
			Algo::Render::Direct::renderNormalVertices<PFP>(myMap, position, laplacian, 1.0f) ;
			glEndList() ;
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

	if(argc < 2)
	{
		Dart d1 = myMap.newFace(3) ;
		Dart d2 = myMap.newFace(3) ;
		myMap.sewFaces(myMap.begin(), d2) ;
		myMap.mergeFaces(myMap.begin()) ;
	}
	else
	{
		char* filename = argv[1] ;

		GLint t1 = glutGet(GLUT_ELAPSED_TIME) ;
		std::vector<std::string> attrNames ;
		if(!Algo::Import::importMesh<PFP>(myMap, filename, attrNames))
		{
			CGoGNerr << "could not import " << filename << CGoGNendl ;
			return 1 ;
		}

		mgw->position = myMap.getAttribute<PFP::VEC3>(VERTEX, attrNames[0]) ;
		GLint t2 = glutGet(GLUT_ELAPSED_TIME) ;
		GLfloat seconds = (t2 - t1) / 1000.0f ;
		CGoGNout << "import: " << seconds << " sec" << CGoGNendl ;

//		DartMarker m(myMap) ;
//		myMap.closeMap(m) ;
	}

	CGoGNout << myMap.getNbOrbits(VERTEX) << CGoGNendl ;
	CGoGNout << myMap.getNbOrbits(EDGE) << CGoGNendl ;
	CGoGNout << myMap.getNbOrbits(FACE) << CGoGNendl ;

	// compute width and position of object for centering
	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, mgw->position) ;
	CGoGNout << bb << CGoGNendl ;

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
