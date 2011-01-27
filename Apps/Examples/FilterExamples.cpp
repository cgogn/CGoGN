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

#include "FilterExamples.h"

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

void TW_CALL originalPositionsCB(void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->loadOriginalPositions() ;
}

void TW_CALL addNoiseCB(void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->addNoise() ;
}

void TW_CALL averageFilterCB(void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->filter(Algo::Filters2D::AVERAGE) ;
}

void TW_CALL taubinFilterCB(void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->filter(Algo::Filters2D::TAUBIN) ;
}

void TW_CALL bilateralFilterCB(void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->filter(Algo::Filters2D::BILATERAL) ;
}

void TW_CALL averageNormalsFilterCB(void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->filter(Algo::Filters2D::AVERAGE_NORMAL) ;
}

void TW_CALL MMSEFilterCB(void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->filter(Algo::Filters2D::MMSE) ;
}

void TW_CALL SUSANFilterCB(void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->filter(Algo::Filters2D::SUSAN) ;
}

void TW_CALL TNBAFilterCB(void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->filter(Algo::Filters2D::TNBA) ;
}

void TW_CALL VNBAFilterCB(void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->filter(Algo::Filters2D::VNBA) ;
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
	vbo_render(NULL)
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
	originalPosition = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "original_position") ;
	position2 = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "position2") ;

	myMap.copyAttribute(originalPosition, position) ;

	vbo_render = new Algo::Render::VBO::MapRender_VBO<PFP>(myMap, allDarts) ;
	vbo_render->initPrimitives(Algo::Render::VBO::TRIANGLES) ;
	vbo_render->initPrimitives(Algo::Render::VBO::LINES) ;
	updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS) ;
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

	TwAddSeparator(viewer_bar, "", "") ;

	TwAddButton(viewer_bar, "Reload Original Pos", originalPositionsCB, this, "") ;
	TwAddButton(viewer_bar, "Add Noise", addNoiseCB, this, "") ;

	TwAddSeparator(viewer_bar, "", "") ;

	TwAddButton(viewer_bar, "Average Positions Filter", averageFilterCB, this, "") ;
	TwAddButton(viewer_bar, "Taubin Filter", taubinFilterCB, this, "") ;
	TwAddButton(viewer_bar, "Bilateral Filter", bilateralFilterCB, this, "") ;
	TwAddButton(viewer_bar, "Average Normals Filter", averageNormalsFilterCB, this, "") ;
	TwAddButton(viewer_bar, "MMSE Filter", MMSEFilterCB, this, "") ;
	TwAddButton(viewer_bar, "SUSAN Filter", SUSANFilterCB, this, "") ;
	TwAddButton(viewer_bar, "TNBA Filter", TNBAFilterCB, this, "") ;
	TwAddButton(viewer_bar, "VNBA Filter", VNBAFilterCB, this, "") ;
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
		vbo_render->draw(Algo::Render::VBO::LINES) ;
	}

	if(renderNormals)
		glCallList(dl_norm) ;

	glEnable(GL_POLYGON_OFFSET_FILL) ;
	glPolygonOffset(1.0f, 1.0f) ;

	if(renderObject)
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

		glEnable(GL_LIGHTING) ;
		glDisable(GL_CULL_FACE) ;
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

	glColor3f(1.0f, 1.0f, 1.0f) ;
	int x = getWinWidth() - 170 ;
	printString2D(
		x,
		20,
		"Keys :\n"
		"----\n"
		"'x' : export mesh\n"
	) ;
}
/*
void MyGlutWin::myKeyboard(unsigned char keycode, int x, int y)
{
	switch(keycode)
	{
		case 'x' : {
			bool success = Export::exportOFF<PFP>(myMap, allDarts, "export.off") ;
		}
	}
}
*/
void MyGlutWin::updateVBOdata(int upType)
{
	// update data of the VBO
	if(upType & Algo::Render::VBO::POSITIONS)
		vbo_render->updateData(Algo::Render::VBO::POSITIONS, position) ;
	if(upType & Algo::Render::VBO::NORMALS)
	{
		Algo::Geometry::computeNormalVertices<PFP>(myMap, position, normal) ;
		vbo_render->updateData(Algo::Render::VBO::NORMALS, normal) ;
		if(renderNormals)
			initDLNormals() ;
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
	switch (renderMode)
	{
		case NORMAL : {
			glLineWidth(1.0f) ;
			glDisable(GL_LIGHTING) ;
			glColor3fv(colNormal.data()) ;
			Algo::Render::Direct::renderNormalVertices<PFP>(myMap, position, normal, normalScaleFactor * normalBaseSize) ;
			break ;
		}
	}
}

/*
void myGlutWin::myKeyboard(unsigned char keycode, int, int)
{
	GLint t1,t2;
	GLfloat seconds;
	float h;
	switch(keycode)
	{
	case 'V':
		t1 = glutGet(GLUT_ELAPSED_TIME);
		Algo::Filters2D::filterVNBA<PFP>(myMap,0.01f,0.0f);
		Algo::Filters2D::shiftPositions<PFP>(myMap);
		t2 = glutGet(GLUT_ELAPSED_TIME);
		seconds = (t2 - t1) / 1000.0f;
		std::cout << "VNBA filter: "<< seconds<<"sec"<<std::endl;
		h = Algo::Filters2D::computeHaussdorf<PFP>(myMap);
		std::cout<<"Haussdorff: "<<h<<std::endl;
		initDL();
		if (renderLines)
			initDLLines();
		glutPostRedisplay();
		break;

	}
}
*/

/**********************************************************************************************
 *                                      MAIN FUNCTION                                         *
 **********************************************************************************************/

int main(int argc, char **argv)
{
	if(argc < 2)
	{
		std::cerr << "syntax: " << argv[0] << " filename" << std::endl ;
		return 1 ;
	}

	MyGlutWin* mgw = new MyGlutWin(&argc, argv, 800, 800) ;

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
