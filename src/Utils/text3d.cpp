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

#include "Utils/text3d.h"

namespace CGoGN
{
namespace Utils
{

Strings3D::Strings3D():
	m_nbChars(0)
{ }

void Strings3D::init(float scale)
{
	glGenBuffersARB(1, &m_vbo1);
	m_shader.loadShaders("text.vert", "text.frag");
	m_shader.bind();
	m_uniform_position = glGetUniformLocationARB(m_shader.program_handler(),"strPos");
	m_uniform_color = glGetUniformLocationARB(m_shader.program_handler(),"color");
	m_uniform_scale = glGetUniformLocationARB(m_shader.program_handler(),"scale");
	glUniform1f(m_uniform_scale, scale);
	m_shader.unbind();

	// load texture
	std::string font_finename = Utils::GLSLShader::findFile("font_cgogn.png");
	ilInit();
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	ilEnable(IL_ORIGIN_SET);

	ilGenImages(1,&m_imgName);
	ilBindImage(m_imgName);
	ilLoadImage(font_finename.c_str());

	glGenTextures( 1, &m_idTexture);
	glBindTexture(GL_TEXTURE_2D, m_idTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, WIDTHTEXTURE, HEIGHTTEXTURE, 0, GL_LUMINANCE,  GL_UNSIGNED_BYTE, (GLvoid*)(ilGetData()));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_uniform_texture = glGetUniformLocationARB(m_shader.program_handler(),"FontTexture");
}


void Strings3D::setScale(float scale)
{
	m_shader.bind();
	glUniform1f(m_uniform_scale, scale);
	m_shader.unbind();
}

Strings3D::~Strings3D()
{
	ilDeleteImages(1,&m_imgName);
	glDeleteBuffers(1, &m_vbo1);
}

unsigned int Strings3D::addString(const std::string& str)
{
	unsigned int id=m_strings.size();
	m_strings.push_back(str);
	m_nbChars += str.length();
	return id;
}

unsigned int Strings3D::addString(const std::string& str, const Geom::Vec3f& pos)
{
	unsigned int id=m_strings.size();
	m_strings.push_back(str);
	m_nbChars += str.length();
	m_strTranslate.push_back(pos);
	return id;
}

unsigned int Strings3D::sendOneStringToVBO(const std::string& str, float **buffervbo)
{
	unsigned int nbc = str.length();

	float x=0.0f;

	float* buffer = *buffervbo;

	for(unsigned int j=0; j< nbc;++j)
	{
		unsigned int ci = str[j]-32;
		float u  = float(ci%CHARSPERLINE)/float(CHARSPERLINE);
		float v  = float(ci/CHARSPERLINE)/float(CHARSPERCOL)+ 1.0f/HEIGHTTEXTURE;
		float u2 = u + float(REALWIDTHFONT)/float(WIDTHTEXTURE);
		float v2 = v + float(WIDTHFONT-1)/float(HEIGHTTEXTURE);

		*buffer++ = x;
		*buffer++ = 0;
		*buffer++ = u;
		*buffer++ = v2;

		float xf = x+float(REALWIDTHFONT)/25.f;

		*buffer++ = xf;
		*buffer++ = 0;
		*buffer++ = u2;
		*buffer++ = v2;

		*buffer++ = xf;
		*buffer++ = float(WIDTHFONT)/25.f;
		*buffer++ = u2;
		*buffer++ = v;

		*buffer++ = x;
		*buffer++ = float(WIDTHFONT)/25.f;
		*buffer++ = u;
		*buffer++ = v;

		x = xf; // + space ?
	}

	*buffervbo = buffer;

	return 4*nbc;
}

void Strings3D::sendToVBO()
{
	// send coord / texcoord of strings

	// alloc buffer
	glBindBufferARB(GL_ARRAY_BUFFER, m_vbo1);
	glBufferDataARB(GL_ARRAY_BUFFER, m_nbChars*16*sizeof(float), 0, GL_STREAM_DRAW);
	float* buffer =  reinterpret_cast<float*>(glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE));

	// fill buffer
	unsigned int pos=0; // pos of first index in vbo for current string
	unsigned int nbw = m_strings.size();
	for (unsigned int i=0; i<nbw; ++i)
	{
		unsigned int nb = sendOneStringToVBO(m_strings[i], &buffer);
		m_strpos.push_back(std::pair<unsigned int, unsigned int>(pos,nb));
		pos += nb;
	}

	glUnmapBufferARB(GL_ARRAY_BUFFER);
}



void Strings3D::predraw(const Geom::Vec3f& color)
{
	m_shader.bind();
	glUniform1iARB(m_uniform_texture,0);

	glUniform3fvARB(m_uniform_color,1,color.data());

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, m_idTexture);
	glEnable(GL_TEXTURE_2D);

	glDisable(GL_LIGHTING);

	glBindBufferARB(GL_ARRAY_BUFFER, m_vbo1);
	glVertexPointer(4, GL_FLOAT, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
}

void Strings3D::postdraw()
{
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	m_shader.unbind();
}


void Strings3D::draw(unsigned int idSt, const Geom::Vec3f& pos)
{
	glUniform3fv(m_uniform_position, 1, pos.data());
	glDrawArrays(GL_QUADS, m_strpos[idSt].first , m_strpos[idSt].second );
}


void Strings3D::drawAll(const Geom::Vec3f& color)
{
	predraw(color);
	if (m_strpos.size() != m_strTranslate.size())
	{
		std::cerr << "Strings3D: for drawAll use exclusively addString with position"<< std::endl;
		return;
	}

	unsigned int nb = m_strpos.size();
	for (unsigned int idSt=0; idSt<nb; ++idSt)
	{
		glUniform3fv(m_uniform_position, 1, m_strTranslate[idSt].data());
		glDrawArrays(GL_QUADS, m_strpos[idSt].first , m_strpos[idSt].second );
	}
	postdraw();
}


}
}

