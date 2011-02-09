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

#include <stdlib.h>

#include "Utils/glutwin.h"
#include <math.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include <iostream>

namespace CGoGN
{

namespace Utils
{

SimpleGlutWin* SimpleGlutWin::instance=NULL;
float SimpleGlutWin::lightZeroPosition[4]= {0.0f,0.0f,10000.0f,1.0f};
float SimpleGlutWin::lightZeroColor[4]= {0.9f,0.9f,0.9f,1.0f};
float SimpleGlutWin::scalefactor = 1.0f;
float SimpleGlutWin::trans_x=0.;
float SimpleGlutWin::trans_y=0.;
float SimpleGlutWin::trans_z=0.;

float SimpleGlutWin::foc=2.0f;

float SimpleGlutWin::curquat[4]={0.0f,0.0f,0.0f,0.0f};
float SimpleGlutWin::lastquat[4]={0.0f,0.0f,0.0f,0.0f};
int SimpleGlutWin::spinning = 0;
int SimpleGlutWin::moving = 0;
int SimpleGlutWin::newModel = 1;
int SimpleGlutWin::translating=0;
int SimpleGlutWin::scaling=0;
int SimpleGlutWin::beginx=0;
int SimpleGlutWin::beginy=0;
int SimpleGlutWin::W=0;
int SimpleGlutWin::H=0;
bool SimpleGlutWin::m_noMouse=false;

SimpleGlutWin::SimpleGlutWin(int* argc, char **argv, int winX, int winY)
{
	instance = this;
	spinning = 0;
	moving = 0;
	newModel = 1;
	scalefactor = 1.0f;
	trans_x=0.;
	trans_y=0.;
	trans_z=-50.0f;

	std::cout << "Initialisation Glut" << std::endl;
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow(argv[0]);
	glutReshapeWindow(winX,winY);
  	W=winX; H=winY;
	trackball(curquat, 0.0f, 0.0f, 0.0f, 0.0f);
	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);
	glutVisibilityFunc(vis);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);

	std::cout << "Initialisation OpenGL" << std::endl;
	shaderOk = Utils::GLSLShader::init();

	glViewport(0, 0, W, H);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0f,1.0f,-1.0f*H/W,1.0f*H/W,foc,500.0f);
	glMatrixMode(GL_MODELVIEW);

	glClearColor(0.1,0.1,0.3,0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glEnable(GL_LIGHTING);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightZeroPosition);
	glEnable(GL_LIGHT0);

	//Store context infos
#ifdef WIN32
	m_drawable = wglGetCurrentDC();
	m_context = wglGetCurrentContext();
#else
	m_dpy = glXGetCurrentDisplay();
	m_drawable = glXGetCurrentDrawable();
	m_context = glXGetCurrentContext();
#endif
	// Call other initialization (possibly overloaded in instances)
	instance->init();
}

void SimpleGlutWin::releaseContext()
{
#ifdef WIN32
	wglMakeCurrent(NULL,NULL);
#else
	glxMakeCurrent(m_dpy,None,NULL);
#endif
}

void SimpleGlutWin::useContext()
{
#ifdef WIN32
	wglMakeCurrent(m_drawable, m_context);
#else
	glxMakeCurrent(m_dpy, m_drawable, m_context);
#endif

}



void SimpleGlutWin::recalcModelView(void)
{
	GLfloat m[4][4];

	glPopMatrix();
	glPushMatrix();

	glTranslatef(trans_x,trans_y,trans_z+foc);

	build_rotmatrix(m, curquat);
	glMultMatrixf(&m[0][0]);

	float sc = getScale();
	glScalef(sc,sc,sc);

	newModel = 0;
}

void SimpleGlutWin::redraw(void)
{
	if (newModel) 
	{
	    recalcModelView();
	}
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	instance->myRedraw();

	glutSwapBuffers();
}

void SimpleGlutWin::reshape(int w, int h)
{
	if (w>0) W = w;
	if (h>0) H = h;
	
	glViewport(0,0,W,H);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0f,1.0f,-1.0f*H/W,1.0f*H/W,foc,500.0f);
	glMatrixMode(GL_MODELVIEW);
	
	recalcModelView();
}

void SimpleGlutWin::animate(void)
{
	add_quats(lastquat, curquat, curquat);
	newModel = 1;
	glutPostRedisplay();
}

void SimpleGlutWin::motion(int x, int y)
{
	if (!m_noMouse)
	{
		if (scaling || translating) 
		{
			if (scaling) 
			{
				scalefactor = scalefactor * (1.0f + (((float) (beginy - y)) / H));
			}
			else if (translating) 
			{
				trans_x += 0.01f*(x - beginx);
				trans_y += 0.01f*(beginy - y);
			}
			beginx = x;
			beginy = y;
			newModel = 1;
			glutPostRedisplay();
			return;
		}
		
		if (moving) 
		{
			trackball(lastquat, (2.0f * beginx - W) / W,(H - 2.0f * beginy) / H,
								(2.0f * x - W) / W,(H - 2.0f * y) / H );
			beginx = x;
			beginy = y;
			spinning = 1;
			//    glutIdleFunc(animate);
			animate();
		}
	}

	instance->myMouseMotion(x, y);
}

void SimpleGlutWin::vis(int visible)
{
	if (spinning) 
	{
		glutIdleFunc(NULL);
	}
//	if (visible == GLUT_VISIBLE)
//	{
//		if (spinning) glutIdleFunc(animate);
//	}
//	else
//	{
//		if (spinning) glutIdleFunc(NULL);
//	}
}

void SimpleGlutWin::mouse(int button, int state, int x, int y)
{
	if (!m_noMouse)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
		{
			spinning = 0;
			glutIdleFunc(NULL);
			moving = 1;
			beginx = x;
			beginy = y;
			scaling = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) != 0;
			translating = (glutGetModifiers() & GLUT_ACTIVE_CTRL) != 0;
		}
		else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) 
		{
			moving = 0;
		}
	}
	instance->myMouseClick(button, state, x, y);
}

void SimpleGlutWin::keyboard(unsigned char keycode, int x, int y)
{
	if (keycode == 27)
	{
		instance->exitCallback();
		exit(0);
	}
	instance->myKeyboard(keycode, x, y);
}

int SimpleGlutWin::mainLoop()
{
	glutMainLoop();
	return 0;
}

void SimpleGlutWin::capturePNG(const char* filename)
{
	ILuint imgName;
	ilGenImages(1,&imgName);
	ilBindImage(imgName);
	ilutGLScreen();
	
	// save image
	ilEnable(IL_FILE_OVERWRITE);
	ilSaveImage((ILstring) filename);
	ilDeleteImages(1,&imgName);
}

GLfloat SimpleGlutWin::getOrthoScreenRay(int x, int y, Geom::Vec3f& rayA, Geom::Vec3f& rayB)
{
	GLdouble model[16];
	GLdouble proj[16];
	GLint view[4];
	
	// get Z from depth buffer
	GLfloat prof;
	glReadPixels(x,H-y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&prof);
	
	// get matrices
	glGetDoublev(GL_MODELVIEW_MATRIX,model);
	glGetDoublev(GL_PROJECTION_MATRIX,proj);
	glGetIntegerv(GL_VIEWPORT,view);
	
	// unproject x,y,0
	GLdouble xx, yy, zz;
	gluUnProject(x, H-y, 0.0f, model, proj, view, &xx, &yy, &zz);
	rayA[0] = float(xx);
	rayA[1] = float(yy);
	rayA[2] = float(zz);
	
	// unprojet x,y,z
	gluUnProject(x, H-y, prof, model, proj, view, &xx, &yy, &zz);
	rayB[0] = float(xx);
	rayB[1] = float(yy);
	rayB[2] = float(zz);
	
	return prof;
}

void SimpleGlutWin::printString2D(int x, int y, const std::string& str) 
{
	// PASSAGE EN 2D
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, W, H,0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	
	// AFFICHAGE EN 2D
	int x1 = x;
	for (unsigned i=0; i<str.length(); ++i) 
	{
		char c = str[i];
		if (c=='\n') 
		{
			x1 = x;
			y += 14;
		}
		glRasterPos2i(x1,y);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
		x1 += glutBitmapWidth(GLUT_BITMAP_HELVETICA_12,c);
	}
	
	// RETOUR EN 3D
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

} // namespace Utils
} // namespace CGoGN
