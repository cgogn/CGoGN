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

#include "Utils/pointSprite.h"

namespace CGoGN
{

namespace Utils
{

#include "pointSprite.vert"
#include "pointSprite.frag"
#include "pointSprite.geom"

GLuint PointSprite::m_idTexture = 0;
GLuint PointSprite::m_uniform_texture = 0;

unsigned char* PointSprite::m_ptrSphere = NULL;


//std::string PointSprite::vertexShaderText =
//"ATTRIBUTE vec3 VertexPosition;\n"
//"void main ()\n"
//"{\n"
//"	gl_Position = vec4(VertexPosition,1.0);\n"
//"}";


//std::string PointSprite::geometryShaderText =
//"uniform float size;\n"
//"uniform mat4 ModelViewMatrix;\n"
//"uniform mat4 ProjectionMatrix;\n"
//"VARYING_OUT vec2 texCoord;\n"
//"VARYING_OUT vec2 positionFragIn;\n"
//"VARYING_OUT vec4 mvpFragIn;\n"
//"void main()\n"
//"{\n"
//"	vec4 posCenter = ModelViewMatrix * POSITION_IN(0);\n"
//"	vec4 pos = posCenter + vec4(-size, size, 0.0, 0.0);\n"
//"	positionFragIn = posCenter.zw;\n"
//"	mvpFragIn.x = ProjectionMatrix[2][2];\n"
//"	mvpFragIn.y = ProjectionMatrix[3][2];\n"
//"	mvpFragIn.z = ProjectionMatrix[2][3];\n"
//"	mvpFragIn.w = ProjectionMatrix[3][3];\n"
//"	texCoord = vec2(0.0,1.0);\n"
//"	gl_Position = ProjectionMatrix *  pos;\n"
//"	EmitVertex();\n"
//"	pos = posCenter + vec4(-size, -size, 0.0, 0.0);\n"
//"	texCoord = vec2(0.0,0.0);\n"
//"	gl_Position = ProjectionMatrix *  pos;\n"
//"	EmitVertex();\n"
//"	pos = posCenter + vec4( size, size, 0.0, 0.0);\n"
//"	texCoord = vec2(1.0,1.0);\n"
//"	gl_Position = ProjectionMatrix *  pos;\n"
//"	EmitVertex();\n"
//"	pos = posCenter + vec4( size,-size, 0.0, 0.0);\n"
//"	texCoord = vec2(1.0,0.0);\n"
//"	gl_Position = ProjectionMatrix *  pos;\n"
//"	EmitVertex();\n"
//"	EndPrimitive();\n"
//"}";


//std::string PointSprite::fragmentShaderText =
//"uniform sampler2D SpriteTexture;\n"
//"uniform float size;\n"
//"uniform vec3 color;\n"
//"VARYING_FRAG vec2 texCoord;\n"
//"VARYING_FRAG vec2 positionFragIn;\n"
//"VARYING_FRAG vec4 mvpFragIn;\n"
//"void main(void)\n"
//"{\n"
//"	float lum = texture2D(SpriteTexture, texCoord).s;\n"
//"	if (lum==0.0)\n"
//"		discard;\n"
//"	vec2 v = texCoord-vec2(0.5,0.5);\n"
//"	float z = size * sqrt(1.0-dot(v,v));\n"
//"	vec2 zfrag = positionFragIn + vec2(z,0.0);\n"
//"	gl_FragDepth = 0.5 + 0.5 * dot(zfrag, mvpFragIn.xy) / dot(zfrag, mvpFragIn.zw);\n"
//"	gl_FragColor = vec4(color,0.0)*lum;\n"
//"}";


PointSprite::PointSprite(float radius)
{
	std::string glxvert(*GLSLShader::DEFINES_GL);
	glxvert.append(vertexShaderText);

	std::string glxgeom = GLSLShader::defines_Geom("points","triangle_strip",4);
	glxgeom.append(geometryShaderText);

	std::string glxfrag(*GLSLShader::DEFINES_GL);
	glxfrag.append(fragmentShaderText);

	loadShadersFromMemory(glxvert.c_str(), glxfrag.c_str(), glxgeom.c_str(), GL_POINTS, GL_TRIANGLE_STRIP);

	bind();
	m_uniform_size = glGetUniformLocation(program_handler(),"size");
	m_uniform_color = glGetUniformLocation(program_handler(),"color");
	glUniform1f(m_uniform_size, radius);
	unbind();

	// load texture
	if (m_ptrSphere == NULL)
	{
		computeSphere();
		glGenTextures(1, &m_idTexture);
		glBindTexture(GL_TEXTURE_2D, m_idTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, 1, WIDTHSPRITE, WIDTHSPRITE, 0, GL_LUMINANCE,  GL_UNSIGNED_BYTE, (GLvoid*)m_ptrSphere);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	m_uniform_texture = glGetUniformLocation(program_handler(),"SpriteTexture");
}

PointSprite::~PointSprite()
{
	if (m_ptrSphere!=NULL)
	{
		delete[] m_ptrSphere;
		glDeleteTextures(1, &m_idTexture);
	}
}

unsigned int PointSprite::setAttributePosition(VBO* vbo)
{
	return bindVA_VBO("VertexPosition", vbo);
}

void PointSprite::predraw(const Geom::Vec3f& color)
{
	bind();
	glUniform1i(m_uniform_texture, 0);
	glUniform3fv(m_uniform_color, 1, color.data());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_idTexture);
	glEnable(GL_TEXTURE_2D);
}

void PointSprite::postdraw()
{
	glDisable(GL_TEXTURE_2D);
	unbind();
}

void PointSprite::setSize(float radius)
{
	bind();
	glUniform1f(m_uniform_size, radius);
	unbind();
}

void PointSprite::computeSphere()
{
	if (m_ptrSphere == NULL) // normally useless
	{
		m_ptrSphere = new unsigned char[WIDTHSPRITE*WIDTHSPRITE];
	}

	unsigned char* ptr = m_ptrSphere;

	Geom::Vec3f eye(0.0f , 0.0f, -1.0f);
	Geom::Vec3f light(5.0f, 3.0f, 10.0f);
	light.normalize();
	float r2 = float((WIDTHSPRITE - 1) * (WIDTHSPRITE - 1)) / 4.0f;
	float mid = 0.5f - float(WIDTHSPRITE / 2);

	for (unsigned int i = 0; i < WIDTHSPRITE; ++i)
	{
		for (unsigned int j = 0; j < WIDTHSPRITE; ++j)
		{
			float x = float(i) + mid;
			float y = float(j) + mid;
			float a =  x*x + y*y;
			if (r2 >= a)
			{
				float z = sqrt(r2 - a);
				Geom::Vec3f P(x, y, z);
				P.normalize();
				float col = P * light;
				if (col < 0.0f)
					col = 0.02f; // ambiant

				*ptr++ = (unsigned char)(255.0f * col);
			}
			else
				*ptr++ = (unsigned char)0;
		}
	}
}

} // namespace Utils

} // namespace CGoGN
