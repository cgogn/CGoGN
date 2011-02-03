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

namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace VBO
{



template <typename ATTR_HANDLER>
void MapRender_VBO::updateData(int upType, const ATTR_HANDLER& attrib, ConvertAttrib* conv)
{
	// choisit le bon buffer en fonction du paramètre upType
	unsigned int indexVBO = -1 ;
	if (upType == POSITIONS)
		indexVBO = POSITIONS_BUFFER ;
	else if (upType == NORMALS)
		indexVBO = NORMALS_BUFFER ;
	else if (upType == COLORS)
		indexVBO = COLORS_BUFFER ;
	else
	{
		std::cout << "MapRender_VBO::updateData : should not get here.. Bad type to update.." << std::endl ;
		return ;
	}

	m_allocatedBuffers[indexVBO] = true ;
	m_usedBuffers[indexVBO] = true ;

	if (conv)
		fillBufferConvert(indexVBO, attrib, conv) ;
	else
		fillBufferDirect(indexVBO, attrib) ;
}



template <typename ATTR_HANDLER>
void MapRender_VBO::fillBufferDirect(unsigned int indexVBO, const ATTR_HANDLER& attrib)
{
	AttribMultiVect<typename ATTR_HANDLER::DATA_TYPE>* mv = attrib.getDataVector() ;

	std::vector<void*> addr;
	unsigned int byteTableSize;
	unsigned int nbb = mv->getStartAddresses(addr, byteTableSize);

	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[indexVBO]);
	glBufferDataARB(GL_ARRAY_BUFFER, nbb * byteTableSize, 0, GL_STREAM_DRAW);

	unsigned int offset = 0;
//	unsigned int offsetInc = byteTableSize;

	for (unsigned int i = 0; i < nbb; ++i)
	{
		glBufferSubDataARB(GL_ARRAY_BUFFER, offset, byteTableSize, addr[i]);
		offset += byteTableSize;
	}
}


template <typename ATTR_HANDLER>
void MapRender_VBO::fillBufferConvert(unsigned int indexVBO, const ATTR_HANDLER& attrib, ConvertAttrib* conv)
{
	AttribMultiVect<typename ATTR_HANDLER::DATA_TYPE>* mv = attrib.getDataVector() ;

	std::vector<void*> addr;
	unsigned int byteTableSize;
	unsigned int nbb = mv->getStartAddresses(addr, byteTableSize);

	// alloue la memoire pour le buffer et initialise le conv
	conv->reserve(mv->BlockSize());

	// bind buffer to update
	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[indexVBO]);
	glBufferDataARB(GL_ARRAY_BUFFER, nbb * conv->sizeBuffer(), 0, GL_STREAM_DRAW);

	unsigned int offset = 0;
//	unsigned int offsetInc = byteTableSize;

	for (unsigned int i = 0; i < nbb; ++i)
	{
		// convertit les donnees dans le buffer de conv
		conv->convert(addr[i]);
		// update sub-vbo
		glBufferSubDataARB(GL_ARRAY_BUFFER, offset, conv->sizeBuffer(), conv->buffer());
		// block suivant
		offset += conv->sizeBuffer();
	}

	// libere la memoire de la conversion
	conv->release();
}

template<typename PFP>
inline void MapRender_VBO::addTri(typename PFP::MAP& map, Dart d, std::vector<GLuint>& tableIndices)
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
void MapRender_VBO::initTriangles(typename PFP::MAP& map, const FunctorSelect& good, std::vector<GLuint>& tableIndices)
{
	DartMarker m(map);
	tableIndices.reserve(4*map.getNbDarts()/3);

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
void MapRender_VBO::initTrianglesOptimized(typename PFP::MAP& map, const FunctorSelect& good, std::vector<GLuint>& tableIndices)
{
#define LIST_SIZE 20
	DartMarker m(map);
	// reserve memory for triangles ( nb indices == nb darts )
	// and a little bit more
	// if lots of polygonal faces, realloc is done by vector 
	tableIndices.reserve(4*map.getNbDarts()/3);

	for (Dart dd = map.begin(); dd != map.end(); map.next(dd))
	{
		if (!m.isMarked(dd))
		{
			std::list<Dart> bound;

			if(good(dd))
				addTri<PFP>(map,dd,tableIndices);
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
void MapRender_VBO::initLines(typename PFP::MAP& map, const FunctorSelect& good, std::vector<GLuint>& tableIndices)
{
	DartMarker m(map);
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
void MapRender_VBO::initLinesOptimized(typename PFP::MAP& map, const FunctorSelect& good, std::vector<GLuint>& tableIndices)
{
#define LIST_SIZE 20
	DartMarker m(map);
	// reserve memory for edges indices ( nb indices == nb darts)
	tableIndices.reserve(map.getNbDarts());

	for (Dart dd = map.begin(); dd != map.end(); map.next(dd))
	{
		if (!m.isMarked(dd) && good(dd))
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
void MapRender_VBO::initPoints(typename PFP::MAP& map, const FunctorSelect& good, std::vector<GLuint>& tableIndices)
{
	CellMarker m(map, VERTEX_ORBIT) ;
	tableIndices.reserve(map.getNbDarts()/5);

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
void MapRender_VBO::initPrimitives(typename PFP::MAP& map, const FunctorSelect& good, int prim, bool optimized)
{
	std::vector<GLuint> tableIndices;

	// indice du VBO a utiliser
	int vbo_ind = 0;
	switch(prim)
	{
		case FLAT_TRIANGLES:
			initFlatTriangles<PFP>(map,good);
			break;

		case TRIANGLES:
			if(optimized)
				initTrianglesOptimized<PFP>(map,good,tableIndices);
			else
				initTriangles<PFP>(map,good,tableIndices) ;
			m_nbIndicesTri = tableIndices.size();
			vbo_ind = m_VBOBuffers[TRIANGLE_INDICES];
			break;
		case LINES:
			if(optimized)
				initLinesOptimized<PFP>(map,good,tableIndices);
			else
				initLines<PFP>(map,good,tableIndices) ;
			m_nbIndicesLines = tableIndices.size();
			vbo_ind = m_VBOBuffers[LINE_INDICES];
			break;
		case POINTS:
			initPoints<PFP>(map,good,tableIndices);
			m_nbIndicesPoints = tableIndices.size();
			vbo_ind = m_VBOBuffers[POINT_INDICES];
			break;
		default:
			std::cerr << "problem initializing VBO indices" << std::endl;
			break;
	}
	int size = tableIndices.size();

	// setup du buffer d'indices
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, vbo_ind);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, size*sizeof(GLuint), &(tableIndices[0]), GL_STREAM_DRAW);
}




template<typename PFP>
void MapRender_VBO::initFlatTriangles(typename PFP::MAP& map, const FunctorSelect& good)
{
	std::vector<Geom::Vec3f> tableFlat;
	tableFlat.reserve(3*map.getNbDarts()); // 3 in case of polygonal faces (less chance of realloc, but bigger allocation)

	// map VBO of points for vertices positions
	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[POSITIONS_BUFFER]);
	Geom::Vec3f* tablePos = reinterpret_cast<Geom::Vec3f*>(glMapBuffer(GL_ARRAY_BUFFER,GL_READ_ONLY));

	m_nbFlatElts=0;
	// traversal of map for creating buffers
	DartMarker m(map);
	for(Dart dd = map.begin(); dd != map.end(); map.next(dd))
	{
		if(!m.isMarked(dd) && good(dd))
		{
			Dart a = dd;
			Dart b = map.phi1(a);
			Dart c = map.phi1(b);

			Geom::Vec3f& P = tablePos[map.getEmbedding(a, VERTEX_ORBIT)];
			Geom::Vec3f& Q = tablePos[map.getEmbedding(b, VERTEX_ORBIT)];
			Geom::Vec3f& R = tablePos[map.getEmbedding(c, VERTEX_ORBIT)];

			Geom::Vec3f U = Q-P;
			Geom::Vec3f V = R-P;
			Geom::Vec3f N = U^V;
			N.normalize();

			// loop to cut a polygon in triangle on the fly (works only with convex faces)
			do
			{
				tableFlat.push_back(tablePos[map.getEmbedding(a, VERTEX_ORBIT)]);
				tableFlat.push_back(N);
				tableFlat.push_back(tablePos[map.getEmbedding(b, VERTEX_ORBIT)]);
				tableFlat.push_back(N);
				tableFlat.push_back(tablePos[map.getEmbedding(c, VERTEX_ORBIT)]);
				tableFlat.push_back(N);
				b = c;
				c = map.phi1(b);
			} while (c != dd);
			m.markOrbit(FACE_ORBIT, dd);
		}
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	m_nbFlatElts = tableFlat.size()/2;

	//creating VBO for flat
	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[FLAT_BUFFER]);
	glBufferDataARB(GL_ARRAY_BUFFER, tableFlat.size() * sizeof(Geom::Vec3f), (char*)(&(tableFlat[0])), GL_STREAM_DRAW);

}





} // namespace VBO

} // namespace Render

} // namespace Algo

} // namespace CGoGN
