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

#ifndef _TUTO2_
#define _TUTO2_



#include "Utils/Qt/qtSimple.h"
#include "Utils/cgognStream.h"
#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"

#include "Algo/Render/GL2/mapRender.h"
#include "Utils/Shaders/shaderSimpleColor.h"
#include "Utils/Shaders/shaderColorPerVertex.h"
#include "Utils/vbo.h"

#include "Algo/Histogram/histogram.h"
#include "Algo/Histogram/qthistodraw.h"
#include "Utils/Qt/qtpopup.h"

using namespace CGoGN ;

struct PFP: public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

typedef PFP::MAP MAP ;
typedef PFP::VEC3 VEC3 ;

/*
 * color map class for histogram color
 */
class ColMap : public Algo::Histogram::HistoColorMap
{
public:
	Geom::Vec3f color(double f) const { return Utils::color_map_BCGYR(float(f)); }
};

/*
 * Attribute conversion to double
 */
//class AttConv: public Algo::Histogram::AttributeConvert
//{
////	VertexAttribute<float>& m_va;
//public:
//	/// constructor with attribute reference
//	AttConv(VertexAttribute<float>& va): m_va(va){}
//
//	/// just call begin of attribute handler
//	unsigned int begin() const { return m_va.begin();}
//
//	/// just call end of attribute handler
//	unsigned int end() const  { return m_va.end();}
//
//	/// just call next  of attribute handler
//	void next(unsigned int& i) const { m_va.next(i);}
//	unsigned int nbElements() const  { return m_va.nbElements();}
//	double operator[](unsigned int i) const { return double(m_va[i])*0.33;}
//};

class AttConv: public Algo::Histogram::AttributeConvert<VertexAttribute<float, MAP> >
{
public:
	/// constructor with attribute reference
	AttConv(VertexAttribute<float, MAP>& va): Algo::Histogram::AttributeConvert<VertexAttribute<float, MAP> >(va){}
	double operator[](unsigned int i) const { return double(attrib[i]) * 0.33; }
};

class MyQT: public Utils::QT::SimpleQT
{
	Q_OBJECT

public:
	MyQT():
		m_render(NULL),
		m_positionVBO(NULL),
		m_shader2(NULL),
		l_cm(NULL),
		l_histo(NULL),
		l_popup(NULL),
		l_histodraw(NULL)
	{}

	void cb_redraw();
	void cb_initGL();

protected:
	MAP myMap;

	// render, VBO & shader
	Algo::Render::GL2::MapRender* m_render;
	Utils::VBO* m_positionVBO;	// position 3D
	Utils::VBO* m_colorVBO2;	// color per vertex for edge drawing
	Utils::ShaderColorPerVertex* m_shader2;

	// some ptr
	ColMap*  l_cm;
	Algo::Histogram::Histogram* l_histo;
	Utils::QT::QtPopUp* l_popup;
	Algo::Histogram::RenderHistogram* l_histodraw;

	unsigned int l_nbc;
	unsigned int l_nbq;

public:
	// example of simple map creation
	void createMap(const std::string& filename);

	void cb_Save();

	void cb_exit();

	void cb_keyPress(int keycode);

public slots:
	void clickHisto(unsigned int i, unsigned int j);
};

#endif
