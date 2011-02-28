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

#include "Algo/Render/topo3_vboRender.h"
#include "Topology/generic/dart.h"

#include <string.h>

namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace VBO
{


topo3_VBORender::topo3_VBORender():
m_topo_dart_width(2.0f), m_topo_relation_width(3.0f),m_color_save(NULL)
{
	glGenBuffersARB(5, m_VBOBuffers);
}


topo3_VBORender::~topo3_VBORender()
{
	glDeleteBuffersARB(5, m_VBOBuffers);
	if (m_attIndex.map() != NULL)
		static_cast<AttribMap*>(m_attIndex.map())->removeAttribute(m_attIndex);

	if (m_color_save!=NULL)
	{
		delete[] m_color_save;
	}
}


void topo3_VBORender::setDartWidth(float dw)
{
	m_topo_dart_width = dw;
}


void topo3_VBORender::setRelationWidth(float pw)
{
	m_topo_relation_width = pw;
}

void topo3_VBORender::setDartColor(Dart d, float r, float g, float b)
{
	float RGB[6];
	RGB[0]=r; RGB[1]=g; RGB[2]=b;
	RGB[3]=r; RGB[4]=g; RGB[5]=b;
	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[4]);
	glBufferSubData(GL_ARRAY_BUFFER, m_attIndex[d]*3*sizeof(float), 6*sizeof(float),RGB);
}

void topo3_VBORender::setAllDartsColor(float r, float g, float b)
{
	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[4]);
	GLvoid* ColorDartsBuffer = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);
	float* colorDartBuf = reinterpret_cast<float*>(ColorDartsBuffer);
	for (unsigned int i=0; i < 2*m_nbDarts; ++i)
	{
		*colorDartBuf++ = r;
		*colorDartBuf++ = g;
		*colorDartBuf++ = b;
	}

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[4]);
	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER);
}

void topo3_VBORender::drawDarts()
{
//	glColor3f(1.0f,1.0f,1.0f);
	glLineWidth(m_topo_dart_width);
	glPointSize(2.0f*m_topo_dart_width);

	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[4]);
	glColorPointer(3, GL_FLOAT, 0, 0);
	glEnableClientState(GL_COLOR_ARRAY);

	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[0]);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_LINES, 0, m_nbDarts*2);

 	glVertexPointer(3, GL_FLOAT, 6*sizeof(GL_FLOAT), 0);

	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[4]);
 	glColorPointer(3, GL_FLOAT, 6*sizeof(GL_FLOAT), 0);
 	glDrawArrays(GL_POINTS, 0, m_nbDarts)
 	;
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

}

void topo3_VBORender::overdrawDart(Dart d, float width, float r, float g, float b)
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


void topo3_VBORender::drawRelation1()
{
	glLineWidth(m_topo_relation_width);
	glColor3f(0.0f,1.0f,1.0f);
	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[1]);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_LINES, 0, m_nbDarts*2);
	glDisableClientState(GL_VERTEX_ARRAY);
}


void topo3_VBORender::drawRelation2()
{
	glLineWidth(m_topo_relation_width);
	glColor3f(1.0f,0.0f,0.0f);
	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[2]);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_QUADS, 0, m_nbRel2*4);
	glDisableClientState(GL_VERTEX_ARRAY);
}


void topo3_VBORender::drawRelation3()
{
	glLineWidth(m_topo_relation_width);
	glColor3f(1.0f,1.0f,0.0f);
	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[3]);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_QUADS, 0, m_nbRel3*4);
	glDisableClientState(GL_VERTEX_ARRAY);
}


void topo3_VBORender::drawTopo()
{
	drawDarts();
	drawRelation1();
	drawRelation2();
	drawRelation3();
}

void topo3_VBORender::pushColors()
{
	m_color_save = new float[6*m_nbDarts];
	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[4]);
	void* colorBuffer = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);

	memcpy(m_color_save, colorBuffer, 6*m_nbDarts*sizeof(float));
	glUnmapBuffer(GL_ARRAY_BUFFER);
}


void topo3_VBORender::popColors()
{
	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[4]);
	void* colorBuffer = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);

	memcpy(colorBuffer, m_color_save, 6*m_nbDarts*sizeof(float));
	glUnmapBuffer(GL_ARRAY_BUFFER);

	delete[] m_color_save;
	m_color_save=0;
}


Dart topo3_VBORender::colToDart(float* color)
{
	unsigned int r = (unsigned int)(color[0]*255.0f);
	unsigned int g = (unsigned int)(color[1]*255.0f);
	unsigned int b = (unsigned int)(color[2]*255.0f);

	unsigned int id = r + 255*g +255*255*b;

	if (id==0)
		return Dart::nil();
	return  Dart(id-1);

}

void topo3_VBORender::dartToCol(Dart d, float& r, float& g, float& b)
{
	unsigned int lab = d.index + 1; // add one to avoid picking the black of screen

	r = float(lab%255) / 255.0f; lab = lab/255;
	g = float(lab%255) / 255.0f; lab = lab/255;
	b = float(lab%255) / 255.0f; lab = lab/255;
	if (lab!=0)
		std::cerr << "Error picking color, too many darts"<< std::endl;
}


Dart topo3_VBORender::picking(unsigned int x, unsigned int y)
{
	//more easy picking for
	unsigned int dw = m_topo_dart_width;
	m_topo_dart_width+=2;

	// save clear color and set to zero
	float cc[4];
	glGetFloatv(GL_COLOR_CLEAR_VALUE,cc);

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_LIGHTING);
	// draw in back buffer (not shown on screen)
	drawDarts();

	// restore dart with
	m_topo_dart_width = dw;

	// read the pixel under the mouse in back buffer
	glReadBuffer(GL_BACK);
	float color[3];
	glReadPixels(x,y,1,1,GL_RGB,GL_FLOAT,color);

	glClearColor(cc[0], cc[1], cc[2], cc[3]);

	return colToDart(color);
}




}//end namespace VBO

}//end namespace Algo

}//end namespace Render

}//end namespace CGoGN
