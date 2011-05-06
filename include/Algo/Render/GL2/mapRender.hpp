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

#include "Topology/generic/dartmarker.h"
#include "Topology/generic/cellmarker.h"
#include "Utils/vbo.h"

namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace GL2
{

template<typename PFP>
inline void MapRender::addTri(typename PFP::MAP& map, Dart d, std::vector<GLuint>& tableIndices)
{
	Dart a = d;
	Dart b = map.phi1(a);
	Dart c = map.phi1(b);

	// loop to cut a polygon in triangle on the fly (works only with convex faces)
	do
	{
		tableIndices.push_back(map.getEmbedding(d, VERTEX_ORBIT));
		tableIndices.push_back(map.getEmbedding(b, VERTEX_ORBIT));
		tableIndices.push_back(map.getEmbedding(c, VERTEX_ORBIT));
		b = c;
		c = map.phi1(b);
	} while (c != d);
}


template<typename PFP>
void MapRender::initTriangles(typename PFP::MAP& map, const FunctorSelect& good, std::vector<GLuint>& tableIndices, unsigned int thread)
{
	DartMarker m(map, thread);
	tableIndices.reserve(4 * map.getNbDarts() / 3);

	for(Dart dd = map.begin(); dd != map.end(); map.next(dd))
	{
		if(!m.isMarked(dd) && good(dd))
		{
			addTri<PFP>(map, dd, tableIndices);
			m.markOrbit(FACE_ORBIT, dd);
		}
	}
}

template<typename PFP>
void MapRender::initTrianglesOptimized(typename PFP::MAP& map, const FunctorSelect& good, std::vector<GLuint>& tableIndices, unsigned int thread)
{
#define LIST_SIZE 20
	DartMarker m(map, thread);
	// reserve memory for triangles ( nb indices == nb darts )
	// and a little bit more
	// if lots of polygonal faces, realloc is done by vector
	tableIndices.reserve(4 * map.getNbDarts() / 3);

	for (Dart dd = map.begin(); dd != map.end(); map.next(dd))
	{
		if (!m.isMarked(dd))
		{
			std::list<Dart> bound;

			if(good(dd))
				addTri<PFP>(map, dd, tableIndices);
			m.markOrbit(FACE_ORBIT, dd);
			bound.push_back(dd);
			int nb = 1;
			do
			{
				Dart e = bound.back();
				Dart ee = e;
				do
				{
					Dart f = ee;
					do
					{
						if (!m.isMarked(f))
						{
							if(good(f))
								addTri<PFP>(map, f, tableIndices);
							m.markOrbit(FACE_ORBIT, f);
							bound.push_back(map.phi1(f));
							++nb;
							if (nb > LIST_SIZE)
							{
								bound.pop_front();
								--nb;
							}
						}
						f = map.phi1(map.phi2(f));
					} while (f != ee);
					ee = map.phi1(ee);
				} while (ee != e);

				bound.pop_back();
				--nb;
			} while (!bound.empty());
		}
	}
#undef LIST_SIZE
}

template<typename PFP>
void MapRender::initLines(typename PFP::MAP& map, const FunctorSelect& good, std::vector<GLuint>& tableIndices, unsigned int thread)
{
	DartMarker m(map, thread);
	tableIndices.reserve(map.getNbDarts());

	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(!m.isMarked(d) && good(d))
		{
			tableIndices.push_back(map.getEmbedding(d, VERTEX_ORBIT));
			tableIndices.push_back(map.getEmbedding(map.phi2(d), VERTEX_ORBIT));
			m.markOrbit(EDGE_ORBIT, d);
		}
	}
}

template<typename PFP>
void MapRender::initLinesOptimized(typename PFP::MAP& map, const FunctorSelect& good, std::vector<GLuint>& tableIndices, unsigned int thread)
{
#define LIST_SIZE 20
	DartMarker m(map, thread);

	// reserve memory for edges indices ( nb indices == nb darts)
	tableIndices.reserve(map.getNbDarts());

	for (Dart dd = map.begin(); dd != map.end(); map.next(dd))
	{
		if (!m.isMarked(dd))
		{
			std::list<Dart> bound;
			bound.push_back(dd);
			int nb = 1;
			do
			{
				Dart e = bound.back();
				Dart ee = e;
				do
				{
					Dart f = map.phi2(ee);
					if (!m.isMarked(ee))
					{
						if(good(ee))
							tableIndices.push_back(map.getEmbedding(ee, VERTEX_ORBIT));
						if(good(f))
							tableIndices.push_back(map.getEmbedding(map.phi1(ee), VERTEX_ORBIT));
						m.markOrbit(EDGE_ORBIT, f);

						bound.push_back(f);
						++nb;
						if (nb > LIST_SIZE)
						{
							bound.pop_front();
							--nb;
						}
					}
					ee = map.phi1(f);
				} while (ee != e);
				bound.pop_back();
				--nb;
			} while (!bound.empty());
		}
	}
#undef LIST_SIZE
}

template<typename PFP>
void MapRender::initPoints(typename PFP::MAP& map, const FunctorSelect& good, std::vector<GLuint>& tableIndices, unsigned int thread)
{
	CellMarker m(map, VERTEX_ORBIT, thread) ;
	tableIndices.reserve(map.getNbDarts() / 5);

	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(!m.isMarked(d) && good(d))
		{
			tableIndices.push_back(map.getEmbedding(d, VERTEX_ORBIT));
			m.mark(d) ;
		}
	}
}



template<typename PFP>
void MapRender::initPrimitives(typename PFP::MAP& map, const FunctorSelect& good, int prim, bool optimized, unsigned int thread)
{
	std::vector<GLuint> tableIndices;

	// indice du VBO a utiliser
	int vbo_ind = 0;

	switch(prim)
	{
		case POINTS:
			initPoints<PFP>(map, good, tableIndices, thread);
			m_nbIndices[POINT_INDICES] = tableIndices.size();
			vbo_ind = m_indexBuffers[POINT_INDICES];
			break;
		case LINES:
			if(optimized)
				initLinesOptimized<PFP>(map, good, tableIndices, thread);
			else
				initLines<PFP>(map, good, tableIndices, thread) ;
			m_nbIndices[LINE_INDICES] = tableIndices.size();
			vbo_ind = m_indexBuffers[LINE_INDICES];
			break;
		case TRIANGLES:
			if(optimized)
				initTrianglesOptimized<PFP>(map, good, tableIndices, thread);
			else
				initTriangles<PFP>(map, good, tableIndices, thread) ;
			m_nbIndices[TRIANGLE_INDICES] = tableIndices.size();
			vbo_ind = m_indexBuffers[TRIANGLE_INDICES];
			break;
		case FLAT_TRIANGLES:
			break;
		default:
			CGoGNerr << "problem initializing VBO indices" << CGoGNendl;
			break;
	}
	unsigned int size = tableIndices.size();

	// setup du buffer d'indices
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, vbo_ind);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(GLuint), &(tableIndices[0]), GL_STREAM_DRAW);
}

} // namespace GL2

} // namespace Render

} // namespace Algo

} // namespace CGoGN
