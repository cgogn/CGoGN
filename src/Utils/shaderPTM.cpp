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

#include "Utils/shaderPTM.h"

namespace CGoGN
{

namespace Utils
{

std::string ShaderPTM::vertexShaderText =
"ATTRIBUTE vec3 VertexPosition;\n"
"ATTRIBUTE vec3 Fi,Fn;\n"
"ATTRIBUTE vec3 a0,a3,a6,a9,a12;\n"
"uniform mat4 ModelViewProjectionMatrix;\n"
"uniform mat4 ModelViewMatrix;\n"
//"uniform mat4 NormalMatrix;\n"
"uniform vec3 cameraPosition;\n"
"uniform int projectionType;\n"
"VARYING_VERT vec3 vcolour;\n"
"INVARIANT_POS;\n"
"void main ()\n"
"{\n"
"		vec3 Fj = cross(Fn, Fi) ;\n"
"\n"
"		vec3 eyeV = cameraPosition - VertexPosition;\n"
"		eyeV = normalize(eyeV) ;\n"
"		\n"
"		if (dot(eyeV,Fn) < 0.0)\n"
"			vcolour = vec3(0.0,0.0,0.0) ;\n"
"		else {\n"
"			float u = dot(eyeV,Fi) ;\n"
"			float v = dot(eyeV,Fj) ;\n"
"\n"
"			/** orthographic --> nothing to add **/\n"
"			/** authalic **/\n"
"			if (projectionType == 1) {\n"
"				float k = sqrt(2.0 / (dot(eyeV,Fn) + 1.0) ) ;\n"
"				u *= k ;\n"
"				v *= k ;\n"
"			}\n"
"			/** conform **/\n"
"			if (projectionType == 2) {\n"
"				float k = dot(eyeV,Fn) + 1.0 ;\n"
"				u /= k ;\n"
"				v /= k ;\n"
"			}\n"
"\n"
"			/** Function evaluation **/\n"
"			float t = a0.x + a0.y*v + a0.z*u + a3.x*u*v + a3.y*v*v + a3.z*u*u + a6.x*u*v*v + a6.y*u*u*v + a6.z*v*v*v + a9.x*u*u*u + a9.y*u*u*v*v + a9.z*u*v*v*v + a12.x*u*u*u*v + a12.y*v*v*v*v + a12.z*u*u*u*u ;\n"
"			vcolour = vec3(t,t,t) ;\n"
"			\n"
"			/*vcolour = vec3 (\n"
"				u*u*u*a9.x + v*v*v*a8.x + u*u*v*a7.x + u*v*v*a6.x + u*u*a5.x + v*v*a4.x + u*v*a3.x + u*a2.x + v*a1.x + a0.x,\n"
"				u*u*u*a9.y + v*v*v*a8.y + u*u*v*a7.y + u*v*v*a6.y + u*u*a5.y + v*v*a4.y + u*v*a3.y + u*a2.y + v*a1.y + a0.y,\n"
"				u*u*u*a9.z + v*v*v*a8.z + u*u*v*a7.z + u*v*v*a6.z + u*u*a5.z + v*v*a4.z + u*v*a3.z + u*a2.z + v*a1.z + a0.z) ;*/\n"
"			}\n"

" 		vec3 Position = vec3 (ModelViewMatrix * vec4 (VertexPosition, 1.0));\n"
"		gl_Position = ModelViewProjectionMatrix * vec4 (VertexPosition, 1.0);\n"
"}";


std::string ShaderPTM::fragmentShaderText =
"PRECISON;\n"
"VARYING_FRAG vec3 vcolour;\n"
"FRAG_OUT_DEF;\n"
"void main (void)\n"
"{\n"
"	gl_FragColor = vec4(vcolour,1.0) ;\n"
"}";

ShaderPTM::ShaderPTM():
	m_cameraPos(Geom::Vec3f(10.0f,10.0f,1000.0f)),
	m_proj(0)
{
	// get choose GL defines (2 or 3)
	// and compile shaders
	std::string glxvert(*GLSLShader::DEFINES_GL);
	glxvert.append(vertexShaderText);
	std::string glxfrag(*GLSLShader::DEFINES_GL);
	glxfrag.append(fragmentShaderText);

	loadShadersFromMemory(glxvert.c_str(), glxfrag.c_str());

	// and get and fill uniforms
	bind();
	getLocations();
	sendParams();
}

void ShaderPTM::getLocations()
{
	m_unif_cameraPos = glGetUniformLocation(this->program_handler(), "cameraPosition");
	m_unif_proj = glGetUniformLocation(this->program_handler(), "projectionType");
}

void ShaderPTM::sendParams()
{
	glUniform3fv(m_unif_cameraPos,  1, m_cameraPos.data());
	glUniform1i(m_unif_proj, m_proj);
}

void ShaderPTM::setCameraPos(const Geom::Vec3f& camera)
{
	this->bind();
	glUniform3fv(m_unif_cameraPos,1, camera.data());
	m_cameraPos = camera ;
}

void ShaderPTM::setProjectionType(const unsigned int& proj)
{
	this->bind() ;
	glUniform1i (m_unif_proj, proj) ;
	m_proj = proj ;
}

void ShaderPTM::setParams(const Geom::Vec3f& camera, const unsigned int& proj)
{
	m_cameraPos = camera ;
	m_proj = proj ;
	bind();
	sendParams();
}

void ShaderPTM::restoreUniformsAttribs()
{
	getLocations();
	sendParams();

//	TODO
//	bindVA_VBO("cameraPosition", m_cameraPos);
//	bindVA_VBO("projectionType", m_proj);
}

unsigned int ShaderPTM::setAttributePosition(VBO* vbo)
{
	m_vboPos = vbo;
	return bindVA_VBO("VertexPosition", vbo);
}

unsigned int ShaderPTM::setAttributeFi(VBO* vbo)
{
	m_vboPos = vbo;
	return bindVA_VBO("Fi", vbo);
}

unsigned int ShaderPTM::setAttributeFn(VBO* vbo)
{
	m_vboPos = vbo;
	return bindVA_VBO("Fn", vbo);
}

unsigned int ShaderPTM::setAttributeA0(VBO* vbo)
{
	m_vboPos = vbo;
	return bindVA_VBO("a0", vbo);
}

unsigned int ShaderPTM::setAttributeA3(VBO* vbo)
{
	m_vboPos = vbo;
	return bindVA_VBO("a3", vbo);
}

unsigned int ShaderPTM::setAttributeA6(VBO* vbo)
{
	m_vboPos = vbo;
	return bindVA_VBO("a6", vbo);
}

unsigned int ShaderPTM::setAttributeA9(VBO* vbo)
{
	m_vboPos = vbo;
	return bindVA_VBO("a9", vbo);
}

unsigned int ShaderPTM::setAttributeA12(VBO* vbo)
{
	m_vboPos = vbo;
	return bindVA_VBO("a12", vbo);
}

} // namespace Utils

} // namespace CGoGN
