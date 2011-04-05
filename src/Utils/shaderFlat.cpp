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


#include <string.h>
#include <GL/glew.h>
#include "Utils/shaderFlat.h"



namespace CGoGN
{
namespace Utils
{

std::string ShaderFlat::vertexShaderText = "\
#version 330 \n\
in vec3 VertexPosition;\n\
void main()\n\
{\n\
	gl_Position = vec4(VertexPosition, 1.0f);\n\
}";


std::string ShaderFlat::geometryShaderText = "\
#version 330\n\
layout (triangles) in;\n\
layout (triangle_strip, max_vertices = 3 ) out;\n\
uniform float explode;\n\
uniform mat4 ModelViewProjectionMatrix;\n\
uniform mat4 NormalMatrix;\n\
uniform mat4 ModelViewMatrix;\n\
uniform vec3 lightPosition;\n\
uniform vec4 materialDiffuse;\n\
uniform vec4 materialAmbient;\n\
flat out vec4 ColorFS;\n\
void main(void)\n\
{\n\
	vec3 v1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;\n\
	vec3 v2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;\n\
	vec3 N  = cross(v1,v2);\n\
	N  =  normalize (vec3(NormalMatrix*vec4(N,0.0))); \n\
	vec3 center = gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz + gl_in[2].gl_Position.xyz; \n\
	center /= 3.0;\n\
 	vec4 newPos =  ModelViewMatrix * vec4(center,0.0);\n\
	vec3 L =  normalize (lightPosition - newPos.xyz);\n\
	float lambertTerm = dot(N,L);\n\
	ColorFS = materialAmbient;\n\
	if(lambertTerm > 0.0)\n\
		ColorFS = materialDiffuse * lambertTerm;\n\
	int i;\n\
	for(i=0; i< gl_in.length(); i++)\n\
	{\n\
		vec3 displ = center - gl_in[i].gl_Position.xyz;\n\
		displ *= explode ;\n\
		gl_Position = gl_in[i].gl_Position + vec4(displ.xyz,0.0);\n\
		gl_Position = ModelViewProjectionMatrix *  gl_Position;\n\
		EmitVertex();\n\
	}\n\
	EndPrimitive();\n\
}";

std::string ShaderFlat::fragmentShaderText = "\
#version 330\n\
precision highp float;\n\
flat in vec4 ColorFS; \n\
out vec4 FragColor;\n\
void main()\n\
{\n\
	FragColor = ColorFS;\n\
}";


ShaderFlat::ShaderFlat()
{
	this->loadShadersFromMemory(vertexShaderText.c_str(), fragmentShaderText.c_str(),geometryShaderText.c_str(),GL_TRIANGLES, GL_TRIANGLE_STRIP);
	m_unif_ambiant   = glGetUniformLocation(this->program_handler(),"materialAmbient");
	m_unif_diffuse   = glGetUniformLocation(this->program_handler(),"materialDiffuse");
	m_unif_lightPos =  glGetUniformLocation(this->program_handler(),"lightPosition");

	//Default values
	Geom::Vec4f amb(0.05f,0.05f,0.1f,0.0f);
	Geom::Vec4f diff(0.1f,1.0f,0.1f,0.0f);
	Geom::Vec3f lp(10.0f,10.0f,1000.0f);

	setParams(0.9f, amb,diff,lp);

}

void ShaderFlat::setParams(float expl, const Geom::Vec4f& ambiant, const Geom::Vec4f& diffuse, const Geom::Vec3f& lightPos)
{
	this->bind();

	glUniform1f (m_unif_explode, expl);
	glUniform4fv(m_unif_ambiant,  1, ambiant.data());
	glUniform4fv(m_unif_diffuse,  1, diffuse.data());

	glUniform3fv(m_unif_lightPos, 1,lightPos.data());
}


}
}


