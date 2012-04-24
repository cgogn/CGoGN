#include <iostream>
#include <stdlib.h>
#include "Utils/os_spec.h"

#include "GL/glut.h"

#include <hypermap/hyperdart.h>
#include <hypermap/ehypermap1.h>

using namespace CGoGN;

// definition of the type of the dart
class MyDP: public DP::DefaultDartParameters
{
public:
	static const unsigned	nbInvolutions = 0;
	static const unsigned	nbPermutations = 2;
};


// some typedef to simplify 
typedef HMDartObj<MyDP> ODART;
typedef eHyperMap1<ODART> HMap;
typedef HMap::Dart Dart;


// more easy to write
#define NIL (map.nil())

// a function to draw the hyper map
void drawHM(HMap& map, float length)
{
	std::vector<gmtl::Vec3f> vertices;
	vertices.reserve(2048);

	std::vector<gmtl::Vec3f> alpha0s;
	alpha0s.reserve(2048);

	std::vector<gmtl::Vec3f> alpha1s;
	alpha0s.reserve(2048);

	unsigned int lab=0;

	// lest's draw the darts
	glColor3f(1.0f,1.0f,1.0f);
	glLineWidth(2.0);
	glBegin(GL_LINES);
	for (Dart d = map.begin(); d != map.end(); ++d)
	{	
		d->setLabel(lab++);
		Dart e = map.otherInEdge(d);
		const gmtl::Vec3f& A = map.getVertexPosition(d);
		if (e != NIL)
		{
			const gmtl::Vec3f& B = map.getVertexPosition(e);
			// shrink a little bit
			gmtl::Vec3f C = (A + B)/2.0f;
			gmtl::Vec3f P1 = A*0.9f + C*0.1f;
			gmtl::Vec3f P2 = C*0.9f + A*0.1f;
			glVertex3fv(P1.getData());
			glVertex3fv(P2.getData());
			vertices.push_back(P1);
			alpha0s.push_back(P2);
			// now the small bar at end 
 			gmtl:: Vec3f vp = P2-P1;
 			vp[2] = vp[1];
 			vp[1] = vp[0];
 			vp[0] = -vp[2];
 			vp[2] = 0.0f;
 			gmtl::normalize(vp);
 			P1 = P2 - (length*vp);
 			P2 = P2 + (length*vp);
 			glVertex3fv(P1.getData());
 			glVertex3fv(P2.getData());
		}
		else
		{
			vertices.push_back(A);
			alpha0s.push_back(A);
		}
	}
	glEnd();

	// lest's draw the alphas
	glLineWidth(4.0);
	glBegin(GL_LINES);
	for (Dart d = map.begin(); d != map.end(); ++d)
	{
		unsigned int l = d->getLabel();
		unsigned int  l0;
		unsigned int  l1;
		Dart e = map.alpha0(d);
		if (e!= NIL)
		{
			l0 = e->getLabel();
			glColor3f(0.5f,0.0f,0.0f);
			glVertex3fv(alpha0s[l].getData());
			glColor3f(1.0f,0.7f,0.2f);
			glVertex3fv(alpha0s[l0].getData());
		}
		e = map.alpha1(d);
		if (e!= NIL)
		{
			l1 = e->getLabel();
			glColor3f(0.0f,0.5f,0.0f);
			glVertex3fv(vertices[l].getData());
			glColor3f(0.7f,1.0f,0.2f);
			glVertex3fv(vertices[l1].getData());
		}
	}
	glEnd();

	// points for vertex extremities of darts
	glPointSize(7.0f);
	glColor3f(1.0f,1.0f,1.0f);
	glBegin(GL_POINTS);

	for (std::vector<gmtl::Vec3f>::iterator it = vertices.begin(); it!=vertices.end(); ++it)
	{
		glVertex3fv((*it).getData());
	}
	glEnd();
}


// a function to create a big face from isolated darts
void bigFace(HMap& map)
{
	int pair=0;
	for (Dart d = map.begin(); d != map.end(); ++d)
	{
		Dart e = d;
		e++;
		if (e == NIL)
			e = map.begin();
		if (pair%2)
			map.alpha0Sew(d,e);
		else
			map.alpha1Sew(d,e);	
		pair++;
	}
}

void unsewAll0(HMap& map)
{
	for (Dart d = map.begin(); d != map.end(); ++d)
	{
		Dart e = map.alpha0(d);
		if (e!= NIL)
			map.alpha0Unsew(d);
	}
}

void unsewAll1(HMap& map)
{
	for (Dart d = map.begin(); d != map.end(); ++d)
	{
		Dart e = map.alpha1(d);
		if (e!= NIL)
			map.alpha1Unsew(d);
	}
}

#undef NIL




// Hyper Map (declared here for easy access in glut function)
HMap myMap;

//	glut window size
int W,H;

//Redraw Callback
void redraw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_LIGHTING);

	drawHM(myMap,2.0f);

	glutSwapBuffers();
}

//Reshape callback (window resizing)
void reshape(int w, int h)
{
  glViewport(0, 0, w, h);
  W = w;
  H = h;
}

//Mouse Callback
void  mouseClick(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN ) 
	{
		// get back X,Y
		GLint viewport[4];
		GLdouble modelview[16];
		GLdouble projection[16];
		GLdouble winX, winY, winZ;
		GLdouble posX, posY, posZ;
		glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
		glGetDoublev( GL_PROJECTION_MATRIX, projection );
		glGetIntegerv( GL_VIEWPORT, viewport );
		gluUnProject( double(x), double(H-y), 0.0,  modelview, projection, viewport, &posX, &posY, &posZ);

		// create a dart 
		Dart d = myMap.newEmbDart(posX, posY,0.0f);
	}

	glutPostRedisplay();
}

//Keyboard Callback
void keyboard(unsigned char keycode, int x, int y)
{
	switch(keycode)
	{
		case 27: //ESC
			exit(0);
			break;
		case 'a':
			bigFace(myMap);
			break;;
		case 'b':
			unsewAll0(myMap);
			break;;
		case 'c':
			unsewAll1(myMap);
			break;;
		case ' ':
			while (!myMap.empty())
				myMap.deleteDart(myMap.begin());
			break;

		default:
			break;
	}

  glutPostRedisplay();

}


// main: some glut & gl inits
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("ConvexHull");
	glutReshapeWindow(800,800);
	W=800; H=800;
	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouseClick);
	glutKeyboardFunc(keyboard);

	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glViewport(0, 0, W, H); 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho( -W, W,-H, H, -10.0f, 10.0f);
	glMatrixMode(GL_MODELVIEW);

	glutMainLoop();
	return 0;
}
