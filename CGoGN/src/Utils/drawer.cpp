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

#include "GL/glew.h"

#define CGoGN_UTILS_DLL_EXPORT 1
#include "Utils/drawer.h"

#include "Utils/vbo_base.h"
#include "Utils/svg.h"

namespace CGoGN
{

namespace Utils
{

	Drawer::Drawer(int lineMode) :
	m_currentWidth(1.0f),
	m_currentSize(1.0f),
	m_shader(NULL),
	m_shaderL(NULL),
	m_shaderSF(NULL),
	//m_shaderCL(NULL),
	//m_shader3DCL(NULL),
	m_lineMode(lineMode),
	m_withShading(false)
{
	m_vboPos = new Utils::VBO();
	m_vboPos->setDataSize(3);

	m_vboCol = new Utils::VBO();
	m_vboCol->setDataSize(3);

	m_shader = new Utils::ShaderColorPerVertex();
	m_shader->setAttributePosition(m_vboPos);
	m_shader->setAttributeColor(m_vboCol);

	m_shaderL = m_shader;

	if (GLSLShader::CURRENT_OGL_VERSION >=3)
	{
		if (lineMode == 1)
			m_shaderL = new Utils::ShaderBoldColorLines();
		if (lineMode == 2)
			m_shaderL = new Utils::ShaderBold3DColorLines();
			m_shaderL->setAttributePosition(m_vboPos);
			m_shaderL->setAttributeColor(m_vboCol);
			m_shaderL->setNoClippingPlane();
	}


	m_shaderSF = new Utils::ShaderSimpleFlat();
	m_shaderSF->setAttributePosition(m_vboPos);
	m_shaderSF->setAttributeColor(m_vboCol);

	Utils::GLSLShader::registerShader(NULL, m_shader);
	Utils::GLSLShader::registerShader(NULL, m_shaderL);
	Utils::GLSLShader::registerShader(NULL, m_shaderSF);

	m_dataPos.reserve(128);
	m_dataCol.reserve(128);
	m_begins.reserve(16);
//	m_modes.reserve(16);
}

Drawer::~Drawer()
{
	Utils::GLSLShader::unregisterShader(NULL, m_shader);
	Utils::GLSLShader::unregisterShader(NULL, m_shaderL);
	Utils::GLSLShader::unregisterShader(NULL, m_shaderSF);
	delete m_vboPos;
	delete m_vboCol;

	if (m_shaderL != m_shader)
		delete m_shaderL;
	delete m_shader;
	delete m_shaderSF;
}

//Utils::ShaderColorPerVertex* Drawer::getShader()
//{
//	return m_shader;
//}

std::vector<Utils::GLSLShader*> Drawer::getShaders()
{
	std::vector<Utils::GLSLShader*> shaders;
	shaders.push_back(m_shader);
	if (m_shaderL != m_shader)
		shaders.push_back(m_shaderL);

	shaders.push_back(m_shaderSF);

	return shaders;
}

void Drawer::updateMatrices(const glm::mat4& projection, const glm::mat4& modelview)
{
	m_shader->updateMatrices(projection,modelview);
	if (m_shaderL != m_shader)
		m_shaderL->updateMatrices(projection,modelview);

	m_shaderSF->updateMatrices(projection, modelview);
}


void Drawer::lineWidth(float lw)
{
	m_currentWidth = lw;
}

void Drawer::pointSize(float ps)
{
	m_currentSize = ps;
}

int Drawer::begin(GLenum mode)
{
	int res = int(m_begins.size());

	if (mode == GL_POINTS)
		m_begins.push_back(PrimParam(uint32(m_dataPos.size()), mode, m_currentSize));
	else
		m_begins.push_back(PrimParam(uint32(m_dataPos.size()), mode, m_currentWidth));

	return res;
}

void Drawer::end()
{
	m_begins.back().nb = uint32(m_dataPos.size() - m_begins.back().begin);
}

void Drawer::color(const Geom::Vec3f& col)
{
	if (m_dataPos.size() == m_dataCol.size())
		m_dataCol.push_back(col);
	else
		m_dataCol.back() = col;
}

void Drawer::color3f(float r, float g, float b)
{
	color(Geom::Vec3f(r,g,b));
}

unsigned int Drawer::vertex(const Geom::Vec3f& v)
{
	if (m_dataPos.size() == m_dataCol.size())
	{
		if (m_dataCol.empty())
			m_dataCol.push_back(Geom::Vec3f(1.0f, 1.0f, 1.0f));
		else
			m_dataCol.push_back( m_dataCol.back());
	}
	m_dataPos.push_back(v);
	return  uint32(m_dataPos.size()-1);
}

unsigned int Drawer::vertex3f(float r, float g, float b)
{
	return vertex(Geom::Vec3f(r,g,b));
}

void Drawer::newList(GLenum comp)
{
	m_compile = comp;
	m_dataPos.clear();
	m_dataCol.clear();
	m_begins.clear();
}

void Drawer::endList()
{
	unsigned int nbElts = uint32(m_dataPos.size());
	
	if (nbElts == 0)
		return;

	m_vboPos->bind();
	glBufferData(GL_ARRAY_BUFFER, nbElts * sizeof(Geom::Vec3f), &(m_dataPos[0]), GL_STREAM_DRAW);

	m_vboCol->bind();
	glBufferData(GL_ARRAY_BUFFER, nbElts * sizeof(Geom::Vec3f), &(m_dataCol[0]), GL_STREAM_DRAW);

	// free memory
	std::vector<Geom::Vec3f> tempo;
	tempo.swap(m_dataPos);
	std::vector<Geom::Vec3f> tempo2;
	tempo2.swap(m_dataCol);

	for (auto it = m_begins.begin(); it != m_begins.end(); ++it)
	{
		switch (it->mode)
		{
		case GL_POINTS:
			m_begins_point.push_back(*it);
			break;
		case GL_LINES:
		case GL_LINE_STRIP:
		case GL_LINE_LOOP:
			m_begins_line.push_back(*it);
			break;
		default:
			m_begins_face.push_back(*it);
			break;
		}
	}


	if (m_compile != GL_COMPILE)
		callList();
}


void Drawer::updatePositions(unsigned int first, unsigned int nb, const Geom::Vec3f* P)
{
	m_vboPos->bind();
	glBufferSubData(GL_ARRAY_BUFFER, first * sizeof(Geom::Vec3f), nb * sizeof(Geom::Vec3f), P);
}

void Drawer::updatePositions(unsigned int first, unsigned int nb, const float* P)
{
	m_vboPos->bind();
	glBufferSubData(GL_ARRAY_BUFFER, first * 3 * sizeof(float), nb * 3 * sizeof(float), P);
}


void Drawer::callList(float opacity)
{
	//if (m_begins.empty())
	//	return;

	//m_shader->setOpacity(opacity);
	//m_shader->enableVertexAttribs();
	//for (std::vector<PrimParam>::iterator pp = m_begins.begin(); pp != m_begins.end(); ++pp)
	//{
	//	if (pp->mode == GL_POINTS)
	//	{
	//		glPointSize(pp->width);
	//		glDrawArrays(GL_POINTS, pp->begin, pp->nb);
	//	}
	//}
	//m_shader->disableVertexAttribs();

	//m_shaderL->setOpacity(opacity);
	//m_shaderL->enableVertexAttribs();
	//for (std::vector<PrimParam>::iterator pp = m_begins.begin(); pp != m_begins.end(); ++pp)
	//{
	//	if (pp->mode != GL_POINTS)
	//	{
	//		m_shaderL->setLineWidth(pp->width);
	//		m_shaderL->bind();
	//		glDrawArrays(pp->mode, pp->begin, pp->nb);
	//	}
	//}
	//m_shaderL->disableVertexAttribs();

	if (m_begins.empty())
		return;

	// rendering lines
	m_shaderL->setOpacity(opacity);
	m_shaderL->enableVertexAttribs();
	for (std::vector<PrimParam>::iterator pp = m_begins_line.begin(); pp != m_begins_line.end(); ++pp)
	{
		m_shaderL->setLineWidth(pp->width);
		m_shaderL->bind();
		glDrawArrays(pp->mode, pp->begin, pp->nb);
	}
	m_shaderL->disableVertexAttribs();

	
	if (m_withShading)
	{
		// rendering faces
		m_shaderSF->enableVertexAttribs();
		for (std::vector<PrimParam>::iterator pp = m_begins_face.begin(); pp != m_begins_face.end(); ++pp)
		{
			glDrawArrays(pp->mode, pp->begin, pp->nb);
		}
		m_shaderSF->disableVertexAttribs();

		// rendering points
		m_shader->setOpacity(opacity);
		m_shader->enableVertexAttribs();
		for (std::vector<PrimParam>::iterator pp = m_begins_point.begin(); pp != m_begins_point.end(); ++pp)
		{
			glPointSize(pp->width);
			glDrawArrays(pp->mode, pp->begin, pp->nb);
		}
		m_shader->disableVertexAttribs();
	}
	else
	{
		// rendering faces & points
		m_shader->setOpacity(opacity);
		m_shader->enableVertexAttribs();
		for (std::vector<PrimParam>::iterator pp = m_begins_face.begin(); pp != m_begins_face.end(); ++pp)
		{
			glDrawArrays(pp->mode, pp->begin, pp->nb);
		}
		for (std::vector<PrimParam>::iterator pp = m_begins_point.begin(); pp != m_begins_point.end(); ++pp)
		{
			glPointSize(pp->width);
			glDrawArrays(pp->mode, pp->begin, pp->nb);
		}

		m_shader->disableVertexAttribs();
	}
}



//
//void Drawer::callSubList(int index, float opacity)
//{
//	if (index >= int(m_begins.size()))
//		return;
//	PrimParam* pp = & (m_begins[index]);
//
//	if (pp->mode == GL_POINTS)
//	{
//		m_shader->setOpacity(opacity);
//		m_shader->enableVertexAttribs();
//		glPointSize(pp->width);
//		glDrawArrays(pp->mode, pp->begin, pp->nb);
//		m_shader->disableVertexAttribs();
//	}
//	else
//	{
//		m_shaderL->setOpacity(opacity);
//		m_shaderL->setLineWidth(pp->width);
//		m_shaderL->enableVertexAttribs();
//		glDrawArrays(pp->mode, pp->begin, pp->nb);
//		m_shaderL->disableVertexAttribs();
//	}
//}

//void Drawer::callSubLists(int first, int nb, float opacity)
//{
//	m_shader->setOpacity(opacity);
//	m_shader->enableVertexAttribs();

//	int last = first+nb;
//	for (int i = first; i< last; ++i)
//		if (i < int(m_begins.size()))
//		{
//			PrimParam* pp = & (m_begins[i]);

//			if (pp->mode == GL_POINTS)
//				glPointSize(pp->width);
//			if ((pp->mode == GL_LINES) || (pp->mode == GL_LINE_LOOP) || (pp->mode == GL_LINE_STRIP))
//				glLineWidth(pp->width);
//			glDrawArrays(pp->mode, pp->begin, pp->nb);
//		}

//	m_shader->disableVertexAttribs();
//}

//void Drawer::callSubLists(std::vector<int> indices, float opacity)
//{
//	m_shader->setOpacity(opacity);

//	m_shader->enableVertexAttribs();

//	for (std::vector<int>::iterator it = indices.begin(); it != indices.end(); ++it)
//		if (*it < int(m_begins.size()))
//		{
//			PrimParam* pp = & (m_begins[*it]);

//			if (pp->mode == GL_POINTS)
//				glPointSize(pp->width);
//			if ((pp->mode == GL_LINES) || (pp->mode == GL_LINE_LOOP) || (pp->mode == GL_LINE_STRIP))
//				glLineWidth(pp->width);
//			glDrawArrays(pp->mode, pp->begin, pp->nb);
//		}

//	m_shader->disableVertexAttribs();
//}


void Drawer::toSVG(Utils::SVG::SVGOut& svg)
{
	const Geom::Vec3f* ptrP = reinterpret_cast<Geom::Vec3f*>(m_vboPos->lockPtr());
	const Geom::Vec3f* ptrC = reinterpret_cast<Geom::Vec3f*>(m_vboCol->lockPtr());

	Utils::SVG::SvgGroup* svg1 = new Utils::SVG::SvgGroup("points", svg.m_model, svg.m_proj);
	Utils::SVG::SvgGroup* svg2 = new Utils::SVG::SvgGroup("lines", svg.m_model, svg.m_proj);
	Utils::SVG::SvgGroup* svg3 = new Utils::SVG::SvgGroup("faces", svg.m_model, svg.m_proj);

	for (std::vector<PrimParam>::iterator pp = m_begins.begin(); pp != m_begins.end(); ++pp)
	{
		svg1->setWidth(pp->width);
		if (pp->mode == GL_POINTS)
		{
			unsigned int end = pp->begin + pp->nb;
			svg1->beginPoints();
			for (unsigned int i=pp->begin; i<end; ++i)
				svg1->addPoint(ptrP[i], ptrC[i]);
			svg1->endPoints();
		}

		svg2->setWidth(pp->width);
		if (pp->mode == GL_LINES)
		{
			unsigned int end = pp->begin + pp->nb;
			svg2->beginLines();
			for (unsigned int i=pp->begin; i<end; i+=2)
				svg2->addLine(ptrP[i], ptrP[i+1], ptrC[i]);
			svg2->endLines();
		}

		svg3->setWidth(pp->width);
		if ((pp->mode == GL_LINE_LOOP) || (pp->mode == GL_POLYGON))
		{
			unsigned int end = pp->begin + pp->nb-1;
			svg3->beginLines();
			for (unsigned int i=pp->begin; i<=end; ++i)
				svg3->addLine(ptrP[i], ptrP[i+1], ptrC[i]);
			svg3->addLine(ptrP[end], ptrP[pp->begin], ptrC[end]);
			svg3->endLines();
		}
		if (pp->mode == GL_TRIANGLES)
		{
			unsigned int end = pp->begin + pp->nb;
			svg3->beginLines();
			for (unsigned int i=pp->begin; i<end; i+=3)
			{
				svg3->addLine(ptrP[i],   ptrP[i+1], ptrC[i]);
				svg3->addLine(ptrP[i+1], ptrP[i+2], ptrC[i+1]);
				svg3->addLine(ptrP[i+2], ptrP[i],   ptrC[i+2]);
			}
			svg3->endLines();
		}
		if (pp->mode == GL_QUADS)
		{
			unsigned int end = pp->begin + pp->nb;
			svg3->beginLines();
			for (unsigned int i=pp->begin; i<end; i+=4)
			{
				svg3->addLine(ptrP[i],   ptrP[i+1], ptrC[i]);
				svg3->addLine(ptrP[i+1], ptrP[i+2], ptrC[i+1]);
				svg3->addLine(ptrP[i+2], ptrP[i+3], ptrC[i+2]);
				svg3->addLine(ptrP[i+3], ptrP[i],   ptrC[i+3]);
			}
			svg3->endLines();
		}
	}

	svg.addGroup(svg1);
	svg.addGroup(svg2);
	svg.addGroup(svg3);

	m_vboPos->releasePtr();
	m_vboCol->releasePtr();
}

} // namespace Utils

} // namespace CGoGN
