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
#ifdef WITH_QT

#include <GL/glew.h>
#include "Utils/Shaders/shaderTextureMask.h"


namespace CGoGN
{

namespace Utils
{

#include "shaderTextureMask.vert"
#include "shaderTextureMask.frag"

//std::string ShaderTextureMask::vertexShaderText =
//		"ATTRIBUTE vec3 VertexPosition;\n"
//		"ATTRIBUTE vec2 VertexTexCoord;\n"
//		"uniform mat4 ModelViewProjectionMatrix;\n"
//		"VARYING_VERT vec2 texCoord;\n"
//		"INVARIANT_POS;\n"
//		"void main ()\n"
//		"{\n"
//		"	gl_Position = ModelViewProjectionMatrix * vec4 (VertexPosition, 1.0);\n"
//		"	texCoord = VertexTexCoord;\n"
//		"}";
//
//
//std::string ShaderTextureMask::fragmentShaderText =
//		"PRECISON;\n"
//		"VARYING_FRAG vec2 texCoord;\n"
//		"uniform sampler2D textureUnit;\n"
//		"uniform sampler2D textureUnitMask;\n"
//		"FRAG_OUT_DEF;\n"
//		"void main()\n"
//		"{\n"
//		"	float m = texture2D(textureUnitMask,texCoord).r;\n"
//		"	if (m < 0.5)"
//		"		discard;"
//		"	gl_FragColor=texture2D(textureUnit,texCoord)*m;\n"
//		"}";


ShaderTextureMask::ShaderTextureMask()
{
	std::string glxvert(*GLSLShader::DEFINES_GL);
	glxvert.append(vertexShaderText);

	std::string glxfrag(*GLSLShader::DEFINES_GL);
	glxfrag.append(fragmentShaderText);

	loadShadersFromMemory(glxvert.c_str(), glxfrag.c_str());

	m_unif_unit = glGetUniformLocation(this->program_handler(), "textureUnit");
	m_unif_unitMask = glGetUniformLocation(this->program_handler(), "textureUnitMask");
}

void ShaderTextureMask::setTextureUnits(GLenum texture_unit, GLenum texture_unitMask)
{
	this->bind();
	int unit = texture_unit - GL_TEXTURE0;
	glUniform1iARB(m_unif_unit,unit);
	m_unit = unit;
	unit = texture_unitMask - GL_TEXTURE0;
	glUniform1iARB(m_unif_unitMask,unit);
	m_unitMask = unit;
}

void ShaderTextureMask::setTextures(Utils::GTexture* tex, Utils::GTexture* texMask)
{
	m_tex_ptr = tex;
	m_texMask_ptr = texMask;
}

void ShaderTextureMask::activeTextures()
{
	glActiveTexture(GL_TEXTURE0+m_unit);
	m_tex_ptr->bind();
	glActiveTexture(GL_TEXTURE0+m_unitMask);
	m_texMask_ptr->bind();
}



unsigned int ShaderTextureMask::setAttributePosition(VBO* vbo)
{
	m_vboPos = vbo;
	return bindVA_VBO("VertexPosition", vbo);
}

unsigned int ShaderTextureMask::setAttributeTexCoord(VBO* vbo)
{
	m_vboTexCoord = vbo;
	return bindVA_VBO("VertexTexCoord", vbo);
}

void ShaderTextureMask::restoreUniformsAttribs()
{
	bindVA_VBO("VertexPosition", m_vboPos);
	bindVA_VBO("VertexTexCoord", m_vboTexCoord);
	glUniform1iARB(m_unif_unit,m_unit);
	glUniform1iARB(m_unif_unitMask,m_unitMask);
}

} // namespace Utils

} // namespace CGoGN

#else
#pragma message(__FILE__ " not compiled because of mising Qt")
#endif
