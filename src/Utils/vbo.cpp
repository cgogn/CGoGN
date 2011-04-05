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

#include "Utils/vbo.h"
#include "Utils/GLSLShader.h"

namespace CGoGN
{
namespace Utils
{


VBO::VBO()
{
	glGenBuffersARB(1,&m_id);
	m_refs.reserve(4);
}

VBO::~VBO()
{
	glDeleteBuffersARB(1,&m_id);
	for(std::vector<GLSLShader*>::iterator it = m_refs.begin(); it != m_refs.end(); ++it)
	{
		(*it)->unbindVBO(this);
	}
}

void VBO::ref(GLSLShader* sh)
{
	//already referenced ?
	for(std::vector<GLSLShader*>::iterator it = m_refs.begin(); it != m_refs.end(); ++it)
		if (*it == sh )
			return;
	// no then add
	m_refs.push_back(sh);
}


}
}

