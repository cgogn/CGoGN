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

#include "Algo/Render/gl3mapRender.h"

namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace GL3
{

MapRender::MapRender():
	m_nbIndicesTri(0),
	m_nbIndicesLines(0)
{
	glGenBuffersARB(4, m_VBOBuffers) ;

	for(unsigned int i = 0; i < NB_BUFFERS; ++i)
	{
		m_allocatedAttributes[i] = false ;
		m_usedAttributes[i] = false ;
		m_AttributesDataSize[i]=0;
	}
}


MapRender::~MapRender()
{
	glDeleteBuffersARB(4, m_VBOBuffers);
	delete[] m_VBOBuffers ;

	for(unsigned int i = 4; i < NB_BUFFERS; ++i)
	{
		if (m_allocatedAttributes[i])
			glDeleteBuffersARB(4, &(m_VBOBuffers[i]));
	}
}


MapRender::MapRender(const MapRender& mrvbo):
	m_nbIndicesTri(0),
	m_nbIndicesLines(0)
{
	glGenBuffersARB(3, m_VBOBuffers) ; // gen only for indices
	// get back others from mrvbo
	for(unsigned int i = FLAT_BUFFER; i < NB_BUFFERS; ++i)
	{
		m_allocatedAttributes[i-FLAT_BUFFER] = mrvbo.m_allocatedAttributes[i] ;
		m_usedAttributes[i-FLAT_BUFFER] = mrvbo.m_usedAttributes[i] ;
		m_VBOBuffers[i] = mrvbo.m_VBOBuffers[i];
	}
}


void MapRender::enableVertexAttrib(unsigned int index)
{
	m_usedAttributes[index] = true ;
}

void MapRender::disableVertexAttrib(unsigned int index)
{
	m_usedAttributes[index] = false ;
}


void MapRender::initPrimitives(int prim, std::vector<GLuint>& tableIndices)
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
		default:
			std::cerr << "problem initializing VBO indices" << std::endl;
			break;
	}
	int size = tableIndices.size();

	// setup du buffer d'indices
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, vbo_ind);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, size*sizeof(GLuint), &(tableIndices[0]), GL_STREAM_DRAW);
}


void MapRender::drawTriangles(bool bindColors)
{

	for(unsigned int i = FIRST_ATTRIBUTE_BUFFER; i < NB_BUFFERS; ++i)
	{
		unsigned int j = i-FIRST_ATTRIBUTE_BUFFER;
		if(m_usedAttributes[j])
		{
			glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[i]);
			glEnableVertexAttribArray(j);
			glVertexAttribPointer(j, m_AttributesDataSize[j], GL_FLOAT, false, 0, 0);
		}
	}

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[TRIANGLE_INDICES]);
//	glEnableClientState(GL_INDEX_ARRAY);
	glDrawElements(GL_TRIANGLES, m_nbIndicesTri, GL_UNSIGNED_INT, 0);
//	glDisableClientState(GL_INDEX_ARRAY);

	for(unsigned int j = 0; j < NB_BUFFERS-FIRST_ATTRIBUTE_BUFFER; ++j)
	{
		if(m_usedAttributes[j])
		{
			glDisableVertexAttribArray(j);
		}
	}
}

void MapRender::drawLines(bool bindColors)
{
	for(unsigned int i = FIRST_ATTRIBUTE_BUFFER; i < NB_BUFFERS; ++i)
	{
		unsigned int j = i-FIRST_ATTRIBUTE_BUFFER;
		if(m_usedAttributes[j])
		{
			glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[i]);
			glEnableVertexAttribArray(j);
			glVertexAttribPointer(j, m_AttributesDataSize[j], GL_FLOAT, false, 0, 0);

		}
	}

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[LINE_INDICES]);
//	glEnableClientState(GL_INDEX_ARRAY);
	glDrawElements(GL_LINES, m_nbIndicesLines, GL_UNSIGNED_INT, 0);
//	glDisableClientState(GL_INDEX_ARRAY);

	for(unsigned int j = 0; j < NB_BUFFERS-FIRST_ATTRIBUTE_BUFFER; ++j)
	{
		if(m_usedAttributes[j])
		{
			glDisableVertexAttribArray(j);
		}
	}
}

void MapRender::drawPoints(bool bindColors)
{
	for(unsigned int i = FIRST_ATTRIBUTE_BUFFER; i < NB_BUFFERS; ++i)
	{
		unsigned int j = i-FIRST_ATTRIBUTE_BUFFER;
		if(m_usedAttributes[j])
		{
			glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[i]);
			glEnableVertexAttribArray(j);
			glVertexAttribPointer(j, m_AttributesDataSize[j], GL_FLOAT, false, 0, 0);
		}
	}

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[POINT_INDICES]);
//	glEnableClientState(GL_INDEX_ARRAY);
	glDrawElements(GL_POINTS, m_nbIndicesPoints, GL_UNSIGNED_INT, 0) ;
//	glDisableClientState(GL_INDEX_ARRAY);

	for(unsigned int j = 0; j < NB_BUFFERS-FIRST_ATTRIBUTE_BUFFER; ++j)
	{
		if(m_usedAttributes[j])
		{
			glDisableVertexAttribArray(j);
		}
	}
}


void MapRender::draw(int prim, bool bindColors)
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


void MapRender::drawFlat()
{
	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[FLAT_BUFFER]);

	// attrib 0 = position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, m_AttributesDataSize[0], GL_FLOAT, false, 6*sizeof(GL_FLOAT), 0);
	// attrib 1 = normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, m_AttributesDataSize[1], GL_FLOAT, false, 6*sizeof(GL_FLOAT),  (GLvoid*)(3*sizeof(GL_FLOAT)));

	glDrawArrays(GL_TRIANGLES, 0, m_nbFlatElts);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}


} // namespace GL3

} // namespace Render

} // namespace Algo

} // namespace CGoGN
