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

#include "Utils/glutwin.h"

#include "Topology/map/map2.h"

#include "Geometry/matrix.h"
#include "Geometry/vector_gen.h"
#include "Algo/Import/import.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Render/map_glRender.h"
#include "Algo/Render/vbo_MapRender.h"
#include "Algo/Render/topo_vboRender.h"

#include "Topology/generic/cellmarker.h"


using namespace CGoGN ;

struct PFP {
	// definition de la carte
	typedef Map2 MAP;

	// definition du type de reel utilise
	typedef float REAL;
	// definition du type de vecteur (point) utilise
	typedef Geom::Vector<3,REAL> VEC3;
	// definition du type de matrice 3x3 utilise
	typedef Geom::Matrix<3,3,REAL> MATRIX33;
	// definition du type de matrice 4x4 utilise
	typedef Geom::Matrix<4,4,REAL> MATRIX44;

	// definition du type du AttributeHandler de vecteur 3D
	typedef AttributeHandler<VEC3> TVEC3;
};

// qq initialisation cachees (car penible syntaxiquement)
INIT_STATICS_MAP();


PFP::MAP myMap;
SelectorTrue allDarts ;

PFP::TVEC3 position ;
PFP::TVEC3 normal ;
AttributeHandler<Geom::Vec4f> color ;

class myGlutWin: public Utils::SimpleGlutWin
{
public:
     void myRedraw();

     float gWidthObj;
     Geom::Vec3f gPosObj;

     bool render_obj;
     bool render_topo;

     Algo::Render::VBO::MapRender_VBO<PFP>* m_render;
     Algo::Render::VBO::topo_VBORenderMapD* m_render_topo;

 	myGlutWin(	int* argc, char **argv, int winX, int winY):SimpleGlutWin(argc,argv,winX,winY)
    {
 		render_obj = true;
 		render_topo = false;
    }

 	void myKeyboard(unsigned char keycode, int x, int y);
};

/// Gestion des touches clavier
void myGlutWin::myKeyboard(unsigned char keycode, int x, int y)
{
        switch(keycode) {
                /** affichage **/
                case 'o' : {
                        render_obj = !render_obj;
                        glutPostRedisplay();
                        break;
                }
                case 't' : {
                		render_topo = !render_topo;
                		glutPostRedisplay();
                		break;
                }
                default : {
                		std::cout << "not yet implemented" << std::endl;
                		break;
                }

        }
}


void myGlutWin::myRedraw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	// met l'objet au milieu de l'ecran a la bonne taille
	float sc = 50.0f/gWidthObj;
	glScalef(sc,sc,sc);
	glTranslatef(-gPosObj[0],-gPosObj[1],-gPosObj[2]);


	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glDisable(GL_LIGHTING);

	if(render_obj)
	{
		// on trace les ligne devant
		glDisable( GL_POLYGON_OFFSET_FILL );
		glColor3f(1.0f,1.0f,0.0f);
		m_render->draw(Algo::Render::VBO::LINES);

		// et on decale les faces vers l'arriere
		glEnable( GL_POLYGON_OFFSET_FILL );
		glPolygonOffset( 1.0f, 1.0f );

		glColor3f(0.0f,0.5f,0.0f);
		m_render->draw(Algo::Render::VBO::TRIANGLES);
		glDisable( GL_POLYGON_OFFSET_FILL );
	}

	if(render_topo)
	{
		m_render_topo->drawTopo();
	}


	glPopMatrix();
}



int main(int argc, char **argv)
{
	/// Utilisation des Marker

	/// on reprend la carte de tuto1

	Dart d2 = myMap.newOrientedFace(3);
	Dart d3 = myMap.newOrientedFace(4);
	myMap.sewFaces(d2,d3);

	position = myMap.addAttribute<Geom::Vec3f>(VERTEX_ORBIT, "position");

	position[d2] = PFP::VEC3(0.0f, 0.0f, 0.0f);
	d2 = myMap.phi1(d2);
	position[d2] = PFP::VEC3(2.0f, 0.0f, 0.0f);
	d2 = myMap.phi1(d2);
	position[d2] = PFP::VEC3(1.0f, 3.0f, 0.0f);
	d2 = myMap.phi1(d2);
	d3 = myMap.phi<11>(d3);
	position[d3] = PFP::VEC3(0.0f, -2.0f, 0.0f);
	d3 = myMap.phi1(d3);
	position[d3] = PFP::VEC3(2.0f, -2.0f, 0.0f);
	d3 = myMap.phi1(d3);

//	unsigned int idAttV2 = myMap.addAttribute<float>(VERTEX_ORBIT,"reel");
//	PFP::AttributeHandler<float> tableReels(idAttV2,VERTEX_ORBIT,myMap);
//
//	tableReels[d2] = 3.5f;
//	tableReels[myMap.phi1(d2)] = 3.7f;
//
//	// Attention ici on cree un attribut de FACE donc les face sont maintenant plangees
//	// l'attribut cree se detruira a la fin de la portee de l'objet
//	PFP::MAP::AutoAttributeHandler<Geom::Vec3f> tableRGB(myMap, FACE_ORBIT, "RGB"); // "RGB" optionnel
//
//	tableRGB[d3] = Geom::Vec3f(1.0f,2.0f,3.0f);
//
//	// acces par les brins
//	for (Dart d = myMap.begin(); d!= myMap.end(); myMap.next(d))
//	{
//		std::cout << "Brin "<<d.label()<< " reel="<<tableReels[d]<< "  RGB="<<tableRGB[d]<<std::endl;
//	}
//
//	//acces direct par balayge du tableau
//	for (unsigned int id = tableRGB.begin(); id != tableRGB.end(); tableRGB.next(id))
//	{
//		std::cout << "RGB["<<id<<"] = "<<tableRGB.at(id)<<std::endl;
//	}

	CellMarkerStore cm(myMap, VERTEX_ORBIT);

	cm.mark(d2);
	cm.mark(d3);
	cm.unmarkAll();


    // un peu d'interface
	myGlutWin mgw(&argc,argv,800,800);

    // calcul de la bounding box
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
    // pour l'affichage
    mgw.gWidthObj = std::max<float>( std::max<float>(bb.size(0),bb.size(1)),bb.size(2));
    mgw.gPosObj =  (bb.min() +  bb.max()) /2.0f;


    // allocation des objets necessaires pour le rendu
    mgw.m_render = new Algo::Render::VBO::MapRender_VBO<PFP>(myMap, allDarts);
    mgw.m_render_topo = new Algo::Render::VBO::topo_VBORenderMapD();

    // maj des donnees de position
    mgw.m_render->updateData(Algo::Render::VBO::POSITIONS, position);
    // creation des primitives de rendu a partir de la carte
    mgw.m_render->initPrimitives(Algo::Render::VBO::TRIANGLES);
    mgw.m_render->initPrimitives(Algo::Render::VBO::LINES);

    // creation des primitives de rendu de la topologie a partir de la carte
    mgw.m_render_topo->updateData<PFP>(myMap, position, 0.9f, 0.9f);

    mgw.mainLoop();

    delete mgw.m_render;

    return 0;
}
