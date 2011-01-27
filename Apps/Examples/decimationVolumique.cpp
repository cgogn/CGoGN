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
/*
 * TODO
 * - pb dans l'ordre des coeff shrinkage du rendu topo
 * - rendu explod object
 * - modifier le update du rendu topo
 * - shader pour l'affichage
 */

#include "squelette3carte.h"

#include "Algo/DecimationVolumique/decimator.h"


#include "Utils/GLSLShader.h"

/**********************************************************************************************
 *                                 AntTweakBar CALLBACKS                                      *
 **********************************************************************************************/
void TW_CALL setRenderObjectCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->render_obj = *(GLboolean*)value ;
	if(mgw->render_obj) {
		mgw->initRender() ;
	}
	glutPostRedisplay() ;
}
void TW_CALL getRenderObjectCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(GLboolean*)value = mgw->render_obj ;
}

void TW_CALL setRenderObjStyleCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->renderObjStyle = *(MyGlutWin::renderObjStyleType*)value ;
	glutPostRedisplay() ;
}
void TW_CALL getRenderObjStyleCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(GLenum*)value = mgw->renderObjStyle ;
}

void TW_CALL setRenderTopoCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->render_topo = *(GLboolean*)value ;
	glutPostRedisplay() ;
}
void TW_CALL getRenderTopoCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(GLboolean*)value = mgw->render_topo ;
}

void TW_CALL setRenderTopoStyleCB(const void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	mgw->renderTopoStyle = *(MyGlutWin::renderTopoStyleType*)value;

	if(mgw->renderTopoStyle == MyGlutWin::T_ALL)
	{
		TwDefine("Viewer/phi1_explod visible=false");
		TwDefine("Viewer/phi2_explod visible=false");
		TwDefine("Viewer/phi3_explod visible=false");
	}
	else if(mgw->renderTopoStyle == MyGlutWin::T_EXPLOD)
	{
		TwDefine("Viewer/phi1_explod visible=true");
		TwDefine("Viewer/phi2_explod visible=true");
		TwDefine("Viewer/phi3_explod visible=true");
	}

	glutPostRedisplay() ;
}
void TW_CALL getRenderTopoStyleCB(void* value, void* clientData)
{
	MyGlutWin* mgw = static_cast<MyGlutWin*>(clientData) ;
	*(GLenum*)value = mgw->renderTopoStyle ;
}


/**********************************************************************************************
 *                                 MyGlutWin IMPLEMENTATION                                   *
 **********************************************************************************************/


MyGlutWin::MyGlutWin(int* argc, char **argv, int winX, int winY) :
	GlutWin_ATB(argc, argv, winX, winY),
	renderObjStyle(O_ALL),
	renderTopoStyle(T_ALL),
	render_obj(true),
	render_topo(false),
	render_help(true)
{
	//AJOUT Shader
	if (this->shaderOk)
	{
		shaders[0].loadShaders("clippingSphere_vs.txt","clippingSphere_ps.txt","clippingSphere_gs.txt");
		shaders[1].loadShaders("vol_clip_vs.txt","vol_clip_ps.txt","vol_clip_gs.txt");
		shaders[2].loadShaders("flat_vs.txt","flat_ps.txt","flat_gs.txt");
		shaders[3].loadShaders("flat.vs","flat.fs","flat.gs");
	}
}


void MyGlutWin::myRedraw(void)
{
	glClearColor(backCol[0],backCol[1],backCol[2],backCol[3]);
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
		m_render->updateData(Algo::Render::VBO::POSITIONS, position );
		//m_render->initPrimitives(Algo::Render::VBO::POINTS);

		if(renderObjStyle == O_ALL)
		{
			m_render->initPrimitives(Algo::Render::VBO::TRIANGLES);
		    m_render->initPrimitives(Algo::Render::VBO::LINES);

			glEnable( GL_POLYGON_OFFSET_FILL );
			glPolygonOffset( 1.0f, 1.0f );

			glColor3f(0.0f,0.5f,0.0f);
			//shaders[3].bind();
			m_render->draw(Algo::Render::VBO::TRIANGLES);
			//shaders[3].unbind();
			glDisable( GL_POLYGON_OFFSET_FILL );
			// on trace les ligne devant
			glColor3f(1.0f,1.0f,0.0f);

			m_render->draw(Algo::Render::VBO::LINES);

		}
		else if(renderObjStyle == O_FACES)
		{
			m_render->initPrimitives(Algo::Render::VBO::TRIANGLES);

			glEnable( GL_POLYGON_OFFSET_FILL );
			glPolygonOffset( 1.0f, 1.0f );

			glColor3f(0.0f,0.5f,0.0f);
			m_render->draw(Algo::Render::VBO::TRIANGLES);
			glDisable( GL_POLYGON_OFFSET_FILL );
		}
		else if(renderObjStyle == O_LINE)
		{
			m_render->initPrimitives(Algo::Render::VBO::LINES);

			// on trace les ligne devant
			glDisable( GL_POLYGON_OFFSET_FILL );
			glColor3f(1.0f,1.0f,0.0f);
			m_render->draw(Algo::Render::VBO::LINES);
		}
		else if(renderObjStyle == O_EXPLOD)
		{

		}
	}

	if(render_topo)
	{
		if(renderTopoStyle == T_ALL)
		{
			glDisable( GL_POLYGON_OFFSET_FILL );
			glColor3f(1.0f,1.0f,0.0f);
			m_render_topo->updateData<PFP>(myMap,allDarts,position,0.9,0.9,0.9);
			m_render_topo->drawTopo();
		}
		else if(renderTopoStyle == T_EXPLOD)
		{
			glDisable( GL_POLYGON_OFFSET_FILL );
			glColor3f(1.0f,1.0f,0.0f);
			m_render_topo->updateData<PFP>(myMap,allDarts,position,coefTopoExplod[0],coefTopoExplod[1],coefTopoExplod[2]);
			m_render_topo->drawTopo();
		}
	}

	if(render_help)
	{
        glColor3f(1.0f,1.0f,1.0f);
        int x = getWinWidth() - 200 ;
        printString2D(x,20,
        		"Keys:\n"
        		"d: decimation\n"
                "h: affiche cette aide\n"
        );
	}


	glPopMatrix();
}

/// Gestion des touches clavier
void MyGlutWin::myKeyboard(unsigned char keycode, int x, int y)
{
        switch(keycode) {
                /** affichage **/
        		case 'd' :
        		{
        				int nbVertices = myMap.getNbOrbits(VERTEX_ORBIT) ;

        				std::cout << "nb darts = " << myMap.getNbOrbits(DART_ORBIT) << std::endl;

        				GLint t1 = glutGet(GLUT_ELAPSED_TIME);
        			 	Algo::DecimationVolumique::decimate<PFP>(myMap, Algo::DecimationVolumique::S_Random, Algo::DecimationVolumique::A_Centroid, position, nbVertices * 0.75);
        				GLint t2 = glutGet(GLUT_ELAPSED_TIME);
        				GLfloat seconds = (t2 - t1) / 1000.0f;
        				std::cout << "decimation: "<< seconds << "sec" << std::endl;
        				break;
        		}
        		case 'h':
        		{
        				render_help = !render_help;
        				break;
        		}
                default :
                {
                		std::cout << "not implemented" << std::endl;
                		break;
                }

        }
}

void MyGlutWin::init()
{
	coefObjExplod = Geom::Vec3f(1.,1.,1.);
	coefTopoExplod = Geom::Vec3f(0.9,0.9,0.9);
	backCol = Geom::Vec4f(0.533, 0.560, 0.984, 0.984);
}

void MyGlutWin::initRender()
{
    // maj des donnees de position
	GLint t1 = glutGet(GLUT_ELAPSED_TIME);
    m_render->updateData(Algo::Render::VBO::POSITIONS, position );
	GLint t2 = glutGet(GLUT_ELAPSED_TIME);
	GLfloat seconds = (t2 - t1) / 1000.0f;
	std::cout << "updateData: "<< seconds << "sec" << std::endl;
    // creation des primitives de rendu a partir de la carte
	t1 = glutGet(GLUT_ELAPSED_TIME);
    m_render->initPrimitives(Algo::Render::VBO::TRIANGLES);
    m_render->initPrimitives(Algo::Render::VBO::LINES);
    t2 = glutGet(GLUT_ELAPSED_TIME);
    seconds = (t2 - t1) / 1000.0f;
    std::cout << "initPrimitives: "<< seconds << "sec" << std::endl;

    // creation des primitives de rendu de la topologie a partir de la carte
    t1 = glutGet(GLUT_ELAPSED_TIME);
	//m_render_topo->updateData<PFP>(myMap,allDarts,position,coefTopoExplod[0],coefTopoExplod[1],coefTopoExplod[2]);
    t2 = glutGet(GLUT_ELAPSED_TIME);
    seconds = (t2 - t1) / 1000.0f;
    std::cout << "map update: "<< seconds << "sec" << std::endl;
}

void MyGlutWin::initGUI()
{
	//Menu bar
	viewer_bar = TwNewBar("Viewer") ;
	TwDefine("Viewer position='16 16' size='220 450' valueswidth='70'") ;

	//Rendering
	TwAddVarCB(viewer_bar, "Show Object", TW_TYPE_BOOL8, setRenderObjectCB, getRenderObjectCB, this, "");
	TwType tw_render_style = TwDefineEnum("Shading","All,Faces,Lines,Explod") ;
	TwAddVarCB(viewer_bar, "Render Style", tw_render_style, setRenderObjStyleCB, getRenderObjStyleCB, this, "") ;
	TwAddVarCB(viewer_bar, "Show Topology", TW_TYPE_BOOL8, setRenderTopoCB, getRenderTopoCB, this, "") ;
	TwType tw_render_topo = TwDefineEnum("TopoShading","All,Explod");
	TwAddVarCB(viewer_bar, "Render Topology", tw_render_topo, setRenderTopoStyleCB, getRenderTopoStyleCB, this, "") ;
	TwAddVarRW(viewer_bar, "phi1_explod", TW_TYPE_FLOAT, &coefTopoExplod[1], "min=0 max=1 step=0.1 label='Phi1 Explod Coeff'");
	TwAddVarRW(viewer_bar, "phi2_explod", TW_TYPE_FLOAT, &coefTopoExplod[2], "min=0 max=1 step=0.1 label='Phi2 Explod Coeff'");
	TwAddVarRW(viewer_bar, "phi3_explod", TW_TYPE_FLOAT, &coefTopoExplod[0], "min=0 max=1 step=0.1 label='Phi3 Explod Coeff'");
	TwDefine("Viewer/phi1_explod visible=false");
	TwDefine("Viewer/phi2_explod visible=false");
	TwDefine("Viewer/phi3_explod visible=false");

	TwAddSeparator(viewer_bar, "", "") ;

	//Visualisation
	TwAddVarRW(viewer_bar, "background color", TW_TYPE_COLOR4F, &backCol, "");
}

/******** Partie a supprimer ********/
void maillageTest()
{
	/// on creer une carte
	Dart dd = Algo::Modelisation::Polyhedron<PFP>::createOrientedPolyhedron(myMap,4);
	Dart ee = Algo::Modelisation::Polyhedron<PFP>::createOrientedPolyhedron(myMap,4);
    myMap.sewVolumes(dd,ee);

    // plongement
    Dart t1 = dd;
    position[t1] = PFP::VEC3(0.0f, 0.0f, 3.0f);
    t1 = myMap.phi1(t1);
    position[t1] = PFP::VEC3(0.0f, 1.0f, 3.0f);
    t1 = myMap.phi1(t1);
    position[t1] = PFP::VEC3(1.0f, 0.5f, 3.0f);
    t1 = myMap.phi_1(myMap.phi2(dd));
    position[t1] = PFP::VEC3(0.5f, 0.5f, 4.0f);

    Dart t2 = ee;
    position[t2] = PFP::VEC3(0.0f, 1.0f, 3.0f);
    t2 = myMap.phi1(t2);
    position[t2] = PFP::VEC3(0.0f, 0.0f, 3.0f);
    t2 = myMap.phi1(t2);
    position[t2] = PFP::VEC3(1.0f, 0.5f, 3.0f);
    t2 = myMap.phi_1(myMap.phi2(ee));
    position[t2] = PFP::VEC3(0.5f, 0.5f, 2.0f);
}
/***************************/

int main(int argc, char **argv)
{
	//Initilisation du plongement geometrique
	position = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "position");

	/*
	 * Chargement de maillages
	 */
	if(argc < 2)
	{
			std::cerr << "usage : " << argv[0] << " -off" << std::endl;
			std::cerr << "or" << std::endl;
			std::cerr << "usage : " << argv[0] << " -tet" << std::endl;
			std::cerr << "or" << std::endl;
			std::cerr << "usage : " << argv[0] << " -ts" << std::endl;
			return 1;
	}
	else if(std::string(argv[1]) == "-off")
	{
		std::cout << argv[1] << std::endl;
		if (argc != 4) {
			std::cerr << "usage : " << argv[0] << " -off <filename.off> <filename.ele>" << std::endl;
			return 1;
		}

		Algo::Import::importOFFWithELERegions<PFP>(myMap,position,argv[2],argv[3]);
	}
	else if(std::string(argv[1]) == "-tet")
	{
			if (argc != 3) {
				std::cerr << "usage : " << argv[0] << " -tet <filename.tet>" << std::endl;
				return 1;
			}

			Algo::Import::importTet<PFP>(myMap,position,argv[2]);
	}
	else if(std::string(argv[1]) == "-ts")
	{
			if (argc != 3) {
				std::cerr << "usage : " << argv[0] << " -ts <filename.ts>" << std::endl;
				return 1;
			}

			Algo::Import::importTs<PFP>(myMap,position,argv[2]);

			//scalar = myMap.getAttribute<PFP::REAL>(VERTEX_ORBIT, "scalar");
	}
	else
	{
		maillageTest();
	}

	//std::cout << "Nb Tetrahedron = " << myMap.getNbOrbits(VOLUME_ORBIT) << std::endl;
	//		" / Nb Edges = " << myMap.getNbOrbits(EDGE_ORBIT) <<
	//		" / Nb Vertices = " << myMap.getNbOrbits(VERTEX_ORBIT) << std::endl;

    // un peu d'interface
	MyGlutWin* mgw = new MyGlutWin(&argc,argv,1200,800);

    // calcul de la bounding box
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap,position);
    // pour l'affichage
    mgw->gWidthObj = std::max<float>( std::max<float>(bb.size(0),bb.size(1)),bb.size(2));
    mgw->gPosObj =  (bb.min() +  bb.max()) /2.0f;


    // allocation des objets necessaires pour le rendu
    mgw->m_render = new Algo::Render::VBO::MapRender_VBO<PFP>(myMap, allDarts);
    //mgw->m_render_topo = new Algo::Render::VBO::topo3_VBORenderMap<PFP::MAP>();
    mgw->m_render_topo = new Algo::Render::VBO::topo3_VBORenderMapD();

    mgw->init();
    mgw->initRender();
    mgw->initGUI();
    mgw->mainLoop();

    delete mgw->m_render;
    delete mgw->m_render_topo;
    delete mgw;

    return 0;
}
