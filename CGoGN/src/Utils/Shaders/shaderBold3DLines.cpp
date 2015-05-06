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
#include "Utils/Shaders/shaderBold3DLines.h"

namespace CGoGN
{

namespace Utils
{

#include "shaderBold3DLines.vert"
#include "shaderBold3DLines.geom"
#include "shaderBold3DLines.frag"


ShaderBold3DLines::ShaderBold3DLines() :
	m_lineWidth(0.01f),
	m_color	(0.0f, 0.0f, 0.0f, 0.0f),
	m_planeClip(0.0f,0.0f,0.0f,0.0f)
{
	m_nameVS = "ShaderBold3DLines_vs";
	m_nameFS = "ShaderBold3DLines_fs";
	m_nameGS = "ShaderBold3DLines_gs";

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

void ShaderBold3DLines::getLocations()
{
	bind();
	*m_uniform_lineWidth = glGetUniformLocation(this->program_handler(), "lineWidth");
	*m_uniform_color = glGetUniformLocation(this->program_handler(), "lineColor");
	*m_unif_planeClip = glGetUniformLocation(this->program_handler(), "planeClip");
	unbind();
}

void ShaderBold3DLines::sendParams()
{
	bind();
	glUniform1f(*m_uniform_lineWidth, m_lineWidth);
	glUniform4fv(*m_uniform_color, 1, m_color.data());
	glUniform4fv(*m_unif_planeClip, 1, m_planeClip.data());
	unbind();
}


void ShaderBold3DLines::setLineWidth(float pix)
{
	glm::i32vec4 viewport;
	glGetIntegerv(GL_VIEWPORT, &(viewport[0]));
	m_lineWidth = pix;
	bind();
	glUniform1f(*m_uniform_lineWidth, m_lineWidth);
	unbind();
}



void ShaderBold3DLines::setColor(const Geom::Vec4f& color)
{
	bind();
	glUniform4fv(*m_uniform_color, 1, color.data());
	m_color = color;
	unbind();
}

unsigned int ShaderBold3DLines::setAttributePosition(VBO* vbo)
{
	m_vboPos = vbo;
	bind();
	unsigned int id = bindVA_VBO("VertexPosition", vbo);
	unbind();
	return id;
}


void ShaderBold3DLines::restoreUniformsAttribs()
{
	getLocations();
	sendParams();

	bind();
	bindVA_VBO("VertexPosition", m_vboPos);
	unbind();
}


void ShaderBold3DLines::setClippingPlane(const Geom::Vec4f& plane)
{
	if (*m_unif_planeClip > 0)
	{
		double Nn = sqrt(plane[0] * plane[0] + plane[1] * plane[1] + plane[2] * plane[2]);
		if ((fabs(Nn - 1.0) > 0.0000001) && (fabs(Nn)>0.000001))
			m_planeClip = plane / Nn;
		else
			m_planeClip = plane;
		bind();
		glUniform4fv(*m_unif_planeClip, 1, m_planeClip.data());
		unbind();
	}
}



} // namespace Utils

} // namespace CGoGN
