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
#include "Topology/map/map2.h"
#include "Topology/generic/embeddedMap2.h"

#include "Geometry/vector_gen.h"

#include "Algo/Import/import.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Render/vbo_MapRender.h"

#include "Algo/Geometry/area.h"
#include "Algo/Geometry/normal.h"

#include "Algo/Parallel/parallel_foreach.h"

#include "averaging.h"

using namespace CGoGN ;


/**
 * Struct that contains some informations about the types of the manipulated objects
 * Mainly here to be used by the algorithms that are parameterized by it
 */
struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef EmbeddedMap2<Map2> MAP;
};

/**
 * A class for a little interface and rendering
 */
class MyGlutWin: public Utils::SimpleGlutWin
{
public:
     void myRedraw();

     PFP::REAL gWidthObj;
     PFP::VEC3 gPosObj;

     Algo::Render::VBO::MapRender_VBO* m_render;

 	MyGlutWin(int* argc, char **argv, int winX, int winY) : SimpleGlutWin(argc, argv, winX, winY), m_render(NULL) {}
 	~MyGlutWin()
 	{
 		if (m_render !=NULL)
 		delete m_render ;
 	}
};

// Routine d'affichage
void MyGlutWin::myRedraw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	// center the object
	float sc = 50.0f / gWidthObj;
	glScalef(sc, sc, sc);
	glTranslatef(-gPosObj[0], -gPosObj[1], -gPosObj[2]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_LIGHTING);

	// draw the lines
	glColor3f(1.0f, 1.0f, 0.0f);
	m_render->draw(Algo::Render::VBO::LINES);

	// draw the faces
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	glColor3f(0.0f, 0.5f, 0.0f);
	m_render->draw(Algo::Render::VBO::TRIANGLES);
	glDisable(GL_POLYGON_OFFSET_FILL);

	glPopMatrix();
}




template <typename XXX>
class ThreadArea: public Algo::Parallel::CGoGNThread<typename XXX::MAP>
{
protected:
	const typename XXX::TVEC3& m_positions;
	float area;
public:
	ThreadArea(typename XXX::MAP& map, const typename XXX::TVEC3& pos, unsigned int th):
		Algo::Parallel::CGoGNThread<typename XXX::MAP>(map,th),
		m_positions(pos),
		area(0.0f) {}

	void operator()()
	{
		// 3 times just for fun !!!
		area += Algo::Geometry::totalArea<XXX>(this->m_map, m_positions, SelectorTrue(), this->m_threadId);
		area += Algo::Geometry::totalArea<XXX>(this->m_map, m_positions, SelectorTrue(), this->m_threadId);
		area += Algo::Geometry::totalArea<XXX>(this->m_map, m_positions, SelectorTrue(), this->m_threadId);
	}

	float getTripleValue() { return area;}
};


template <typename XXX>
class ThreadNormals: public Algo::Parallel::CGoGNThread<typename XXX::MAP>
{
protected:
	const typename XXX::TVEC3& m_positions;
	typename XXX::TVEC3& m_normals;
public:
	ThreadNormals(typename XXX::MAP& map, const typename XXX::TVEC3& pos, typename XXX::TVEC3& norm, unsigned int th):
		Algo::Parallel::CGoGNThread<typename XXX::MAP>(map,th),
		m_positions(pos),
		m_normals(norm) {}

	void operator()()
	{
		Algo::Geometry::computeNormalVertices<XXX>(this->m_map, m_positions, m_normals, SelectorTrue(), this->m_threadId);
	}

};


//
//template<typename XXX>
//class Thread0
//{
//protected:
//	typename XXX::MAP& m_map;
//	MyGlutWin& m_mgw;
//	unsigned int m_th;
//	SelectorTrue m_selt;
//public:
//	Thread0(typename XXX::MAP& map,MyGlutWin& mgw, unsigned int th):
//		m_map(map), m_mgw(mgw), m_th(th) {}
//
//	void operator()()
//	{
//		std::cout << "Begin render init"<<std::endl;
//		m_mgw.useContext();
//
//		// instanciation of the renderer (here using VBOs)
//		m_mgw.m_render = new Algo::Render::VBO::MapRender_VBO();
//
//	    // update the renderer (geometry and primitives)
//	    m_mgw.m_render->updateData(Algo::Render::VBO::POSITIONS, position);
//
//	    m_mgw.m_render->initPrimitives<PFP>(m_map, m_selt, Algo::Render::VBO::TRIANGLES,m_th);
//	    m_mgw.m_render->initPrimitives<PFP>(m_map, m_selt, Algo::Render::VBO::LINES,m_th);
//
//	    m_mgw.releaseContext();
//	    std::cout<< "Render OK "<< std::endl;
//
//	}
//};



template <typename XXX>
class calculFunctor1 : public Algo::Parallel::FunctorMapThreaded<typename XXX::MAP>
{
protected:
	typename XXX::TVEC3& m_positions;
	typename XXX::TVEC3& m_normals;
public:
	calculFunctor1( typename XXX::MAP& map, typename XXX::TVEC3& pos, typename XXX::TVEC3& norm, unsigned int id=0):
		Algo::Parallel::FunctorMapThreaded<typename XXX::MAP>(map,id), m_positions(pos), m_normals(norm) {}

	bool operator()(Dart d)
	{
		typename XXX::VEC3 n1 = Algo::Geometry::vertexNormal<XXX>(this->m_map, d, m_positions);
		typename XXX::VEC3 n2 = Algo::Geometry::vertexNormal<XXX>(this->m_map, this->m_map.phi1(d), m_positions);
		typename XXX::VEC3 n3 = Algo::Geometry::vertexNormal<XXX>(this->m_map, this->m_map.phi_1(d), m_positions);
		typename XXX::VEC3 n = n1+n2+n3;
		n1 = Algo::Geometry::vertexNormal<XXX>(this->m_map, d, m_positions);
		n2 = Algo::Geometry::vertexNormal<XXX>(this->m_map, this->m_map.phi1(d), m_positions);
		n3 = Algo::Geometry::vertexNormal<XXX>(this->m_map, this->m_map.phi_1(d), m_positions);
		n += n1+n2+n3;
		n.normalize();
		m_normals[d] =  n;
//		m_normals[d] = Algo::Geometry::vertexNormal<XXX>(this->m_map, d, m_positions);
		return false;
	}

	Algo::Parallel::FunctorMapThreaded<typename XXX::MAP>* duplicate(unsigned int id)
	{
		calculFunctor1<XXX>* copy = new calculFunctor1<XXX>(this->m_map,m_positions,m_normals,id);
		return reinterpret_cast<Algo::Parallel::FunctorMapThreaded<typename XXX::MAP>*>(copy);
	}
};







template <typename XXX>
class LengthEdgeFunctor : public Algo::Parallel::FunctorMapThreadedResult<typename XXX::MAP, std::pair<double,unsigned int> >
{
protected:
	typename XXX::TVEC3& m_positions;
	double m_length;
	unsigned int m_nb;
public:
	LengthEdgeFunctor( typename XXX::MAP& map, typename XXX::TVEC3& pos, unsigned int id=0):
		Algo::Parallel::FunctorMapThreadedResult< typename XXX::MAP, std::pair<double,unsigned int> >(map,id),
		m_positions(pos),
		m_length(0.0),
		m_nb(0) {}

	bool operator()(Dart d)
	{
		Dart dd = this->m_map.phi2(d);
		typename XXX::VEC3 V = m_positions[dd] - m_positions[d];
		m_length += V.norm();
		m_nb++;
		return false;
	}

	Algo::Parallel::FunctorMapThreaded<typename XXX::MAP>* duplicate(unsigned int id)
	{
		LengthEdgeFunctor<XXX>* copy = new LengthEdgeFunctor<XXX>(this->m_map,m_positions,id);
		return reinterpret_cast<Algo::Parallel::FunctorMapThreaded<typename XXX::MAP>*>(copy);
	}

	std::pair<double,unsigned int> getResult() { return std::pair<double,unsigned int>(m_length,m_nb);}

};






int main(int argc, char **argv)
{
	// declaration of the map
	PFP::MAP myMap;

	// this selector is going to select all the darts
	SelectorTrue allDarts;


    // instanciation of the interface
	MyGlutWin mgw(&argc, argv, 800, 800);
	glewInit();

	// release context (leave it for thread 0);
	mgw.releaseContext();

	std::vector<std::string> attrNames ;
	if(!Algo::Import::importMesh<PFP>(myMap, argv[1], attrNames))
	{
		std::cerr << "could not import " << argv[1] << std::endl ;
		return 1 ;
	}

	AttributeHandler<PFP::VEC3> position;
	AttributeHandler<PFP::VEC3> facesnormals;

	// cree un handler d'attribut pour la position des points (créé lors de l'import)
	position = myMap.getAttribute<PFP::VEC3>(VERTEX_ORBIT, attrNames[0]) ;


	std::cout << "Sommets:"<< position.end()<< std::endl;
	// cree un handler pour les normales aux sommets
	AttributeHandler<PFP::VEC3> normal = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "normal");
	AttributeHandler<PFP::VEC3> normal2 = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "normal2");


	// ajout de 4 threads pour les markers
	myMap.addThreadMarker(4);



	//Algorithmes en //

	boost::thread thread1( ThreadArea<PFP>(myMap,position,1));
	boost::thread thread2( ThreadNormals<PFP>(myMap,position,normal,2));
	thread1.join();
	thread2.join();

	// parallelisation de boucle sans resultat
	calculFunctor1<PFP> tf1(myMap,position,normal);
	Algo::Parallel::foreach_orbit<PFP>(myMap,VERTEX_ORBIT, tf1,4);
	std::cout << "ok:"<< std::endl;


	// parallelisation de boucle avec resultats stockes

	// vector pour le resultat (ici paire double/int pour faire la moyenne des longueurs des aretes)
	std::vector<std::pair<double,unsigned int> > lengthp;
	LengthEdgeFunctor <PFP> tflef(myMap,position);			// le foncteur
	// on lance l'algo parallelise (4 threads, buffer de 16384 brins par thread)
	Algo::Parallel::foreach_orbit_res< PFP,std::pair<double,unsigned int> >(myMap,EDGE_ORBIT, tflef, 4 , 16384,lengthp);
	// on calcule la somme des resultats
	std::pair<double,unsigned int> le = Algo::Parallel::sumPairResult<double,unsigned int>(lengthp);
	std::cout << "length :" <<le.first/le.second<< std::endl;




	// on enleve les markers ajoutes
	myMap.removeThreadMarker(4);


    return 0;
}
