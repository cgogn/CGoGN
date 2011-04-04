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

#include "Utils/glutwin_atb.h"

#include <stdlib.h>
#include <math.h>
#include <iostream>

namespace CGoGN
{

namespace Utils
{

GlutWin_ATB* GlutWin_ATB::instance = NULL ;

int GlutWin_ATB::rotating = 0 ;
int GlutWin_ATB::scaling = 0 ;
int GlutWin_ATB::translating = 0 ;
int GlutWin_ATB::beginx = 0 ;
int GlutWin_ATB::beginy = 0 ;
float GlutWin_ATB::curquat[4] = { 0.0f, 0.0f, 0.0f, 0.0f } ;
float GlutWin_ATB::lastquat[4] = { 0.0f, 0.0f, 0.0f, 0.0f } ;
int GlutWin_ATB::newModel = 1 ;
float GlutWin_ATB::scalefactor = 1.0f ;
float GlutWin_ATB::focal = 2.5f ;
float GlutWin_ATB::trans_x = 0.0f ;
float GlutWin_ATB::trans_y = 0.0f ;
float GlutWin_ATB::trans_z = 0.0f ;

int GlutWin_ATB::W = 0 ;
int GlutWin_ATB::H = 0 ;

float GlutWin_ATB::lightZeroPosition[4]= { 0.0f, 0.0f, 10000.0f, 1.0f } ;
float GlutWin_ATB::lightZeroColor[4]= { 0.9f, 0.9f, 0.9f, 1.0f } ;


GlutWin_ATB::GlutWin_ATB(int* argc, char **argv, int winX, int winY)
{
	instance = this ;

	newModel = 1 ;
	scalefactor = 1.0f ;
	trans_x = 0.0f ;
	trans_y = 0.0f ;
	trans_z = -200.0f ;

	std::cout << "Initialisation Glut" << std::endl ;
	glutInit(argc, argv) ;
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH) ;
	glutCreateWindow(argv[0]) ;
	glutReshapeWindow(winX,winY) ;
  	W = winX ;
	H = winY ;
	trackball(curquat, 0.0f, 0.0f, 0.0f, 0.0f) ;

	// AntTweakBar
	TwInit(TW_OPENGL, NULL) ;
	TwWindowSize(W, H) ;
	glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT) ;
	TwGLUTModifiersFunc(glutGetModifiers) ;

	glutDisplayFunc(redraw) ;
	glutReshapeFunc(reshape) ;
	glutIdleFunc(NULL) ;
	glutMouseFunc(mouse) ;
	glutPassiveMotionFunc(passive_mouse) ;
	glutMotionFunc(motion) ;
	glutKeyboardFunc(keyboard) ;

	std::cout << "Initialisation OpenGL" << std::endl ;
	shaderOk = Utils::GLSLShader::init() ;

	glViewport(0, 0, W, H) ;
	glMatrixMode(GL_PROJECTION) ;
	glLoadIdentity() ;
	glFrustum(-1.0f, 1.0f, -1.0f*H/W, 1.0f*H/W, focal, 500.0f) ;
	glMatrixMode(GL_MODELVIEW) ;

	glClearColor(0.1f, 0.1f, 0.3f, 0.0f) ;
	glEnable(GL_DEPTH_TEST) ;
	glEnable(GL_NORMALIZE) ;

	glEnable(GL_LIGHTING) ;
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE) ;
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1) ;
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor) ;
	glLightfv(GL_LIGHT0, GL_POSITION, lightZeroPosition) ;
	glEnable(GL_LIGHT0) ;
}

GlutWin_ATB::~GlutWin_ATB()
{
	// AntTweakBar
	TwTerminate() ;
}

void GlutWin_ATB::redraw()
{
	if(newModel)
		recalcModelView() ;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;

	instance->myRedraw() ;
	// AntTweakBar
	TwDraw() ;

	glutSwapBuffers() ;
}

void GlutWin_ATB::reshape(int w, int h)
{
	if(w > 0) W = w ;
	if(h > 0) H = h ;
	
	glViewport(0, 0, W, H) ;
	
	glMatrixMode(GL_PROJECTION) ;
	glLoadIdentity() ;
	glFrustum(-1.0f, 1.0f, -1.0f*H/W, 1.0f*H/W, focal, 500.0f) ;
	glMatrixMode(GL_MODELVIEW) ;
	
	recalcModelView() ;
	
	// AntTweakBar
	TwWindowSize(W, H) ;
}

void GlutWin_ATB::recalcModelView()
{
	GLfloat m[4][4] ;

	glPopMatrix() ;
	glPushMatrix() ;

	glTranslatef(trans_x, trans_y, trans_z) ;

	build_rotmatrix(m, curquat) ;
	glMultMatrixf(&m[0][0]) ;

	newModel = 0 ;
}

void GlutWin_ATB::motion(int x, int y)
{
	// AntTweakBar
	if(TwEventMouseMotionGLUT(x, y))
	{
		glutPostRedisplay() ;
		return ;
	}

	if(rotating)
	{
		trackball(lastquat, (2.0f * beginx - W) / W, (H - 2.0f * beginy) / H,
							(2.0f * x - W) / W, (H - 2.0f * y) / H ) ;
		add_quats(lastquat, curquat, curquat) ;
		newModel = 1 ;
		glutPostRedisplay() ;
	}
	else if(scaling)
	{
		trans_z -= 0.1f*(x - beginx);
		trans_z -= 0.1f*(y - beginy);
		newModel = 1 ;
		glutPostRedisplay() ;
	}
	else if(translating)
	{
		trans_x += 0.05f * (x - beginx) ;
		trans_y += 0.05f * (beginy - y) ;
		newModel = 1 ;
		glutPostRedisplay() ;
	}

	instance->myMotion(x, y) ;

	beginx = x ;
	beginy = y ;
}

void GlutWin_ATB::mouse(int button, int state, int x, int y)
{
	// AntTweakBar
	if(TwEventMouseButtonGLUT(button, state, x, y))
	{
		glutPostRedisplay() ;
		return ;
	}

	beginx = x ;
	beginy = y ;

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if(! ((glutGetModifiers() & GLUT_ACTIVE_SHIFT) || (glutGetModifiers() & GLUT_ACTIVE_CTRL)) )
			rotating = 1 ;
	}
	else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		rotating = 0 ;
	else if(button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{
		if(! ((glutGetModifiers() & GLUT_ACTIVE_SHIFT) || (glutGetModifiers() & GLUT_ACTIVE_CTRL)) )
			scaling = 1 ;
	}
	else if(button == GLUT_MIDDLE_BUTTON && state == GLUT_UP)
		scaling = 0 ;
	else if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		if(! ((glutGetModifiers() & GLUT_ACTIVE_SHIFT) || (glutGetModifiers() & GLUT_ACTIVE_CTRL)) )
			translating = 1 ;
	}
	else if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
		translating = 0 ;

	// zoom a la mollette
	if (button == 4 && state == GLUT_UP)
	{
		scalefactor = scalefactor * 1.1f;
		newModel = 1 ;
		glutPostRedisplay() ;
	}
	if (button == 3 && state == GLUT_UP)
	{
		scalefactor = scalefactor * 0.9f;
		newModel = 1 ;
		glutPostRedisplay() ;
	}



	instance->myMouse(button, state, x, y) ;
}

void GlutWin_ATB::passive_mouse(int x, int y)
{
	// AntTweakBar
	if(TwEventMouseMotionGLUT(x, y))
	{
		glutPostRedisplay() ;
		return ;
	}
}

void GlutWin_ATB::keyboard(unsigned char keycode, int x, int y)
{
	// AntTweakBar
	if(TwEventKeyboardGLUT(keycode, x, y))
	{
		glutPostRedisplay() ;
		return ;
	}

	if(keycode == 27)
	{
		instance->exitCallback();
		exit(0) ;
	}
	instance->myKeyboard(keycode, x, y) ;
}

int GlutWin_ATB::mainLoop()
{
	glutMainLoop() ;
	return 0 ;
}

GLfloat GlutWin_ATB::getOrthoScreenRay(int x, int y, Geom::Vec3f& rayA, Geom::Vec3f& rayB)
{
	GLdouble model[16] ;
	GLdouble proj[16] ;
	GLint view[4] ;

	// get Z from depth buffer
	GLfloat prof ;
	glReadPixels(x, H-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &prof) ;

	// get matrices
	glGetDoublev(GL_MODELVIEW_MATRIX, model) ;
	glGetDoublev(GL_PROJECTION_MATRIX, proj) ;
	glGetIntegerv(GL_VIEWPORT, view) ;

	// unproject x,y,0
	GLdouble xx, yy, zz ;
	gluUnProject(x, H-y, 0.0f, model, proj, view, &xx, &yy, &zz) ;
	rayA[0] = float(xx) ;
	rayA[1] = float(yy) ;
	rayA[2] = float(zz) ;

	// unprojet x,y,z
	gluUnProject(x, H-y, prof, model, proj, view, &xx, &yy, &zz) ;
	rayB[0] = float(xx) ;
	rayB[1] = float(yy) ;
	rayB[2] = float(zz) ;

	return prof ;
}

void GlutWin_ATB::capturePNG(const char* filename)
{
/*	ILuint imgName ;
	ilGenImages(1, &imgName) ;
	ilBindImage(imgName) ;
	ilutGLScreen() ;

	// save image
	ilEnable(IL_FILE_OVERWRITE) ;
	ilSaveImage((ILstring)filename) ;
	ilDeleteImages(1, &imgName) ;*/
}

void GlutWin_ATB::printString2D(int x, int y, const std::string& str) 
{
	// PASSAGE EN 2D
	glMatrixMode(GL_PROJECTION) ;
	glPushMatrix() ;
	glLoadIdentity() ;
	glOrtho(0, W, H, 0, -1, 1) ;
	glMatrixMode(GL_MODELVIEW) ;
	glPushMatrix() ;
	glLoadIdentity() ;
	glDisable(GL_DEPTH_TEST) ;
	glDisable(GL_LIGHTING) ;

	unsigned char blank[180];
	for (unsigned int i=0; i< 180; ++i)
		blank[i]=(unsigned char)(0);

	// AFFICHAGE EN 2D
	int x1 = x ;
	for(unsigned int i = 0; i < str.length(); ++i)
	{
		char c = str[i] ;
		if(c == '\n')
		{
			x1 = x ;
			y += 14 ;
		}
		glRasterPos2i(x1,y+2);
		glDrawPixels(12,15,GL_LUMINANCE,GL_UNSIGNED_BYTE, blank);

		glRasterPos2i(x1, y) ;
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c) ;
		x1 += glutBitmapWidth(GLUT_BITMAP_HELVETICA_12, c) ;
	}

	// RETOUR EN 3D
	glMatrixMode(GL_PROJECTION) ;
	glPopMatrix() ;
	glMatrixMode(GL_MODELVIEW) ;
	glPopMatrix() ;
	glEnable(GL_DEPTH_TEST) ;
	glEnable(GL_LIGHTING) ;
}


} // namespace Utils

} // namespace CGoGN
