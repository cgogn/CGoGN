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

#include <iostream>
#include <time.h>
#include <algorithm>

#include "Utils/GLSLShader.h"
#include "Utils/glutwin.h"

#include "Topology/generic/parameters.h"
#include "Topology/map/map2.h"
#include "Topology/generic/embeddedMap2.h"

#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"

#include "Algo/Render/vbo_MapRender.h"
#include "Algo/Render/topo_vboRender.h"
#include "Algo/Geometry/normal.h"

#include "Algo/Selection/raySelector.h"

#include "Algo/Import/import.h"
#include "Algo/Geometry/boundingbox.h"


/// pour simplifier l'ecriture du code
using namespace CGoGN;


/// definition de la structure qui decrit le type de carte utilise

struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef EmbeddedMap2<Map2> MAP;
};


/// definition de la carte en  global, plus facile 
PFP::MAP myMap;

// handler d'attribut de position par sommet
AttributeHandler<PFP::VEC3> position;

// handler d'attribut de normale par sommet
AttributeHandler<PFP::VEC3> normal;

/// fonction qui renvoit vrai (appliquée à un brin)
//SelectorTrue allDarts;

/// encore 1 typedef pour simplifier l'ecriture du code
typedef PFP::VEC3 Point3D;

/// pile des brins selectionnes (6 max)
std::vector<Dart> selected_darts;

/// 3 brins de la carte encore en global pour pouvoir utiliser le callback clavier de glut !
Dart d_carre;
Dart d_tri;
Dart d_maison;

///Fonctions a connaitre:
///
/// phi1(d), phi2(d) : evident ?
/// phi<21>(d) equivalent a phi2(phi1(d)), phi<121212> equivalent a ....
///
///  Creer un face: newOrientedFace(nb_cotes); renvoit un Dart
///






///  Fonction Carre: construit un carre et renvoit un brin
///

Dart Carre()
{
}


///  Fonction Triangle: construit un triangle et renvoit un brin

Dart Triangle()
{

}


///  Fonction Colle: effectue la couture du carre et du triangle, le carre ne bouge pas
/// Attention aux plongements des sommets topologiquement modifies
/// Fonction a utiliser: sewFaces(d,e): colle deux faces par d et e

/// appel clavier touche 'c'

void Colle(Dart d, Dart e)
{
}


///  Fonction Colle: effectue la couture du carre et du triangle, les points de l'arete commune
/// se placent a mi-chemin

/// appel clavier touche 'C'

void ColleMilieu(Dart d, Dart e)
{

}



/// Fonction qui construite une "maison" (un carré et un triangle collé)
/// faire la topologie puis plonger le tout
/// on renvoit le brin du carre qui est colle au triangle
/// Utiliser embedCell

Dart Maison()
{

}


/// decouper le carre de la maison en 2 carre (un a gauche un a droite)
/// le triangle du haut devient un carre (de forme triangulaire
/// essayer de couper d'abord uniquement les arêtes pour voir ce qui se passe.
/// deplacer les deux nouveaux sommets 
/// Fonctions à utiliser:
/// 	cutEdge(brin): coupe une arête en deux: *---  devient *---*---
///		cutFace(b1,b2): coupe une face en deux, la nouvelle arête relie les deux sommets portes par b1 et b2

///touche 'x' on selectionne les 2 aretes a couper

void Decoupage(Dart d, Dart dd)
{
}



/// appliquer une homothetie a une face (0.95 par ex) a partir d'un brin
/// Attention on ne connait pas le nombre de cotes de face !!
/// Indice: 2 passes: calculer le centre, puis bouger les sommets
/// appel clavier  touche 'r' pout le carre
/// appel clavier  touche 'R' pout le triangle
void Reduction(Dart e)
{


}


/// faire touner une face !!!
/// chaque sommet avance vers le suivant
/// appel clavier touche 'T' pout le carre
/// appel clavier touche 'U' pout le triangle


void Tourne(Dart d)
{

}



/// Fonction de creation de la carte (appelee par le main)

void createMap()
{
//	d_carre = Carre();
//	d_tri = Triangle();
// 	d_maison = Maison();

}


/// Coupe une face carre en 2 en utilisant cutEdge et splitFace 
/// splitFace(d1,d2): le suivant (phi1) de d1 devient celui de d2
/// et inversement: faire un dessin

void coupe_carre(Dart dd)
{

}





class myGlutWin: public Utils::SimpleGlutWin
{
public:

	/**
	 * position of object
	 */
	PFP::VEC3 gPosObj;

	/**
	 * width of object
	 */
	float gWidthObj;

	Algo::Render::VBO::MapRender_VBO* m_render;

	Algo::Render::VBO::topo_VBORenderMapD* m_render_topo;

	/**
	 * redraw CB
	 */
	void myRedraw();

	/**
	 * keyboard CB
	 */
	void myKeyboard(unsigned char keycode, int x, int y);

	/**
	 * GL initialization
	 */
	void myInitGL();

	/**
	 * dessine les cellules et brins selectionne
	 */
	void drawSelected();

	/**
	 * mise a jour des modifications de la carte dans les buffer OpenGL
	 */
	void updateRender();


	/**
	 * rendering topology ?
	 */
	bool renderTopo;

	/**
	 * rendering help ?
	 */
	bool aff_help;


	myGlutWin(	int* argc, char **argv, int winX, int winY) :
		SimpleGlutWin(argc,argv,winX,winY),
		m_render(NULL),m_render_topo(NULL),aff_help(true) {}
};


/// Variables pour le picking
PFP::VEC3 rayA;
PFP::VEC3 rayB;
std::vector<PFP::VEC3> inters;
std::vector<Dart> d_faces;
std::vector<Dart> d_edges;
std::vector<Dart> d_vertices;


// fonction qui calcule la distance a utiliser pour la selection
float computeSelectRadius(int x, int y, int pixelRadius)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	// get depth
	glReadPixels( x, viewport[3]-y , 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

	// if we have click on background, consider that we are in the middle of space
	// to avoid too big radius
	if (winZ == 1.0f)	// depth vary in [0-1]
		winZ = 0.5f;

	winX = (float)x;
	winY = (float)viewport[3] - (float)y;

//	get first point in object space
	gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
	PFP::VEC3 p(posX,posY,posZ);

//	get second point in object space
	gluUnProject( winX+pixelRadius, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
	PFP::VEC3 q(posX,posY,posZ);

	// compute & return distance
	q -= p;
	return float(q.norm());
}


void myGlutWin::drawSelected()
{
	typedef Dart Dart;

// FACES
	glLineWidth(5.0f);
	for(unsigned int i=0; i < d_faces.size(); ++i)
	{
		// fait varier la couleur du plus pres au plus loin
		float c = float (i) / float (d_faces.size());

		glBegin(GL_LINE_LOOP);
		glColor3f(1.0f - c , c ,0.);
		Dart d = d_faces[i];
		do
		{
			const PFP::VEC3& P = position[d];
			glVertex3fv(P.data());
			d = myMap.phi1(d);
		} while (d!=d_faces[i]);
		glEnd();
	}

//edges
	glLineWidth(7.0f);
	glBegin(GL_LINES);

	for(unsigned int i=0; i < d_edges.size(); ++i)
	{
		// fait varier la couleur du plus pres au plus loin
		float c = float (i) / float (d_edges.size());
		glColor3f(1.0f - c , c ,0.);

		Dart d = d_edges[i];
		const PFP::VEC3& P = position[d];
		glVertex3fv(P.data());
		d = myMap.phi1(d);
		const PFP::VEC3& Q =  position[d];
		glVertex3fv(Q.data());
	}
	glEnd();

//VERTICES
//edges
	glPointSize(9.0f);
	glBegin(GL_POINTS);

	for(unsigned int i=0; i < d_vertices.size(); ++i)
	{
		// fait varier la couleur du plus pres au plus loin
		float c = float (i) / float (d_vertices.size());
		glColor3f(1.0f - c , c ,0.);
		Dart d = d_vertices[i];
		const PFP::VEC3& P = position[d];
		glVertex3fv(P.data());
	}
	glEnd();
	
	glLineWidth(7.0f);
	for(unsigned int i=0; i < selected_darts.size(); ++i)
	{
		// fait varier la couleur du plus pres au plus loin
		Dart d = selected_darts[i];
		switch(i)
		{
			case 0:
				m_render_topo->overdrawDart(d, 5, 1.0f,0.0f,0.0f);
				break;
			case 1:
				m_render_topo->overdrawDart(d, 5, 0.0f,1.0f,0.0f);
				break;
			case 2:
				m_render_topo->overdrawDart(d, 5, 0.0f,0.0f,1.0f);
				break;
			case 3:
				m_render_topo->overdrawDart(d, 5, 1.0f,1.0f,0.0f);
				break;
			case 4:
				m_render_topo->overdrawDart(d, 5, 1.0f,0.0f,1.0f);
				break;
			case 5:
				m_render_topo->overdrawDart(d, 5, 0.0f,1.0f,1.0f);
				break;
			default:
				break;
		}
	}
}

void myGlutWin::myInitGL()
{
	glClearColor(0.2,0.2,0.2,0.);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightZeroPosition);
	glEnable(GL_LIGHT0);
}

void myGlutWin::updateRender()
{
	if (m_render == NULL)
		m_render = new Algo::Render::VBO::MapRender_VBO() ;

	m_render->initPrimitives<PFP>(myMap, SelectorTrue(),Algo::Render::VBO::TRIANGLES);
//	m_render->initPrimitives<PFP>(myMap, SelectorTrue(),Algo::Render::VBO::LINES);
	m_render->updateData(Algo::Render::VBO::POSITIONS, position);

	Algo::Geometry::computeNormalVertices<PFP>(myMap, position, normal) ;
	m_render->updateData(Algo::Render::VBO::NORMALS, normal);


	if (m_render_topo == NULL)
		m_render_topo = new Algo::Render::VBO::topo_VBORenderMapD() ;

	m_render_topo->updateData<PFP>(myMap,position,0.9f,0.9f);
}

void myGlutWin::myRedraw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/// centrage de la scene
	glPushMatrix();
	float sc = 50./gWidthObj;
	glScalef(sc,sc,sc);
	glTranslatef(-gPosObj[0],-gPosObj[1],-gPosObj[2]);

	/// Affichage de la topologie si demande
	glDisable(GL_LIGHTING);

	drawSelected();

	glEnable( GL_POLYGON_OFFSET_FILL );
	glPolygonOffset( 0.2f, 0.2f );
	m_render_topo->drawTopo();

	/// qq setup OpenGL
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_FLAT);
	glFrontFace(GL_CCW);

	/// Rendu faces surlignage
//	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
//	glLineWidth(2.0f);
	/// Vert devant
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glColor3f(0.0f,0.9f,0.3f);
	/// Rouge derriere
	glColorMaterial(GL_BACK, GL_DIFFUSE);
	//glColor3f(0.9f,0.3f,0.3f);
	glColor3f(0.0f,0.9f,0.3f);

	/// decalage pour surlignage non clignotant
	glEnable( GL_POLYGON_OFFSET_FILL );
	glPolygonOffset( 1.0f, 1.0f );

	/// Rendu faces pleines
	glPolygonMode(GL_FRONT,GL_FILL);
	glPolygonMode(GL_BACK,GL_NONE);
	/// Vert devant
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glColor3f(0.1f,0.5f,0.1f);

	/// Rouge derriere
	glColorMaterial(GL_BACK, GL_DIFFUSE);
	//glColor3f(0.5f,0.1f,0.1f);
	glColor3f(0.1f,0.5f,0.1f);

	m_render->draw(Algo::Render::VBO::TRIANGLES) ;

	glDisable( GL_POLYGON_OFFSET_FILL );

	glPopMatrix();

	if (aff_help)
	{
		glColor3f(1.0f,1.0f,1.0f);
		printString2D(10,20,"Keys:\nt: affichage topologie\nf: selection face (position souris)\na: selection aretes\ns: selection sommet\nd: selection brin\n0: vide la pile des brins selectionnes\nD: info brin");
	}
}

void myGlutWin::myKeyboard(unsigned char keycode, int x, int y)
{
	switch(keycode)
	{
	case 't':
		renderTopo = !renderTopo;
		glutPostRedisplay();
		break;

	case '0':
		selected_darts.clear();
		glutPostRedisplay();
		break;

	case 'd':
	{
		if (selected_darts.size()>5) 
			break;

		glPushMatrix();

		float sc = 50./gWidthObj;
		glScalef(sc,sc,sc);
		glTranslatef(-gPosObj[0],-gPosObj[1],-gPosObj[2]);

		/// calcul du rayon
		getOrthoScreenRay(x,y,rayA,rayB);
		PFP::VEC3 AB = rayB-rayA;

		/// recuperation des faces intersectees
		d_faces.clear();
		d_edges.clear();
		d_vertices.clear();
		std::vector<Dart> darts;
		Algo::Selection::dartsRaySelection<PFP>(myMap, position, rayA, AB, darts, SelectorTrue());

		glPopMatrix();

		if (!darts.empty()) 
		{
			selected_darts.push_back(darts[0]);
			redraw();
			glutSwapBuffers();
		}
		break;
	}

	case 'D':
	{
		glPushMatrix();

		float sc = 50./gWidthObj;
		glScalef(sc,sc,sc);
		glTranslatef(-gPosObj[0],-gPosObj[1],-gPosObj[2]);

		/// calcul du rayon
		getOrthoScreenRay(x,y,rayA,rayB);
		PFP::VEC3 AB = rayB-rayA;

		/// recuperation des faces intersectees
		d_faces.clear();
		d_edges.clear();
		d_vertices.clear();
		std::vector<Dart> darts;
		Algo::Selection::dartsRaySelection<PFP>(myMap, position, rayA, AB, darts, SelectorTrue());

		glPopMatrix();

		if (!darts.empty()) 
		{
			redraw();
			std::stringstream ss;
			Dart d1 = myMap.phi1(darts[0]);
			Dart d2 = myMap.phi2(darts[0]);
			ss << "dart:" << darts[0].index<<" /phi1:"<< d1.index<<" /phi2:"<< d2.index;

			const PFP::VEC3& P = position[darts[0]];
			ss << " /Emb:" << P;
			glColor3f(1.,1.,0.);
			printString2D(x+8,y+8,ss.str());
			glutSwapBuffers();
		}
		break;
	}

	case 'f':
	{
		glPushMatrix();

		float sc = 50./gWidthObj;
		glScalef(sc,sc,sc);
		glTranslatef(-gPosObj[0],-gPosObj[1],-gPosObj[2]);


		/// calcul du rayon
		getOrthoScreenRay(x,y,rayA,rayB);
		PFP::VEC3 AB = rayB-rayA;

		/// recuperation des faces intersectees
		d_faces.clear();
		d_edges.clear();
		d_vertices.clear();
	
		Algo::Selection:: facesRaySelection<PFP>(myMap, position, SelectorTrue(), rayA, AB, d_faces);

		glPopMatrix();

		if (!d_faces.empty()) {
			redraw();
			std::stringstream ss;
			ss << "face " << d_faces[0].index/3;
			glColor3f(1.,0.,0.);
			printString2D(x+8,y+8,ss.str());
			glutSwapBuffers();
		}
		break;
	}

	case 'a':
	{
		glPushMatrix();

		float sc = 50./gWidthObj;
		glScalef(sc,sc,sc);
		glTranslatef(-gPosObj[0],-gPosObj[1],-gPosObj[2]);
		
		/// calcul du rayon
		getOrthoScreenRay(x,y,rayA,rayB);
		PFP::VEC3 AB = rayB-rayA;
		float AB2 = AB.norm2();

		d_faces.clear();
		d_edges.clear();
		d_vertices.clear();

		// compute distance  threshold (depends on width of object and scale factor)
		// not good selection vary with density of point
//		float dist = gWidthObj/(100.0f*getScale());
		float dist = computeSelectRadius(x,y,4);
		std::cout << "Distance obj = "<<dist << std::endl;

		Algo::Selection::edgesRaySelection<PFP>(myMap, position, SelectorTrue(), rayA, AB, d_edges,dist);

		glPopMatrix();

		if (!d_edges.empty())
		{
			redraw();
			std::stringstream ss;
			Dart dd = myMap.phi2(d_edges[0]);
			ss << "dart: " << d_edges[0].index<<" phi1: "<< myMap.phi1(d_edges[0]).index;
			if (dd != d_edges[0])
				ss << std::endl<< "phi2: " << dd.index<<" phi1: "<< myMap.phi1(dd).index;

			glColor3f(1.,1.,0.);
			printString2D(x+8,y+8,ss.str());
			glutSwapBuffers();
		}

		break;
	}

	case 's':
	{
		glPushMatrix();

		float sc = 50./gWidthObj;
		glScalef(sc,sc,sc);
		glTranslatef(-gPosObj[0],-gPosObj[1],-gPosObj[2]);

		/// Rayon
		getOrthoScreenRay(x,y,rayA,rayB);
		PFP::VEC3 AB = rayB-rayA;
		float AB2 = AB.norm2();

		d_faces.clear();
		d_edges.clear();
		d_vertices.clear();

		// compute distance  threshold (depends on width of object and scale factor)
		// not good selection vary with density of point
//		float dist = gWidthObj/(100.0f*getScale());
		float dist = computeSelectRadius(x,y,6);

		Algo::Selection::verticesRaySelection<PFP>(myMap, position,rayA, AB, d_vertices,dist,SelectorTrue());
		std::cout << "Distance obj = "<<dist << std::endl;
		
		glPopMatrix();

		if (!d_vertices.empty())
		{
			redraw();
			std::stringstream ss;
			ss << "dart: " << d_vertices[0].index << ": " << position[d_vertices[0]];
			glColor3f(1.,1.,0.);
			printString2D(x+8,y+8,ss.str());
			glutSwapBuffers();
		}

		break;
	}

	case 'h':
		aff_help =!aff_help;
		glutPostRedisplay();
		break;

	case 'r':
		if (!selected_darts.empty())
		{
			Reduction(selected_darts[0]);
			glutPostRedisplay();
		}
		break;

	case 'T':
		if (!selected_darts.empty())
		{
			Tourne(selected_darts[0]);
			glutPostRedisplay();
		}
		break;

	case 'c':
		
		if (selected_darts.size()>=2)
		{
			Colle(selected_darts[0], selected_darts[1]);
			glutPostRedisplay();
		}
		break;

	case 'C':
		if (selected_darts.size()>=2)
		{
			ColleMilieu(selected_darts[0], selected_darts[1]);
			glutPostRedisplay();
		}
		break;

	case 'x':
		if (selected_darts.size()>=2)
		{
			Decoupage(selected_darts[0], selected_darts[1]);
			glutPostRedisplay();
		}
		break;

	}
}

int main(int argc, char **argv)
{
	/// init glut interface and
	myGlutWin mgw(&argc,argv,800,800);
	mgw.init();

	if (argc == 2)
	{
		std::vector<std::string> attrNames ;
		Algo::Import::importMesh<PFP>(myMap, argv[1], attrNames) ;
		position = myMap.getAttribute<PFP::VEC3>(VERTEX_ORBIT, attrNames[0]) ;
		normal = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "normal");
		mgw.updateRender();
	}
	else
	{
		position = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "position");
		normal = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "normal");
		createMap();
		mgw.updateRender();	// ne pas oublier de mettre à jour openGL après chaque modif dans la carte
	}

	if (myMap.getNbDarts()==0)
		exit(0);

    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
    mgw.gWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
    mgw.gPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

	mgw.mainLoop();

	return 0;
}
