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

#include <GL/glew.h>
#include "Utils/shaderColorPerVertex.h"

namespace CGoGN
{

namespace Utils
{

std::string ShaderColorPerVertex::vertexShaderText =
		"ATTRIBUTE vec3 VertexPosition;\n"
		"ATTRIBUTE vec3 VertexColor;\n"
		"uniform mat4 ModelViewProjectionMatrix;\n"
		"VARYING_VERT vec3 color;\n"
		"INVARIANT_POS;\n"
		"void main ()\n"
		"{\n"
		"	gl_Position = ModelViewProjectionMatrix * vec4 (VertexPosition, 1.0);\n"
		"	color = VertexColor;\n"
		"}";


std::string ShaderColorPerVertex::fragmentShaderText =
		"PRECISON;\n"
		"VARYING_FRAG vec3 color;\n"
		"FRAG_OUT_DEF;\n"
		"void main()\n"
		"{\n"
		"	gl_FragColor=vec4(color,0.0);\n"
		"}";


ShaderColorPerVertex::ShaderColorPerVertex()
{
	std::string glxvert(*GLSLShader::DEFINES_GL);
	glxvert.append(vertexShaderText);

	std::string glxfrag(*GLSLShader::DEFINES_GL);
	glxfrag.append(fragmentShaderText);

	loadShadersFromMemory(glxvert.c_str(), glxfrag.c_str());
}

unsigned int ShaderColorPerVertex::setAttributePosition(VBO* vbo)
{
	m_vboPos = vbo;
	return bindVA_VBO("VertexPosition", vbo);
}

unsigned int ShaderColorPerVertex::setAttributeColor(VBO* vbo)
{
	m_vboCol = vbo;
	return bindVA_VBO("VertexColor", vbo);
}

void ShaderColorPerVertex::restoreUniformsAttribs()
{
	bindVA_VBO("VertexPosition", m_vboPos);
	bindVA_VBO("VertexColor", m_vboCol);
}

} // namespace Utils

} // namespace CGoGN
