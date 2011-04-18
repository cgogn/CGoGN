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

#include "Utils/drawer.h"
#include "Utils/shaderColorPerVertex.h"
#include "Utils/vbo.h"

namespace CGoGN
{
namespace Utils
{



Drawer::Drawer():
		m_currentWidth(1.0f)
{
	m_vboPos = new Utils::VBO();
	m_vboPos->setDataSize(3);

	m_vboCol = new Utils::VBO();
	m_vboCol->setDataSize(3);

	m_shader = new Utils::ShaderColorPerVertex();

	m_shader->setAttributePosition(m_vboPos);
	m_shader->setAttributeColor(m_vboCol);

	Utils::GLSLShader::registerRunning(m_shader);

	m_dataPos.reserve(128);
	m_dataCol.reserve(128);
	m_begins.reserve(16);
//	m_modes.reserve(16);
}

Drawer::~Drawer()
{
	Utils::GLSLShader::unregisterRunning(m_shader);
	delete m_shader;
	delete m_vboPos;
	delete m_vboCol;
}


void Drawer::lineWidth(float lw)
{
	m_currentWidth=lw;
}

void Drawer::pointSize(float ps)
{
	m_currentWidth = ps;
}

void Drawer::begin(GLenum mode)
{
	m_begins.push_back(PrimParam(m_dataPos.size(),mode, m_currentWidth));
}

void Drawer::end()
{
	m_begins.back().nb = m_dataPos.size() - m_begins.back().begin;
}


void Drawer::color(const Geom::Vec3f& col)
{
	if (m_dataPos.size() == m_dataCol.size())
		m_dataCol.push_back(col);
	else
		m_dataCol.back() = col;
}

void Drawer::color3f(float r, float g, float b)
{
	color(Geom::Vec3f(r,g,b));
}

void Drawer::vertex(const Geom::Vec3f& v)
{
	if (m_dataPos.size() == m_dataCol.size())
	{
		if (m_dataCol.empty())
			m_dataCol.push_back(Geom::Vec3f(1.,1.,1.));
		else
			m_dataCol.push_back( m_dataCol.back());
	}

	m_dataPos.push_back(v);
}

void Drawer::vertex3f(float r, float g, float b)
{
	vertex(Geom::Vec3f(r,g,b));
}


void Drawer::newList(GLenum comp)
{
	m_compile = comp;
	m_dataPos.clear();
	m_dataCol.clear();
	m_begins.clear();
}



void Drawer::endList()
{
	unsigned int nbElts = m_dataPos.size();

	m_vboPos->bind();
	glBufferData(GL_ARRAY_BUFFER, nbElts*sizeof(Geom::Vec3f), &(m_dataPos[0]), GL_STREAM_DRAW);

	m_vboCol->bind();
	glBufferData(GL_ARRAY_BUFFER, nbElts*sizeof(Geom::Vec3f), &(m_dataCol[0]), GL_STREAM_DRAW);

	// free memory
	std::vector<Geom::Vec3f> tempo;
	tempo.swap(m_dataPos);
	std::vector<Geom::Vec3f> tempo2;
	tempo2.swap(m_dataCol);

	if (m_compile != GL_COMPILE)
		callList();
}


void Drawer::callList()
{
	if (m_begins.empty())
		return;

	//draw
	m_shader->enableVertexAttribs();
	for (std::vector<PrimParam>::iterator pp = m_begins.begin(); pp != m_begins.end(); ++pp)
	{
		if (pp->mode == GL_POINTS)
			glPointSize(pp->width);
		if ((pp->mode == GL_LINES) || (pp->mode == GL_LINE_LOOP))
			glLineWidth(pp->width);
		glDrawArrays(pp->mode, pp->begin, pp->nb);
	}
 	m_shader->disableVertexAttribs();
}


}
}


