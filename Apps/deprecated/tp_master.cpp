/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2012, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

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

// pour simplifier l'ecriture du code
using namespace CGoGN;

// déclaration de la classe utilisée pour le TP
class Map2TP;

// definition de la structure qui decrit le type de carte utilise
struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef Map2TP MAP;
};

/// typedef pour simplifier l'ecriture du code
typedef PFP::VEC3 Point3D;

// Variables pour la gestion des plongements
//		handler d'attribut de position par sommet
VertexAttribute<Point3D> position;
//		handler d'attribut de normale par sommet
VertexAttribute<Point3D> normal;


/// Fonctions a connaitre:
///
/// phi1(d), phi2(d) : les parcours topologiques
///
/// Creer un face: newOrientedFace(nb_cotes); renvoit un Dart
///
/// Creer un plongement de sommet :
/// 	Point3D P = Point3D(0.0f,0.0f,0.0f);
///
/// Lire le plongement d'un sommet (celui du brin d)
///		P = position[d];
///
/// Assigner un plongement à un sommet (celui du brin d)
///		embedVertex(d,P);

class Map2TP : public Map2
{
private:
	// 3 brins de la carte
	Dart d_carre;
	Dart d_tri;
	Dart d_multiFaces;

	// Assigne un nouveau plongement au sommet. Les anciens plongements sont libérés.
	void newVertex(Dart d) {
		setOrbitEmbeddingOnNewCell<VERTEX>(d);
	}

public:
	//  Fonction Carre: construit un carre et renvoit un brin
	Dart Carre()
	{
		Point3D P[4];
		P[0] = Point3D(0.0f,0.0f,0.0f);
		P[1] = Point3D(1.0f,0.0f,0.0f);
		P[2] = Point3D(1.0f,1.0f,0.0f);
		P[3] = Point3D(0.0f,1.0f,0.0f);

		return NIL;
	}

	//  Fonction Triangle: construit un triangle et renvoit un brin
	Dart Triangle()
	{
		Point3D P[3];
		P[0] = Point3D(0.0f,1.1f,0.0f);
		P[1] = Point3D(1.0f,1.1f,0.0f);
		P[2] = Point3D(0.5f,2.0f,0.0f);

		return NIL;
	}

	// Construit un polygone de taille "size"
	Dart Polygone(Point3D* P, unsigned size)
	{
		return NIL;
	}

	// appel clavier touche 'c'
	void Colle(Dart d, Dart e)
	{
	}

	// appel clavier touche 'm'
	void ColleMilieu(Dart d, Dart e)
	{
	}

	// Construit une figure
	Dart MultiFaces()
	{
		Point3D P[6];
		P[0] = Point3D(2.0f,0.0f,0.0f);
		P[1] = Point3D(3.0f,0.0f,0.0f);
		P[2] = Point3D(3.5f,1.0f,0.0f);
		P[3] = Point3D(3.0f,1.8f,0.0f);
		P[4] = Point3D(2.0f,1.8f,0.0f);
		P[5] = Point3D(1.5f,1.0f,0.0f);
		Dart hexa = Polygone(P,6);

		Point3D Q[3];
		Q[0] = Point3D(2.0f,2.0f,0.0f);
		Q[1] = Point3D(3.0f,2.0f,0.0f);
		Q[2] = Point3D(2.5f,3.0f,0.0f);
		Dart triangle = Polygone(Q,3);

		Point3D R[4];
		R[0] = Point3D(4.0f,0.0f,0.0f);
		R[1] = Point3D(5.0f,0.0f,0.0f);
		R[2] = Point3D(5.0f,1.0f,0.0f);
		R[3] = Point3D(4.0f,1.0f,0.0f);
		Dart carre = Polygone(R,4);

		Point3D S[3];
		S[0] = Point3D(4.0f,1.2f,0.0f);
		S[1] = Point3D(5.0f,1.2f,0.0f);
		S[2] = Point3D(4.5f,2.2f,0.0f);
		Dart triangle2 = Polygone(S,3);

		Dart haut_carre = phi1(phi1(carre));
		Dart hexa2 = phi1(phi1(phi1(hexa)));

	//	Test du phi1Sew
	//	myMap.phi1sew(hexa,hexa2);
	//	myMap.phi1sew(myMap.phi1(hexa),myMap.phi_1(carre));

		// Génére un maillage à la fin
//		Colle(haut_carre,triangle2);
//		Colle(hexa2,triangle);
//		Colle(phi_1(triangle2),phi_1(hexa2));
//		Colle(phi1(hexa),phi_1(carre));

		return hexa;
	}

	// Fonction de creation de la carte (appelee par le main)
	void createMap()
	{
		d_carre = Carre();
		d_tri = Triangle();
//		d_multiFaces = MultiFaces();

//		Colle(phi1(phi1(d_carre)),d_tri);
//		Colle(phi_1(d_multiFaces),phi1(d_carre));
//		Colle(phi_1(phi_1(d_multiFaces)),phi1(d_tri));
	}

	// Touche x
	void coupeFace(Dart d, Dart e)
	{
	}

	// Touche y
	void coupeArete(Dart d) {
		Point3D p = position[d];
		Point3D q = position[phi1(d)];
		Point3D milieu = (p + q + Point3D(0.2f,0.2f,0.0f) /* decalage pour voir le point */ )/2;
	}

	// Touche z
	void arrondi(Dart d) {
		Dart e = phi_1(d);
		coupeArete(d);
		coupeArete(e);
		e = phi1(e);
		Point3D P = position[d];
		Point3D Q = position[e];
		Point3D R = position[phi1(d)];
		Point3D M = (P + P + Q + R)/4;

		position[d] = M;
	}

	unsigned face_size(Dart d) {
		return 0;
	}

	// Touche p
	void triangule() {
	}

	// Touche q
	void fusionSommet(Dart d)
	{
		Point3D P = position[d];
		Point3D Q = position[phi1(d)];
		Point3D M = (P + Q)/2;
	}

	// Longueur d'une arête
	double longueur(Dart d)
	{
		Point3D P = position[d];
		Point3D Q = position[phi1(d)];
		Point3D V = P-Q;
		return V.norm();
	}

	// Simplification touche k
	void simplifie()
	{
	}

};

// definition de la carte comme variable globale
PFP::MAP myMap;

// pile des brins selectionnes (6 max)
std::vector<Dart> selected_darts;

// Variables pour le picking (sélection de brins à la souris)
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
	m_render_topo = new Algo::Render::GL2::TopoRender() ;

	m_ds = new Utils::Drawer() ;

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
	// update des normales aux sommets
	Algo::Geometry::computeNormalVertices<PFP>(myMap, position, normal) ;

	// update du VBO position (contexte GL necessaire)
	m_positionVBO->updateData(position);
	m_normalVBO->updateData(normal);

	// update des primitives du renderer
	m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::TRIANGLES);
	m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::LINES);

	m_render_topo->updateData<PFP>(myMap, position, 0.9f, 0.9f);

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
	// coordonnées du clic et calcul du rayon dans l'espace 3D
	int x,y;
	glMousePosition(x,y);
	float dist = getOrthoScreenRay(x,y,rayA,rayB);
	PFP::VEC3 AB = rayB-rayA;

	// tableau de brins pour la sélection
	std::vector<Dart> darts;

	switch(keycode)
	{
	// Bascule affichage topologie - normal
	case 't':
		renderTopo = !renderTopo;
		break;

	// Oublie les brins sélectionnés
	case '0':
		selected_darts.clear();
		break;

	// Sélectionne un brin
	case 'd':
		Algo::Selection::dartsRaySelection<PFP>(myMap, position, rayA, AB, darts, SelectorTrue());

		if (!darts.empty() && selected_darts.size() < 6)
		{
			selected_darts.push_back(darts[0]);
		}
		break;

	// Affiche les informations sur un brin
	case 'D':
		Algo::Selection::dartsRaySelection<PFP>(myMap, position, rayA, AB, darts, SelectorTrue());

		if (!darts.empty()) 
		{
			std::stringstream ss;
			Dart d1 = myMap.phi1(darts[0]);
			Dart d2 = myMap.phi2(darts[0]);
			ss << "dart:" << darts[0].index<<" /phi1:"<< d1.index<<" /phi2:"<< d2.index;

			const Point3D& P = position[darts[0]];
			ss << " /Emb:" << P;
			statusMsg(ss.str().c_str());
		}
		break;

	// Sélectionne des faces
	case 'f':
		d_faces.clear();
		Algo::Selection:: facesRaySelection<PFP>(myMap, position, SelectorTrue(), rayA, AB, d_faces);

		if (!d_faces.empty())
		{
			std::stringstream ss;
			ss << "Face " << d_faces[0].index/3;
			statusMsg(ss.str().c_str());
		}
		break;

	// Sélectionne des arêtes
	case 'a':
		d_edges.clear();
		Algo::Selection::edgesRaySelection<PFP>(myMap, position, SelectorTrue(), rayA, AB, d_edges,dist);

		if (!d_edges.empty())
		{
			std::stringstream ss;
			Dart dd = myMap.phi2(d_edges[0]);
			ss << "Arete:  dart: " << d_edges[0].index<<" phi1: "<< myMap.phi1(d_edges[0]).index;
			if (dd != d_edges[0])
				ss << " phi2: " << dd.index<<" phi1: "<< myMap.phi1(dd).index;
			statusMsg(ss.str().c_str());
		}
		break;

	// Sélectionne des sommets
	case 's':
		d_vertices.clear();
		Algo::Selection::verticesRaySelection<PFP>(myMap, position,rayA, AB, d_vertices,dist,SelectorTrue());

		if (!d_vertices.empty())
		{
			std::stringstream ss;
			ss << "Sommet:  dart: " << d_vertices[0].index << ": " << position[d_vertices[0]]<< "( id emb:"<< myMap.getEmbedding<VERTEX>(d_vertices[0])<<")"<< std::endl;
			statusMsg(ss.str().c_str());
		}
		break;

	// Opérations utilisant 2 brins
	case 'c':
	case 'm':
	case 'x':
		if (selected_darts.size() >= 2)
		{
			switch (keycode) {
			// Colle 2 arêtes
			case 'c':
				myMap.Colle(selected_darts[0], selected_darts[1]);
				break;
			// Colle 2 arêtes en leur milieu
			case 'm':
				myMap.ColleMilieu(selected_darts[0], selected_darts[1]);
				break;
			// Coupe une face en 2 avec une nouvelle arête
			case 'x':
				myMap.coupeFace(selected_darts[0], selected_darts[1]);
				break;
			}
			selected_darts.clear();
		}
		break;

	// Opérations utilisant un brin
	case 'y':
	case 'z':
	case 'q':
		if (selected_darts.size() >= 1)
		{
			switch (keycode) {
			// Coupe une arête en 2 avec un nouveau sommet
			case 'y':
				myMap.coupeArete(selected_darts[0]);
				break;
			// Arrondi
			case 'z':
				myMap.arrondi(selected_darts[0]);
				break;
			// Fusion de sommet
			case 'q':
				myMap.fusionSommet(selected_darts[0]);
				break;
			}
			selected_darts.clear();
		}
		break;

	// Opérations globales
	// Triangulation
	case 'p':
		myMap.triangule();
		selected_darts.clear();
		break;
	// Simplification
	case 'k':
		myMap.simplifie();
		selected_darts.clear();
		break;
	}
	// Mise à jour de l'affichage
	updateGL();
}

int main(int argc, char **argv)
{
	// interface:
	QApplication app(argc, argv);
	MyQT sqt;

	if (argc == 2) {
		std::vector<std::string> attrNames ;
		Algo::Import::importMesh<PFP>(myMap, argv[1], attrNames) ;
		position = myMap.getAttribute<Point3D, VERTEX>(attrNames[0]) ;
		normal = myMap.addAttribute<PFP::VEC3, VERTEX>("normal");
		Algo::Geometry::computeNormalVertices<PFP>(myMap, position, normal) ;
	}
	else {
		position = myMap.addAttribute<Point3D, VERTEX>("position");
		normal = myMap.addAttribute<PFP::VEC3, VERTEX>("normal");
		myMap.createMap();
		Algo::Geometry::computeNormalVertices<PFP>(myMap, position, normal) ;
	}

	if (myMap.getNbDarts()==0) {
		CGoGNout << "Aucun brin dans la carte. Sortie ..." << CGoGNendl;
		exit(0);
	}

	// Parametre de la fenetre en fonction de la taille du maillage à afficher
	Geom::BoundingBox<Point3D> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
	float lWidthObj = std::max(std::max(bb.size(0), bb.size(1)), bb.size(2));
	Point3D lPosObj = (bb.min() +  bb.max()) / 2;
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
