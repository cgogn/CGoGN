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

#include <GL/glew.h>
#include "Utils/Shaders/shaderWallPaper.h"


namespace CGoGN
{

namespace Utils
{
#include "shaderWallPaper.vert"
#include "shaderWallPaper.frag"


ShaderWallPaper::ShaderWallPaper():
	m_tex_ptr(NULL)
{
	std::string glxvert(*GLSLShader::DEFINES_GL);
	glxvert.append(vertexShaderText);

	std::string glxfrag(*GLSLShader::DEFINES_GL);
	glxfrag.append(fragmentShaderText);

	loadShadersFromMemory(glxvert.c_str(), glxfrag.c_str());
	
	m_unif_unit = glGetUniformLocation(this->program_handler(), "textureUnit");

	m_vboPos = new Utils::VBO();
	m_vboPos->setDataSize(3);
	m_vboPos->allocate(4);
	Geom::Vec3f* ptrPos = reinterpret_cast<Geom::Vec3f*>(m_vboPos->lockPtr());

	ptrPos[0] = Geom::Vec3f(-1,-1, 0.9999999);
	ptrPos[1] = Geom::Vec3f( 1,-1, 0.9999999);
	ptrPos[2] = Geom::Vec3f( 1, 1, 0.9999999);
	ptrPos[3] = Geom::Vec3f(-1, 1, 0.9999999);

	m_vboPos->releasePtr();

	bindVA_VBO("VertexPosition", m_vboPos);

	m_vboTexCoord = new Utils::VBO();
	m_vboTexCoord->setDataSize(2);

	m_vboTexCoord = new Utils::VBO();
	m_vboTexCoord->setDataSize(2);
	m_vboTexCoord->allocate(4);
	Geom::Vec2f* ptrTex = reinterpret_cast<Geom::Vec2f*>(m_vboTexCoord->lockPtr());

	ptrTex[0] = Geom::Vec2f(0.0,0.0);
	ptrTex[1] = Geom::Vec2f(1.0,0.0);
	ptrTex[2] = Geom::Vec2f(1.0,1.0);
	ptrTex[3] = Geom::Vec2f(0.0,1.0);

	m_vboTexCoord->releasePtr();

	bindVA_VBO("VertexTexCoord", m_vboTexCoord);
}

ShaderWallPaper::~ShaderWallPaper()
{
	delete m_vboPos;
	delete m_vboTexCoord;
}

void ShaderWallPaper::setTextureUnit(GLenum texture_unit)
{
	this->bind();
	int unit = texture_unit - GL_TEXTURE0;
	glUniform1iARB(*m_unif_unit,unit);
	m_unit = unit;
}

void ShaderWallPaper::setTexture(Utils::GTexture* tex)
{
	m_tex_ptr = tex;
}

void ShaderWallPaper::activeTexture()
{
	glActiveTexture(GL_TEXTURE0 + m_unit);
	m_tex_ptr->bind();
}

void ShaderWallPaper::activeTexture(CGoGNGLuint texId)
{
	glActiveTexture(GL_TEXTURE0 + m_unit);
	glBindTexture(GL_TEXTURE_2D, *texId);
}

void ShaderWallPaper::restoreUniformsAttribs()
{
	m_unif_unit = glGetUniformLocation(this->program_handler(), "textureUnit");

	bindVA_VBO("VertexPosition", m_vboPos);
	bindVA_VBO("VertexTexCoord", m_vboTexCoord);
	
	this->bind();
	glUniform1iARB(*m_unif_unit,m_unit);
	this->unbind();
}

void ShaderWallPaper::draw()
{
	this->activeTexture();
	this->enableVertexAttribs();
	glDrawArrays(GL_QUADS, 0, 4);
	this->disableVertexAttribs();
}

} // namespace Utils

} // namespace CGoGN
