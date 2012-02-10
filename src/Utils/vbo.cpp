/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2011, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.u-strasbg.fr/                                         *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include "Utils/vbo.h"
#include "Utils/GLSLShader.h"
#include <stdio.h>
#include <string.h>

namespace CGoGN
{

namespace Utils
{

VBO::VBO() : m_nbElts(0), m_lock(false)
{
	glGenBuffers(1, &m_id);
	m_refs.reserve(4);
}

VBO::VBO(const VBO& vbo) :
	m_data_size(vbo.m_data_size),
	m_nbElts(vbo.m_nbElts),
	m_lock(false)
{
	unsigned int nbbytes =  sizeof(float) * m_data_size * m_nbElts;

	glGenBuffers(1, &m_id);

	vbo.bind();
	void* src = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

	bind();
	glBufferData(GL_ARRAY_BUFFER, nbbytes, src, GL_STREAM_DRAW);

	vbo.bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

VBO::~VBO()
{
	if (m_lock)
		releasePtr();
	glDeleteBuffers(1, &m_id);
}



void VBO::sameAllocSameBufferSize(const VBO& vbo)
{
	m_data_size = vbo.m_data_size;
	m_nbElts = vbo.m_nbElts;
	unsigned int nbbytes =  sizeof(float) * m_data_size * m_nbElts;
	bind();
	glBufferData(GL_ARRAY_BUFFER, nbbytes, NULL, GL_STREAM_DRAW);
}

void* VBO::lockPtr()
{
	if (m_lock)
	{
		CGoGNerr <<" Error already locked VBO"<< CGoGNendl;
		return NULL;
	}

	m_lock = true;
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
	return glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
}

const void* VBO::lockPtr() const
{
	if (m_lock)
	{
		CGoGNerr <<" Error already locked VBO"<< CGoGNendl;
		return NULL;
	}

	m_lock = true;
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
	return glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
}

void VBO::releasePtr() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	m_lock = false;
}

void VBO::copyData(void *ptr) const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
	glGetBufferSubData(GL_ARRAY_BUFFER,0,m_nbElts*m_data_size*sizeof(float), ptr);
}

void VBO::allocate(unsigned int nbElts)
{
	m_nbElts = nbElts;
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
	glBufferData(GL_ARRAY_BUFFER, nbElts * m_data_size*sizeof(float), 0, GL_STREAM_DRAW);
}


} // namespace Utils

} // namespace CGoGN
