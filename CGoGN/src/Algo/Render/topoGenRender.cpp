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

#define CGoGN_ALGO_DLL_EXPORT 1

#include "Algo/Render/GL2/topoRender.h"
#include "Utils/GLSLShader.h"

namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace GL2
{

TopoGenRender::TopoGenRender(float bs):
	m_nbDarts(0),
	m_nbRel2(0),
	m_topo_dart_width(2.0f),
	m_topo_relation_width(3.0f),
	m_normalShift(0.0f),
	m_boundShift(bs),
	m_dartsColor(1.0f,1.0f,1.0f,0.0f),
	m_colorPhi1(0.0f,1.0f,1.0f,0.0f),
	m_colorPhi2(1.0f,0.0f,0.0f,0.0f),
	m_dartsBoundaryColor(0.7f,1.0f,0.7f,0.0f),
	m_bufferDartPosition(NULL),
	m_sharedShaders(false),
	m_nameIndex("dart_index2"),
	m_ke(0.9f),
	m_kf(0.9f),
	m_wb(false),
	m_ns(0.0f)
{
	m_vbo0 = new Utils::VBO();
	m_vbo1 = new Utils::VBO();
	m_vbo2 = new Utils::VBO();

	m_vbo0->setDataSize(3);
	m_vbo1->setDataSize(3);
	m_vbo2->setDataSize(3);

	m_shader1 = new Utils::ShaderSimpleColor(true,false);

	// binding VBO - VA
	m_vaId = m_shader1->setAttributePosition(m_vbo1);

	// registering for auto matrices update
	Utils::GLSLShader::registerShader(NULL, m_shader1);
}


TopoGenRender::TopoGenRender(Utils::ShaderSimpleColor* ssc, float bs):
	m_nbDarts(0),
	m_nbRel2(0),
	m_topo_dart_width(2.0f),
	m_topo_relation_width(3.0f),
	m_normalShift(0.0f),
	m_boundShift(bs),
	m_dartsColor(1.0f,1.0f,1.0f,0.0f),
	m_colorPhi1(0.0f,1.0f,1.0f,0.0f),
	m_colorPhi2(1.0f,0.0f,0.0f,0.0f),
	m_dartsBoundaryColor(0.7f,1.0f,0.7f,0.0f),
	m_bufferDartPosition(NULL),
	m_shader1(ssc),
	m_sharedShaders(true),
	m_nameIndex("dart_index2"),
	m_ke(0.9f),
	m_kf(0.9f),
	m_wb(false),
	m_ns(0.0f)
{
	m_vbo0 = new Utils::VBO();
	m_vbo1 = new Utils::VBO();
	m_vbo2 = new Utils::VBO();

	m_vbo0->setDataSize(3);
	m_vbo1->setDataSize(3);
	m_vbo2->setDataSize(3);

	// binding VBO - VA
	m_vaId = m_shader1->setAttributePosition(m_vbo1);

	// registering for auto matrices update
	Utils::GLSLShader::registerShader(NULL, m_shader1);

}


TopoGenRender::~TopoGenRender()
{
	if (m_shader1)
		Utils::GLSLShader::unregisterShader(NULL, m_shader1);

	if (!m_sharedShaders)
	{
		if (m_shader1)
			delete m_shader1;
	}


	delete m_vbo2;
	delete m_vbo1;
	delete m_vbo0;

	if (m_bufferDartPosition!=NULL)
		delete[] m_bufferDartPosition;
}



void TopoGenRender::setClippingPlane(const Geom::Vec4f& plane)
{
	m_shader1->setClippingPlane(plane);
}


void TopoGenRender::setNoClippingPlane()
{
	this->setClippingPlane(Geom::Vec4f(0.0f,0.0f,0.0f,0.0f));
}



void TopoGenRender::setDartWidth(float dw)
{
	m_topo_dart_width = dw;
}


void TopoGenRender::setRelationWidth(float pw)
{
	m_topo_relation_width = pw;
}


void TopoGenRender::setColorPhi1(const Geom::Vec4f& col)
{
	m_colorPhi1 = col;
}

void TopoGenRender::setColorPhi2(const Geom::Vec4f& col)
{
	m_colorPhi2 = col;
}


void TopoGenRender::setInitialDartsColor(float r, float g, float b)
{
	m_dartsColor = Geom::Vec4f(r,g,b,0.0);
}


void TopoGenRender::setInitialBoundaryDartsColor(float r, float g, float b)
{
	m_dartsBoundaryColor = Geom::Vec4f(r,g,b,0.0);
}


void TopoGenRender::drawDarts()
{
	if (m_nbDarts==0)
		return;

	m_shader1->changeVA_VBO(m_vaId, m_vbo0);
	m_shader1->setColor(m_dartsColor);
	m_shader1->enableVertexAttribs();

	glLineWidth(m_topo_dart_width);
	glDrawArrays(GL_LINES, 0, m_nbDarts*2);

	// change the stride to take 1/2 vertices
	m_shader1->enableVertexAttribs(6*sizeof(GL_FLOAT));

	glPointSize(2.0f*m_topo_dart_width);
	glDrawArrays(GL_POINTS, 0, m_nbDarts);

	m_shader1->disableVertexAttribs();
}


void TopoGenRender::drawRelation1()
{
	if (m_nbDarts==0)
		return;


	glLineWidth(m_topo_relation_width);

	m_shader1->changeVA_VBO(m_vaId, m_vbo1);
	m_shader1->setColor(m_colorPhi1);
	m_shader1->enableVertexAttribs();

	glDrawArrays(GL_LINES, 0, m_nbDarts*2);

	m_shader1->disableVertexAttribs();
}


void TopoGenRender::drawRelation2()
{
	if (m_nbRel2==0)
		return;

	glLineWidth(m_topo_relation_width);

	m_shader1->changeVA_VBO(m_vaId, m_vbo2);
	m_shader1->setColor(m_colorPhi2);
	m_shader1->enableVertexAttribs();

	glDrawArrays(GL_LINES, 0, m_nbRel2*2);

	m_shader1->disableVertexAttribs();
}


void TopoGenRender::drawTopo(int code)
{
	if (m_nbDarts==0)
		return;

	if (code&1)
	{
		drawDarts();
	}

	glLineWidth(m_topo_relation_width);

	if (code&2)
	{
		drawRelation1();
	}

	if (code&4)
	{
		drawRelation2();
	}
}




Dart TopoGenRender::colToDart(float* color)
{
	unsigned int r = (unsigned int)(color[0]*255.0f);
	unsigned int g = (unsigned int)(color[1]*255.0f);
	unsigned int b = (unsigned int)(color[2]*255.0f);

	unsigned int id = r + 255*g +255*255*b;

	if (id == 0)
		return NIL;
	return Dart(id-1);
}


void TopoGenRender::dartToCol(Dart d, float& r, float& g, float& b)
{
	// here use d.index beacause it is what we want (and not map.dartIndex(d) !!)
	unsigned int lab = d.index + 1; // add one to avoid picking the black of screen

	r = float(lab%255) / 255.0f; lab = lab/255;
	g = float(lab%255) / 255.0f; lab = lab/255;
	b = float(lab%255) / 255.0f; lab = lab/255;
	if (lab!=0)
		CGoGNerr << "Error picking color, too many darts"<< CGoGNendl;
}



void TopoGenRender::svgout2D(const std::string& filename, const glm::mat4& model, const glm::mat4& proj)
{
	Utils::SVG::SVGOut svg(filename,model,proj);
	toSVG(svg);
	svg.write();
}


void TopoGenRender::toSVG(Utils::SVG::SVGOut& svg)
{

	// PHI2 / beta2
	Utils::SVG::SvgGroup* svg1 = new Utils::SVG::SvgGroup("phi2", svg.m_model, svg.m_proj);
	svg1->setToLayer();
	const Geom::Vec3f* ptr = reinterpret_cast<Geom::Vec3f*>(m_vbo2->lockPtr());
	svg1->setWidth(m_topo_relation_width);
	svg1->beginLines();
	for (unsigned int i=0; i<m_nbRel2; ++i)
		svg1->addLine(ptr[2*i], ptr[2*i+1],Geom::Vec3f(0.8f,0.0f,0.0f));
	svg1->endLines();
	m_vbo2->releasePtr();

	svg.addGroup(svg1);

	//PHI1 /beta1
	Utils::SVG::SvgGroup* svg2 = new Utils::SVG::SvgGroup("phi1", svg.m_model, svg.m_proj);
	svg2->setToLayer();
	ptr = reinterpret_cast<Geom::Vec3f*>(m_vbo1->lockPtr());
	svg2->setWidth(m_topo_relation_width);
	svg2->beginLines();
	for (unsigned int i=0; i<m_nbRel1; ++i)
		svg2->addLine(ptr[2*i], ptr[2*i+1],Geom::Vec3f(0.0f,0.7f,0.7f));
	svg2->endLines();
	m_vbo1->releasePtr();

	svg.addGroup(svg2);

	Utils::SVG::SvgGroup* svg3 = new Utils::SVG::SvgGroup("darts", svg.m_model, svg.m_proj);
	svg3->setToLayer();
	svg3->setWidth(m_topo_dart_width);
	svg3->beginLines();
	for (unsigned int i=0; i<m_nbDarts; ++i)
		svg3->addLine(ptr[2*i], ptr[2*i+1], Geom::Vec3f(m_dartsColor[0],m_dartsColor[1],m_dartsColor[2]));
	svg3->endLines();

	svg.addGroup(svg3);

	Utils::SVG::SvgGroup* svg4 = new Utils::SVG::SvgGroup("dartEmb", svg.m_model, svg.m_proj);
	svg4->setWidth(m_topo_dart_width);
	svg4->setToLayer();
	svg4->beginPoints();
	for (unsigned int i=0; i<m_nbDarts; ++i)
			svg4->addPoint(ptr[2*i],Geom::Vec3f(m_dartsColor[0],m_dartsColor[1],m_dartsColor[2]));
	svg4->endPoints();

	svg.addGroup(svg4);

	m_vbo0->releasePtr();
}


void TopoGenRender::setNormalShift(float ns)
{
	m_ns = ns;
}


void TopoGenRender::setBoundaryShift(float bs)
{
	m_boundShift = bs;
}

void TopoGenRender::setExplodeEdge(float ke)
{
	m_ke = ke;
}

void TopoGenRender::setExplodeFace(float kf)
{
	m_kf = kf;
}

void TopoGenRender::setWithBoundary(bool wb)
{
	m_wb = wb;
}

void TopoGenRender::addColoredDart(Dart d, float r, float g, float b)
{
	m_coloredDarts.push_back(ColoredDart(d,r,g,b));
}

void TopoGenRender::removeColoredDart(Dart d)
{
	for (auto it=m_coloredDarts.begin(); it != m_coloredDarts.end(); ++it)
	{
		if (it->d == d)
		{
			(*it) = m_coloredDarts.back();
			 m_coloredDarts.pop_back();
			 return;
		}
	}
}



} // namespace GL2

} // namespace Render

} // namespace Algo

} // namespace CGoGN
