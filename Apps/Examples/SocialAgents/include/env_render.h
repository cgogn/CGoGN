#ifndef ENVMAP_RENDER
#define ENVMAP_RENDER

#include "env_map.h"
#include <GL/glut.h>


static void renderPoint(EnvMap& m, Dart& d)
{
	PFP::VEC3 p = m.position[d];
	glBegin(GL_POINTS);
		glVertex3f(p[0],p[1],p[2]);
	glEnd();
}

static void renderDart(EnvMap& m, Dart d)
{
	PFP::VEC3 p1 = m.position[d];
	PFP::VEC3 p2 = m.position[m.map.phi1(d)];

	glBegin(GL_LINES);
		glVertex3f(p1[0],p1[1],p1[2]);
		glVertex3f(p2[0],p2[1],p2[2]);
	glEnd();
}

static void renderFace(EnvMap& m, Dart& d)
{
	Dart dd=d;
	do {
		renderDart(m,dd);
		dd = m.map.phi1(dd);
	}while(dd!=d);
}

static void renderCellOfDim(EnvMap& m,Dart d, unsigned int dim)
{
	switch(dim) {
		case 0 : renderPoint(m,d); break;
		case 1 : renderDart(m,d);  break;
		case 2 : renderFace(m,d);  break;
	}
}

static void renderAllPoints(EnvMap& m)
{
	glBegin(GL_POINTS);
	for(Dart d=m.map.begin();d!=m.map.end();m.map.next(d)) {
		PFP::VEC3 p = m.position[d];
		glVertex3f(p[0],p[1],p[2]);
	}
	glEnd();
}

static void renderPredictionTriangle(EnvMap& m, Dart d, PFP::VEC3 p)
{
	glBegin(GL_LINE_LOOP);
		glVertex3fv(&p[0]);
		glVertex3fv(& m.position[d][0]);
		glVertex3fv(& m.position[m.map.phi1(d)][0]);
	glEnd();
}


#endif
