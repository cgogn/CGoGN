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

#include "Topology/generic/parameters.h"
#include "Topology/map/map3.h"
#include "Topology/generic/embeddedMap3.h"

#include "Geometry/matrix.h"
#include "Geometry/vector_gen.h"
#include "Algo/Import/import.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Render/map_glRender.h"
#include "Algo/Render/vbo_MapRender.h"
#include "Algo/Modelisation/tetrahedron.h"
#include "Algo/Modelisation/primitives3d.h"
#include "Algo/Modelisation/polyhedron.h"
#include "Algo/Modelisation/subdivision.h"

#include "Algo/Render/topo3_vboRender.h"

//#include "Algo/Render/topo_vboRender.h"

#include "Topology/generic/cellmarker.h"

//#include "testMaps.h"


using namespace CGoGN ;

struct PFP: public PFP_STANDARD
{
	// definition de la carte
	typedef EmbeddedMap3<Map3> MAP;
	//typedef Map3 MAP;

};

PFP::MAP myMap;
SelectorTrue allDarts;
PFP::TVEC3 position ;
Dart dglobal;

unsigned int idNorm;
unsigned int idCol;

class myGlutWin: public Utils::SimpleGlutWin
{
public:

     void myRedraw();

     float gWidthObj;
     Geom::Vec3f gPosObj;

     bool aff_help;
     bool render_line;
     bool render_volume;
     bool render_topo;

    Algo::Render::VBO::MapRender_VBO* m_render;
    Algo::Render::VBO::topo3_VBORenderMapD* m_render_topo;

    void updateVBO();

 	myGlutWin(	int* argc, char **argv, int winX, int winY):SimpleGlutWin(argc,argv,winX,winY)
    {
 	   aff_help = false;
 	   render_line = true;
 	   render_volume = false;
 	   render_topo = true;

    }

 	void myKeyboard(unsigned char keycode, int x, int y);
};

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

	glColor3f(0.0f,1.0f,.0f);
	m_render->draw(Algo::Render::VBO::POINTS);

	if(render_line)
	{
		// on trace les ligne devant
		glDisable( GL_POLYGON_OFFSET_FILL );
		glColor3f(1.0f,1.0f,0.0f);
		m_render->draw(Algo::Render::VBO::LINES);
	}
	// et on decale les faces vers l'arriere

	if(render_volume)
	{
		glEnable( GL_POLYGON_OFFSET_FILL );
		glPolygonOffset( 1.0f, 1.0f );

		glColor3f(0.0f,0.5f,0.0f);
		m_render->draw(Algo::Render::VBO::TRIANGLES);
		glDisable( GL_POLYGON_OFFSET_FILL );
	}

	if(render_topo)
	{
		glDisable( GL_POLYGON_OFFSET_FILL );
		glColor3f(1.0f,1.0f,0.0f);
		m_render_topo->drawTopo();

		Dart d = myMap.phi2( myMap.begin());
		m_render_topo->overdrawDart(d,5,1.0f,0.0f,1.0f);

	}

    //affichage de l'aide
    if (aff_help) {
            glColor3f(1.0f,1.0f,1.0f);
            printString2D(10,20,"Keys:\n\
                    l: affichage des lignes de bordures\n\
                    v: affichage des volumes\n\
                    h: affiche cette aide\n\n\n\n\
                    1 : swap 2->2\n");
    }


    glPopMatrix();


}

void myGlutWin::updateVBO()
{
	m_render->updateData(Algo::Render::VBO::POSITIONS, position );
	m_render->initPrimitives<PFP>(myMap,allDarts,Algo::Render::VBO::LINES);
	m_render->initPrimitives<PFP>(myMap,allDarts,Algo::Render::VBO::POINTS);
	m_render->initPrimitives<PFP>(myMap,allDarts,Algo::Render::VBO::TRIANGLES);
	m_render_topo->updateData<PFP>(myMap, allDarts, position,  0.9, 0.9, 0.9);
}


/// Gestion des touches clavier
void myGlutWin::myKeyboard(unsigned char keycode, int x, int y)
{
        switch(keycode) {
                /** affichage **/
        	case 'D':
        	{
				Dart d = myMap.begin();
				m_render_topo->setDartColor(d, 1.0f,0.0f,0.0f);
				d = myMap.phi1(d);
				m_render_topo->setDartColor(d, 0.0f,1.0f,0.0f);
				d = myMap.phi1(d);
				m_render_topo->setDartColor(d, 0.0f,0.0f,1.0f);
				glutPostRedisplay();

        	}
        		break;
        	case 'Q':
        		m_render_topo->setAllDartsColor(1.0f,1.0f,1.0f);
        		glutPostRedisplay();
        	break;

        		case 'c' : { //Cut Edge OK
						Dart d = 9;
						//Dart d = dglobal;

						Dart f = myMap.phi1(d);
						myMap.cutEdge(d);
						Dart e = myMap.phi1(d);
						position[e] = position[d];
						position[e] += position[f];
						position[e] *= 0.5;

						m_render_topo->updateData<PFP>(myMap, allDarts, position, 0.9, 0.9, 0.9);
						m_render_topo->setDartColor(d, 0.0f,1.0f,0.0f);
						glutPostRedisplay();
						break;
        		}
        		case 's' : { //Split Face OK
						Dart d = 25;
						//Dart d = dglobal;

						myMap.splitFace(d, myMap.phi1(myMap.phi1(d)));

						m_render_topo->updateData<PFP>(myMap, allDarts, position, 0.9, 0.9, 0.9);
						glutPostRedisplay();
						break;
        		}
        		case 'f' : { //Flip Edge
						//Dart d = 25;
        				//Dart d = myMap.phi2(25);
						Dart d = dglobal;

						Dart r = myMap.phi2(d);
						Dart e = myMap.phi2(myMap.phi3(d));

						unsigned int p1 = myMap.getDartEmbedding(VERTEX_ORBIT, myMap.phi_1(e));
						unsigned int p2 = myMap.getDartEmbedding(VERTEX_ORBIT, myMap.phi_1(r));


						myMap.flipFace(d);

						myMap.setDartEmbedding(VERTEX_ORBIT, r, p1 );
						myMap.setDartEmbedding(VERTEX_ORBIT, e, p2 );

						Dart dd = d;
						do {
							Dart e = myMap.phi2(dd);
							Dart e2= myMap.phi<32>(dd);//myMap.phi2(myMap.phi3(dd));
							myMap.setDartEmbedding(VERTEX_ORBIT, dd, myMap.getDartEmbedding(VERTEX_ORBIT,e));
							myMap.setDartEmbedding(VERTEX_ORBIT, myMap.phi3(dd), myMap.getDartEmbedding(VERTEX_ORBIT, e2));
							dd = myMap.phi1(dd);
						} while( dd!=d);


						m_render_topo->updateData<PFP>(myMap, allDarts, position, 0.9, 0.9, 0.5);
						glutPostRedisplay();
						break;
        		}
        		case 'b' : {
        			Dart d = dglobal;
        			//Dart d = myMap.phi<323>(myMap.phi_1(myMap.phi<232>(myMap.phi1(myMap.phi<112>(dglobal)))));


        			std::cout << "boundary : " << myMap.isBoundaryVolume(d) << std::endl;

        			updateVBO();
        			glutPostRedisplay();
        			break;
        		}
        		case 'E' : { //Collapse Edge
						//Dart d = 10;
						//Dart d = myMap.phi2(25);
						Dart d = dglobal;

        				//Dart d = myMap.phi<23>(myMap.phi1(myMap.phi<232>(dglobal)));

//						Dart dd = myMap.phi2(d) ;
//
//						PFP::VEC3 a = position[d];
//						PFP::VEC3 v2 = position[dd] ;
//
//						// Compute the approximated position
//						a =  (a + v2) / PFP::REAL(2);
//
//						position[d] = a;

						myMap.collapseEdge(d);

						updateVBO();
						glutPostRedisplay();
						break;
        		}
        		case 'F' : { //Collapse Face OK
						Dart d = 10;
						//Dart d = myMap.phi2(25);
						//Dart d = myMap.phi<232>(myMap.phi1(myMap.phi<232>(dglobal)));
        				//Dart d = dglobal;


//        				Dart d1 = myMap.phi1(d);
//        				Dart d11 = myMap.phi1(myMap.phi1(d));
//        				Dart d111 = myMap.phi1(myMap.phi1(myMap.phi1(d)));
//
//        				std::cout << "d=" << d << " d1=" << d1 << " d11=" << d11 << " d111=" << d111 << std::endl;

						myMap.collapseFace(d);

//        				myMap.collapseEdge(d);
//       				myMap.collapseEdge(d1);
//        				myMap.collapseEdge(d11,false,false);

						updateVBO();
						glutPostRedisplay();
						break;
        		}
        		case 'V' : { //Collapse Volume
        				m_render_topo->updateData<PFP>(myMap, allDarts, position,  0.9, 0.9, 0.9);
        				glutPostRedisplay();
        				break;
        		}
        		case 'd' : { //Volume Degree OK
        				std::cout << "degree = " << myMap.volumeDegree(dglobal) << std::endl;
        				break;
        		}
        		case 'C' : {
        				Dart d = dglobal;

						Algo::Modelisation::hexaCutVolume<PFP, AttributeHandler<PFP::VEC3>, PFP::VEC3>(myMap,d , position);

						updateVBO();
        				glutPostRedisplay();
        				break;
        		}
                case 'l' : {
                        render_line = !render_line;
                        glutPostRedisplay();
                        break;
                }
                case 'v' : {
                		render_volume = !render_volume;
                		glutPostRedisplay();
                		break;
                }
                case 't' : {
                		render_topo = !render_topo;
                		glutPostRedisplay();
                		break;
                }
                case 'h': {
                        aff_help = !aff_help;
                        glutPostRedisplay();
                        break;
                }
                case '1': {
                		Algo::Modelisation::Tetrahedron::swap2To2<PFP>(myMap, dglobal, position);
                		updateVBO();
						glutPostRedisplay();
                        break;
                }
                case '2' : {
						Algo::Modelisation::Tetrahedron::swap4To4<PFP>(myMap, dglobal, position);
						updateVBO();						glutPostRedisplay();
						break;
                }
                case '3' : {
						Algo::Modelisation::Tetrahedron::swap2To3<PFP>(myMap, dglobal, position);
						updateVBO();
						glutPostRedisplay();
                		break;
                }
                case '4' : {
                		Algo::Modelisation::Tetrahedron::swap3To2<PFP>(myMap, dglobal, position);
                		updateVBO();
                		glutPostRedisplay();
                		break;
                }
                default : {
                		std::cout << "not implemented" << std::endl;
                		break;
                }

        }
}


int main(int argc, char **argv)
{
	std::vector<std::string> attrNames ;
	Algo::Import::importInESS<PFP>(myMap, argv[1], attrNames);
	position = myMap.getAttribute<PFP::VEC3>(VERTEX_ORBIT, attrNames[0]) ;

     //plongement
//	Algo::Modelisation::Primitive3D<PFP> prim(myMap,position);
//	dglobal = prim.hexaGrid_topo(3,3,3);
//	prim.embedHexaGrid(1.0f,1.0f,1.0f);
//	Geom::Matrix44f mat;
//	mat.identity();
//	Geom::scale(2.0f, 2.0f,2.0f,mat);
//	prim.transform(mat);


	Dart d = Algo::Modelisation::Polyhedron<PFP>::createOrientedPolyhedron(myMap,6);
	dglobal=d;

	position[d] = PFP::VEC3(0);
	position[myMap.phi1(d)] = PFP::VEC3(1,0,0);
	position[myMap.phi1(myMap.phi1(d))] = PFP::VEC3(1,0,1);
	position[myMap.phi_1(d)] = PFP::VEC3(0,0,1);

	d = myMap.phi_1(myMap.phi2(myMap.phi_1(myMap.phi_1(myMap.phi2(myMap.phi_1(d))))));
	position[d] = PFP::VEC3(1,1,0);
	position[myMap.phi1(d)] = PFP::VEC3(0,1,0);
	position[myMap.phi1(myMap.phi1(d))] = PFP::VEC3(0,1,1);
	position[myMap.phi_1(d)] = PFP::VEC3(1,1,1);

//	Dart d = Algo::Modelisation::Polyhedron<PFP>::createOrientedPolyhedron(myMap,4);
//	dglobal=d;
//
//	Dart t1 = d;
//	position[t1] = PFP::VEC3(0.0f, 0.0f, 3.0f);
//	t1 = myMap.phi1(t1);
//	position[t1] = PFP::VEC3(0.0f, 1.0f, 3.0f);
//	t1 = myMap.phi1(t1);
//	position[t1] = PFP::VEC3(1.0f, 0.5f, 3.0f);
//	t1 = myMap.phi_1(myMap.phi2(d));
//	position[t1] = PFP::VEC3(0.5f, 0.5f, 4.0f);

	//Dart d = Algo::Modelisation::Polyhedron<PFP>::createOrientedPrism(myMap);
//	Algo::Modelisation::Polyhedron<PFP> p(myMap, position);
//	dglobal = p.cube_topo(3,1,1);
//	p.embedCube(1.0f,1.0f,1.0f);


	//dglobal = createMap9<PFP>(myMap,position);


    // un peu d'interface
	myGlutWin mgw(&argc,argv,800,800);

    // calcul de la bounding box
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap,position);
    // pour l'affichage
    mgw.gWidthObj = std::max<float>( std::max<float>(bb.size(0),bb.size(1)),bb.size(2));
    mgw.gPosObj =  (bb.min() +  bb.max()) /2.0f;


    // allocation des objets necessaires pour le rendu
    mgw.m_render = new Algo::Render::VBO::MapRender_VBO();
    mgw.m_render_topo = new Algo::Render::VBO::topo3_VBORenderMapD();


    mgw.updateVBO();



    mgw.mainLoop();

    delete mgw.m_render;

    return 0;
}
