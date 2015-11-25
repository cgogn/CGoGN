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

#include "Utils/Shaders/shaderRadiancePerVertex_P.h"

namespace CGoGN
{

namespace Utils
{

#include "shaderRadiancePerVertex_P.vert"
#include "shaderRadiancePerVertex_P.frag"

ShaderRadiancePerVertex_P::ShaderRadiancePerVertex_P() :
	m_vboPos(NULL),
	m_vboTang(NULL),
	m_vboNorm(NULL),
	m_vboBiNorm(NULL),
	m_vboParam(NULL),
	m_tex_ptr(NULL),
	m_tex_unit(-1)
{
	compile();
}

ShaderRadiancePerVertex_P::~ShaderRadiancePerVertex_P()
{
}

void ShaderRadiancePerVertex_P::compile()
{
	m_nameVS = "ShaderRadiancePerVertex_P_vs";
	m_nameFS = "ShaderRadiancePerVertex_P_fs";
	m_nameGS = "ShaderRadiancePerVertex_P_gs";

	const int nb_coefs = 15;

	std::stringstream s ;
	s << "#define NB_COEFS " << nb_coefs << std::endl ;

	std::string glxvert(GLSLShader::defines_gl());
	glxvert.append(s.str()) ;
	glxvert.append(vertexShaderText);

	std::string glxfrag(GLSLShader::defines_gl());
	glxfrag.append(fragmentShaderText) ;

	loadShadersFromMemory(glxvert.c_str(), glxfrag.c_str());

	bind();
	*m_uniform_cam = glGetUniformLocation(this->program_handler(), "camera");
	*m_uniform_tex = glGetUniformLocation(this->program_handler(), "texture");
	unbind();
}

void ShaderRadiancePerVertex_P::setCamera(const Geom::Vec3f& camera)
{
	m_camera = camera;
	bind();
	glUniform3fv(*m_uniform_cam, 1, m_camera.data()) ;
	unbind();
}

unsigned int ShaderRadiancePerVertex_P::setAttributePosition(VBO* vbo)
{
	m_vboPos = vbo;
	bind();
	unsigned int id = bindVA_VBO("VertexPosition", vbo);
	unbind();
	return id;
}

unsigned int ShaderRadiancePerVertex_P::setAttributeTangent(VBO* vbo)
{
	m_vboTang = vbo;
	bind();
	unsigned int id = bindVA_VBO("VertexTangent", vbo);
	unbind();
	return id;
}

unsigned int ShaderRadiancePerVertex_P::setAttributeNormal(VBO* vbo)
{
	m_vboNorm = vbo;
	bind();
	unsigned int id = bindVA_VBO("VertexNormal", vbo);
	unbind();
	return id;
}

unsigned int ShaderRadiancePerVertex_P::setAttributeBiNormal(VBO* vbo)
{
	m_vboBiNorm = vbo;
	bind();
	unsigned int id = bindVA_VBO("VertexBiNormal", vbo);
	unbind();
	return id;
}

unsigned int ShaderRadiancePerVertex_P::setAttributeRadiance(VBO* vbo, Utils::Texture<2, Geom::Vec3f>* texture, GLenum tex_unit)
{
	m_vboParam = vbo;
	m_tex_ptr = texture;
	m_tex_unit = tex_unit - GL_TEXTURE0;

	bind();
	unsigned int id = bindVA_VBO("VertexParam", vbo);
	glUniform1iARB(*m_uniform_tex, m_tex_unit);
	glActiveTexture(tex_unit);
	if (m_tex_ptr)
		m_tex_ptr->bind() ;
	unbind();

	return id;
}

} // namespace Utils

} // namespace CGoGN
