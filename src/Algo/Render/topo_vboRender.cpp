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


#include "Algo/Render/topo_vboRender.h"


namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace VBO
{

topo_VBORender::topo_VBORender():
m_topo_dart_width(2.0f), m_topo_relation_width(3.0f)
{
	glGenBuffersARB(4, m_VBOBuffers);
}

topo_VBORender::~topo_VBORender()
{
	glDeleteBuffersARB(4, m_VBOBuffers);
	if (m_attIndex.map() != NULL)
		static_cast<AttribMap*>(m_attIndex.map())->removeAttribute(m_attIndex);
}


void topo_VBORender::setDartWidth(float dw)
{
	m_topo_dart_width = dw;
}

void topo_VBORender::setRelationWidth(float pw)
{
	m_topo_relation_width = pw;
}

void topo_VBORender::setDartColor(Dart d, float r, float g, float b)
{
	float RGB[6];
	RGB[0]=r; RGB[1]=g; RGB[2]=b;
	RGB[3]=r; RGB[4]=g; RGB[5]=b;
	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[3]);
	glBufferSubData(GL_ARRAY_BUFFER, m_attIndex[d]*3*sizeof(float), 6*sizeof(float),RGB);
}

void topo_VBORender::setAllDartsColor(float r, float g, float b)
{
	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[3]);
	GLvoid* ColorDartsBuffer = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);
	float* colorDartBuf = reinterpret_cast<float*>(ColorDartsBuffer);
	for (unsigned int i=0; i < 2*m_nbDarts; ++i)
	{
		*colorDartBuf++ = r;
		*colorDartBuf++ = g;
		*colorDartBuf++ = b;
	}

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[3]);
	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER);
}



void topo_VBORender::drawDarts()
{
	glColor3f(1.0f,1.0f,1.0f);
	glLineWidth(m_topo_dart_width);
	glPointSize(2.0f*m_topo_dart_width);

	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[3]);
	glColorPointer(3, GL_FLOAT, 0, 0);
	glEnableClientState(GL_COLOR_ARRAY);

	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[0]);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);

	glDrawArrays(GL_LINES, 0, m_nbDarts*2);

 	glVertexPointer(3, GL_FLOAT, 6*sizeof(GL_FLOAT), 0);
	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[3]);
	glColorPointer(3, GL_FLOAT, 6*sizeof(GL_FLOAT), 0);

 	glDrawArrays(GL_POINTS, 0, m_nbDarts);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

void topo_VBORender::drawRelation1()
{
	glLineWidth(m_topo_relation_width);
	glColor3f(0.0f,1.0f,1.0f);
	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[1]);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_LINES, 0, m_nbDarts*2);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void topo_VBORender::drawRelation2()
{
	glLineWidth(m_topo_relation_width);
	glColor3f(1.0f,0.0f,0.0f);
	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[2]);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_LINES, 0, m_nbRel2*2);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void topo_VBORender::drawTopo()
{
	drawDarts();
	drawRelation1();
	drawRelation2();
}



void topo_VBORender::overdrawDart(Dart d, float width, float r, float g, float b)
{
	unsigned int indexDart =  m_attIndex[d];

	glColor3f(r,g,b);
	glLineWidth(width);
	glPointSize(2.0f*width);

	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[0]);

	glVertexPointer(3, GL_FLOAT, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_LINES, indexDart, 2);

 	glVertexPointer(3, GL_FLOAT, 0, 0);
 	glDrawArrays(GL_POINTS, indexDart, 1);

	glDisableClientState(GL_VERTEX_ARRAY);
}



}//end namespace VBO

}//end namespace Algo

}//end namespace Render

}//end namespace CGoGN
