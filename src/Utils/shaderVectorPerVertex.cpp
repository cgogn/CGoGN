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
#include "Utils/shaderVectorPerVertex.h"

namespace CGoGN
{

namespace Utils
{

std::string ShaderVectorPerVertex::vertexShaderText =
		"ATTRIBUTE vec3 VertexPosition;\n"
		"ATTRIBUTE vec3 VertexVector;\n"
		"VARYING_VERT vec3 VectorAttrib;\n"
		"INVARIANT_POS;\n"
		"void main ()\n"
		"{\n"
		"	VectorAttrib = VertexVector;\n"
		"	gl_Position = vec4(VertexPosition, 1.0);\n"
		"}";


std::string ShaderVectorPerVertex::geometryShaderText =
		"uniform float vectorScale;\n"
		"uniform mat4 ModelViewProjectionMatrix;\n"
		"VARYING_IN vec3 VectorAttrib[];\n"
		"void main()\n"
		"{\n"
		"	gl_Position = ModelViewProjectionMatrix * POSITION_IN(0);\n"
		"	EmitVertex();\n"
		"	gl_Position = ModelViewProjectionMatrix * (POSITION_IN(0) + vec4(VectorAttrib[0] * vectorScale, 0.0));\n"
		"	EmitVertex();\n"
		"	EndPrimitive();\n"
		"}";


std::string ShaderVectorPerVertex::fragmentShaderText =
		"PRECISON;\n"
		"uniform vec4 vectorColor;\n"
		"FRAG_OUT_DEF;\n"
		"void main()\n"
		"{\n"
		"	gl_FragColor = vectorColor;\n"
		"}";


ShaderVectorPerVertex::ShaderVectorPerVertex() :
	m_scale(1.0f),
	m_color(Geom::Vec4f(1.0f, 0.0f, 0.0f, 0.0f))
{
	std::string glxvert(*GLSLShader::DEFINES_GL);
	glxvert.append(vertexShaderText);

	std::string glxgeom = GLSLShader::defines_Geom("points", "line_strip", 4);
	glxgeom.append(geometryShaderText);

	std::string glxfrag(*GLSLShader::DEFINES_GL);
	glxfrag.append(fragmentShaderText);

	loadShadersFromMemory(glxvert.c_str(), glxfrag.c_str(), glxgeom.c_str(), GL_POINTS, GL_LINE_STRIP);

	// get and fill uniforms
	bind();
	getLocations();
	sendParams();
}

void ShaderVectorPerVertex::getLocations()
{
	m_uniform_scale = glGetUniformLocation(this->program_handler(), "vectorScale");
	m_uniform_color = glGetUniformLocation(this->program_handler(), "vectorColor");
}

void ShaderVectorPerVertex::sendParams()
{
	glUniform1f(m_uniform_scale, m_scale);
	glUniform4fv(m_uniform_color, 1, m_color.data());
}

void ShaderVectorPerVertex::setScale(float scale)
{
	bind();
	glUniform1f(m_uniform_scale, scale);
	m_scale = scale;
}

void ShaderVectorPerVertex::setColor(const Geom::Vec4f& color)
{
	bind();
	glUniform4fv(m_uniform_color, 1, color.data());
	m_color = color;
}

unsigned int ShaderVectorPerVertex::setAttributePosition(VBO* vbo)
{
	m_vboPos = vbo;
	return bindVA_VBO("VertexPosition", vbo);
}

unsigned int ShaderVectorPerVertex::setAttributeVector(VBO* vbo)
{
	m_vboVec = vbo;
	return bindVA_VBO("VertexVector", vbo);
}

void ShaderVectorPerVertex::restoreUniformsAttribs()
{
	getLocations();
	sendParams();

	bindVA_VBO("VertexPosition", m_vboPos);
	bindVA_VBO("VertexVector", m_vboVec);
}

} // namespace Utils

} // namespace CGoGN
