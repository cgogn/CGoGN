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
#include "Utils/os_spec.h"

#include "Utils/GLSLShader.h"
#include "Utils/glutwin.h"

#include "Topology/generic/parameters.h"
#include "Topology/map/map2.h"
#include "Topology/generic/embeddedMap2.h"

#include "Geometry/vector_gen.h"

#include "Algo/Import/import.h"

#include "Algo/Render/map_glRender.h"
#include "Algo/Render/vbo_MapRender.h"

#include "Algo/Geometry/normal.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Geometry/area.h"
#include "Algo/Geometry/intersection.h"

#include "Algo/Modelisation/polyhedron.h"
#include "Algo/Modelisation/subdivision.h"
#include "Algo/Modelisation/extrusion.h"

#include "Algo/MC/marchingcubeGen.h"

#include "Topology/generic/ecell.h"

#include "Algo/Export/export.h"

using namespace CGoGN;

struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef EmbeddedMap2<Map2> MAP;
};


PFP::MAP myMap;
SelectorTrue allDarts;
PFP::TVEC3 position;
PFP::TVEC3 normal;


class myGlutWin: public Utils::SimpleGlutWin
{
public:
        Geom::Vec4f colDif;

        Geom::Vec4f colSpec;

        Geom::Vec4f colClear;

        Geom::Vec4f colNormal;

        float shininess;

        /**
         * position of object
         */
        Geom::Vec3f gPosObj;

        /**
         * width of object
         */
        float gWidthObj;

        /**
         * factor to apply to normal drawing
         */
        float normalScaleFactor;

        /**
         * redraw CB
         */
        void myRedraw();

        /**
         * keyboard CB
         */
        void myKeyboard(unsigned char keycode, int x, int y);

		 /**
		 * inverse the normal when computing normal
		 */
        bool invertedNormals;

        /**
         * inverse object for culling
         */
        bool invertedObject;

        /**
         * rendering normals ?
         */
        bool renderNormal;

        /**
         * rendering lines ?
         */
        bool renderLines;

        /**
        * aide affichee
        */
        bool aff_help;

        /**
         * style of rendering
         */
        int renderStyle;

        Algo::Render::GL2::MapRender_VBO* m_render;

        /**
         * render mode enum
         */
        enum { CLEAR=1, LINE, FLAT, GOURAUD, PHONG, NORMAL,NONE };

	myGlutWin(	int* argc, char **argv, int winX, int winY) :
				SimpleGlutWin(argc,argv,winX,winY),
				renderNormal(false),
				renderLines(false),
				aff_help(true)
        {
			if (this->shaderOk) shaders[0].loadShaders("phong_vs.txt","phong_ps.txt");

//			m_cb = new Algo::Render::GL2::GL2_CB_PositionNormal<PFP>(myMap);
//			m_render = new Algo::Render::GL2::MapRender_VBO(myMap, allDarts, m_cb);
//
//			m_render->initBuffers();
//			m_render->updateData(Algo::Render::GL2::POSITIONS, position );
//			m_render->updateData(Algo::Render::GL2::NORMALS, normal );
//			m_render->initPrimitives<PFP>(myMap, good, Algo::Render::GL2::TRIANGLES);
//			m_render->initPrimitives<PFP>(myMap, good, Algo::Render::GL2::LINES);

		}
};


void myGlutWin::myRedraw(void)
{

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPushMatrix();

        float sc = 50./gWidthObj;
        glScalef(sc,sc,sc);
        glTranslatef(-gPosObj[0],-gPosObj[1],-gPosObj[2]);

        glEnable(GL_LIGHTING);
    	glEnable(GL_COLOR_MATERIAL);
    	glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);
    	glColor3f(0.7f,0.8f,1.0f);

    	glMaterialfv(GL_FRONT,GL_SPECULAR,colSpec.data());
    	glMaterialf( GL_FRONT, GL_SHININESS, shininess );

    	Algo::Geometry::computeNormalVertices<PFP>(myMap, position, normal) ;

    	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
//        m_render->draw(Algo::Render::GL2::TRIANGLES);
       	Algo::Render::Direct::renderTriQuadPoly<PFP>(myMap, Algo::Render::Direct::FLAT, 0.9 , position, normal, normal);

        glDisable(GL_LIGHTING);
        glColor3f(0.0f,0.0f,0.0f);
//        m_render->draw(Algo::Render::GL2::LINES);
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      	Algo::Render::Direct::renderTriQuadPoly<PFP>(myMap, Algo::Render::Direct::LINE, 1.0, position, normal);
//      	glPointSize(5.0f);
//      	glColor3f(1.0f,.0f,.0f);
//        glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
//      	Algo::Render::Direct::renderTriQuadPoly<PFP>(myMap, Algo::Render::Direct::LINE, 0.9, position, normal);


//    	Algo::Render::Direct::renderTriQuadPoly<PFP>(myMap, Algo::Render::Direct::FLAT, 0.9, position, normal);
//		TESTED OK


        glPopMatrix();

        // a faire en dernier pour que ca dessine au dessus
        if (aff_help) {
                glColor3f(1.0f,1.0f,1.0f);
                printString2D(10,20,"Keys:\n----\nf : flat (DL)\ng: gouraud (DL) \np: phong ( strip DL)\na: dual\nu: simplification (nbfaces/2)\nC : catmullclark\nR : square3\nL: Loop\nO: inverse objet\nn: affiche normales\nZ/z: shininess\nD/d: focale\nv: calcul fps\nh: affiche l'aide");
        }

}

void myGlutWin::myKeyboard(unsigned char keycode, int, int)
{

	switch (keycode)
	{
	case 'a':
		{
			unsigned int end = position.end();
			for(unsigned int it = position.begin(); it !=  end; position.next(it))
			{
				position[it] += normal[it] * 0.1;
			}

			m_render->updateData(Algo::Render::GL2::POSITIONS, position );
		}
		break;

	case 'z':
		{
			unsigned int end = normal.end();
			for(unsigned int it = normal.begin(); it !=  end; normal.next(it))
			{
				normal[it] *= -1.0f;
			}

			m_render->updateData(Algo::Render::GL2::NORMALS, normal );
		}
		break;

	case 's':
		{
			GLint t1 = glutGet(GLUT_ELAPSED_TIME);
			Algo::Modelisation::CatmullClarkSubdivision<PFP>(myMap, position);
			GLint t2 = glutGet(GLUT_ELAPSED_TIME);
			GLfloat seconds = (t2 - t1) / 1000.0f;
			std::cout << "catmull-clark: "<< seconds << "sec" << std::endl;

			Algo::Geometry::computeNormalVertices<PFP>(myMap, position, normal) ;

			break;
		}

	case 'l':
		{
			GLint t1 = glutGet(GLUT_ELAPSED_TIME);
			Algo::Modelisation::LoopSubdivision<PFP>(myMap, position);
			Algo::Modelisation::LoopSubdivision<PFP>(myMap, position);
			Algo::Modelisation::LoopSubdivision<PFP>(myMap, position);
			Algo::Modelisation::LoopSubdivision<PFP>(myMap, position);
			Algo::Modelisation::LoopSubdivision<PFP>(myMap, position);
			GLint t2 = glutGet(GLUT_ELAPSED_TIME);
			GLfloat seconds = (t2 - t1) / 1000.0f;
			std::cout << "loop: "<< seconds << "sec" << std::endl;

			Algo::Geometry::computeNormalVertices<PFP>(myMap, position, normal, allDarts) ;

			break;
		}

	case 'd':
		{
			GLint t1 = glutGet(GLUT_ELAPSED_TIME);
			Algo::Modelisation::Sqrt3Subdivision<PFP>(myMap, position);
			GLint t2 = glutGet(GLUT_ELAPSED_TIME);
			GLfloat seconds = (t2 - t1) / 1000.0f;
			std::cout << "dual: "<< seconds << "sec" << std::endl;

			Algo::Geometry::computeNormalVertices<PFP>(myMap, position, normal, allDarts) ;

			break;
		}
	}

	glutPostRedisplay();
}



class Scal3D
{
protected:
	int m_size;
	int m_mid;
public:
	Scal3D(int size) : m_size(size), m_mid(size/2) {}
	float getVoxSizeX() {return 1.0f;}
	float getVoxSizeY() {return 1.0f;}
	float getVoxSizeZ() {return 1.0f;}

	long getWidthX() { return m_size;}
	long getWidthY() { return m_size;}
	long getWidthZ() { return m_size;}

	float getVoxel(long x, long y, long z)
	{
		float  rad = sqrt(float((x-m_mid)*(x-m_mid) + (y-m_mid)*(y-m_mid) + (z-m_mid)*(z-m_mid)));
		float  radx = sqrt(float((y-m_mid)*(y-m_mid) + (z-m_mid)*(z-m_mid)));
		float  rady = sqrt(float((x-m_mid)*(x-m_mid) + (z-m_mid)*(z-m_mid)));
		float  radz = sqrt(float((x-m_mid)*(x-m_mid) + (y-m_mid)*(y-m_mid)));

		float val = 1.0f - rad/m_mid;

		if ((val <0.0f) || (radz < m_size/10)||(rady < m_size/8)||(radx < m_size/6))
			return 0.0f;
		return val;
	}
};




int main(int argc, char **argv)
{

//    GLint t1 = glutGet(GLUT_ELAPSED_TIME);

	if (argc == 1)
	{
		position = myMap.addAttribute<Geom::Vec3f>(VERTEX_ORBIT, "position");

		std::cout <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<std::endl;
//		typedef uint8 DATATYPE;
//		Algo::MC::Image<DATATYPE> myImg;
//		myImg.loadPNG3D("liver.png");
//
//		std::cout << "Image chargee"<<std::endl;
//		std::cout << myImg.getWidthX() <<"x"<< myImg.getWidthY() <<"x"<< myImg.getWidthZ() << "voxels"<<std::endl;
//
//		// ajouté pour vérifier que ça marche sur les images non bool
//// 		myImg.Blur3();
//// 		std::cout << "Image lissee"<<std::endl;
//
//		// fonction de fenetrage de type superieur à ...
//		Algo::MC::WindowingGreater<DATATYPE> myWindFunc;
//
//		// valeur utilisee 0 (donc objet = tout ce qui est diff de 0)
//		myWindFunc.setIsoValue(DATATYPE(127));
//
//		// instanciation du mc
//		std::cout << "mc init"<<std::endl;
//		Algo::MC::MarchingCube<DATATYPE,Algo::MC::WindowingGreater,PFP> mc(&myImg, &myMap, position, myWindFunc, false);
//	//	MarchingCube<DATATYPE,WindowingDiff> mc(&myImg, &myMap, myWindFunc, false);
//
//		// realisation du maillage
//		mc.simpleMeshing();
//		std::cout << "mc ok"<<std::endl;

//		typedef float DATATYPE;
//
//		// fonction de fenetrage de type superieur à ...
//		Algo::MC::WindowingGreater<DATATYPE> myWindFunc;
//
//		// valeur utilisee 0 (donc objet = tout ce qui est diff de 0)
//		myWindFunc.setIsoValue(DATATYPE(0.32f));
//
//		Scal3D myImg(256);
//
//		std::cout << "mc init"<<std::endl;
//		Algo::MC::MarchingCubeGen<DATATYPE, Scal3D, Algo::MC::WindowingGreater,PFP> mc(&myImg, &myMap, position, myWindFunc, false);
//
//		//realisation du maillage
//		mc.simpleMeshing();

		std::vector<Geom::Vec3f> objV;
		objV.push_back(Geom::Vec3f(-1,-1,37));
		objV.push_back(Geom::Vec3f(-1,-2,37));
		objV.push_back(Geom::Vec3f(+1,-2,37));
		objV.push_back(Geom::Vec3f(+1,-1,37));
		objV.push_back(Geom::Vec3f(+2,-1,37));
		objV.push_back(Geom::Vec3f(+2,+1,37));
		objV.push_back(Geom::Vec3f(+1,+1,37));
		objV.push_back(Geom::Vec3f(+1,+2,37));
		objV.push_back(Geom::Vec3f(-1,+2,37));
		objV.push_back(Geom::Vec3f(-1,+1,37));
		objV.push_back(Geom::Vec3f(-2,+1,37));
		objV.push_back(Geom::Vec3f(-2,-1,37));

		std::vector<Geom::Vec3f> pathV;
		std::vector<float> pathRadius;
		pathV.push_back(Geom::Vec3f(0,0,0));		pathRadius.push_back(1.);
		pathV.push_back(Geom::Vec3f(0,0,10));		pathRadius.push_back(1.5);
		pathV.push_back(Geom::Vec3f(10,0,10));		pathRadius.push_back(1.9);
		pathV.push_back(Geom::Vec3f(10,10,10));		pathRadius.push_back(2.5);
		pathV.push_back(Geom::Vec3f(10,10,20));		pathRadius.push_back(3.0);
		pathV.push_back(Geom::Vec3f(10,11,25));		pathRadius.push_back(1.0);
		pathV.push_back(Geom::Vec3f(11,13,30));		pathRadius.push_back(1.5);
		pathV.push_back(Geom::Vec3f(0,0,40));		pathRadius.push_back(.5);
		pathV.push_back(Geom::Vec3f(-20,-10,0.0));	pathRadius.push_back(1.9);
		pathV.push_back(Geom::Vec3f(-10,0 ,-20));	pathRadius.push_back(1.3);
		pathV.push_back(Geom::Vec3f(5,0 ,-10));		pathRadius.push_back(1.);
	//	Algo::Modelisation::extrusion<PFP>(myMap, objV, Geom::Vec3f(0.0,0.0,37.0), Geom::Vec3f(0.0,0.0,1.0),false, pathV, false);
		Algo::Modelisation::extrusion_scale_prim<PFP>(myMap, position, objV, Geom::Vec3f(0.0,0.0,37.0), Geom::Vec3f(0.0,0.0,1.0),false, pathV, false, pathRadius);
	}

//	{
//		Algo::Modelisation::Primitive<PFP> prim(myMap,position);
//
//		prim.grid_topo(2,2);
//
//		prim.embedGrid(8.0f,8.0f);
//
//		Algo::Modelisation::Primitive<PFP> prim2(myMap,position);
//
//		prim2.tore_topo(7,4);
//		prim2.embedTore(25.0f,5.0f);
//
//		GLint t1 = glutGet(GLUT_ELAPSED_TIME);
//
//		myMap.getAttributeContainer(VERTEX_ORBIT).toggleProcess(position);
////		myMap.getAttributeContainer(VERTEX_ORBIT).toggleProcess(idPipo);
//
//		PFP::MAP::EVertex::setContainer(myMap.getAttributeContainer(VERTEX_ORBIT));
//		PFP::MAP::EVertex::setMap(myMap);
//		PFP::MAP::EVertex ec(0);
//
//		GLint t2 = glutGet(GLUT_ELAPSED_TIME);
//			GLfloat seconds = (t2 - t1) / 1000.0f;
//			std::cout << "triangulation: "<< seconds << "sec" << std::endl;
//
//		Marker m = myMap.getNewMarker(VERTEX_ORBIT);
//		myMap.markEmbVertex(myMap.begin(),m);
//
//		Dart xd = myMap.alpha1(myMap.begin());
//		if (myMap.isMarkedEmbVertex(xd,m))
//		{
//			std::cout << "Marke"<< std::endl;
//		}
//		xd = myMap.phi2(xd);
//		if (myMap.isMarkedEmbVertex(xd,m))
//		{
//			std::cout << "Marke aussi"<< std::endl;
//		}
//
//		myMap.clearEmbMarkers(m,VERTEX_ORBIT);
//		myMap.releaseEmbMarker(m,VERTEX_ORBIT);
//
//	}
	else
	{
		SelectorTrue allDarts;
//		GLint t1 = glutGet(GLUT_ELAPSED_TIME);

		std::vector<std::string> attrNames ;
    	if(!Algo::Import::importMesh<PFP>(myMap, std::string(argv[1]), attrNames))
    	{
    		std::cerr << "could not import " << std::string(argv[1]) << std::endl ;
    		exit(1);
    	}
		position = myMap.getAttribute<PFP::VEC3>(VERTEX_ORBIT, attrNames[0]) ;

		GLint t1 = glutGet(GLUT_ELAPSED_TIME);

//		Algo::Export::exportCTM<PFP>(myMap, position, std::string("pipo.ctm"));

//        Algo::Modelisation::LoopSubdivision<PFP, PFP::TVEC3, PFP::VEC3>(myMap,position);
//        Algo::Modelisation::LoopSubdivision<PFP, PFP::TVEC3, PFP::VEC3>(myMap,position);
//        Algo::Modelisation::CatmullClark<PFP, PFP::TVEC3, PFP::VEC3>(myMap,position);
//        Algo::Modelisation::CatmullClark<PFP, PFP::TVEC3, PFP::VEC3>(myMap,position);


//		PFP::MAP::EVertex::setContainer(myMap.getAttributeContainer(VERTEX_ORBIT));
//		PFP::MAP::EVertex::setMap(myMap);
//		PFP::MAP::EVertex ec(0);
//		myMap.getAttributeContainer(VERTEX_ORBIT).toggleProcess(position);
//
//		Algo::Modelisation::LoopSubdivision<PFP, PFP::EVERTEX, PFP::EVERTEX>(myMap,ec);
//		Algo::Modelisation::LoopSubdivision<PFP, PFP::EVERTEX, PFP::EVERTEX>(myMap,ec);
//	    Algo::Modelisation::CatmullClark<PFP, PFP::EVERTEX, PFP::EVERTEX>(myMap,ec);
//	    Algo::Modelisation::CatmullClark<PFP, PFP::EVERTEX, PFP::EVERTEX>(myMap,ec);


		GLint t2 = glutGet(GLUT_ELAPSED_TIME);
		GLfloat seconds = (t2 - t1) / 1000.0f;
		std::cout << "subdiv: "<< seconds << "sec" << std::endl;


//        myMap.getAttributeContainer(VERTEX_ORBIT).toggleProcess(position);
//		PFP::MAP::EVertex::setContainer(myMap.getAttributeContainer(VERTEX_ORBIT));
//		PFP::MAP::EVertex::setMap(myMap);
//		PFP::MAP::EVertex ec(0);
//        Algo::Modelisation::triangleSubdivide<PFP, PFP::EVERTEX, PFP::EVERTEX>(myMap,ec);
//        Algo::Modelisation::triangleSubdivide<PFP, PFP::EVERTEX, PFP::EVERTEX>(myMap,ec);
//        Algo::Modelisation::triangleSubdivide<PFP, PFP::EVERTEX, PFP::EVERTEX>(myMap,ec);

        std::cout << "Nombre de sommets plonges  "<< myMap.getNbCells(VERTEX_ORBIT)<<std::endl;
        std::cout << "Nombre de sommets topo: "<< myMap.getNbOrbits(0)<<std::endl;
        std::cout << "Nombre de faces topo: "<< myMap.getNbOrbits(2)<<std::endl;


//		GLint t2 = glutGet(GLUT_ELAPSED_TIME);
//		GLfloat seconds = (t2 - t1) / 1000.0f;
//		std::cout << "import: "<< seconds << "sec" << std::endl;

	}

		float area = Algo::Geometry::totalArea<PFP>(myMap, position) ;
		std::cout << "total area = " << area << std::endl ;

		bool inter = Algo::Geometry::areTrianglesInIntersection<PFP>(myMap,0,10,position) ;
		std::cout << "intersection = " << inter << std::endl ;

		normal = myMap.addAttribute<Geom::Vec3f>(VERTEX_ORBIT, "normals");
//		PFP::NORMAL normal(normal,myMap);



//		Algo::Geometry::computeNormalVertices<PFP>(myMap, position, normal, allDarts) ;

//		AttributeHandler<Mark> tm(0,myMap);
//		for (Dart dd=myMap.begin(); dd!= myMap.end(); myMap.next(dd))
//		{
//			unsigned int a = myMap.getEmbedding(dd, 0);
//			std::cout << "dart:"<<dd.index<< " / ";
//			std::cout << "emb:"<<a<< " / ";
//			std::cout<<position[a]<< " / ";
//			std::cout << tm[a]<< std::endl;
//		}



        Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap,position);

        std::cout << "BB: "<< bb.min() << " / "<< bb.max() << std::endl;

       // compute width and position of object for centering

        myGlutWin mgw(&argc,argv,800,800);
        mgw.gPosObj = bb.center();

        float tailleX = bb.size(0);
        float tailleY = bb.size(1);
        float tailleZ = bb.size(2);

        mgw.gWidthObj = std::max<float>( std::max<float>(tailleX,tailleY),tailleZ);
        mgw.normalScaleFactor = std::min<float>( std::min<float>(tailleX,tailleY),tailleZ) / 50.0f;


        mgw.colClear = Geom::Vec4f(0.2f,0.2f,0.2f,0.1);
        mgw.colDif = Geom::Vec4f(0.8f,0.9f,0.7f,1.0f);
        mgw.colSpec = Geom::Vec4f(0.9f,0.9f,0.9f,1.0f);
        mgw.colNormal = Geom::Vec4f(1.0f,0.0f,0.0f,1.0f);
        mgw.shininess=80.0f;


        mgw.mainLoop();

        return 0;
}
