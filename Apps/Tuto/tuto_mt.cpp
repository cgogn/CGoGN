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

#include <iostream>

#include "tuto_mt.h"

#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"

#include "Geometry/vector_gen.h"

#include "Algo/Import/import.h"
#include "Algo/Geometry/boundingbox.h"
#include "Utils/GLSLShader.h"
//#include "Algo/Geometry/area.h"
#include "Algo/Geometry/normal.h"
//#include "Algo/Modelisation/polyhedron.h"
#include "Algo/Tiling/Surface/square.h"

#include "Algo/Parallel/parallel_foreach.h"

#include "Utils/cgognStream.h"
#include "Utils/chrono.h"

using namespace CGoGN ;

/**
 * Struct that contains some informations about the types of the manipulated objects
 * Mainly here to be used by the algorithms that are parameterized by it
 */
struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef EmbeddedMap2 MAP;
};


PFP::MAP myMap;
VertexAttribute<PFP::VEC3> position;
VertexAttribute<PFP::VEC3> position2;
VertexAttribute<PFP::VEC3> normal;



void MyQT::cb_initGL()
{
	Utils::GLSLShader::setCurrentOGLVersion(2);

	// create the render
	m_render = new Algo::Render::GL2::MapRender();

 	// create VBO for position
	m_positionVBO = new Utils::VBO();
	m_positionVBO->updateData(position);

	m_normalVBO =  new Utils::VBO();

	m_shader = new Utils::ShaderSimpleColor();
	m_shader->setAttributePosition(m_positionVBO);
	m_shader->setColor(Geom::Vec4f(1.,1.,0.,0.));


	m_lines = new Utils::ShaderVectorPerVertex();
	m_lines->setAttributePosition(m_positionVBO);
	m_lines->setAttributeVector(m_normalVBO);
	m_lines->setScale(2.0f);
	m_lines->setColor(Geom::Vec4f(0.0f, 1.0f, 0.2f, 0.0f));

	Algo::Surface::Geometry::computeNormalVertices<PFP>(myMap, position, normal) ;

	m_render->initPrimitives<PFP>(myMap, Algo::Render::GL2::LINES);
	m_render->initPrimitives<PFP>(myMap, Algo::Render::GL2::POINTS);

	registerShader(m_shader);
	registerShader(m_lines);

	m_normalVBO->updateData(normal);

}

void MyQT::cb_redraw()
{
	m_render->draw(m_shader, Algo::Render::GL2::LINES);
	m_render->draw(m_lines, Algo::Render::GL2::POINTS);
}


void MyQT::cb_keyPress(int code)
{
	Utils::Chrono ch;
	ch.start();

	switch(code)
	{
	case 'a':
		threadSimple();
		break;
	case 'z':
		threadStorage();
		break;
	case 'q':
		threadAttrib();
		break;
	default:
		break;
	}
	CGoGNout << "time = "<< ch.elapsed() << CGoGNendl;

	updateGL();
}



//
// Simple attribute parallel functor and traversor
//
template <typename XXX>
class UnshrinkFunctor : public FunctorAttribThreaded
{
protected:
	VertexAttribute<typename XXX::VEC3>& m_positions;
	VertexAttribute<typename XXX::VEC3>& m_positions2;
public:
	UnshrinkFunctor( VertexAttribute<typename XXX::VEC3>& pos, VertexAttribute<typename XXX::VEC3>& pos2):
		FunctorAttribThreaded(),m_positions(pos),m_positions2(pos2)
	{}

	void run(unsigned int i, unsigned int /*threadID*/)
	{
		m_positions2[i] = 1.1f * m_positions[i];
	}
};


void MyQT::threadAttrib()
{
	UnshrinkFunctor<PFP> funct(position,position2);
	Algo::Parallel::foreach_attrib(myMap.getAttributeContainer<VERTEX>(), funct);

	myMap.swapAttributes(position,position2);
	m_positionVBO->updateData(position);
	m_lines->setAttributePosition(m_positionVBO);
	updateGL();
}



//
// Simple thread that traverse a map
//
template <typename XXX>
class ShrinkFunctor : public FunctorMapThreaded<typename XXX::MAP >
{
protected:
	VertexAttribute<typename XXX::VEC3>& m_positions;
	VertexAttribute<typename XXX::VEC3>& m_positions2;
public:
	ShrinkFunctor( typename XXX::MAP& map, VertexAttribute<typename XXX::VEC3>& pos, VertexAttribute<typename XXX::VEC3>& pos2):
		FunctorMapThreaded< typename XXX::MAP>(map),
		m_positions(pos),m_positions2(pos2)
	{}

	void run(Dart d, unsigned int /*threadID*/)
	{
		typename XXX::VEC3 Q(0,0,0);
		int nb=0;
		Traversor2VVaE<typename XXX::MAP> trav(this->m_map,d);
		for (Dart e=trav.begin(); e!=trav.end(); e = trav.next())
		{
			Q += m_positions[e];
			nb++;
		}
		m_positions2[d] = Q/nb;
	}
	// no need to duplicate here functor can be shared (no data), call foreach with true parameter
};


void MyQT::threadSimple()
{
	ShrinkFunctor<PFP> funct(myMap,position,position2);
	Algo::Parallel::foreach_cell<PFP::MAP,VERTEX>(myMap, funct);

	myMap.swapAttributes(position,position2);
	m_positionVBO->updateData(position);
	m_lines->setAttributePosition(m_positionVBO);
	updateGL();
}






// Thread foreach with storage (computing average length of edges)

template <typename XXX>
class LengthEdgeFunctor : public FunctorMapThreaded<typename XXX::MAP >
{
protected:
	VertexAttribute<typename XXX::VEC3>& m_positions;
	double m_length;
	unsigned int m_nb;
public:
	LengthEdgeFunctor( typename XXX::MAP& map, VertexAttribute<typename XXX::VEC3>& pos):
		FunctorMapThreaded< typename XXX::MAP>(map),
		m_positions(pos), m_length(0.0), m_nb(0)
	{}

	double getLength() { return m_length;}

	unsigned int getNb() { return m_nb;}

	void run(Dart d, unsigned int /*threadID*/)
	{
		Dart dd = this->m_map.phi2(d);
		typename XXX::VEC3 V = m_positions[dd] - m_positions[d];
		m_length += V.norm();
		m_nb++;
	}

	// no need to duplicate here, we create 1 functor by thread (see bellow)
};

void MyQT::threadStorage()
{
	// functor need storage so we need one per thread
	std::vector<FunctorMapThreaded<PFP::MAP>*> functs;
	unsigned int nbthreads = Algo::Parallel::optimalNbThreads();

	for (unsigned int i=0; i<nbthreads; ++i)
	{
		LengthEdgeFunctor<PFP>* lef = new LengthEdgeFunctor<PFP>(myMap,position);
		functs.push_back(lef);
	}

	CGoGNout << "using "<< nbthreads << " threads"<< CGoGNendl;
	Algo::Parallel::foreach_cell<PFP::MAP,EDGE>(myMap, functs);

	//compute average length from each thread result and delete functors
	double average = 0;
	unsigned int all = 0;
	for (unsigned int i=0; i<nbthreads; ++i)
	{
		LengthEdgeFunctor<PFP>* lef = dynamic_cast<LengthEdgeFunctor<PFP>*>(functs[i]);
		average += lef->getLength();
		all += lef->getNb();
		delete lef;
	}
	average /= all;

	CGoGNout << "AVERAGE LENGTH "<< average << CGoGNendl;

}


int main(int argc, char **argv)
{
	// interface:
	QApplication app(argc, argv);
	MyQT sqt;


 	sqt.statusMsg("Neww to create a sphere or Load for a mesh file");
 	CGoGNStream::allToConsole(&sqt);

 	if (!position.isValid())
 		position = myMap.addAttribute<PFP::VEC3, VERTEX>("position");

	if (!position2.isValid())
 		position2 = myMap.addAttribute<PFP::VEC3, VERTEX>("position2");

	if (!normal.isValid())
 		normal = myMap.addAttribute<PFP::VEC3, VERTEX>("normal");

	unsigned int nbt = 64;
	if (argc==2)
		nbt = atoi(argv[1]);
 		// create a sphere
     Algo::Surface::Tilings::Square::Cylinder<PFP> prim(myMap, nbt, nbt, true, true);
    prim.embedIntoSphere(position, 20.0f);


   //  bounding box
	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
	float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
	Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);
	CGoGNout << "lPosObj=" << lPosObj << CGoGNendl;
	CGoGNout << "lWidthObj=" << lWidthObj << CGoGNendl;
	sqt.setParamObject(lWidthObj,lPosObj.data());

//	myMap.enableQuickTraversal<EDGE>() ;
//	myMap.enableQuickTraversal<VERTEX>() ;

	sqt.show();


	return app.exec();
}
