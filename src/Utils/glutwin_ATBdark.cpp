#include "Utils/glutwin_ATBdark.h"

namespace CGoGN {
namespace Utils {

GlutWin_ATBdark* GlutWin_ATBdark::instance = NULL ;

int GlutWin_ATBdark::rotating = 0 ;
int GlutWin_ATBdark::scaling = 0 ;
int GlutWin_ATBdark::translating = 0 ;
int GlutWin_ATBdark::beginPos[2] = {0,0} ;
float GlutWin_ATBdark::curquat[4] = { 0.0f, 0.0f, 0.0f, 0.0f } ;
float GlutWin_ATBdark::lastquat[4] = { 0.0f, 0.0f, 0.0f, 0.0f } ;
bool GlutWin_ATBdark::newModel = true ;
float GlutWin_ATBdark::scalefactor = 1.0f ;
float GlutWin_ATBdark::focal = 2.5f ;
float GlutWin_ATBdark::trans[3] = { 0.0f, 0.0f, -50.0f} ;

int GlutWin_ATBdark::W = 0 ;
int GlutWin_ATBdark::H = 0 ;



GlutWin_ATBdark::GlutWin_ATBdark(int* argc, char **argv, int winX, int winY)
{

	instance = this ;

	newModel = true ;
	scalefactor = 1.0f ;
//	trans[0] = 0.0f ;
//	trans[1] = 0.0f ;
//	trans[2] = -50.0f ;

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
}

GlutWin_ATBdark::~GlutWin_ATBdark() {
	// AntTweakBar
	TwTerminate() ;
}


void GlutWin_ATBdark::redraw()
{
	if(newModel)
		recalcModelView() ;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;

	instance->myRedraw() ;
	// AntTweakBar
	TwDraw() ;

	glutSwapBuffers() ;
}

void GlutWin_ATBdark::reshape(int w, int h)
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

void GlutWin_ATBdark::recalcModelView()
{
	GLfloat m[4][4] ;

		glPopMatrix() ;
		glPushMatrix() ;

		glTranslatef(trans[0], trans[1], trans[2]+focal) ;

		build_rotmatrix(m, curquat) ;
		glMultMatrixf(&m[0][0]) ;

		float sc = getScale() ;
		glScalef(sc, sc, sc) ;

		newModel = false ;
}

void GlutWin_ATBdark::motion(int x, int y)
{
	// AntTweakBar
		if(TwEventMouseMotionGLUT(x, y))
		{
			glutPostRedisplay() ;
			return ;
		}

		if(rotating)
		{
			trackball(lastquat, (2.0f * beginPos[0] - W) / W, (H - 2.0f * beginPos[1]) / H,
								(2.0f * x - W) / W, (H - 2.0f * y) / H ) ;
			add_quats(lastquat, curquat, curquat) ;
			newModel = true ;
			glutPostRedisplay() ;
		}
		else if(scaling)
		{
			scalefactor = scalefactor * (1.0f + (((float) (beginPos[1] - y)) / H)) ;
			newModel = true ;
			glutPostRedisplay() ;
		}
		else if(translating)
		{
			trans[0] += 0.01f * (x - beginPos[0]) ;
			trans[1] += 0.01f * (beginPos[1] - y) ;
			newModel = true ;
			glutPostRedisplay() ;
		}

		instance->myMotion(x, y) ;

		beginPos[0] = x ;
		beginPos[1] = y ;
}

void GlutWin_ATBdark::mouse(int button, int state, int x, int y)
{
	// AntTweakBar
	if(TwEventMouseButtonGLUT(button, state, x, y))
	{
		glutPostRedisplay() ;
		return ;
	}

	beginPos[0] = x ;
	beginPos[1] = y ;

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
		scalefactor = scalefactor * 0.9f;
		newModel = true ;
		glutPostRedisplay() ;
	}
	if (button == 3 && state == GLUT_UP)
	{
		scalefactor = scalefactor * 1.1f;
		newModel = true ;
		glutPostRedisplay() ;
	}



	instance->myMouse(button, state, x, y) ;
}

void GlutWin_ATBdark::passive_mouse(int x, int y)
{
	// AntTweakBar
	if(TwEventMouseMotionGLUT(x, y))
	{
		glutPostRedisplay() ;
		return ;
	}
}

void GlutWin_ATBdark::keyboard(unsigned char keycode, int x, int y)
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

int GlutWin_ATBdark::mainLoop()
{
	glutMainLoop() ;
	return 0 ;
}

GLfloat GlutWin_ATBdark::getOrthoScreenRay(int x, int y, Geom::Vec3f& rayA, Geom::Vec3f& rayB)
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

//void GlutWin_ATBdark::capturePNG(const char* filename)
//{
//	ILuint imgName ;
//	ilGenImages(1, &imgName) ;
//	ilBindImage(imgName) ;
//	ilutGLScreen() ;
//
//	// save image
//	ilEnable(IL_FILE_OVERWRITE) ;
//	ilSaveImage((ILstring)filename) ;
//	ilDeleteImages(1, &imgName) ;
//}


} // namespace Utils
} // namespace CGoGN
