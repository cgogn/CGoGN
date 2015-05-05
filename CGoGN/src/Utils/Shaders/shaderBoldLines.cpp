/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2012, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/
#define CGoGN_UTILS_DLL_EXPORT 1
#include "Utils/Shaders/shaderBoldLines.h"

namespace CGoGN
{

namespace Utils
{

#include "shaderBoldLines.vert"
#include "shaderBoldLines.geom"
#include "shaderBoldLines.frag"


ShaderBoldLines::ShaderBoldLines() :
	m_lineWidth(0.01f),
	m_color	(0.0f, 0.0f, 0.0f, 0.0f),
	m_planeClip(0.0f,0.0f,0.0f,0.0f)
{
	m_nameVS = "ShaderBoldLines_vs";
	m_nameFS = "ShaderBoldLines_fs";
	m_nameGS = "ShaderBoldLines_gs";

	std::string glxvert(GLSLShader::defines_gl());
	glxvert.append(vertexShaderText);

	std::string glxgeom = GLSLShader::defines_Geom("lines", "triangle_strip", 6);
	glxgeom.append(geometryShaderText);

	std::string glxfrag(GLSLShader::defines_gl());
	glxfrag.append(fragmentShaderText);

	loadShadersFromMemory(glxvert.c_str(), glxfrag.c_str(), glxgeom.c_str(), GL_LINES, GL_TRIANGLE_STRIP,6);

	// get and fill uniforms
	getLocations();
	sendParams();
}

void ShaderBoldLines::getLocations()
{
	bind();
	*m_uniform_lineWidth = glGetUniformLocation(this->program_handler(), "lineWidth");
	*m_uniform_color = glGetUniformLocation(this->program_handler(), "lineColor");
	*m_unif_planeClip = glGetUniformLocation(this->program_handler(), "planeClip");
	unbind();
}

void ShaderBoldLines::sendParams()
{
	bind();
	glUniform2fv(*m_uniform_lineWidth, 1, m_lineWidth.data());
	glUniform4fv(*m_uniform_color, 1, m_color.data());

	if (*m_unif_planeClip > 0)
		glUniform4fv(*m_unif_planeClip, 1, m_planeClip.data());

	unbind();
}


void ShaderBoldLines::setLineWidth(float pix)
{
	glm::i32vec4 viewport;
	glGetIntegerv(GL_VIEWPORT, &(viewport[0]));
	m_pixWidth = pix;
	m_lineWidth[0] = float(double(m_pixWidth) / double(viewport[2]));
	m_lineWidth[1] = float(double(m_pixWidth) / double(viewport[3]));
	bind();
	glUniform2fv(*m_uniform_lineWidth, 1, m_lineWidth.data());
	unbind();
}

void ShaderBoldLines::updatePixelWidth()
{
	glm::i32vec4 viewport;
	glGetIntegerv(GL_VIEWPORT, &(viewport[0]));
	m_lineWidth[0] = float(double(m_pixWidth) / double(viewport[2]));
	m_lineWidth[1] = float(double(m_pixWidth) / double(viewport[3]));
	bind();
	glUniform2fv(*m_uniform_lineWidth, 1, m_lineWidth.data());
	unbind();
}



void ShaderBoldLines::setColor(const Geom::Vec4f& color)
{
	bind();
	glUniform4fv(*m_uniform_color, 1, color.data());
	m_color = color;
	unbind();
}

unsigned int ShaderBoldLines::setAttributePosition(VBO* vbo)
{
	m_vboPos = vbo;
	bind();
	unsigned int id = bindVA_VBO("VertexPosition", vbo);
	unbind();
	return id;
}


void ShaderBoldLines::restoreUniformsAttribs()
{
	getLocations();
	sendParams();

	bind();
	bindVA_VBO("VertexPosition", m_vboPos);
	unbind();
}


void ShaderBoldLines::setClippingPlane(const Geom::Vec4f& plane)
{
	if (*m_unif_planeClip > 0)
	{
		m_planeClip = plane;
		bind();
		glUniform4fv(*m_unif_planeClip, 1, plane.data());
		unbind();
	}
}



} // namespace Utils

} // namespace CGoGN
