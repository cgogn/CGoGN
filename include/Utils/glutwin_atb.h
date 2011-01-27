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

#ifndef __GLUTWIN_ATB
#define __GLUTWIN_ATB

#include <vector>
#include "Utils/os_spec.h"
#include "Utils/GLSLShader.h"

#define __X_GL_H
#define GLAPIENTRY
#include <GL/glut.h>

#include "Utils/trackball.h"
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <string>

#include "AntTweakBar.h"
#include "Geometry/vector_gen.h"


// a function to ease the definition of new enums in AntTweakBar
inline TwType TwDefineEnum(const char* name, const std::string& in)
{
	std::vector<TwEnumVal> vals ;
	std::vector<std::string> fields ;

	char separator=',' ;
	int length = (int)in.length() ;
	int start = 0 ;
	int end = 0 ;
	while(start < length) {
		end = (int)in.find(separator,start) ;
		if(end < 0) {
			end = length ;
		}
		if(end - start > 0) {
			while(in[start]==' ' && start < end) { start++ ; }
			while(in[end] == ' ' && end > start) { end-- ; }
			if(end - start > 0) {
				fields.push_back(in.substr(start, end - start)) ;
			}
		}
		start = end + 1 ;
	}

	vals.resize(fields.size()) ;
	for(unsigned int i=0; i<fields.size(); i++) { // Create the enum description for AntTweakBar
		vals[i].Value = i ;
		vals[i].Label = &(fields[i][0]) ;
	}

	return TwDefineEnum(name, &(vals[0]), (unsigned int)vals.size()) ;
}


namespace CGoGN
{

namespace Utils
{


class GlutWin_ATB
{
protected:
	static GlutWin_ATB* instance ;

	// internal values used for the manipulation of object
	static int rotating ;
	static int scaling ;
	static int translating ;
	static int beginx ;
	static int beginy ;
	static float curquat[4] ;
	static float lastquat[4] ;
	static int newModel ;
	static float scalefactor ;
	static float focal ;
	static float trans_x ;
	static float trans_y ;
	static float trans_z ;

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
	static GLfloat lightZeroPosition[4] ;
	static GLfloat lightZeroColor[4] ;

	virtual void myKeyboard(unsigned char, int, int) {}
	virtual void myMouse(int, int, int, int) {}
	virtual void myMotion(int, int) {}
	virtual void myRedraw() {}

public:
	GlutWin_ATB(int* argc, char **argv, int winX, int winY) ;
	~GlutWin_ATB() ;
	
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
	void capturePNG(const char* filename) ;

	/**
	* Print text in 2D on screen
	* 
	* @param x screen coordinate (in pixel)
	* @param y screen coordinate (in pixel)
	* @param str strin to print: \n is authorized (and handled)
	*/
	void printString2D(int x, int y, const std::string& str) ;
	
	/**
	* callback called just before exiting (ESC pressed)
	*/
	virtual void exitCallback() {}
} ;

} // namespace Utils

} // namespace CGoGN

#endif
