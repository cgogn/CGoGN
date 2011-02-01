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
#include <typeinfo>

#include <iostream>
#include "Utils/os_spec.h"

#include "Utils/GLSLShader.h"
#include "Utils/glutwin.h"

#include "Topology/generic/parameters.h"
#include "Topology/map/map2.h"

#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"

#include "Algo/Import/import.h"

#include "Algo/Render/map_glRender.h"
#include "Algo/Render/vbo_MapRender.h"

#include "Algo/Geometry/normal.h"

#include "Container/fakeAttribute.h"
#include "Topology/generic/ecell.h"

#include <fstream>

using namespace CGoGN;


// Q: ou mettre lerp

template <typename X, typename PFP>
struct lerpStruct
{
	inline static X apply( X u, X v, double a)
	{
		return u*a + v*(1.0-a);
	}
};

template <typename PFP>
struct lerpStruct<typename PFP::EVERTEX ,PFP>
{
	inline static typename PFP::EVERTEX apply(typename PFP::EVERTEX u, typename PFP::EVERTEX v, double a)
	{
		typename PFP::EVERTEX ec;
		ec.lerp(u,v,a);
		return ec;
	}
};


template < typename X, typename PFP>
inline X  lerp( X u, X v, double a )
{
	return lerpStruct< X, PFP >::apply( u,v,a );
}





struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef Map2 MAP;
};


//class MyPosFunctor : public FunctorAttribute<Geom::Vec3f>
//{
//public:
//	bool operator()(Geom::Vec3f& P)
//	{
//		std::cout << "Point: "<< P<< std::endl;
//		return false;
//	}
//};
//
//
//
//class ShiftFunctor : public FunctorAttribute<Geom::Vec3f>
//{
//public:
//	inline bool operator()(Geom::Vec3f& P)
//	{
//		P[0]-=0.1f;
//		return false;
//	}
//};


template <typename PF, typename EMBV, typename EMB>
inline void genAlgoDart(typename PF::MAP& map, EMBV& vp, Dart d1)
{
	Dart d2 = map.phi1(d1);
	Dart d3 = map.phi1(d2);

	vp[d1] = lerp<EMB,PF>(vp[d2],vp[d3],0.5);

//	EMB v1 = vp[d2];
//	EMB v2 = vp[d3];
//	EMB v3 = v1+v2;
//	vp[d1] = v3 / 2.0;
}



template <typename EMBV, typename EMB>
inline void genAlgoPipo(EMBV& vp, unsigned int i)
{
//	vp[i] = lerp(vp[i+1], vp[i+2],0.5);

	std::cout << "GO ...."<<std::endl;
//	EMB v1 = vp[i+1];
//	std::cout << "v1 ok"<<std::endl;
//	EMB v2 = vp[i+2];
//	std::cout << "v2 ok"<<std::endl;
//	EMB v3 = v1 + v2;
//	std::cout << "v3 ok"<<std::endl;
//	vp[i] = v3/2.0;
//	std::cout << "affectation"<<std::endl;

	vp[i] = (vp[i+1] + vp[i+2])/2.0;
}


struct Pipos
{
	float t;
	static std::string CGoGNnameOfType() { return "Pipos";}
};

int main(int argc, char **argv)
{

		Geom::Vec3f xxx = Geom::Vec3f(0);

		std::cout<< "VEC:"<<xxx<<std::endl;


		PFP::MAP myMap;
/*
		Dart d = myMap.newFace(3);
		Dart e = myMap.newFace(4);
		
		myMap.sewFaces(d,e);

		uint i1 = myMap.newEmbedding(VERTEX_ORBIT);
		uint i2 = myMap.newEmbedding(VERTEX_ORBIT);
		uint i3 = myMap.newEmbedding(VERTEX_ORBIT);
		uint i4 = myMap.newEmbedding(VERTEX_ORBIT);
		uint i5 = myMap.newEmbedding(VERTEX_ORBIT);
		uint i6 = myMap.newEmbedding(VERTEX_ORBIT);


		myMap.embedOrbit(VERTEX_ORBIT,e,i1);
		Dart f = myMap.phi1(e);
		myMap.embedOrbit(VERTEX_ORBIT,f,i2);
		f = myMap.phi1(f);
		myMap.embedOrbit(VERTEX_ORBIT,f,i3);
		f = myMap.phi1(f);
		myMap.embedOrbit(VERTEX_ORBIT,f,i4);


		f = myMap.phi_1(d);
		myMap.embedOrbit(VERTEX_ORBIT,f,i5);


		uint codePositions = myMap.addAttribute<Geom::Vec3f>(VERTEX_ORBIT, "position");

		PFP::TVEC3 tablePosition(myMap, codePositions);

		f = e;
		tablePosition[f] = Geom::Vec3f(2.0f,2.0f,0.0f);
		f = myMap.phi1(f);
		tablePosition[f] = Geom::Vec3f(2.0f,0.0f,0.0f);
		f = myMap.phi1(f);
		tablePosition[f] = Geom::Vec3f(4.0f,0.0f,0.0f);
		f = myMap.phi1(f);
		tablePosition[f] = Geom::Vec3f(4.0f,2.0f,0.0f);

		f = myMap.phi1(d);
		tablePosition[f] = Geom::Vec3f(-0.5f,1.0f,0.0f);

		PFP::VEC3 n;
		Algo::Geometry::vertexNormal<PFP>(myMap,d,tablePosition,n);

		std::cout << "Normal"<< n<< std::endl;

		PFP::VEC3 bb1;
		PFP::VEC3 bb2;
		Algo::Render::computeBoundingBox<PFP>(myMap,bb1,bb2,tablePosition);
		std::cout << "BB:"<<bb1 << " / "<< bb2<<std::endl;


		f=e;
		std::cout <<"Pos: "<< tablePosition[f]<< std::endl;
		std::cout <<"Refs: "<< tablePosition.nbRefs(f)<< std::endl;
		f = myMap.phi1(f);
		std::cout <<" Pos: "<< tablePosition[f]<< std::endl;
		std::cout <<"Refs: "<< tablePosition.nbRefs(f)<< std::endl;
		f = myMap.phi1(f);
		std::cout <<" Pos: "<< tablePosition[f]<< std::endl;
		std::cout <<"Refs: "<< tablePosition.nbRefs(f)<< std::endl;
		f = myMap.phi1(f);
		std::cout <<" Pos: "<< tablePosition[f]<< std::endl;
		std::cout <<"Refs: "<< tablePosition.nbRefs(f)<< std::endl;


		uint codeNormals = myMap.addAttribute<Geom::Vec3f>(VERTEX_ORBIT, "normals");
		PFP::NORMAL tableNormal(codeNormals,myMap);

//		PFP::MAP::AutoAttributeHandler<Geom::Vec4f> tableColors(myMap,VERTEX_ORBIT,"colors", "Geom::Vec4f");
//		tableColors[f] = Geom::Vec4f(1.0f,0.0f,0.0f,1.0f);


//		PFP::MAP::AutoAttributeHandler< NoOutAttrib<std::vector<Dart> > > tableDarts(myMap,VERTEX_ORBIT,"pipo", "Tutu");

//		std::vector<Dart> vd;
//		vd.push_back(d);
//		vd.push_back(e);

//		tableDarts[f].val.push_back(d);
//		tableDarts[f].val.push_back(e);
//
//		for (std::vector<Dart>::iterator id = tableDarts[f].val.begin(); id != tableDarts[f].val.end(); id ++)
//		{
//			std::cout << "Dart: "<< (*id)->getLabel() <<std::endl;
//		}

//		uint codeStr = myMap.addAttribute< NoMathAttribute<std::string> >(VERTEX_ORBIT, "strings");
//		PFP::AttributeHandler< NoMathAttribute<std::string> > tablestring(codeStr,myMap);

//		PFP::MAP::AutoAttributeHandler< NoMathIOAttribute< std::vector<Dart> > > tableDarts(myMap,VERTEX_ORBIT,"pipo", "Tutu");
//		tableDarts[d].push_back(f);

		PFP::MAP::AutoAttributeHandler< double > tableDouble(myMap,VERTEX_ORBIT,"pipo", "double");


		AttribContainer& cont = myMap.getAttributeContainer(VERTEX_ORBIT);
//		AttribMultiVect<Geom::Vec3f>& vect = cont.getDataVector<Geom::Vec3f>(codePositions);
//		AttribMultiVect<Geom::Vec3f>& nvect = cont.getDataVector<Geom::Vec3f>(codeNormals);
//		AttribMultiVect<double>& dvect = cont.getDataVector<double>(tableDouble.id());

		PFP::AttributeHandler<Geom::Vec3f> vect(codePositions,myMap);
		PFP::AttributeHandler<double> dvect(tableDouble.id(),myMap);

		Dart d0 = d;
		Dart d1 = myMap.phi1(d0);
		Dart d2 = myMap.phi1(d1);

		myMap.setDartEmbedding(d0, VERTEX_ORBIT, 0);
		myMap.setDartEmbedding(d1, VERTEX_ORBIT, 1);
		myMap.setDartEmbedding(d2, VERTEX_ORBIT, 2);


		dvect[d0]=0.01;
		dvect[d1]=3.1456;
		dvect[d2]=6.0456;

		cont.toggleProcess(codePositions);
		cont.toggleProcess(codeNormals);
		cont.toggleProcess(tableDouble.id());

		PFP::MAP::EVertex::setContainer(myMap.getAttributeContainer(VERTEX_ORBIT));
		PFP::MAP::EVertex::setMap(myMap);
		PFP::MAP::EVertex ec(0);


		std::cout << "Avant:"<<std::endl;
		std::cout << vect[d0]<< " / "<< vect[d1]<< " / " << vect[d2]<<std::endl;
		std::cout << dvect[d0]<< " / "<< dvect[d1]<< " / " << dvect[d2]<<std::endl;


		genAlgoDart<PFP, PFP::EVERTEX, PFP::EVERTEX >(myMap, ec, d);

		std::cout << "Apres e0 = e1+e2 / 4"<<std::endl;
		std::cout << vect[d0]<< " / "<< vect[d1]<< " / " << vect[d2]<<std::endl;
		std::cout << dvect[d0]<< " / "<< dvect[d1]<< " / " << dvect[d2]<<std::endl;


		genAlgoDart<PFP, PFP::AttributeHandler<Geom::Vec3f>, Geom::Vec3f>(myMap, vect , d);


		GLint xt1 = glutGet(GLUT_ELAPSED_TIME);

		for (int i=0; i< 1000000000; ++i)
		{
//			vect[d0] = lerp<Geom::Vec3f,PFP>( vect[d1], vect[d2], 3.14 );
			float a=3.14;
			vect[d0] = vect[d1]*a + vect[d2]*(1-a);
		}
        GLint xt2 = glutGet(GLUT_ELAPSED_TIME);
        GLfloat xseconds = (xt2 - xt1) / 1000.0f;
        std::cout << "parcours: "<< xseconds << "sec" << std::endl;


*/

		AttributeHandler< NoMathIOAttribute<Pipos> > tablestring = myMap.addAttribute< NoMathIOAttribute<Pipos> >(VERTEX_ORBIT, "strings");


		PFP::TVEC3 tableNormal = myMap.addAttribute<Geom::Vec3f>(VERTEX_ORBIT, "normals");

		std::vector<std::string> attrNames ;
        if (!Algo::Import::importMesh<PFP>(myMap, "liver.trian", attrNames))
        {
        	std::cerr << "Import fail" << std::endl;
        	exit(1);
        }
		PFP::TVEC3 tablePosition = myMap.getAttribute<PFP::VEC3>(VERTEX_ORBIT, attrNames[0]);

        std::cout << "Nombre de sommets: "<< myMap.getNbCells(VERTEX_ORBIT)<<std::endl;
		SelectorTrue allDarts;
		Algo::Geometry::computeNormalVertices<PFP>(myMap, tablePosition, tableNormal, allDarts);

		for (unsigned int it = tablePosition.begin(); it != tablePosition.end(); tablePosition.next(it))
		{
			std::cout << tablePosition[it] << "  /  "<< tableNormal[it]<< std::endl;
		}



//		typedef NoMathIOAttribute< struct Pipos > ATT;
//		uint codeStr = myMap.createAttribute< ATT >(VERTEX_ORBIT, "strings", "std::string");
		//		PFP::AttributeHandler< NoMathAttribute<std::string> > tablestring(codeStr,myMap);

/*

		// PARCOURS METHODE 1 (optimum)
		AttribContainer& cont = myMap.getAttributeContainer(VERTEX_ORBIT);
		AttribMultiVect<Geom::Vec3f>& vect = cont.getDataVector<Geom::Vec3f>(codePositions);
		AttribMultiVect<Geom::Vec3f>& nvect = cont.getDataVector<Geom::Vec3f>(codeNormals);
		AttribMultiVect<double>& dvect = cont.getDataVector<double>(tableDouble.id());

		unsigned int end = cont.end(); // cont.end() est un peu plus qu'un acces, si fixe alors stocker
		for (unsigned int it = cont.begin(); it !=  end; cont.next(it))
		{
			// remarque it est aussi le embedding des brin du sommet
			Geom::Vec3f& P = vect[it];
			P[0] -=0.1f;
		}


		for (uint i=0; i< 1000000; ++i)
			{
				unsigned int id = myMap.newEmbedding(VERTEX_ORBIT);
				vect[i] = Geom::Vec3f(i,i,i);
				nvect[i] = Geom::Vec3f(0.5f*i,0.5f*i,0.5f*i);
				dvect[i] = double (0.1*i);
			}

//		Geom::Vec3f
//		std::cout << "test lerp "<< std::endl;
//


		cont.toggleProcess(codePositions);
		cont.toggleProcess(codeNormals);
		cont.toggleProcess(tableDouble.id());

		EVertex::setContainer(myMap.getAttributeContainer(VERTEX_ORBIT));
		EVertex ec(0);


		std::cout << "Avant:"<<std::endl;
		std::cout << vect[0]<< " / "<< vect[1]<< " / " << vect[2]<<std::endl;
		std::cout << tableNormal.at(0)<< " / "<< tableNormal.at(1)<< " / " << tableNormal.at(2)<<std::endl;


		genAlgo< EVertex,EVertex >(ec,0);


		std::cout << "Apres e0 = e1+e2 / 4"<<std::endl;
		std::cout << vect[0]<< " / "<< vect[1]<< " / " << vect[2]<<std::endl;
		std::cout << tableNormal.at(0)<< " / "<< tableNormal.at(1)<< " / " << tableNormal.at(2)<<std::endl;



		{
//		e0 = (e1+e2)/4.0;

		GLint xt1 = glutGet(GLUT_ELAPSED_TIME);

		for (int i=0; i< 10000000; ++i)
		{
			genAlgo< EVertex,EVertex >(ec,i);

		}
        GLint xt2 = glutGet(GLUT_ELAPSED_TIME);
        GLfloat xseconds = (xt2 - xt1) / 1000.0f;
        std::cout << "parcours: "<< xseconds << "sec" << std::endl;
		}
		{


		AttribMultiVect<Geom::Vec3f>& vp = cont.getDataVector<Geom::Vec3f>(codePositions);
		AttribMultiVect<Geom::Vec3f>& vn = cont.getDataVector<Geom::Vec3f>(codeNormals);
		AttribMultiVect<double>& vd = cont.getDataVector<double>(tableDouble.id());
		GLint xt1 = glutGet(GLUT_ELAPSED_TIME);
		for (int i=0; i< 10000000; ++i)
		{
//			genAlgo< AttribMultiVect<Geom::Vec3f>, Geom::Vec3f>(vp,i);
			unsigned int j = (i*37) %1000500;
			unsigned int j1 = (j+5254) %1000500;
			unsigned int j2 = (j+7506) %1000500;
			vp[j] = lerp(vp[j1], vp[j2],0.5);
			vn[j] = lerp(vn[j1], vn[j2],0.5);
			vd[j] = lerp(vd[j1], vd[j2],0.5);
		}

        GLint xt2 = glutGet(GLUT_ELAPSED_TIME);
        GLfloat xseconds = (xt2 - xt1) / 1000.0f;
        std::cout << "parcours: "<< xseconds << "sec" << std::endl;

		}
//		e0.lerp(e1,e2,0.5);

//		cont.interpole(2,0,1,0.6);
//		std::cout << vect[0]<< " / "<< vect[1]<< " / " << vect[2]<<std::endl;
//		cont.add(0,1);
//		std::cout << "Apres e0 = e1+e2 / 4"<<std::endl;
//		std::cout << vect[0]<< " / "<< vect[1]<< " / " << vect[2]<<std::endl;
//		std::cout << tableNormal.at(0)<< " / "<< tableNormal.at(1)<< " / " << tableNormal.at(2)<<std::endl;
*/



/*
        // PARCOURS METHODE 2  (1.25 fois plus lent au pire sur l'acces)
   		end = tablePosition.end(); // cont.end() est un peu plus qu'un acces, si fixe alors stocker
 		for (unsigned int it = tablePosition.begin(); it != end; tablePosition.next(it))
		{
			tablePosition.at(it)[0] -=0.1f;
		}

        // PARCOURS METHODE 3  (1.7 fois plus lent au pire sur le temps d'accès)
        ShiftFunctor shPos;
        myMap.foreach_attribute<Geom::Vec3f>(codePositions,shPos);


        GLint t1 = glutGet(GLUT_ELAPSED_TIME);

        for (unsigned int k=0; k<500000;++k)
        {
    		PFP::TVEC3::IndexedType& vect = tablePosition.indexed();
    		unsigned int end = tablePosition.end();
    		for (unsigned int it = tablePosition.begin(); it != end; tablePosition.next(it))
    		{
    			vect[it][0] -=0.1f;
    		}
        }

        GLint t2 = glutGet(GLUT_ELAPSED_TIME);
        GLfloat seconds = (t2 - t1) / 1000.0f;
        std::cout << "parcours: "<< seconds << "sec" << std::endl;

        for (unsigned int k=0; k<500000;++k)
         {
    		AttribContainer& cont = myMap.getAttributeContainer(VERTEX_ORBIT);
    		AttribMultiVect<Geom::Vec3f>& vect2 = cont.getDataVector<Geom::Vec3f>(codePositions);
    		unsigned int end = cont.end(); // cont.end() est un peu plus qu'un acces, si fixe alors stocker
    		for (unsigned int it = cont.begin(); it != end; cont.next(it))
    		{
    			vect2[it][0] -=0.1f;
    		}
        }

        t1 = glutGet(GLUT_ELAPSED_TIME);
        seconds = (t1 - t2) / 1000.0f;
        std::cout << "parcours: "<< seconds << "sec" << std::endl;


        for (unsigned int k=0; k<500000;++k)
        {
            ShiftFunctor shPos;
            myMap.foreach_attribute<Geom::Vec3f>(codePositions,shPos);

         }
        t2 = glutGet(GLUT_ELAPSED_TIME);
         seconds = (t2 - t1) / 1000.0f;
         std::cout << "parcours: "<< seconds << "sec" << std::endl;

*/


//		Algo::Render::Direct::renderTriQuadPoly<PFP>(myMap,Algo::Render::Direct::LINE,0.0,allDarts,tablePosition,tableNormal);

//		Algo::Render::VBO::VBO_PointsN_CB<PFP> vbocb(myMap);
//		Algo::Render::VBO::mapRender<PFP> mr(myMap, allDarts, &vbocb) ;
//
//		mr.initBuffers();
//		mr.initPrimitives<PFP>(myMap, good, Algo::Render::VBO::TRIANGLES);
//		mr.updateData(Algo::Render::VBO::POSITIONS, codePositions);
//		mr.updateData(Algo::Render::VBO::NORMALS, codeNormals);
//		mr.draw(Algo::Render::VBO::TRIANGLES);


        return 0;
}
