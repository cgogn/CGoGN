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

#include <string.h>
#include <GL/glew.h>
#include "Utils/Shaders/shaderExplodeVolumes.h"

namespace CGoGN
{

namespace Utils
{
#include "shaderExplodeVolumes.vert"
#include "shaderExplodeVolumes.frag"
#include "shaderExplodeVolumes.geom"


ShaderExplodeVolumes::ShaderExplodeVolumes()
{
	m_nameVS = "ShaderExplodeVolumes_vs";
	m_nameFS = "ShaderExplodeVolumes_fs";
	m_nameGS = "ShaderExplodeVolumes_gs";

	std::string glxvert(*GLSLShader::DEFINES_GL);
	glxvert.append(vertexShaderText);

	std::string glxgeom(GLSLShader::defines_Geom("quads", "triangle_strip", 4));
	glxgeom.append(geometryShaderText);

	std::string glxfrag(*GLSLShader::DEFINES_GL);
	glxfrag.append(fragmentShaderText);

//	std::cout << "----------------------------------------------------------" << std::endl;
//	std::cout << glxvert << std::endl;
//	std::cout << "----------------------------------------------------------" << std::endl;
//	std::cout << glxfrag << std::endl;
//	std::cout << "----------------------------------------------------------" << std::endl;
//	std::cout << glxgeom << std::endl;
//	std::cout << "----------------------------------------------------------" << std::endl;

	loadShadersFromMemory(glxvert.c_str(), glxfrag.c_str(), glxgeom.c_str(), GL_LINES_ADJACENCY_EXT , GL_TRIANGLE_STRIP,4);

	getLocations();

	//Default values
	m_explodeV = 0.9f;
	m_ambiant = Geom::Vec4f(0.05f, 0.05f, 0.1f, 0.0f);
	m_diffuse = Geom::Vec4f(0.1f, 1.0f, 0.1f, 0.0f);
	m_light_pos = Geom::Vec3f(10.0f, 10.0f, 1000.0f);

	setParams(m_explodeV, m_ambiant, m_diffuse, m_light_pos);
}

void ShaderExplodeVolumes::getLocations()
{
	m_unif_explodeV  = glGetUniformLocation(program_handler(),"explodeV");
	m_unif_ambiant  = glGetUniformLocation(program_handler(),"ambient");
	m_unif_diffuse  = glGetUniformLocation(program_handler(),"diffuse");
	m_unif_lightPos = glGetUniformLocation(program_handler(),"lightPosition");
}

void ShaderExplodeVolumes::setAttributePosition(VBO* vbo)
{
	m_vboPos = vbo;
	bindVA_VBO("VertexPosition", vbo);
}

void ShaderExplodeVolumes::setParams(float explV, const Geom::Vec4f& ambiant, const Geom::Vec4f& diffuse, const Geom::Vec3f& lightPos)
{
	m_explodeV = explV;
	m_ambiant = ambiant;
	m_diffuse = diffuse;
	m_light_pos = lightPos;

	bind();

	glUniform1f(m_unif_explodeV, explV);
	glUniform4fv(m_unif_ambiant, 1, ambiant.data());
	glUniform4fv(m_unif_diffuse, 1, diffuse.data());
	glUniform3fv(m_unif_lightPos, 1, lightPos.data());

	unbind(); // ??
}

void ShaderExplodeVolumes::setExplodeVolumes(float explode)
{
	m_explodeV = explode;
	bind();
	glUniform1f(m_unif_explodeV, explode);
}


void ShaderExplodeVolumes::setAmbiant(const Geom::Vec4f& ambiant)
{
	m_ambiant = ambiant;
	bind();
	glUniform4fv(m_unif_ambiant,1, ambiant.data());
}

void ShaderExplodeVolumes::setDiffuse(const Geom::Vec4f& diffuse)
{
	m_diffuse = diffuse;
	bind();
	glUniform4fv(m_unif_diffuse,1, diffuse.data());
}

void ShaderExplodeVolumes::setLightPosition(const Geom::Vec3f& lp)
{
	m_light_pos = lp;
	bind();
	glUniform3fv(m_unif_lightPos,1,lp.data());
}

void ShaderExplodeVolumes::restoreUniformsAttribs()
{
	m_unif_explodeV   = glGetUniformLocation(program_handler(),"explodeV");
	m_unif_ambiant   = glGetUniformLocation(program_handler(),"ambient");
	m_unif_diffuse   = glGetUniformLocation(program_handler(),"diffuse");
	m_unif_lightPos =  glGetUniformLocation(program_handler(),"lightPosition");

	bind();
	glUniform1f (m_unif_explodeV, m_explodeV);
	glUniform4fv(m_unif_ambiant,  1, m_ambiant.data());
	glUniform4fv(m_unif_diffuse,  1, m_diffuse.data());
	glUniform3fv(m_unif_lightPos, 1, m_light_pos.data());

	bindVA_VBO("VertexPosition", m_vboPos);
	unbind();
}

} // namespace Utils

} // namespace CGoGN
