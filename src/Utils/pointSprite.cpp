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

#include "Utils/pointSprite.h"

namespace CGoGN
{

namespace Utils
{

#include "pointSprite.vert"
#include "pointSprite.frag"
#include "pointSprite.geom"


unsigned char* PointSprite::m_ptrSphere = NULL;

PointSprite* PointSprite::m_instance0 = NULL;



PointSprite::PointSprite(bool withColorPervertex, float radius,  bool with_plane)
{
	std::string defineColor("#define WITH_COLOR_PER_VERTEX 1\n");

	std::string glxvert(*GLSLShader::DEFINES_GL);
	if (withColorPervertex)
		glxvert.append(defineColor);
	glxvert.append(vertexShaderText);

	std::string glxgeom = GLSLShader::defines_Geom("points","triangle_strip",4);
	if (withColorPervertex)
		glxgeom.append(defineColor);
	if (with_plane)
		glxgeom.append("#define WITH_PLANE 1\n");

	glxgeom.append(geometryShaderText);

	std::string glxfrag(*GLSLShader::DEFINES_GL);
	if (withColorPervertex)
		glxfrag.append(defineColor);
	if (with_plane)
		glxfrag.append("#define WITH_PLANE 1\n");
	glxfrag.append(fragmentShaderText);

	loadShadersFromMemory(glxvert.c_str(), glxfrag.c_str(), glxgeom.c_str(), GL_POINTS, GL_TRIANGLE_STRIP,4);

	bind();
	*m_uniform_size = glGetUniformLocation(program_handler(),"size");
	*m_uniform_color = glGetUniformLocation(program_handler(),"colorsprite");
	glUniform1f(*m_uniform_size, radius);
	unbind();

	// load texture
	if (m_ptrSphere == NULL)
	{
		computeSphere();
		m_instance0 = this;
		glGenTextures(1, &(*m_idTexture));
		glBindTexture(GL_TEXTURE_2D, *m_idTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, 1, WIDTHSPRITE, WIDTHSPRITE, 0, GL_LUMINANCE,  GL_UNSIGNED_BYTE, (GLvoid*)m_ptrSphere);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		*m_idTexture = *(this->m_idTexture);
	}

	*m_uniform_texture = glGetUniformLocation(program_handler(),"SpriteTexture");

	if (with_plane)
	{
		*m_uniform_EyePos = glGetUniformLocation(program_handler(),"eyePos");
		*m_uniform_EyeY = glGetUniformLocation(program_handler(),"eyeY");
	}

	*m_uniform_ambiant = glGetUniformLocation(program_handler(),"ambiant");
	*m_uniform_lightPos = glGetUniformLocation(program_handler(),"lightPos");

	setLightPosition(Geom::Vec3f(2000.0,2000.0,2000.0));
	setAmbiantColor(Geom::Vec3f(0.1f,0.1f,0.1f));
}


PointSprite::~PointSprite()
{
	if (m_ptrSphere!=NULL)
	{
		delete[] m_ptrSphere;
		glDeleteTextures(1, &(*m_idTexture));
	}
}

unsigned int PointSprite::setAttributePosition(VBO* vbo)
{
	return bindVA_VBO("VertexPosition", vbo);
}

unsigned int PointSprite::setAttributeColor(VBO* vbo)
{
	return bindVA_VBO("VertexColor", vbo);
}

void PointSprite::predraw(const Geom::Vec3f& color)
{
	bind();
	glUniform1i(*m_uniform_texture, 0);
	glUniform3fv(*m_uniform_color, 1, color.data());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *m_idTexture);
	glEnable(GL_TEXTURE_2D);
}

void PointSprite::predraw()
{
	bind();
	glUniform1i(*m_uniform_texture, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *m_idTexture);
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
	glUniform1f(*m_uniform_size, radius);
	unbind();
}

void PointSprite::setEyePosition(const Geom::Vec3f& ox, const Geom::Vec3f& oy)
{
	bind();
	glUniform3fv(*m_uniform_EyePos, 1, ox.data());
	glUniform3fv(*m_uniform_EyeY, 1, oy.data());
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


void PointSprite::setLightPosition(const Geom::Vec3f& pos)
{
	bind();
	glUniform3fv(*m_uniform_lightPos, 1, pos.data());
	unbind();
}

void PointSprite::setAmbiantColor(const Geom::Vec3f& amb)
{
	bind();
	glUniform3fv(*m_uniform_ambiant, 1, amb.data());
	unbind();
}


} // namespace Utils

} // namespace CGoGN
