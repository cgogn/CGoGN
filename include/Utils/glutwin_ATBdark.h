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

#ifndef __MYGLUTWIN_
#define __MYGLUTWIN_

#include <vector>
#include "Utils/os_spec.h"
#include "Utils/GLSLShader.h"

#define __X_GL_H
#define GLAPIENTRY
#include <GL/glut.h>

#include "Utils/trackball.h"

#include "AntTweakBar.h"
#include "Geometry/vector_gen.h"


namespace CGoGN {

namespace Utils {

/**
 * Glut window with Antweakbar support and enlightning disabled
 */
class GlutWin_ATBdark
{

protected:
	static GlutWin_ATBdark* instance ;

	// internal values used for the manipulation of object
	static int rotating ;
	static int scaling ;
	static int translating ;
	static int beginPos[2] ;
	static float curquat[4] ;
	static float lastquat[4] ;
	static bool newModel ;
	static float scalefactor ;
	static float focal ;
	static float trans[3] ;

	// width and height of windows
	static int W ;
	static int H ;

	// are shaders initialized ?
	bool shaderOk ;
	// table of shaders
	Utils::GLSLShader shaders[8] ;

	// internal callbacks
	static void redraw() ;
	static void recalcModelView() ;
	static void motion(int x, int y) ;
	static void mouse(int button, int state, int x, int y) ;
	static void passive_mouse(int x, int y) ;
	static void keyboard(unsigned char keycode, int x, int y) ;

protected:
	virtual void myKeyboard(unsigned char, int, int) {}
	virtual void myMouse(int, int, int, int) {}
	virtual void myMotion(int, int) {}
	virtual void myRedraw() {}

public:
	GlutWin_ATBdark(int* argc, char **argv, int winX, int winY) ;
	~GlutWin_ATBdark() ;

	int mainLoop() ;
	static void reshape(int w, int h) ;

	static void setFocal(float df) { focal = df ; }
	static float getFocal() { return focal ; }
	static float getScale() { return scalefactor / focal ; }

	int getWinWidth() { return W ; }
	int getWinHeight() { return H ; }

	float* getSceneQuaternion() { return curquat ; }

	/**
	 * get an ray from A to B  deduce from screen coordinates x,y
	 * @param x x in screen coordinates (from glut)
	 * @param y y in screen coordinates (from glut)
	 * @param rayA begin point of ray (close to the screen)
	 * @param rayB end point of ray (far from the screen)
	 * @return the depth value from depth buffer
	 */
	GLfloat getOrthoScreenRay(int x, int y, Geom::Vec3f& rayA, Geom::Vec3f& rayB) ;

	/**
	 * capture the OpenGL viewport and save it into a png image
	 * in fact format of image is deduced from filename (thanks to DevIL)
	 */
//	void capturePNG(const char* filename) ;

	/**
	* callback called just before exiting (ESC pressed)
	*/
	virtual void exitCallback() {}
};


}

}


#endif
