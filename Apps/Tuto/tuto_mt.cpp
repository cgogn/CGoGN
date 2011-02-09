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

 	MyGlutWin(int* argc, char **argv, int winX, int winY) : SimpleGlutWin(argc, argv, winX, winY) {}
 	~MyGlutWin()
 	{
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

AttributeHandler<PFP::VEC3> position;
AttributeHandler<PFP::VEC3> facesnormals;


//
//template<typename PFP>
//void fonction_exemple(typename PFP::MAP& map, const AttributeHandler<typename PFP::VEC3>& position, Dart d)
//{
//	std::cout << "Position brin"<<d.index<< " = "<< position[d]<< std::endl;
//}



// NEED VERTEX_THREAD
template<typename XXX>
class Thread1
{
protected:
	typename XXX::MAP& m_map;
	unsigned int m_th;
public:
	Thread1(typename XXX::MAP& map, unsigned int th):
		m_map(map), m_th(th) {}

	void operator()()
	{
		typename XXX::VEC3 total(0.);
		unsigned int nb=0;
		{
			CellMarker mv(m_map,VERTEX_CELL, m_th);
			for (Dart d = m_map.begin(); d != m_map.end(); m_map.next(d))
			{
				if (! mv.isMarked(d)) // si d non marque:
				{
					typename XXX::VEC3 p = position[d];
					total+=p;
					nb++;
					mv.mark(d);
				}
			}
		}
		{
			CellMarker mv(m_map,VERTEX_CELL, m_th);
			for (Dart d = m_map.begin(); d != m_map.end(); m_map.next(d))
			{
				if (! mv.isMarked(d)) // si d non marque:
				{
					typename XXX::VEC3 p = position[d];
					total+=p;
					nb++;
					mv.mark(d);
				}
			}
		}
		{
			CellMarker mv(m_map,VERTEX_CELL, m_th);
			for (Dart d = m_map.begin(); d != m_map.end(); m_map.next(d))
			{
				if (! mv.isMarked(d)) // si d non marque:
				{
					typename XXX::VEC3 p = position[d];
					total+=p;
					nb++;
					mv.mark(d);
				}
			}
		}
		{
			CellMarker mv(m_map,VERTEX_CELL, m_th);
			for (Dart d = m_map.begin(); d != m_map.end(); m_map.next(d))
			{
				if (! mv.isMarked(d)) // si d non marque:
				{
					typename XXX::VEC3 p = position[d];
					total+=p;
					nb++;
					mv.mark(d);
				}
			}
		}
		total /= typename XXX::REAL(nb);
		std::cout << "Thread "<< m_th << "-> median pos: "<<total<< std::endl;
	}
};

// NEED DART_THREAD
template<typename XXX>
class Thread2
{
protected:
	typename XXX::MAP& m_map;
	unsigned int m_th;
	SelectorTrue m_selt;
	float area;
public:
	Thread2(typename XXX::MAP& map, unsigned int th):
		m_map(map), m_th(th) {}

	void operator()()
	{
		area = Algo::Geometry::totalArea<XXX>(m_map,position,m_selt,m_th);
		area = Algo::Geometry::totalArea<XXX>(m_map,position,m_selt,m_th);
		area = Algo::Geometry::totalArea<XXX>(m_map,position,m_selt,m_th);
		area = Algo::Geometry::totalArea<XXX>(m_map,position,m_selt,m_th);
		std::cout << "Thread "<< m_th<<"  Total Area= "<< area<< std::endl;
	}
};

// NEED FACE_THREAD
template<typename XXX>
class Thread3
{
protected:
	typename XXX::MAP& m_map;
	unsigned int m_th;
	SelectorTrue m_selt;
public:
	Thread3(typename XXX::MAP& map, unsigned int th):
		m_map(map), m_th(th) {}

	void operator()()
	{
		Algo::Geometry::computeNormalFaces<XXX>(m_map,position,facesnormals,m_selt,m_th);

		typename XXX::VEC3 total(0.);
		unsigned int nb=0;
		for (unsigned int i=facesnormals.begin(); i != facesnormals.end(); facesnormals.next(i))
		{
			typename XXX::VEC3 f = facesnormals[i]/10000.0f;
			total+=f;
			nb++;
		}
		for (unsigned int i=facesnormals.begin(); i != facesnormals.end(); facesnormals.next(i))
		{
			typename XXX::VEC3 f = facesnormals[i]/10000.0f;
			total+=f;
			nb++;
		}
		for (unsigned int i=facesnormals.begin(); i != facesnormals.end(); facesnormals.next(i))
		{
			typename XXX::VEC3 f = facesnormals[i]/10000.0f;
			total+=f;
			nb++;
		}
		for (unsigned int i=facesnormals.begin(); i != facesnormals.end(); facesnormals.next(i))
		{
			typename XXX::VEC3 f = facesnormals[i]/10000.0f;
			total+=f;
			nb++;
		}
		total /= typename XXX::REAL(nb);
		std::cout << "Thread "<< m_th<<"  Face normal :"<<total <<  std::endl;
	}
};


// NEED DART_THREAD
template<typename XXX>
class Thread0
{
protected:
	typename XXX::MAP& m_map;
	MyGlutWin& m_mgw;
	unsigned int m_th;
	SelectorTrue m_selt;
public:
	Thread0(typename XXX::MAP& map,MyGlutWin& mgw, unsigned int th):
		m_map(map), m_mgw(mgw), m_th(th) {}

	void operator()()
	{
		std::cout << "Begin render init"<<std::endl;
		m_mgw.useContext();

		// instanciation of the renderer (here using VBOs)
		m_mgw.m_render = new Algo::Render::VBO::MapRender_VBO();

	    // update the renderer (geometry and primitives)
	    m_mgw.m_render->updateData(Algo::Render::VBO::POSITIONS, position);

	    m_mgw.m_render->initPrimitives<PFP>(m_map, m_selt, Algo::Render::VBO::TRIANGLES,m_th);
	    m_mgw.m_render->initPrimitives<PFP>(m_map, m_selt, Algo::Render::VBO::LINES,m_th);

	    m_mgw.releaseContext();
	    std::cout<< "Render OK "<< std::endl;

	}
};



boost::mutex mutexio;


unsigned int cpt1=0;
unsigned int cpt2=0;



//class ThreadFunc
//{
//protected:
//	FunctorType& m_functor;
//	std::vector<Dart>& m_darts;
//	boost::mutex& m_exclu;
//
//public:
//	ThreadFunc(FunctorType& func, std::vector<Dart>& vd, boost::mutex& mut	):
//		m_functor(func), m_darts(vd), m_exclu(mut) {}
//
//	void operator()()
//	{
//		bool m_finished=false;
//
//		while(!m_finished)
//		{
//			Dart d;
//			bool ok=false;
//			do
//			{
//				m_exclu.lock();
//				if (!m_darts.empty())
//				{
//					d = m_darts.back();
//					m_darts.pop_back();
//					std::cout << "Lecture "<< " = "<< m_darts.size()<< std::endl;
//					ok=true;
//				}
//				else std::cout << "PB vecteur vide"<< std::endl;
//				m_exclu.unlock();
//			} while (!ok);
//
//			if (!d.isNil())
//				m_functor(d);
//			else
//				m_finished=true;
//		}
////		std::cout << "Finished "<< std::endl;
//	}
//};


//#define NBTH 7

//template <typename PFP>
//void parallel_foreach_orbit(typename PFP::MAP& map, unsigned int orbit, FunctorType& func)
//{
//	std::vector<Dart> vd[NBTH];
//
//	boost::thread* threads[NBTH];
//	boost::mutex muts[NBTH];
//
//
//	DartMarker dm(map);
//
//	Dart d=map.begin();
//
//	// NBTH new functions, new thread (with good darts !)
//	for (unsigned int i=0; i<NBTH; ++i)
//	{
//		while (dm.isMarked(d))
//			map.next(d);
//		dm.markOrbit(orbit,d);
//		vd[i].reserve(16);
//		vd[i].push_back(d);
////		threads[i] = new boost::thread(ThreadFunc(func, vd[i], muts[i]));
//		map.next(d);
//	}
////	std::cout << "OK1" << std::endl;
//
//	while ( d != map.end())
//	{
//		// store thre NBTH next darts in tempo v2d
//		for (unsigned int i=0; i<NBTH; ++i)
//		{
//			while ((d !=map.end()) && (dm.isMarked(d)))
//				map.next(d);
//
//			if (d !=map.end())
//			{
//				dm.markOrbit(orbit,d);
////				muts[i].lock();
//					vd[i].push_back(d);
////					std::cout << "Size of"<< i << " = "<< vd[i].size()<< std::endl;
////				muts[i].unlock();
//				map.next(d);
//			}
//			else
//			{
////				muts[i].lock();
//				vd[i].push_back(Dart::nil());
////				muts[i].unlock();
//			}
//		}
//	}
//
//
//	//wait for all theads to be finished
//	for (unsigned int i=0; i< NBTH; ++i)
//	{
////		threads[i]->join();
////		delete threads[i];
//	}
//	std::cout << "------------------------"<<std::endl;
//}

//
//class ThreadFunc
//{
//protected:
//	FunctorType& m_functor;
//	const std::vector<Dart>& m_darts;
//
//public:
//	ThreadFunc(FunctorType& func, const std::vector<Dart>& vd):
//		m_functor(func), m_darts(vd) {}
//
//	void operator()()
//	{
//		for (std::vector<Dart>::const_iterator it = m_darts.begin(); it != m_darts.end(); ++it)
//			m_functor(*it);
//	}
//};


//template <typename PFP>
//void parallel_foreach_orbit(typename PFP::MAP& map, unsigned int orbit, FunctorType& func)
//{
//	std::vector<Dart> vd[NBTH];
//	boost::thread* threads[NBTH];
//
//	DartMarker dm(map);
//	Dart d=map.begin();
//
//	// NBTH new functions, new thread (with good darts !)
//	for (unsigned int i=0; i<NBTH; ++i)
//		vd[i].reserve(4096);
//
//
//	while ( d != map.end())
//	{
//		for (unsigned int i=0; i<NBTH; ++i)
//		{
//			while ((d !=map.end()) && (dm.isMarked(d)))
//				map.next(d);
//
//			if (d !=map.end())
//			{
//				dm.markOrbit(orbit,d);
//				vd[i].push_back(d);
//				map.next(d);
//			}
//		}
//	}
//
//	for (unsigned int i=0; i<NBTH; ++i)
//	{
//		threads[i] = new boost::thread(ThreadFunc(func, vd[i]));
//	}
//
//
//	//wait for all theads to be finished
//	for (unsigned int i=0; i< NBTH; ++i)
//	{
//		threads[i]->join();
//		delete threads[i];
//	}
//}


//
//class ThreadFunc
//{
//protected:
//	FunctorType& m_functor;
//	const std::vector<Dart>& m_darts;
//	boost::barrier& m_sync1;
//	boost::barrier& m_sync2;
//	bool& m_finished;
//public:
//	ThreadFunc(FunctorType& func, const std::vector<Dart>& vd, boost::barrier& s1, boost::barrier& s2, bool& finished):
//		m_functor(func), m_darts(vd), m_sync1(s1), m_sync2(s2), m_finished(finished) {}
//
//	void operator()()
//	{
//		while (!m_finished)
//		{
//			for (std::vector<Dart>::const_iterator it = m_darts.begin(); it != m_darts.end(); ++it)
//				m_functor(*it);
//			m_sync1.wait();
//			m_sync2.wait();
//		}
//	}
//};
//
//#define SZBUFF 2048
//
//template <typename PFP, int NBTH >
//void parallel_foreach_orbit(typename PFP::MAP& map, const FunctorSelect& good, unsigned int orbit, FunctorType& func)
//{
//	std::vector<Dart> vd[NBTH];
//	boost::thread* threads[NBTH];
//
//	DartMarker dm(map);
//	Dart d=map.begin();
//
//	// NBTH new functions, new thread (with good darts !)
//	for (unsigned int i=0; i<NBTH; ++i)
//		vd[i].reserve(SZBUFF);
//
//	// fill each vd buffers with 4096 darts
//	unsigned int nb =0;
//	while (( d != map.end()) && (nb < NBTH*SZBUFF) )
//	{
//		if (good(d) && (!dm.isMarked(d)))
//		{
//			dm.markOrbit(orbit,d);
//			vd[nb%NBTH].push_back(d);
//			nb++;
//		}
//		map.next(d);
//	}
//
//	boost::barrier sync1(NBTH+1);
//	boost::barrier sync2(NBTH+1);
//	bool finished=false;
//	// lauch threads
//	for (unsigned int i=0; i<NBTH; ++i)
//		threads[i] = new boost::thread(ThreadFunc(func, vd[i],sync1,sync2, finished));
//
//	// and continue to traverse the map
//	std::vector<Dart> tempo[NBTH];
//	for (unsigned int i=0; i<NBTH; ++i)
//		tempo[i].reserve(SZBUFF);
//
//	while ( d != map.end())
//	{
//		for (unsigned int i=0; i<NBTH; ++i)
//			tempo[i].clear();
//
//		unsigned int nb =0;
//		while (( d != map.end()) && (nb < NBTH*SZBUFF) )
//		{
//			if (good(d) && (!dm.isMarked(d)))
//			{
//				dm.markOrbit(orbit,d);
//				tempo[nb%NBTH].push_back(d);
//				nb++;
//			}
//			map.next(d);
//		}
//
//		sync1.wait();
//		for (unsigned int i=0; i<NBTH; ++i)
//			vd[i].swap(tempo[i]);
//		sync2.wait();
//	}
//
//	sync1.wait();
//	finished = true;
//	sync2.wait();
//
//	//wait for all theads to be finished
//	for (unsigned int i=0; i< NBTH; ++i)
//	{
//		threads[i]->join();
//		delete threads[i];
//	}
//}
//
//
//template <typename PFP, int NBTH >
//void parallel_foreach_cell(typename PFP::MAP& map, const FunctorSelect& good, unsigned int cell, FunctorType& func, unsigned int szbuff)
//{
//	std::vector<Dart> vd[NBTH];
//	boost::thread* threads[NBTH];
//
//	CellMarker cm(map,cell);
//	Dart d=map.begin();
//
//	// NBTH new functions, new thread (with good darts !)
//	for (unsigned int i=0; i<NBTH; ++i)
//		vd[i].reserve(szbuff);
//
//	// fill each vd buffers with 4096 darts
//	unsigned int nb =0;
//	while (( d != map.end()) && (nb < NBTH*szbuff) )
//	{
//		if (good(d) && (!cm.isMarked(d)))
//		{
//			cm.mark(d);
//			vd[nb%NBTH].push_back(d);
//			nb++;
//		}
//		map.next(d);
//	}
//
//	boost::barrier sync1(NBTH+1);
//	boost::barrier sync2(NBTH+1);
//	bool finished=false;
//	// lauch threads
//	for (unsigned int i=0; i<NBTH; ++i)
//		threads[i] = new boost::thread(ThreadFunc(func, vd[i],sync1,sync2, finished));
//
//	// and continue to traverse the map
//	std::vector<Dart> tempo[NBTH];
//	for (unsigned int i=0; i<NBTH; ++i)
//		tempo[i].reserve(SZBUFF);
//
//	while ( d != map.end())
//	{
//		for (unsigned int i=0; i<NBTH; ++i)
//			tempo[i].clear();
//
//		unsigned int nb =0;
//		while (( d != map.end()) && (nb < NBTH*szbuff) )
//		{
//			if (good(d) && (!cm.isMarked(d)))
//			{
//				cm.mark(d);
//				tempo[nb%NBTH].push_back(d);
//				nb++;
//			}
//			map.next(d);
//		}
//
//		sync1.wait();
//		for (unsigned int i=0; i<NBTH; ++i)
//			vd[i].swap(tempo[i]);
//		sync2.wait();
//	}
//
//	sync1.wait();
//	finished = true;
//	sync2.wait();
//
//	//wait for all theads to be finished
//	for (unsigned int i=0; i< NBTH; ++i)
//	{
//		threads[i]->join();
//		delete threads[i];
//	}
//}



template <typename PFP>
class TestFunctor : public FunctorMap<typename PFP::MAP>
{
protected:
	typename PFP::TVEC3& m_positions;
	typename PFP::TVEC3& m_normals;
public:
	TestFunctor( typename PFP::MAP& map, typename PFP::TVEC3& pos, typename PFP::TVEC3& norm):
		FunctorMap<typename PFP::MAP>(map), m_positions(pos), m_normals(norm) {}

	bool operator()(Dart d)
	{
		typename PFP::VEC3 n1 = Algo::Geometry::vertexNormal<PFP>(this->m_map, d, m_positions);
		typename PFP::VEC3 n2 = Algo::Geometry::vertexNormal<PFP>(this->m_map, this->m_map.phi1(d), m_positions);
		typename PFP::VEC3 n3 = Algo::Geometry::vertexNormal<PFP>(this->m_map, this->m_map.phi_1(d), m_positions);
		typename PFP::VEC3 n = n1+n2+n3;
		n1 = Algo::Geometry::vertexNormal<PFP>(this->m_map, d, m_positions);
		n2 = Algo::Geometry::vertexNormal<PFP>(this->m_map, this->m_map.phi1(d), m_positions);
		n3 = Algo::Geometry::vertexNormal<PFP>(this->m_map, this->m_map.phi_1(d), m_positions);
		n += n1+n2+n3;
		n.normalize();
		m_normals[d] =  n;
//		m_normals[d] = Algo::Geometry::vertexNormal<PFP>(this->m_map, d, m_positions);
		return false;
	}
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
	// cree un handler d'attribut pour la position des points (créé lors de l'import)
	position = myMap.getAttribute<PFP::VEC3>(VERTEX_ORBIT, attrNames[0]) ;
	// cree un handler pour les normales aux sommets
	AttributeHandler<PFP::VEC3> normal = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "normal");
	AttributeHandler<PFP::VEC3> normal2 = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "normal2");

	facesnormals = myMap.addAttribute<PFP::VEC3>(FACE_ORBIT, "face_normal");


	GLint t1 = glutGet(GLUT_ELAPSED_TIME);

	TestFunctor<PFP> tf(myMap,position,normal);
	myMap.foreach_orbit(VERTEX_ORBIT,tf,SelectorTrue());

	GLint t2 = glutGet(GLUT_ELAPSED_TIME);
	GLfloat seconds = (t2 - t1) / 1000.0f;
	std::cout << " linear "<< seconds << "sec" << std::endl;


//	t1 = glutGet(GLUT_ELAPSED_TIME);
//	Algo::Geometry::computeNormalVertices<PFP>(myMap,position,normal,SelectorTrue());
//	t2 = glutGet(GLUT_ELAPSED_TIME);
//	seconds = (t2 - t1) / 1000.0f;
//	std::cout << " compute direct "<< seconds << "sec" << std::endl;


	TestFunctor<PFP> tf2(myMap,position,normal2);

	t1 = glutGet(GLUT_ELAPSED_TIME);

	Algo::Parallel::foreach_cell<PFP>(myMap,VERTEX_ORBIT, tf2,4,1024);

	t2 = glutGet(GLUT_ELAPSED_TIME);
	seconds = (t2 - t1) / 1000.0f;
	std::cout << " parallel 4, 1024 "<< seconds << "sec" << std::endl;


	t1 = glutGet(GLUT_ELAPSED_TIME);

	Algo::Parallel::foreach_cell<PFP>(myMap,VERTEX_ORBIT, tf2,4,8192);

	t2 = glutGet(GLUT_ELAPSED_TIME);
	seconds = (t2 - t1) / 1000.0f;
	std::cout << " parallel 4/8192  "<< seconds << "sec" << std::endl;


	t1 = glutGet(GLUT_ELAPSED_TIME);

	Algo::Parallel::foreach_cell<PFP>(myMap,VERTEX_ORBIT, tf2,8,8192);

	t2 = glutGet(GLUT_ELAPSED_TIME);
	seconds = (t2 - t1) / 1000.0f;
	std::cout << " parallel 8/8192  "<< seconds << "sec" << std::endl;


	t1 = glutGet(GLUT_ELAPSED_TIME);

	Algo::Parallel::foreach_cell<PFP>(myMap,VERTEX_ORBIT, tf2,16,8192);

	t2 = glutGet(GLUT_ELAPSED_TIME);
	seconds = (t2 - t1) / 1000.0f;
	std::cout << " parallel 16/8192  "<< seconds << "sec" << std::endl;



//	for (unsigned int i=normal.begin(); i != normal.end(); normal.next(i))
//	{
//			std::cout<< i<< " : " << normal[i] <<  " == " << normal2[i] << std::endl;
//	}




#ifdef TOTO
	// necessaire pour me MT sinon facenormal[d]=x fait un foreach !!
	myMap.initOrbitEmbedding(FACE_ORBIT);

	GLint t1 = glutGet(GLUT_ELAPSED_TIME);


	myMap.addThreadMarker(7);

// parallele
	boost::thread thread0( Thread0<PFP>(myMap,mgw,0));
	boost::thread thread1( Thread1<PFP>(myMap,1));
	boost::thread thread2( Thread2<PFP>(myMap,2));
	boost::thread thread3( Thread3<PFP>(myMap,3));
	boost::thread thread4( Thread1<PFP>(myMap,4));
	boost::thread thread5( Thread2<PFP>(myMap,5));
	boost::thread thread6( Thread3<PFP>(myMap,6));
	boost::thread thread7( Thread1<PFP>(myMap,7));

	thread0.join();
	thread1.join();
	thread2.join();
	thread3.join();
	thread4.join();
	thread5.join();
	thread6.join();
	thread7.join();



////	// lineaire
//	Thread0<PFP> th0(myMap,mgw,0);
//	th0();
//	Thread1<PFP> th1(myMap,1);
//	th1();
//	Thread2<PFP> th2(myMap,2);
//	th2();
//	Thread3<PFP> th3(myMap,3);
//	th3();
//	Thread1<PFP> th4(myMap,4);
//	th4();
//	Thread2<PFP> th5(myMap,5);
//	th5();
//	Thread3<PFP> th6(myMap,6);
//	th6();
//	Thread1<PFP> th7(myMap,7);
//	th7();

//	GLint t2 = glutGet(GLUT_ELAPSED_TIME);
//	GLfloat seconds = (t2 - t1) / 1000.0f;
//	std::cout << "4 threads: "<< seconds << "sec" << std::endl;
#endif
	mgw.m_render->updateData(Algo::Render::VBO::POSITIONS, position);
    mgw.m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::VBO::TRIANGLES);
    mgw.m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::VBO::LINES);


//	// parcours de toutes les faces de la carte:
//	CellMarker mv(myMap,VERTEX_CELL,th);
//	for (Dart d = myMap.begin(); d != myMap.end(); myMap.next(d))
//	{
//		if (! mv.isMarked(d)) // si d non marque:
//		{
//			fonction_exemple<PFP>(myMap, position, d);
//			mv.mark(d);
//		}
//	}



    // computation of the bounding box
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
    mgw.gWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
    mgw.gPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);


    mgw.useContext();


    mgw.mainLoop();

    return 0;
}
