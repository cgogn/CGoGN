/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2011, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.u-strasbg.fr/                                         *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include <iostream>
#include <time.h>
#include <algorithm>

#include "tp_master.h"

#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"

#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"

#include "Algo/Render/GL2/mapRender.h"
#include "Algo/Render/GL2/topoRender.h"
#include "Algo/Geometry/normal.h"

#include "Algo/Selection/raySelector.h"

#include "Algo/Import/import.h"
#include "Algo/Geometry/boundingbox.h"

#include "Utils/Shaders/shaderSimpleColor.h"
#include "Utils/Shaders/shaderPhong.h"

#include "Utils/drawer.h"

/// pour simplifier l'ecriture du code
using namespace CGoGN;

/// definition de la structure qui decrit le type de carte utilise

struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef EmbeddedMap2 MAP;
};


/// definition de la carte en  global, plus facile 
PFP::MAP myMap;

// handler d'attribut de position par sommet
AttributeHandler<PFP::VEC3> position;

// handler d'attribut de normale par sommet
AttributeHandler<PFP::VEC3> normal;

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
	return Dart::nil();
}


///  Fonction Triangle: construit un triangle et renvoit un brin

Dart Triangle()
{
	return Dart::nil();
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
	return Dart::nil();
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





/// Variables pour le picking
PFP::VEC3 rayA;
PFP::VEC3 rayB;
std::vector<PFP::VEC3> inters;
std::vector<Dart> d_faces;
std::vector<Dart> d_edges;
std::vector<Dart> d_vertices;



void MyQT::drawSelected()
{

	typedef Dart Dart;

// FACES

	m_ds->newList(GL_COMPILE_AND_EXECUTE);
	m_ds->lineWidth(5.0f);
	for(unsigned int i=0; i < d_faces.size(); ++i)
	{
		m_ds->begin(GL_POLYGON);
		// fait varier la couleur du plus pres au plus loin
		float c = float (i) / float (d_faces.size());
		m_ds->color3f(1.0f - c , 0.0f, c);

		Dart d = d_faces[i];
		do
		{
			const PFP::VEC3& P = position[d];
			m_ds->vertex(P);
			d = myMap.phi1(d);
		} while (d!=d_faces[i]);

		m_ds->end();
	}


//edges
	m_ds->begin(GL_LINES);
	m_ds->lineWidth(5.0f);
	for(unsigned int i=0; i < d_edges.size(); ++i)
	{
		// fait varier la couleur du plus pres au plus loin
		float c = float (i) / float (d_edges.size());
		m_ds->color3f(1.0f - c , c ,0.);

		Dart d = d_edges[i];
		const PFP::VEC3& P = position[d];
		m_ds->vertex(P);
		d = myMap.phi1(d);
		const PFP::VEC3& Q =  position[d];
		m_ds->vertex(Q);
	}
	m_ds->end();


//VERTICES
//edges
	m_ds->pointSize(9.0f);
	m_ds->begin(GL_POINTS);

	for(unsigned int i=0; i < d_vertices.size(); ++i)
	{
		// fait varier la couleur du plus pres au plus loin
		float c = float (i) / float (d_vertices.size());
		m_ds->color3f(1.0f - c , 0., c);
		Dart d = d_vertices[i];
		const PFP::VEC3& P = position[d];
		m_ds->vertex(P);
	}
	m_ds->end();

	m_ds->endList();
	
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


void MyQT::cb_initGL()
{
	// choose to use GL version 2
	Utils::GLSLShader::setCurrentOGLVersion(2);

	// create the render
	m_render = new Algo::Render::GL2::MapRender();
	m_render_topo = new Algo::Render::GL2::TopoRenderMapD() ;

	m_ds = new Utils::Drawer();

	// create VBO for position
	m_positionVBO = new Utils::VBO();
	m_normalVBO = new Utils::VBO();

	m_shader = new Utils::ShaderPhong();
	m_shader->setAttributePosition(m_positionVBO);
	m_shader->setAttributeNormal(m_normalVBO);
	m_shader->setDiffuse(Geom::Vec4f(0.0f,0.6f,0.0f,0.0f));
	m_shader->setSpecular(Geom::Vec4f(0.0f,0.0f,0.0f,0.0f)); // no specular

	// using simple shader with color
	m_shader2 = new Utils::ShaderSimpleColor();
	m_shader2->setAttributePosition(m_positionVBO);
	m_shader2->setColor(Geom::Vec4f(0.0f,0.1f,0.0f,0.0f));
	registerShader(m_shader);
	registerShader(m_shader2);
}




void MyQT::cb_redraw()
{

	drawSelected();
	if (renderTopo)
	{
		glEnable( GL_POLYGON_OFFSET_FILL );
		glPolygonOffset( 1.0f, 1.0f );
		m_render_topo->drawTopo();
	}

	glEnable( GL_POLYGON_OFFSET_FILL );
	glPolygonOffset( 1.5f, 1.5f );



	glPolygonOffset( 2.0f, 2.0f );
	glLineWidth(1.0f);
	m_render->draw(m_shader2,Algo::Render::GL2::LINES) ;


	/// Rendu faces pleines
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT,GL_FILL);
	glPolygonMode(GL_BACK,GL_NONE);

	m_render->draw(m_shader,Algo::Render::GL2::TRIANGLES) ;

	glDisable( GL_POLYGON_OFFSET_FILL );
}

void MyQT::cb_keyPress(int keycode)
{
	typedef Dart Dart;

	int x,y;
	glMousePosition(x,y);
	CGoGNout << x << " , "<< y << CGoGNendl;

	switch(keycode)
	{
	case 't':
		renderTopo = !renderTopo;
		if (renderTopo)
			statusMsg("Render Topology");
		else
			statusMsg("");
		updateGL();
		break;

	case '0':
		selected_darts.clear();
		statusMsg("Cleanning selected darts");
		updateGL();
		break;

	case 'd':
	{
		if (selected_darts.size()>5) 
		{
			statusMsg("Already six darts selected");
			break;
		}


		/// calcul du rayon
		getOrthoScreenRay(x,y,rayA,rayB);
		PFP::VEC3 AB = rayB-rayA;

		/// recuperation des faces intersectees
		d_faces.clear();
		d_edges.clear();
		d_vertices.clear();
		std::vector<Dart> darts;
		Algo::Selection::dartsRaySelection<PFP>(myMap, position, rayA, AB, darts, SelectorTrue());
		selected_darts.push_back(darts[0]);

		updateGL();
		break;
	}

	case 'D':
	{
		/// calcul du rayon
		getOrthoScreenRay(x,y,rayA,rayB);
		PFP::VEC3 AB = rayB-rayA;

		/// recuperation des faces intersectees
		d_faces.clear();
		d_edges.clear();
		d_vertices.clear();
		std::vector<Dart> darts;
		Algo::Selection::dartsRaySelection<PFP>(myMap, position, rayA, AB, darts, SelectorTrue());

		if (!darts.empty()) 
		{
			std::stringstream ss;
			Dart d1 = myMap.phi1(darts[0]);
			Dart d2 = myMap.phi2(darts[0]);
			ss << "dart:" << darts[0].index<<" /phi1:"<< d1.index<<" /phi2:"<< d2.index;

			const PFP::VEC3& P = position[darts[0]];
			ss << " /Emb:" << myMap.getEmbedding(VERTEX, darts[0])<< "/"<<P;
			statusMsg(ss.str().c_str());
		}
		break;
	}

	case 'f':
	{
		/// calcul du rayon
		getOrthoScreenRay(x,y,rayA,rayB);
		PFP::VEC3 AB = rayB-rayA;

		/// recuperation des faces intersectees
		d_faces.clear();
		d_edges.clear();
		d_vertices.clear();
	
		Algo::Selection:: facesRaySelection<PFP>(myMap, position, SelectorTrue(), rayA, AB, d_faces);


		if (!d_faces.empty())
		{
			std::stringstream ss;
			ss << "Face " << d_faces[0].index/3;
			statusMsg(ss.str().c_str());
			updateGL();
		}
		break;
	}

	case 'a':
	{
		/// calcul du rayon
		float dist = getOrthoScreenRay(x,y,rayA,rayB);
		PFP::VEC3 AB = rayB-rayA;

		d_faces.clear();
		d_edges.clear();
		d_vertices.clear();

		Algo::Selection::edgesRaySelection<PFP>(myMap, position, SelectorTrue(), rayA, AB, d_edges,dist);

		if (!d_edges.empty())
		{
			std::stringstream ss;
			Dart dd = myMap.phi2(d_edges[0]);
			ss << "Arete:  dart: " << d_edges[0].index<<" phi1: "<< myMap.phi1(d_edges[0]).index;
			if (dd != d_edges[0])
				ss << " phi2: " << dd.index<<" phi1: "<< myMap.phi1(dd).index;
			statusMsg(ss.str().c_str());
			updateGL();
		}

		break;
	}

	case 's':
	{
		/// Rayon
		float dist = getOrthoScreenRay(x,y,rayA,rayB);
		PFP::VEC3 AB = rayB-rayA;

		d_faces.clear();
		d_edges.clear();
		d_vertices.clear();

		Algo::Selection::verticesRaySelection<PFP>(myMap, position,rayA, AB, d_vertices,dist,SelectorTrue());

		if (!d_vertices.empty())
		{
			std::stringstream ss;
			ss << "Sommet:  dart: " << d_vertices[0].index << ": " << position[d_vertices[0]]<< "( id emb:"<< myMap.getEmbedding(VERTEX,d_vertices[0])<<")"<< std::endl; ;
			statusMsg(ss.str().c_str());

			updateGL();
		}

		break;
	}

	case 'r':
		if (!selected_darts.empty())
		{
			Reduction(selected_darts[0]);
			updateGL();
		}
		break;

	case 'T':
		if (!selected_darts.empty())
		{
			Tourne(selected_darts[0]);
			updateGL();
		}
		break;

	case 'c':
		
		if (selected_darts.size()>=2)
		{
			Colle(selected_darts[0], selected_darts[1]);
			updateGL();
		}
		break;

	case 'C':
		if (selected_darts.size()>=2)
		{
			ColleMilieu(selected_darts[0], selected_darts[1]);
			updateGL();
		}
		break;

	case 'x':
		if (selected_darts.size()>=2)
		{
			Decoupage(selected_darts[0], selected_darts[1]);
			updateGL();
		}
		break;

	}
}

int main(int argc, char **argv)
{
	// interface:
	QApplication app(argc, argv);
	MyQT sqt;


	if (argc == 2)
	{
		std::vector<std::string> attrNames ;
		Algo::Import::importMesh<PFP>(myMap, argv[1], attrNames) ;
		position = myMap.getAttribute<PFP::VEC3>(VERTEX, attrNames[0]) ;
		normal = myMap.addAttribute<PFP::VEC3>(VERTEX, "normal");
		Algo::Geometry::computeNormalVertices<PFP>(myMap, position, normal) ;
	}
	else
	{
		position = myMap.addAttribute<PFP::VEC3>(VERTEX, "position");
		normal = myMap.addAttribute<PFP::VEC3>(VERTEX, "normal");
		createMap();
	}

	if (myMap.getNbDarts()==0)
		exit(0);

	// caclul de la bounding box de la scene a afficher
	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
	float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
	Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

	// envoit info BB a l'interface
	sqt.setParamObject(lWidthObj,lPosObj.data());

	// show 1 pour optenir le contexte GL
	sqt.show();

	// update du VBO position (contexte GL necessaire)
	sqt.m_positionVBO->updateData(position);
	sqt.m_normalVBO->updateData(normal);

	// update des primitives du renderer
	sqt.m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::TRIANGLES);
	sqt.m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::LINES);

	sqt.m_render_topo->updateData<PFP>(myMap, position, 0.9f, 0.9f);

	// show final pour premier redraw
	sqt.show();

	// et on attend la fin.
	return app.exec();
}
