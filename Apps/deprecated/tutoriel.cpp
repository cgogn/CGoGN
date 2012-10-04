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

#include "tutoriel.h"

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
class QuadMesh;

// definition de la structure qui decrit le type de carte utilise
struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef QuadMesh MAP;
};

// fonction qui renvoit vrai (pour sélectioner tous les brins)
SelectorTrue allDarts;

// typedef pour le plongement des sommets
typedef PFP::VEC3 Point3D;

// Variables pour la gestion des plongements
//		handler d'attribut de position par sommet
AttributeHandler<Point3D> position;
//		handler d'attribut de normale par sommet
AttributeHandler<PFP::VEC3> normal;


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

class QuadMesh : public EmbeddedMap2
{
private:

public:
	void createMesh() {
	}

	void coupeArete(Dart d) {
		Point3D p = position[d];
		Point3D q = position[phi1(d)];
		Point3D milieu = (p + q + Point3D(0.2f,0.2f,0.0f) /* decalage pour voir le point */ )/2;

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
		Dart dmin = begin();
		double lmin = longueur(dmin);

		Dart d;
		for (d = begin(); d != end(); next(d)) {
			if (longueur(d) < lmin) {
				dmin = d;
				lmin = longueur(d);
			}
		}
	}

};

// definition de la carte comme variable globale
PFP::MAP myQuadMesh;

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
			d = myQuadMesh.phi1(d);
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
		d = myQuadMesh.phi1(d);
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
	// update des normales aux sommets
	Algo::Geometry::computeNormalVertices<PFP>(myQuadMesh, position, normal) ;

	// update du VBO position (contexte GL necessaire)
	m_positionVBO->updateData(position);
	m_normalVBO->updateData(normal);

	// update des primitives du renderer
	m_render->initPrimitives<PFP>(myQuadMesh, CGoGN::allDarts, Algo::Render::GL2::TRIANGLES);
	m_render->initPrimitives<PFP>(myQuadMesh, CGoGN::allDarts, Algo::Render::GL2::LINES);

	m_render_topo->updateData<PFP>(myQuadMesh, position, 0.9f, 0.9f);

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
		Algo::Selection::dartsRaySelection<PFP>(myQuadMesh, position, rayA, AB, darts, SelectorTrue());

		if (!darts.empty() && selected_darts.size() < 6)
		{
			selected_darts.push_back(darts[0]);
		}
		break;

	// Affiche les informations sur un brin
	case 'D':
		Algo::Selection::dartsRaySelection<PFP>(myQuadMesh, position, rayA, AB, darts, SelectorTrue());

		if (!darts.empty()) 
		{
			std::stringstream ss;
			Dart d1 = myQuadMesh.phi1(darts[0]);
			Dart d2 = myQuadMesh.phi2(darts[0]);
			ss << "dart:" << darts[0].index<<" /phi1:"<< d1.index<<" /phi2:"<< d2.index;

			const Point3D& P = position[darts[0]];
			ss << " /Emb:" << P;
			statusMsg(ss.str().c_str());
		}
		break;

	// Sélectionne des faces
	case 'f':
		d_faces.clear();
<<<<<<< HEAD:Apps/Examples/tutoriel.cpp
		Algo::Selection:: facesRaySelection<PFP>(myQuadMesh, position, SelectorTrue(), rayA, AB, d_faces);
=======
		Algo::Selection::facesRaySelection<PFP>(myMap, position, SelectorTrue(), rayA, AB, d_faces);
>>>>>>> 69653d4f2ddc874cfdf5765126462ba58105052e:Apps/deprecated/tutoriel.cpp

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
		Algo::Selection::edgesRaySelection<PFP>(myQuadMesh, position, SelectorTrue(), rayA, AB, d_edges,dist);

		if (!d_edges.empty())
		{
			std::stringstream ss;
			Dart dd = myQuadMesh.phi2(d_edges[0]);
			ss << "Arete:  dart: " << d_edges[0].index<<" phi1: "<< myQuadMesh.phi1(d_edges[0]).index;
			if (dd != d_edges[0])
				ss << " phi2: " << dd.index<<" phi1: "<< myQuadMesh.phi1(dd).index;
			statusMsg(ss.str().c_str());
		}
		break;

	// Sélectionne des sommets
	case 's':
		d_vertices.clear();
		Algo::Selection::verticesRaySelection<PFP>(myQuadMesh, position,rayA, AB, d_vertices,dist,SelectorTrue());

		if (!d_vertices.empty())
		{
			std::stringstream ss;
			ss << "Sommet:  dart: " << d_vertices[0].index << ": " << position[d_vertices[0]]<< "( id emb:"<< myQuadMesh.getEmbedding(VERTEX,d_vertices[0])<<")"<< std::endl; ;
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
				myQuadMesh.coupeArete(selected_darts[0]);
				break;
			}
			selected_darts.clear();
		}
		break;

	// Opérations globales
	// Simplification
	case 'k':
		myQuadMesh.simplifie();
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
		Algo::Import::importMesh<PFP>(myQuadMesh, argv[1], attrNames) ;
		position = myQuadMesh.getAttribute<Point3D>(VERTEX, attrNames[0]) ;
		normal = myQuadMesh.addAttribute<PFP::VEC3>(VERTEX, "normal");
		Algo::Geometry::computeNormalVertices<PFP>(myQuadMesh, position, normal) ;
	}
	else {
		position = myQuadMesh.addAttribute<Point3D>(VERTEX, "position");
		normal = myQuadMesh.addAttribute<PFP::VEC3>(VERTEX, "normal");
		myQuadMesh.createMesh();
		Algo::Geometry::computeNormalVertices<PFP>(myQuadMesh, position, normal) ;
	}

	if (myQuadMesh.getNbDarts()==0) {
		CGoGNout << "Aucun brin dans la carte. Sortie ..." << CGoGNendl;
		exit(0);
	}

	// Parametre de la fenetre en fonction de la taille du maillage à afficher
	Geom::BoundingBox<Point3D> bb = Algo::Geometry::computeBoundingBox<PFP>(myQuadMesh, position);
	float lWidthObj = std::max(std::max(bb.size(0), bb.size(1)), bb.size(2));
	Point3D lPosObj = (bb.min() +  bb.max()) / 2;
	sqt.setParamObject(lWidthObj,lPosObj.data());

	// show pour optenir le contexte GL
	sqt.show();

	// et on attend la fin.
	return app.exec();
}
