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

#include "linearTest.h"

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

void TW_CALL setShowCurvatureCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->showCurvature = *(GLboolean*)value ;
	if(mgw->showCurvature)
	{
		mgw->updateVBOdata(Algo::Render::VBO::COLORS) ;
		TwDefine("Viewer/CurvatureDirections visible=true") ;
		TwDefine("Viewer/CurvType visible=true") ;
		TwDefine("Viewer/CurvDir visible=true") ;
		TwDefine("Viewer/CurvMultiplicator visible=true") ;
	}
	else
	{
		TwDefine("Viewer/CurvatureDirections visible=false") ;
		TwDefine("Viewer/CurvType visible=false") ;
		TwDefine("Viewer/CurvDir visible=false") ;
		TwDefine("Viewer/CurvMultiplicator visible=false") ;
	}
	glutPostRedisplay() ;
}
void TW_CALL getShowCurvatureCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(GLboolean*)value = mgw->showCurvature ;
}

void TW_CALL setShowCurvatureDirectionsCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->showCurvatureDirections = *(GLboolean*)value ;
	glutPostRedisplay() ;
}
void TW_CALL getShowCurvatureDirectionsCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(GLboolean*)value = mgw->showCurvatureDirections ;
}

void TW_CALL setCurvatureTypeCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->curvatureType = *(GLenum*)value ;
	mgw->updateVBOdata(Algo::Render::VBO::COLORS, false) ;
	glutPostRedisplay() ;
}
void TW_CALL getCurvatureTypeCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(GLenum*)value = mgw->curvatureType ;
}

void TW_CALL setCurvatureDirCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->curvatureDir = *(GLenum*)value ;
	glutPostRedisplay() ;
}
void TW_CALL getCurvatureDirCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(GLenum*)value = mgw->curvatureDir ;
}

void TW_CALL setCurvMultiplicatorCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->curvMultiplicator = *(float*)value ;
	mgw->updateVBOdata(Algo::Render::VBO::COLORS, false) ;
	glutPostRedisplay() ;
}
void TW_CALL getCurvMultiplicatorCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(float*)value = mgw->curvMultiplicator ;
}

void TW_CALL setSolverTypeCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->setSolverType(*(GLenum*)value) ;
}
void TW_CALL getSolverTypeCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(GLenum*)value = mgw->getSolverType() ;
}

void TW_CALL smoothCB(void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->smooth() ;
	if(mgw->showCurvature)
		mgw->updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS | Algo::Render::VBO::COLORS) ;
	else
		mgw->updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS) ;
	glutPostRedisplay() ;
}

void TW_CALL smoothCurvatureCB(void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	if(mgw->showCurvature)
	{
		mgw->smoothCurvature() ;
		mgw->updateVBOdata(Algo::Render::VBO::COLORS, false) ;
		glutPostRedisplay() ;
	}
}

void TW_CALL lsmCB(void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->lsm() ;
	if(mgw->showCurvature)
		mgw->updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS | Algo::Render::VBO::COLORS) ;
	else
		mgw->updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS) ;
	glutPostRedisplay() ;
}

void TW_CALL diffCoordCB(void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->matchDiffCoord(false) ;
	if(mgw->showCurvature)
		mgw->updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS | Algo::Render::VBO::COLORS) ;
	else
		mgw->updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS) ;
	glutPostRedisplay() ;
}

/**********************************************************************************************
 *                                 MyGlutWin IMPLEMENTATION                                   *
 **********************************************************************************************/

MyGlutWin::MyGlutWin(int* argc, char **argv, int winX, int winY) :
	GlutWin_ATB(argc, argv, winX, winY),
	renderStyle(GOURAUD),
	renderObject(true),
	renderLines(false),
	renderNormals(false),
	showCurvature(false),
	showCurvatureDirections(false),
	curvatureType(MEAN),
	curvMultiplicator(1.0f),
	smoothAmount(0.5f),
	vbo_render(NULL),
	def_locked(true),
	def_handle(false),
	selecting(false),
	deselecting(false),
	dragging(false),
#ifdef WITH_CUDA
	gpuSolver(false),
#endif
	lockingMarker(myMap, VERTEX_ORBIT),
	handleMarker(myMap, VERTEX_ORBIT),
	solverType(ITERATIVE)
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
	normal = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "normal") ;
	diffCoord = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "diffCoord") ;
	k1 = myMap.addAttribute<PFP::REAL>(VERTEX_ORBIT, "k1") ;
	k2 = myMap.addAttribute<PFP::REAL>(VERTEX_ORBIT, "k2") ;
	K1 = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "K1") ;
	K2 = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "K2") ;
	color = myMap.addAttribute<Geom::Vec4f>(VERTEX_ORBIT, "color") ;

	vbo_render = new Algo::Render::VBO::MapRender_VBO<PFP>(myMap, allDarts) ;
	vbo_render->initPrimitives(Algo::Render::VBO::TRIANGLES) ;
	vbo_render->initPrimitives(Algo::Render::VBO::LINES) ;
	updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS) ;

	Algo::Geometry::computeLaplacianVertices<PFP>(myMap, Algo::Geometry::TOPOLOGICAL, position, diffCoord) ;

	vIndex = myMap.addAttribute<unsigned int>(VERTEX_ORBIT, "index") ;
	nb_vertices = myMap.computeIndexCells(vIndex) ;

	solver = new LinearSolver<CPUSolverTraits>(nb_vertices) ;

	select_area = gWidthObj / (100.0f * getScale()) ;
}

void MyGlutWin::initGUI()
{
	viewer_bar = TwNewBar("Viewer") ;
	TwDefine("Viewer position='16 16' size='220 400' valueswidth='85'") ;

	TwType tw_render_style = TwDefineEnum("Shading","Flat,Gouraud,Phong") ;
	TwAddVarCB(viewer_bar, "RenderStyle", tw_render_style, setRenderStyleCB, getRenderStyleCB, this, "") ;
	TwAddVarCB(viewer_bar, "Shininess", TW_TYPE_FLOAT, setShininessCB, getShininessCB, this, "min=1 max=255 step=1") ;
	TwAddVarCB(viewer_bar, "Focal", TW_TYPE_FLOAT, setFocalCB, getFocalCB, this, "min=0.6 max=5.0 step=0.1") ;
	TwAddVarCB(viewer_bar, "Show Object", TW_TYPE_BOOL8, setRenderObjectCB, getRenderObjectCB, this, "") ;
	TwAddVarCB(viewer_bar, "Show Lines", TW_TYPE_BOOL8, setRenderLinesCB, getRenderLinesCB, this, "") ;
	TwAddVarCB(viewer_bar, "Show Normals", TW_TYPE_BOOL8, setRenderNormalsCB, getRenderNormalsCB, this, "") ;
	TwAddVarCB(viewer_bar, "NormalScaleFactor", TW_TYPE_FLOAT, setNormalScaleFactorCB, getNormalScaleFactorCB, this, "min=0.1 max=5.0 step=0.1") ;
	TwDefine("Viewer/NormalScaleFactor visible=false") ;
	TwAddVarCB(viewer_bar, "FaceShrinkage", TW_TYPE_FLOAT, setFaceShrinkageCB, getFaceShrinkageCB, this, "min=0.0 max=0.9 step=0.01") ;
	TwDefine("Viewer/FaceShrinkage visible=false") ;
	TwAddVarCB(viewer_bar, "Show Curvature", TW_TYPE_BOOL8, setShowCurvatureCB, getShowCurvatureCB, this, "") ;
	TwAddVarCB(viewer_bar, "CurvatureDirections", TW_TYPE_BOOL8, setShowCurvatureDirectionsCB, getShowCurvatureDirectionsCB, this, "") ;
	TwType tw_curvature_type = TwDefineEnum("CurvatureType","Mean,Gaussian") ;
	TwAddVarCB(viewer_bar, "CurvType", tw_curvature_type, setCurvatureTypeCB, getCurvatureTypeCB, this, "") ;
	TwType tw_curvature_dir = TwDefineEnum("CurvatureDir","Both,Min,Max") ;
	TwAddVarCB(viewer_bar, "CurvDir", tw_curvature_dir, setCurvatureDirCB, getCurvatureDirCB, this, "") ;
	TwAddVarCB(viewer_bar, "CurvMultiplicator", TW_TYPE_FLOAT, setCurvMultiplicatorCB, getCurvMultiplicatorCB, this, "min=0.1 max=10.0 step=0.1") ;
	TwDefine("Viewer/CurvatureDirections visible=false") ;
	TwDefine("Viewer/CurvType visible=false") ;
	TwDefine("Viewer/CurvDir visible=false") ;
	TwDefine("Viewer/CurvMultiplicator visible=false") ;

	TwAddSeparator(viewer_bar, "", "") ;

	TwType tw_solver_type = TwDefineEnum("Solver","Iterative,Direct") ;
	TwAddVarCB(viewer_bar, "SolverType", tw_solver_type, setSolverTypeCB, getSolverTypeCB, this, "") ;
#ifdef WITH_CUDA
	TwAddVarRW(viewer_bar, "GPUSolver", TW_TYPE_BOOL8, &gpuSolver, "") ;
#endif

	TwAddSeparator(viewer_bar, "", "") ;

	TwAddButton(viewer_bar, "Smooth", smoothCB, this, "") ;
	TwAddButton(viewer_bar, "SmoothCurvature", smoothCurvatureCB, this, "") ;
	TwAddVarRW(viewer_bar, "Smooth Amount", TW_TYPE_FLOAT, &smoothAmount, "min=0.01 max=1.0 step=0.01") ;
	TwAddButton(viewer_bar, "LSM", lsmCB, this, "") ;
	TwAddButton(viewer_bar, "DiffCoord", diffCoordCB, this, "") ;
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

	if(renderLines)
	{
		glDisable(GL_LIGHTING);
		glColor3f(0.0f,0.0f,0.0f);
		vbo_render->disableBuffers(Algo::Render::VBO::COLORS) ;
		vbo_render->draw(Algo::Render::VBO::LINES) ;
		vbo_render->enableBuffers(Algo::Render::VBO::COLORS) ;
	}

	if(renderNormals)
		glCallList(dl_norm) ;

	glEnable(GL_POLYGON_OFFSET_FILL) ;
	glPolygonOffset(1.0f, 1.0f) ;

	if(renderObject)
	{
		if(showCurvature)
		{
			glEnable(GL_COLOR_MATERIAL) ;
			if(showCurvatureDirections)
				render(CURVATURE_DIRECTIONS) ;
		}
		else
		{
			glDisable(GL_COLOR_MATERIAL) ;
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

		render(VERTICES) ;
	}

	glDisable(GL_POLYGON_OFFSET_FILL) ;

	glPopMatrix() ;

	glColor3f(1.0f, 1.0f, 1.0f) ;
	int x = getWinWidth() - 170 ;
	printString2D(
		x,
		20,
		"Keys :\n"
		"----\n"
		"'l' : define locked vertices\n"
		"'h' : define handle vertices\n"
		"'x' : deselect vertices\n"
	) ;
}

void MyGlutWin::myKeyboard(unsigned char keycode, int x, int y)
{
	switch(keycode)
	{
		case 'l' : {
			def_locked = true ;
			def_handle = false ;
			break ;
		}

		case 'h' : {
			def_handle = true ;
			def_locked = false ;
			break ;
		}

		case 'x' : {
			deselectAllVertices() ;
			glutPostRedisplay() ;
			break ;
		}

		case '+' : {
			select_area *= 1.1f ;
			break ;
		}

		case '-' : {
			select_area /= 1.1f ;
			break ;
		}

		case 's' : {
			bool success = Algo::Export::exportOFF<PFP>(myMap, position, "export.off") ;
			if(!success)
			{
				std::cerr << "warning export of file export.off failed" << std::endl ;
			}
		}
	}
}

void MyGlutWin::myMouse(int button, int state, int x, int y)
{
	if(button == GLUT_LEFT_BUTTON && (glutGetModifiers() & GLUT_ACTIVE_SHIFT) && state == GLUT_DOWN)
		selecting = 1 ;
	else if(button == GLUT_RIGHT_BUTTON && (glutGetModifiers() & GLUT_ACTIVE_SHIFT) && state == GLUT_DOWN)
		deselecting = 1 ;
	else if(button == GLUT_LEFT_BUTTON && (glutGetModifiers() & GLUT_ACTIVE_CTRL) && state == GLUT_DOWN)
		dragging = 1 ;
	else
	{
		selecting = 0 ;
		deselecting = 0 ;
		dragging = 0 ;
	}

	if(selecting)
	{
		glPushMatrix() ;

		float sc = 50.0f / gWidthObj ;
		glScalef(sc, sc, sc) ;
		glTranslatef(-gPosObj[0], -gPosObj[1], -gPosObj[2]) ;

		Geom::Vec3f rayA, rayB ;
		getOrthoScreenRay(x, y, rayA, rayB) ;
		Geom::Vec3f AB = rayB - rayA ;
		Dart d ;
		Algo::Selection::vertexRaySelection<PFP>(myMap, position, rayA, AB, d) ;

		glPopMatrix();

		if(d != myMap.end())
			selectVertex(d) ;

		glutPostRedisplay() ;
	}
	else if(deselecting)
	{
		glPushMatrix() ;

		float sc = 50.0f / gWidthObj ;
		glScalef(sc, sc, sc) ;
		glTranslatef(-gPosObj[0], -gPosObj[1], -gPosObj[2]) ;

		Geom::Vec3f rayA, rayB ;
		getOrthoScreenRay(x, y, rayA, rayB) ;
		Geom::Vec3f AB = rayB - rayA ;
		Dart d ;
		Algo::Selection::vertexRaySelection<PFP>(myMap, position, rayA, AB, d) ;

		glPopMatrix();

		if(d != myMap.end())
			deselectVertex(d) ;

		glutPostRedisplay() ;
	}
}

void MyGlutWin::myMotion(int x, int y)
{
	if(dragging)
	{
		++skipCount ;
		if(skipCount % 5)
			return ;

		float sc = 1.0 / getScale() ;
		Geom::Vec4f d(0.01f*sc*(x-beginx), 0.01f*sc*(beginy-y), 0.0f, 1.0f) ;
		Geom::Vec4f t(0) ;
		GLfloat m[4][4] ;
		Utils::build_rotmatrix(m, getSceneQuaternion()) ;
		for(int i = 0; i < 4; ++i)
			for(int j = 0; j < 4; ++j)
				t[i] += m[i][j] * d[j] ;

		for(unsigned int i = 0; i < handle_vertices.size(); ++i)
		{
			PFP::VEC3& p = position[handle_vertices[i]] ;
			p[0] += t[0] / t[3] ;
			p[1] += t[1] / t[3] ;
			p[2] += t[2] / t[3] ;
		}

		matchDiffCoord(true) ;

		if(showCurvature)
			updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS | Algo::Render::VBO::COLORS) ;
		else
			updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS) ;
		if(renderNormals)
			initDLNormals() ;
		glutPostRedisplay() ;
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

	if(upType & Algo::Render::VBO::COLORS)
	{
		if(recompute)
			Algo::Geometry::computeCurvatureVertices<PFP>(myMap, position, normal, k1, k2, K1, K2) ;
		float minC = std::numeric_limits<float>::max() ;
		float maxC = std::numeric_limits<float>::min() ;
		AttribContainer& vertexAttr = myMap.getAttributeContainer(VERTEX_ORBIT) ;
		for(unsigned int i = vertexAttr.begin(); i < vertexAttr.end(); vertexAttr.next(i))
		{
			PFP::REAL c=0.0;
			if(curvatureType == MEAN)
				c = ( k1[i] + k2[i] ) / PFP::REAL(2) ;
			else if(curvatureType == GAUSSIAN)
				c = k1[i] * k2[i] ;
			minC = c < minC ? c : minC ;
			maxC = c > maxC ? c : maxC ;
		}
		float r = 0.0f, g = 0.0f, b = 0.0f ;
		for(unsigned int i = vertexAttr.begin(); i < vertexAttr.end(); vertexAttr.next(i))
		{
			PFP::REAL c=0.0;
			if(curvatureType == MEAN)
				c = ( k1[i] + k2[i] ) / PFP::REAL(2) ;
			else if(curvatureType == GAUSSIAN)
				c = k1[i] * k2[i] ;
			c *= curvMultiplicator ;
			if(c < 0.0f) { c = c / minC ; r = 1.0f ; g = 1.0f - (2.0f*c) ; b = 1.0f - (2.0f*c) ; }
			else { c = c / maxC ; r = 1.0f - (2.0f*c) ; g = 1.0f - (2.0f*c) ; b = 1.0f ; }
			color[i] = Geom::Vec4f(r,g,b,1.0f) ;
		}
		vbo_render->updateData(Algo::Render::VBO::COLORS, color) ;
	}
}

void MyGlutWin::initDLNormals()
{
	glNewList(dl_norm, GL_COMPILE) ;
	render(NORMAL) ;
	glEndList() ;
}

void MyGlutWin::render(int renderMode)
{
	switch(renderMode)
	{
		case VERTICES : {
			glDisable(GL_LIGHTING) ;
			glPointSize(4.0f);
			glBegin(GL_POINTS);
			glColor3f(1.0f, 0.0f, 0.0f) ;
			for(unsigned int i = 0; i < locked_vertices.size(); ++i)
			{
				if (!handleMarker.isMarked(locked_vertices[i]))
					glVertex3fv(position[locked_vertices[i]].data()) ;
			}
			glColor3f(0.0f, 0.0f, 1.0f) ;
			for(unsigned int i = 0; i < handle_vertices.size(); ++i)
				glVertex3fv(position[handle_vertices[i]].data()) ;
			glEnd() ;
			break ;
		}

		case NORMAL : {
			glLineWidth(1.0f) ;
			glDisable(GL_LIGHTING) ;
			glColor3fv(colNormal.data()) ;
			Algo::Render::Direct::renderNormalVertices<PFP>(myMap, position, normal, normalScaleFactor * normalBaseSize) ;
			break ;
		}

		case CURVATURE_DIRECTIONS : {
			glLineWidth(1.0f) ;
			glDisable(GL_LIGHTING) ;
			AttribContainer vertexAttr = myMap.getAttributeContainer(VERTEX_ORBIT) ;
			if(curvatureDir == 0 || curvatureDir == 2)
			{
				glColor3f(0.0f, 1.0f, 0.0f) ;
				glBegin(GL_LINES) ;
				for(unsigned int i = vertexAttr.begin(); i < vertexAttr.end(); vertexAttr.next(i))
				{
					PFP::VEC3 dir = K1[i] * normalScaleFactor * normalBaseSize * 0.5f ;
					PFP::VEC3 p1 = position[i] - dir ;
					PFP::VEC3 p2 = position[i] + dir ;
					glVertex3fv(p1.data()) ;
					glVertex3fv(p2.data()) ;
				}
				glEnd() ;
			}
			if(curvatureDir == 0 || curvatureDir == 1)
			{
				glColor3f(0.0f, 1.0f, 0.0f) ;
				glBegin(GL_LINES) ;
				for(unsigned int i = vertexAttr.begin(); i < vertexAttr.end(); vertexAttr.next(i))
				{
					PFP::VEC3 dir = K2[i] * normalScaleFactor * normalBaseSize * 0.5f ;
					PFP::VEC3 p1 = position[i] - dir ;
					PFP::VEC3 p2 = position[i] + dir ;
					glVertex3fv(p1.data()) ;
					glVertex3fv(p2.data()) ;
				}
				glEnd() ;
			}
			break ;
		}
	}
}
/*
void MyGlutWin::selectVertices(Dart d, float dist)
{
	gmtl::Vec3f p = myMap.getVertexEmb(d)->getPosition() ;
	std::vector<Embedding*> selected_vertices ;

	Marker visited = myMap.getNewMarker() ;
	std::list<Dart> darts ;
	darts.push_back(d) ;
	std::list<Dart>::iterator it = darts.begin() ;
	do
	{
		myMap.markOrbit(0, *it, visited) ;
		EMB* e = reinterpret_cast<EMB*>(myMap.getVertexEmb(*it)) ;
		gmtl::Vec3f pos = e->getPosition() ;
		pos -= p ;
		if(gmtl::lengthSquared(pos) < dist*dist)
		{
			selected_vertices.push_back(e) ;
			Dart vit = *it ;
			do
			{
				if(!myMap.isMarkedDart(myMap.phi2(vit), visited))
					darts.push_back(myMap.phi2(vit)) ;
				vit = myMap.phi1(myMap.phi2(vit)) ;
			} while(vit != *it) ;
		}
		++it ;
	} while(it != darts.end()) ;
	for(it = darts.begin(); it != darts.end(); ++it)
		myMap.unmarkOrbit(0, *it, visited) ;
	myMap.releaseMarker(visited) ;

	if(def_locked)
	{
		for(unsigned int i = 0; i < selected_vertices.size(); ++i)
		{
			Embedding* sv = selected_vertices[i] ;
			if(!sv->testMarker(lockingMarker))
			{
				locked_vertices.push_back(sv) ;
				sv->setMarker(lockingMarker) ;
			}
		}
		solver->reset(false) ;
	}
	else if(def_handle)
	{
		for(unsigned int i = 0; i < selected_vertices.size(); ++i)
		{
			Embedding* sv = selected_vertices[i] ;
			if(!sv->testMarker(handleMarker))
			{
				handle_vertices.push_back(sv) ;
				sv->setMarker(handleMarker) ;
			}
			if(!sv->testMarker(lockingMarker))
			{
				locked_vertices.push_back(sv) ;
				sv->setMarker(lockingMarker) ;
			}
		}
		solver->reset(false) ;
	}
}
*/
void MyGlutWin::selectVertex(Dart sd)
{
	unsigned int v = myMap.getEmbedding(sd, VERTEX_ORBIT) ;

	if(def_locked)
	{
		if (!lockingMarker.isMarked(v))
		{
			locked_vertices.push_back(v) ;
			lockingMarker.mark(v);
			LinearSolving::resetSolver(solver, false) ;
		}
	}
	else if(def_handle)
	{
		if(!handleMarker.isMarked(v))
		{
			handle_vertices.push_back(v) ;
			handleMarker.mark(v);
		}
		if(!lockingMarker.isMarked(v))
		{
			locked_vertices.push_back(v) ;
			lockingMarker.mark(v) ;
			LinearSolving::resetSolver(solver, false) ;
		}
	}
}

void MyGlutWin::deselectVertex(Dart d)
{
	unsigned int v = myMap.getEmbedding(d, VERTEX_ORBIT) ;

	if(def_locked)
	{
		bool found = false ;
		unsigned int i ;
		for(i = 0; i < locked_vertices.size() && !found; ++i)
		{
			if(locked_vertices[i] == v)
				found = true ;
		}
		if(found)
		{
			lockingMarker.unmark(locked_vertices[i-1]);
			locked_vertices[i-1] = locked_vertices.back() ;
			locked_vertices.pop_back() ;
			LinearSolving::resetSolver(solver, false) ;
		}
		found = false ;
		for(i = 0; i < handle_vertices.size() && !found; ++i)
		{
			if(handle_vertices[i] == v)
				found = true ;
		}
		if(found)
		{
			handleMarker.unmark(handle_vertices[i-1]);
			handle_vertices[i-1] = handle_vertices.back() ;
			handle_vertices.pop_back() ;
		}
	}
	else if(def_handle)
	{
		bool found = false ;
		unsigned int i ;
		for(i = 0; i < handle_vertices.size() && !found; ++i)
		{
			if(handle_vertices[i] == v)
				found = true ;
		}
		if(found)
		{
			handleMarker.unmark(handle_vertices[i-1]);
			handle_vertices[i-1] = handle_vertices.back() ;
			handle_vertices.pop_back() ;
		}
	}
}

void MyGlutWin::deselectAllVertices()
{
	if(def_locked)
	{
		for(unsigned int i = 0; i < locked_vertices.size(); ++i)
			lockingMarker.unmark(locked_vertices[i]);
		locked_vertices.clear() ;
		for(unsigned int i = 0; i < handle_vertices.size(); ++i)
			handleMarker.unmark(handle_vertices[i]);
		handle_vertices.clear() ;
		LinearSolving::resetSolver(solver, false) ;
	}
	else if(def_handle)
	{
		for(unsigned int i = 0; i < handle_vertices.size(); ++i)
			handleMarker.unmark(handle_vertices[i]);
		handle_vertices.clear() ;
	}
}

/**********************************************************************************************
 *                                      SMOOTH & DEFORM                                       *
 **********************************************************************************************/

void MyGlutWin::smooth()
{
	std::cout << "Start smoothing.." << std::flush ;
    GLint t1 = glutGet(GLUT_ELAPSED_TIME) ;

	LinearSolving::initSolver(solver, nb_vertices, true, solverType == DIRECT ? true : false) ;
	LinearSolving::resetSolver(solver, false) ;
	float params[1] ;
	params[0] = smoothAmount ;
	for(int coord = 0; coord < 3; ++coord)
	{
		LinearSolving::setupVariables<PFP, PFP::VEC3>(myMap, solver, lockingMarker, position, coord, vIndex) ;
		LinearSolving::startMatrix(solver) ;
		LinearSolving::addMatrixRows<PFP, PFP::VEC3>(myMap, solver, position, LinearSolving::LAPLACIAN_TOPO, coord, vIndex) ;
		LinearSolving::addMatrixRows<PFP, PFP::VEC3>(myMap, solver, position, LinearSolving::EQUALITY, coord, vIndex, params) ;
		LinearSolving::endMatrix(solver) ;
		LinearSolving::solve(solver) ;
		LinearSolving::getResult<PFP, PFP::VEC3>(myMap, solver, position, coord, vIndex) ;
		LinearSolving::resetSolver(solver, true) ;
	}

	GLint t2 = glutGet(GLUT_ELAPSED_TIME) ;
	GLfloat seconds = (t2 - t1) / 1000.0f ;
	std::cout << "..done (" << seconds << " sec)" << std::endl ;
}

void MyGlutWin::smoothCurvature()
{
	std::cout << "Start smoothing curvature.." << std::flush ;
	GLint t1 = glutGet(GLUT_ELAPSED_TIME) ;

	LinearSolving::initSolver(solver, nb_vertices, true, solverType == DIRECT ? true : false) ;
	LinearSolving::resetSolver(solver, false) ;
	float params[1] ;
	params[0] = smoothAmount ;

	LinearSolving::setupVariables<PFP, PFP::REAL>(myMap, solver, lockingMarker, k1, vIndex) ;
	LinearSolving::startMatrix(solver) ;
	LinearSolving::addMatrixRows<PFP, PFP::REAL>(myMap, solver, k1, LinearSolving::LAPLACIAN_TOPO, vIndex) ;
	LinearSolving::addMatrixRows<PFP, PFP::REAL>(myMap, solver, k1, LinearSolving::EQUALITY, vIndex, params) ;
	LinearSolving::endMatrix(solver) ;
	LinearSolving::solve(solver) ;
	LinearSolving::getResult<PFP, PFP::REAL>(myMap, solver, k1, vIndex) ;

	LinearSolving::resetSolver(solver, true) ;

	LinearSolving::setupVariables<PFP, PFP::REAL>(myMap, solver, lockingMarker, k2, vIndex) ;
	LinearSolving::startMatrix(solver) ;
	LinearSolving::addMatrixRows<PFP, PFP::REAL>(myMap, solver, k2, LinearSolving::LAPLACIAN_TOPO, vIndex) ;
	LinearSolving::addMatrixRows<PFP, PFP::REAL>(myMap, solver, k2, LinearSolving::EQUALITY, vIndex, params) ;
	LinearSolving::endMatrix(solver) ;
	LinearSolving::solve(solver) ;
	LinearSolving::getResult<PFP, PFP::REAL>(myMap, solver, k2, vIndex) ;

	GLint t2 = glutGet(GLUT_ELAPSED_TIME) ;
	GLfloat seconds = (t2 - t1) / 1000.0f ;
	std::cout << "..done (" << seconds << " sec)" << std::endl ;
}

void MyGlutWin::lsm()
{
	if(locked_vertices.size() > 4)
	{
		std::cout << "Start least square mesh.." << std::flush ;
	    GLint t1 = glutGet(GLUT_ELAPSED_TIME) ;

		LinearSolving::initSolver(solver, nb_vertices, true, solverType == DIRECT ? true : false) ;
		LinearSolving::resetSolver(solver, false) ;
		for(int coord = 0; coord < 3; ++coord)
		{
			LinearSolving::setupVariables<PFP, PFP::VEC3>(myMap, solver, lockingMarker, position, coord, vIndex) ;
			LinearSolving::startMatrix(solver) ;
			LinearSolving::addMatrixRows<PFP, PFP::VEC3>(myMap, solver, position, LinearSolving::LAPLACIAN_TOPO, coord, vIndex) ;
			LinearSolving::endMatrix(solver) ;
			LinearSolving::solve(solver) ;
			LinearSolving::getResult<PFP, PFP::VEC3>(myMap, solver, position, coord, vIndex) ;
			LinearSolving::resetSolver(solver, true) ;
		}

	    GLint t2 = glutGet(GLUT_ELAPSED_TIME) ;
	    GLfloat seconds = (t2 - t1) / 1000.0f ;
		std::cout << "..done (" << seconds << " sec)" << std::endl ;
	}
}

void MyGlutWin::matchDiffCoord(bool keepMatrix)
{
//	std::cout << "Start matching diff coord.." << std::flush ;
//	GLint t1 = glutGet(GLUT_ELAPSED_TIME) ;

	LinearSolving::initSolver(solver, nb_vertices, true, solverType == DIRECT ? true : false) ;
	if(!keepMatrix)
		LinearSolving::resetSolver(solver, false) ;
	for(int coord = 0; coord < 3; ++coord)
	{
		LinearSolving::setupVariables<PFP, PFP::VEC3>(myMap, solver, lockingMarker, position, coord, vIndex) ;
		LinearSolving::startMatrix(solver) ;
		FunctorMatrixRowDiffCoord<PFP, CPUSolverTraits> fmrdc(myMap, solver, diffCoord, coord, vIndex) ;
		myMap.foreach_orbit(VERTEX_ORBIT, fmrdc) ;
		LinearSolving::endMatrix(solver) ;
		LinearSolving::solve(solver) ;
		LinearSolving::getResult<PFP, PFP::VEC3>(myMap, solver, position, coord, vIndex) ;
		LinearSolving::resetSolver(solver, true) ;
	}

//	GLint t2 = glutGet(GLUT_ELAPSED_TIME) ;
//	GLfloat seconds = (t2 - t1) / 1000.0f ;
//	std::cout << "..done (" << seconds << " sec)" << std::endl ;
}

/**********************************************************************************************
 *                                      MAIN FUNCTION                                         *
 **********************************************************************************************/

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		std::cerr << "syntax: " << argv[0] << " filename" << std::endl ;
		return 1 ;
	}

	MyGlutWin* mgw = new MyGlutWin(&argc, argv, 1200, 800) ;

	char* filename = argv[1] ;

	mgw->position = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "position") ;

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

	// compute width and position of object for centering
	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, mgw->position) ;
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
