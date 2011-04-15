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

#include "Utils/pointLine.h"

namespace CGoGN
{
namespace Utils
{


std::string PointLine::vertexShaderText =
"ATTRIBUTE vec3 VertexPosition;\n"
"ATTRIBUTE vec3 Data\n;"
"VARYING_VERT vec3 DataVector;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(VertexPosition,1.0);\n"
"	DataVector = Data;\n"
"}";



std::string PointLine::geometryShaderText =
"uniform float scale;\n"
"uniform mat4 ModelViewProjectionMatrix;\n"
"VARYING_IN vec3 DataVector[];\n"
"void main()\n"
"{\n"
"	gl_Position = ModelViewProjectionMatrix *  POSITION_IN(0);\n"
"	EmitVertex();\n"
"	vec4 P2 = POSITION_IN(0)+vec4(scale*DataVector[0],0.0);\n"
"	gl_Position = ModelViewProjectionMatrix * P2;\n"
"	EmitVertex();\n"
"	EndPrimitive();\n"
"}";


std::string PointLine::fragmentShaderText =
"uniform vec3 color;\n"
"void main(void)\n"
"{\n"
"	gl_FragColor = vec4(color,0.0);\n"
"}";



PointLine::PointLine(float scale, const Geom::Vec3f& color)
{
	std::string glxvert(*GLSLShader::DEFINES_GL);
	glxvert.append(vertexShaderText);

	std::string glxgeom = GLSLShader::defines_Geom("points","line_strip",4);
	glxgeom.append(geometryShaderText);

	std::string glxfrag(*GLSLShader::DEFINES_GL);
	glxfrag.append(fragmentShaderText);

	loadShadersFromMemory(glxvert.c_str(), glxfrag.c_str(), glxgeom.c_str(), GL_POINTS, GL_LINE_STRIP);

	bind();
	m_uniform_scale = glGetUniformLocation(program_handler(),"scale");
	m_uniform_color = glGetUniformLocation(program_handler(),"color");
	glUniform1f(m_uniform_scale, scale);
	glUniform3fv(m_uniform_color, 1, color.data());
	unbind();

}


unsigned int PointLine::setAttributePosition(VBO* vbo)
{
	return bindVA_VBO("VertexPosition", vbo);
}


unsigned int PointLine::setAttributeData(VBO* vbo)
{
	return bindVA_VBO("Data", vbo);
}



void PointLine::setScale(float scale)
{
	bind();
	glUniform1f(m_uniform_scale, scale);
	unbind();
}

void PointLine::setColor(const Geom::Vec3f& color)
{
	bind();
	glUniform3fv(m_uniform_color, 1, color.data());
	unbind();
}

}
}
