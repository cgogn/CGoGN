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

#include "Utils/vbo.h"

namespace CGoGN
{

namespace Utils
{

std::string Strings3D::vertexShaderText =
"ATTRIBUTE vec4 VertexPosition;\n"
"uniform mat4 ModelViewMatrix;\n"
"uniform mat4 ProjectionMatrix;\n"
"uniform vec3 strPos;\n"
"uniform float scale;\n"
"VARYING_VERT vec2 tex_coord;\n"
"INVARIANT_POS;\n"
"void main ()\n"
"{\n"
"	vec4 pos = ModelViewMatrix * vec4(strPos,1.0) + vec4(VertexPosition[0]*scale,VertexPosition[1]*scale,0.0,0.0);\n"
"	tex_coord = vec2(VertexPosition[2],VertexPosition[3]);\n"
"	gl_Position = ProjectionMatrix * pos;\n"
"}";


std::string Strings3D::fragmentShaderText1 =
"VARYING_FRAG vec2 tex_coord;\n"
"uniform sampler2D FontTexture;\n"
"uniform vec3 color;\n"
"FRAG_OUT_DEF;\n"
"void main (void)\n"
"{\n"
"	float lum = texture2D(FontTexture, tex_coord).s;\n";

std::string Strings3D::fragmentShaderText2 =
"	gl_FragColor = vec4(color,0.0)*lum;\n"
"}";


GLuint Strings3D::m_idTexture = 0xffffffff;
ILuint Strings3D::m_imgName;


Strings3D::Strings3D(bool withBackground, const Geom::Vec3f& bgc) : m_nbChars(0)
{
	if (m_idTexture == 0xffffffff)
	{
		std::string font_finename = Utils::GLSLShader::findFile("font_cgogn.png");
		ilInit();
		ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
		ilEnable(IL_ORIGIN_SET);

		ilGenImages(1,&m_imgName);
		ilBindImage(m_imgName);
		ilLoadImage(font_finename.c_str());

		glGenTextures(1, &m_idTexture);
		glBindTexture(GL_TEXTURE_2D, m_idTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, WIDTHTEXTURE, HEIGHTTEXTURE, 0, GL_LUMINANCE,  GL_UNSIGNED_BYTE, (GLvoid*)(ilGetData()));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	std::string glxvert(*GLSLShader::DEFINES_GL);
	glxvert.append(vertexShaderText);
	std::string glxfrag(*GLSLShader::DEFINES_GL);

	glxfrag.append(fragmentShaderText1);

	std::string background;
	if (!withBackground)
		glxfrag.append("if (lum == 0.0) discard;\n");
	else if (bgc*bgc > 0.0)
	{
		std::stringstream ss;
		ss << "	if (lum==0.0) gl_FragColor=vec4(";
		ss << bgc[0] << "," << bgc[1] << "," << bgc[2] << ",0.0);\n		else\n";
		background.append(ss.str());
	}
	glxfrag.append(background);
	glxfrag.append(fragmentShaderText2);

	loadShadersFromMemory(glxvert.c_str(), glxfrag.c_str());

	m_vbo1 = new Utils::VBO();
	m_vbo1->setDataSize(4);
	m_vbo1->ref(this);

	bindVA_VBO("VertexPosition", m_vbo1);

	bind();
	m_uniform_position = glGetUniformLocation(program_handler(), "strPos");
	m_uniform_color = glGetUniformLocation(program_handler(), "color");
	m_uniform_scale = glGetUniformLocation(program_handler(), "scale");
	m_uniform_texture = glGetUniformLocation(program_handler(), "FontTexture");
	glUniform1f(m_uniform_scale, 1.0f);
	unbind();
}

void Strings3D::setScale(float scale)
{
	bind();
	glUniform1f(m_uniform_scale, scale);
	unbind();
}

Strings3D::~Strings3D()
{
	ilDeleteImages(1 ,&m_imgName);
	delete m_vbo1;
}

unsigned int Strings3D::addString(const std::string& str)
{
	unsigned int id = m_strings.size();
	m_strings.push_back(str);
	m_nbChars += str.length();
	return id;
}

unsigned int Strings3D::addString(const std::string& str, const Geom::Vec3f& pos)
{
	unsigned int id = m_strings.size();
	m_strings.push_back(str);
	m_nbChars += str.length();
	m_strTranslate.push_back(pos);
	return id;
}

unsigned int Strings3D::sendOneStringToVBO(const std::string& str, float **buffervbo)
{
	unsigned int nbc = str.length();

	float x = 0.0f;

	float* buffer = *buffervbo;

	for(unsigned int j = 0; j < nbc; ++j)
	{
		unsigned int ci = str[j]-32;
		float u  = float(ci % CHARSPERLINE) / float(CHARSPERLINE);
		float v  = float(ci / CHARSPERLINE) / float(CHARSPERCOL) + 1.0f / HEIGHTTEXTURE;
		float u2 = u + float(REALWIDTHFONT) / float(WIDTHTEXTURE);
		float v2 = v + float(WIDTHFONT - 1) / float(HEIGHTTEXTURE);

		*buffer++ = x;
		*buffer++ = 0;
		*buffer++ = u;
		*buffer++ = v2;

		float xf = x + float(REALWIDTHFONT) / 25.f;

		*buffer++ = xf;
		*buffer++ = 0;
		*buffer++ = u2;
		*buffer++ = v2;

		*buffer++ = xf;
		*buffer++ = float(WIDTHFONT) / 25.f;
		*buffer++ = u2;
		*buffer++ = v;

		*buffer++ = x;
		*buffer++ = float(WIDTHFONT) / 25.f;
		*buffer++ = u;
		*buffer++ = v;

		x = xf; // + space ?
	}

	*buffervbo = buffer;

	return 4 * nbc;
}

void Strings3D::sendToVBO()
{
	// send coord / texcoord of strings

	// alloc buffer
	m_vbo1->bind();
	glBufferData(GL_ARRAY_BUFFER, m_nbChars * 16 * sizeof(float), 0, GL_STREAM_DRAW);
	float* buffer = reinterpret_cast<float*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));

	// fill buffer
	unsigned int pos = 0; // pos of first index in vbo for current string
	unsigned int nbw = m_strings.size();
	for (unsigned int i = 0; i < nbw; ++i)
	{
		unsigned int nb = sendOneStringToVBO(m_strings[i], &buffer);
		m_strpos.push_back(std::pair<unsigned int, unsigned int>(pos, nb));
		pos += nb;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void Strings3D::predraw(const Geom::Vec3f& color)
{
	bind();
	glUniform1i(m_uniform_texture, 0);
	glUniform3fv(m_uniform_color, 1, color.data());

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, m_idTexture);
	glEnable(GL_TEXTURE_2D);

	glDisable(GL_LIGHTING);

	enableVertexAttribs();
}

void Strings3D::postdraw()
{
	disableVertexAttribs();
	unbind();
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
		CGoGNerr << "Strings3D: for drawAll use exclusively addString with position"<< CGoGNendl;
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

} // namespace Utils

} // namespace CGoGN
