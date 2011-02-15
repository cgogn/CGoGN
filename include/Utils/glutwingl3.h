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

#ifndef __GLUTWIN_GL3_
#define __GLUTWIN_GL3_

#include "Utils/os_spec.h"
#include "Utils/GLSLShader.h"

#define __X_GL_H
#define GLAPIENTRY

#define GL3_PROTOTYPES
#include "Utils/GL3/gl3.h"
#define __gl_h_

#include <GL/freeglut.h>

#ifndef WIN32
	#include <GL/glx.h>
#endif

#include "Utils/trackball.h"
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <string>
#include <stack>

#include "Geometry/vector_gen.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_projection.hpp"
#include "glm/gtc/matrix_transform.hpp"
namespace CGoGN
{

namespace Utils
{

enum ParameterSpec { DIFFUSE_MATERIAL=1,SPECULAR_MATERIAL=2,AMBIENT_MATERIAL=3,SHININESS=4,DIFFUSE_LIGHT=5,SPECULAR_LIGHT,AMBIENT_LIGHT,LIGHT_POSITION};

APIEXPORT class SimpleGlutWinGL3
{
protected:

	// Matrix
	static glm::mat4 m_projection_matrix;
	static glm::mat4 m_modelView_matrix;
	static std::stack<glm::mat4> m_stack_mv;

	// materials
	static glm::vec4 m_materialAmbient;
	static glm::vec4 m_materialDiffuse;
	static glm::vec4 m_materialSpecular;
	static float m_materialShininess;

	// light
	static glm::vec4 m_lightPosition;
	static glm::vec4 m_lightDiffuse;
	static glm::vec4 m_lightSpecular;
	static glm::vec4 m_lightAmbient;

	// internal values used for the manipulation of object
	static int spinning;
	static int moving;
	static int beginx;
	static int beginy;
	static float curquat[4];
	static float lastquat[4];
	static int newModel;
	static int scaling;
	static int translating;
	static float scalefactor;
	static float foc;

	static float trans_x;
	static float trans_y;
	static float trans_z;

	// width and height of windows
	static int W;
	static int H;

	static bool m_noMouse;

	static float m_obj_sc;
	static glm::vec3 m_obj_pos;

	// are shaders initialized ?
	bool shaderOk;
	// table of shaders
	static Utils::GLSLShader* m_current_shaders;

	// internal callbacks
	static void redraw(void);

	static void reshape(int w, int h);

	static void animate(void);

	static void motion(int x, int y);

	static void vis(int visible);

	static void mouse(int button, int state, int x, int y);

	static void keyboard(unsigned char keycode, int x, int y);

	static void recalcModelView();

	static SimpleGlutWinGL3* instance;

protected:
	static GLfloat lightZeroPosition[4];

	static GLfloat lightZeroColor[4];

	virtual void myKeyboard(unsigned char, int, int) {}

	virtual void myRedraw() {}

	virtual void myMouseMotion(int x, int y) {}

	virtual void myMouseClick(int button, int state, int x, int y) {}

#ifdef WIN32
	HDC m_drawable;
	HGLRC m_context;
#else
	Display* m_dpy;
	GLXDrawable m_drawable;
	GLXContext m_context;
#endif

public:
	//! Others initialization: should be overloaded in derived classes
	virtual void init() {};

	/**
	* launch the interaction loop
	*/
	int mainLoop();

	/**
	* constructor
	*/
	SimpleGlutWinGL3(int* argc, char **argv, int winX, int winY);

	/**
	* destructor
	*/
	virtual ~SimpleGlutWinGL3() {}

	/**
	 * set the focale distance (for a screen width of 2), default value is 1
	 */
	static void setFoc(float df) {if (df>3.0f) df=3.0f; if (df<0.2f) df=0.2f;  foc = df;}

	/**
	 * get the focale distance
	 */
	static float getFoc() {return foc;}

	/**
	 * get the focale distance
	 */
	static float getScale() {return scalefactor/foc;}

	/**
	 * get an ray from A to B  deduce from screen coordinates x,y
	 * @param x x in screen coordinates (from glut)
	 * @param y y in screen coordinates (from glut)
	 * @param rayA begin point of ray (close to the screen)
	 * @param rayB end point of ray (far from the screen)
	 * @return the depth value from depth buffer
	 */
	GLfloat getOrthoScreenRay(int x, int y, Geom::Vec3f& rayA, Geom::Vec3f& rayB);

	/**
	 * get windows width
	 */
	int getWinWidth() { return W;}

	/**
	 * get windows heigth
	 */
	int getWinHeight() { return H;}


	/**
	 * capture the OpenGL viewport and save it into a png image
	 * in fact format of image is deduced from filename (thanks to DevIL)
	 */
	void capturePNG(const char* filename);

	/**
	* Print text in 2D on screen
	*
	* @param x screen coordinate (in pixel)
	* @param y screen coordinate (in pixel)
	* @param str strin to print: \n is authorized (and handled)
	*/
//	void printString2D(int x, int y, const std::string& str);

	/**
	* callback called just before exiting (ESC pressed)
	*/
	virtual void exitCallback() {}

	/**
	* say to interface that it does no more use mouse to move around the scene
	*/
	void setNoMouse(bool b) { m_noMouse = b; }

	/**
	 * release OpenGL context
	 */
	void releaseContext();

	/**
	 * use initial OpenGL context
	 */
	void useContext();

	/**
	 * equivalent to old school glRotate
	 */
	static void oglRotate(float angle, float x, float y, float z);

	/**
	 * equivalent to old school glTranslate
	 */
	static void oglTranslate(float tx, float ty, float tz);

	/**
	 * equivalent to old school glScale
	 */
	static void oglScale(float sx, float sy, float sz);

	/**
	 * pseudo-equivalent to old school glPushMatrix
	 */
	static void oglPushModelViewMatrix();

	/**
	 * pseudo-equivalent to old school glPopMatrix
	 */
	static bool oglPopModelViewMatrix();

	static void setModelViewProjectionMatrix(Utils::GLSLShader* current);

	void setParamObject(float width, float* pos);

	void setCurrentShader(Utils::GLSLShader* sh);
};

} // namespace Utils
} // namespace CGoGN

#endif

