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

namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace VBO
{

template<typename PFP>
MapRender_VBO<PFP>::MapRender_VBO(MAP& map, const FunctorSelect& good) :
	m_map(map),
	m_good(good),
	m_nbIndicesTri(0),
	m_nbIndicesLines(0)
{
	glGenBuffersARB(NB_BUFFERS, m_VBOBuffers) ;
	for(unsigned int i = 0; i < NB_BUFFERS; ++i)
	{
		m_allocatedBuffers[i] = false ;
		m_usedBuffers[i] = false ;
	}
}

template<typename PFP>
MapRender_VBO<PFP>::~MapRender_VBO()
{
	glDeleteBuffersARB(NB_BUFFERS, m_VBOBuffers);
	delete[] m_VBOBuffers ;
}

template <typename PFP>
template <typename ATTR_HANDLER>
void MapRender_VBO<PFP>::updateData(int upType, const ATTR_HANDLER& attrib, ConvertAttrib* conv)
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
		std::cout << "MapRender_VBO<PFP>::updateData : should not get here.. Bad type to update.." << std::endl ;
		return ;
	}

	m_allocatedBuffers[indexVBO] = true ;
	m_usedBuffers[indexVBO] = true ;

	if (conv)
		fillBufferConvert(indexVBO, attrib, conv) ;
	else
		fillBufferDirect(indexVBO, attrib) ;
}

template <typename PFP>
inline void MapRender_VBO<PFP>::enableBuffers(int buffersMask)
{
	if((buffersMask & POSITIONS) && m_allocatedBuffers[POSITIONS_BUFFER])
		m_usedBuffers[POSITIONS_BUFFER] = true ;

	if((buffersMask & NORMALS) && m_allocatedBuffers[NORMALS_BUFFER])
		m_usedBuffers[NORMALS_BUFFER] = true ;

	if((buffersMask & COLORS) && m_allocatedBuffers[COLORS_BUFFER])
		m_usedBuffers[COLORS_BUFFER] = true ;
}

template <typename PFP>
inline void MapRender_VBO<PFP>::disableBuffers(int buffersMask)
{
	if((buffersMask & POSITIONS))
		m_usedBuffers[POSITIONS_BUFFER] = false ;

	if((buffersMask & NORMALS))
		m_usedBuffers[NORMALS_BUFFER] = false ;

	if((buffersMask & COLORS))
		m_usedBuffers[COLORS_BUFFER] = false ;
}

template <typename PFP>
template <typename ATTR_HANDLER>
void MapRender_VBO<PFP>::fillBufferDirect(unsigned int indexVBO, const ATTR_HANDLER& attrib)
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

template <typename PFP>
template <typename ATTR_HANDLER>
void MapRender_VBO<PFP>::fillBufferConvert(unsigned int indexVBO, const ATTR_HANDLER& attrib, ConvertAttrib* conv)
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
inline void MapRender_VBO<PFP>::addTri(Dart d, std::vector<GLuint>& tableIndices)
{
	Dart a = d;
	Dart b = m_map.phi1(a);
	Dart c = m_map.phi1(b);
	
	// loop to cut a polygon in triangle on the fly (works only with convex faces)
	do
	{
		tableIndices.push_back(m_map.getEmbedding(d, VERTEX_ORBIT));
		tableIndices.push_back(m_map.getEmbedding(b, VERTEX_ORBIT));
		tableIndices.push_back(m_map.getEmbedding(c, VERTEX_ORBIT));
		b = c;
		c = m_map.phi1(b);
	} while (c != d);
}

template<typename PFP>
void MapRender_VBO<PFP>::initTriangles(std::vector<GLuint>& tableIndices)
{
	DartMarker m(m_map);
	tableIndices.reserve(4*m_map.getNbDarts()/3);

	for(Dart dd = m_map.begin(); dd != m_map.end(); m_map.next(dd))
	{
		if(!m.isMarked(dd) && m_good(dd))
		{
			addTri(dd, tableIndices);
			m.markOrbit(FACE_ORBIT, dd);
		}
	}
}

template<typename PFP>
void MapRender_VBO<PFP>::initTrianglesOptimized(std::vector<GLuint>& tableIndices)
{
#define LIST_SIZE 20
	DartMarker m(m_map);
	// reserve memory for triangles ( nb indices == nb darts )
	// and a little bit more
	// if lots of polygonal faces, realloc is done by vector 
	tableIndices.reserve(4*m_map.getNbDarts()/3);

	for (Dart dd = m_map.begin(); dd != m_map.end(); m_map.next(dd))
	{
		if (!m.isMarked(dd))
		{
			std::list<Dart> bound;

			if(m_good(dd))
				addTri(dd,tableIndices);
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
							if(m_good(f))
								addTri(f, tableIndices);
							m.markOrbit(FACE_ORBIT, f);
							bound.push_back(m_map.phi1(f));
							++nb;
							if (nb > LIST_SIZE)
							{
								bound.pop_front();
								--nb;
							}
						}
						f = m_map.phi1(m_map.phi2(f));
					} while (f != ee);
					ee = m_map.phi1(ee);
				} while (ee != e);

				bound.pop_back();
				--nb;
			} while (!bound.empty());
		}
	}
#undef LIST_SIZE
}

template<typename PFP>
void MapRender_VBO<PFP>::initLines(std::vector<GLuint>& tableIndices)
{
	DartMarker m(m_map);
	tableIndices.reserve(m_map.getNbDarts());

	for(Dart d = m_map.begin(); d != m_map.end(); m_map.next(d))
	{
		if(!m.isMarked(d) && m_good(d))
		{
			tableIndices.push_back(m_map.getEmbedding(d, VERTEX_ORBIT));
			tableIndices.push_back(m_map.getEmbedding(m_map.phi2(d), VERTEX_ORBIT));
			m.markOrbit(EDGE_ORBIT, d);
		}
	}
}

template<typename PFP>
void MapRender_VBO<PFP>::initLinesOptimized(std::vector<GLuint>& tableIndices)
{
#define LIST_SIZE 20
	DartMarker m(m_map);
	// reserve memory for edges indices ( nb indices == nb darts)
	tableIndices.reserve(m_map.getNbDarts());

	for (Dart dd = m_map.begin(); dd != m_map.end(); m_map.next(dd))
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
					Dart f = m_map.phi2(ee);
					if (!m.isMarked(ee))
					{
						if(m_good(ee))
							tableIndices.push_back(m_map.getEmbedding(ee, VERTEX_ORBIT));
						if(m_good(f))
							tableIndices.push_back(m_map.getEmbedding(m_map.phi1(ee), VERTEX_ORBIT));
						m.markOrbit(EDGE_ORBIT, f);

						bound.push_back(f);
						++nb;
						if (nb > LIST_SIZE)
						{
							bound.pop_front();
							--nb;
						}
					}
					ee = m_map.phi1(f);
				} while (ee != e);
				bound.pop_back();
				--nb;
			} while (!bound.empty());
		}
	}
#undef LIST_SIZE
}

template<typename PFP>
void MapRender_VBO<PFP>::initPoints(std::vector<GLuint>& tableIndices)
{
	CellMarker m(m_map, VERTEX_ORBIT) ;
	tableIndices.reserve(m_map.getNbDarts()/5);

	for(Dart d = m_map.begin(); d != m_map.end(); m_map.next(d))
	{
		if(!m.isMarked(d) && m_good(d))
		{
			tableIndices.push_back(m_map.getEmbedding(d, VERTEX_ORBIT));
			m.mark(d) ;
		}
	}
}

template<typename PFP>
void MapRender_VBO<PFP>::initPrimitives(int prim, bool optimized)
{
	std::vector<GLuint> tableIndices;

	// indice du VBO a utiliser
	int vbo_ind = 0;
	switch(prim)
	{
		case FLAT_TRIANGLES:
			initFlatTriangles();
			break;

		case TRIANGLES:
			if(optimized)
				initTrianglesOptimized(tableIndices);
			else
				initTriangles(tableIndices) ;
			m_nbIndicesTri = tableIndices.size();
			vbo_ind = m_VBOBuffers[TRIANGLE_INDICES];
			break;
		case LINES:
			if(optimized)
				initLinesOptimized(tableIndices);
			else
				initLines(tableIndices) ;
			m_nbIndicesLines = tableIndices.size();
			vbo_ind = m_VBOBuffers[LINE_INDICES];
			break;
		case POINTS:
			initPoints(tableIndices);
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
void MapRender_VBO<PFP>::initPrimitives(int prim, std::vector<GLuint>& tableIndices)
{
	// indice du VBO a utiliser
	int vbo_ind = 0;
	switch(prim)
	{
		case TRIANGLES:
			m_nbIndicesTri = tableIndices.size();
			vbo_ind = m_VBOBuffers[TRIANGLE_INDICES];
			break;
		case LINES:
			m_nbIndicesLines = tableIndices.size();
			vbo_ind = m_VBOBuffers[LINE_INDICES];
			break;
		case POINTS:
			m_nbIndicesPoints = tableIndices.size();
			vbo_ind = m_VBOBuffers[POINT_INDICES];
			break;
		case FLAT_TRIANGLES:
			initFlatTriangles();
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
void MapRender_VBO<PFP>::drawTriangles(bool bindColors)
{
	// buffer d'indices
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[TRIANGLE_INDICES]);
	glEnableClientState(GL_INDEX_ARRAY);

	for(unsigned int i = POSITIONS_BUFFER; i < NB_BUFFERS; ++i)
	{
		if(m_usedBuffers[i])
		{
			glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[i]);
			switch(i)
			{
				case POSITIONS_BUFFER :
					glVertexPointer(3, GL_FLOAT, 0, 0);
					glEnableClientState(GL_VERTEX_ARRAY);
					break ;
				case NORMALS_BUFFER :
					glNormalPointer(GL_FLOAT, 0, 0);
					glEnableClientState(GL_NORMAL_ARRAY);
					break ;
				case COLORS_BUFFER :
					if (bindColors) {
						glColorPointer(4, GL_FLOAT, 0, 0);
						glEnableClientState(GL_COLOR_ARRAY);
					}
					break ;
			}
		}
	}

	glDrawElements(GL_TRIANGLES, m_nbIndicesTri, GL_UNSIGNED_INT, 0);
	glDisableClientState(GL_INDEX_ARRAY);

	for(unsigned int i = POSITIONS_BUFFER; i < NB_BUFFERS; ++i)
	{
		if(m_usedBuffers[i])
		{
			switch(i)
			{
				case POSITIONS_BUFFER :
					glDisableClientState(GL_VERTEX_ARRAY);
					break ;
				case NORMALS_BUFFER :
					glDisableClientState(GL_NORMAL_ARRAY);
					break ;
				case COLORS_BUFFER :
					if (bindColors) {
						glDisableClientState(GL_COLOR_ARRAY);
					}
					break ;
			}
		}
	}
}

template<typename PFP>
void MapRender_VBO<PFP>::drawLines(bool bindColors)
{
	// buffer d'indices
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[LINE_INDICES]);
	glEnableClientState(GL_INDEX_ARRAY);

	for(unsigned int i = POSITIONS_BUFFER; i < NB_BUFFERS; ++i)
	{
		if(m_usedBuffers[i])
		{
			switch(i)
			{
				case POSITIONS_BUFFER :
					glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[i]);
					glVertexPointer(3, GL_FLOAT, 0, 0);
					glEnableClientState(GL_VERTEX_ARRAY);
					break ;
				case NORMALS_BUFFER :
					break ;
				case COLORS_BUFFER :
					if (bindColors) {
						glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[i]);
						glColorPointer(4, GL_FLOAT, 0, 0);
						glEnableClientState(GL_COLOR_ARRAY);
					}
					break ;
			}
		}
	}

	glDrawElements(GL_LINES, m_nbIndicesLines, GL_UNSIGNED_INT, 0);
	glDisableClientState(GL_INDEX_ARRAY);

	for(unsigned int i = POSITIONS_BUFFER; i < NB_BUFFERS; ++i)
	{
		if(m_usedBuffers[i])
		{
			switch(i)
			{
				case POSITIONS_BUFFER :
					glDisableClientState(GL_VERTEX_ARRAY);
					break ;
				case NORMALS_BUFFER :
					break ;
				case COLORS_BUFFER :
					if (bindColors) {
						glDisableClientState(GL_COLOR_ARRAY) ;
					}
					break ;
			}
		}
	}
}

template<typename PFP>
void MapRender_VBO<PFP>::drawPoints(bool bindColors)
{
	// buffer d'indices
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[POINT_INDICES]);
	glEnableClientState(GL_INDEX_ARRAY);

	for(unsigned int i = POSITIONS_BUFFER; i < NB_BUFFERS; ++i)
	{
		if(m_usedBuffers[i])
		{
			switch(i)
			{
				case POSITIONS_BUFFER :
					glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[i]);
					glVertexPointer(3, GL_FLOAT, 0, 0);
					glEnableClientState(GL_VERTEX_ARRAY);
					break ;
				case NORMALS_BUFFER :
					break ;
				case COLORS_BUFFER :
					if (bindColors) {
						glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[i]);
						glColorPointer(4, GL_FLOAT, 0, 0);
						glEnableClientState(GL_COLOR_ARRAY);
					}
					break ;
			}
		}
	}

	glDrawElements(GL_POINTS, m_nbIndicesPoints, GL_UNSIGNED_INT, 0) ;
	glDisableClientState(GL_INDEX_ARRAY);

	for(unsigned int i = POSITIONS_BUFFER; i < NB_BUFFERS; ++i)
	{
		if(m_usedBuffers[i])
		{
			switch(i)
			{
				case POSITIONS_BUFFER :
					glDisableClientState(GL_VERTEX_ARRAY);
					break ;
				case NORMALS_BUFFER :
					break ;
				case COLORS_BUFFER :
					if (bindColors) {
						glDisableClientState(GL_COLOR_ARRAY);
					}
					break ;
				default:
					break;
			}
		}
	}
}

template<typename PFP>
inline void MapRender_VBO<PFP>::draw(int prim, bool bindColors)
{ 
	switch(prim)
	{	
		case TRIANGLES:
			drawTriangles(bindColors);
			break;
		case LINES:
			drawLines(bindColors);
			break;
		case POINTS:
			drawPoints(bindColors);
			break;
		case FLAT_TRIANGLES:
			drawFlat();
			break;
		default:
			break;
	}
}

template<typename PFP>
void MapRender_VBO<PFP>::initFlatTriangles()
{
	std::vector<Geom::Vec3f> tableFlat;
	tableFlat.reserve(3*m_map.getNbDarts()); // 3 in case of polygonal faces (less chance of realloc, but bigger allocation)

	// map VBO of points for vertices positions
	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[POSITIONS_BUFFER]);
	Geom::Vec3f* tablePos = reinterpret_cast<Geom::Vec3f*>(glMapBuffer(GL_ARRAY_BUFFER,GL_READ_ONLY));

	m_nbFlatElts=0;
	// traversal of map for creating buffers
	DartMarker m(m_map);
	for(Dart dd = m_map.begin(); dd != m_map.end(); m_map.next(dd))
	{
		if(!m.isMarked(dd) && m_good(dd))
		{
			Dart a = dd;
			Dart b = m_map.phi1(a);
			Dart c = m_map.phi1(b);

			Geom::Vec3f& P = tablePos[m_map.getEmbedding(a, VERTEX_ORBIT)];
			Geom::Vec3f& Q = tablePos[m_map.getEmbedding(b, VERTEX_ORBIT)];
			Geom::Vec3f& R = tablePos[m_map.getEmbedding(c, VERTEX_ORBIT)];

			Geom::Vec3f U = Q-P;
			Geom::Vec3f V = R-P;
			Geom::Vec3f N = U^V;
			N.normalize();

			// loop to cut a polygon in triangle on the fly (works only with convex faces)
			do
			{
				tableFlat.push_back(tablePos[m_map.getEmbedding(a, VERTEX_ORBIT)]);
				tableFlat.push_back(N);
				tableFlat.push_back(tablePos[m_map.getEmbedding(b, VERTEX_ORBIT)]);
				tableFlat.push_back(N);
				tableFlat.push_back(tablePos[m_map.getEmbedding(c, VERTEX_ORBIT)]);
				tableFlat.push_back(N);
				b = c;
				c = m_map.phi1(b);
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


template<typename PFP>
void MapRender_VBO<PFP>::drawFlat()
{
	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[FLAT_BUFFER]);
 	glVertexPointer(3, GL_FLOAT, 6*sizeof(GL_FLOAT), 0);
	glNormalPointer(GL_FLOAT, 6*sizeof(GL_FLOAT),   (GLvoid*)(3*sizeof(GL_FLOAT)));
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glDrawArrays(GL_TRIANGLES, 0, m_nbFlatElts);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}


} // namespace VBO

} // namespace Render

} // namespace Algo

} // namespace CGoGN
