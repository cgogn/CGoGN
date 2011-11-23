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

#ifndef _TUTO_OGL3_
#define _TUTO_OGL3_

#include <iostream>

#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Geometry/vector_gen.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Modelisation/polyhedron.h"
#include "Algo/Geometry/normal.h"

#include "Algo/Render/GL2/mapRender.h"
#include "Geometry/vector_gen.h"

#include "Utils/Shaders/shaderPhong.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QMainWindow>
#include <QWidget>
#include <QtGui>
#include "Utils/qtSimple.h"
#include "Utils/qtgl.h"

using namespace CGoGN;

struct PFP: public PFP_STANDARD
{
	// definition of the type of the map
	typedef EmbeddedMap2 MAP;
};

class MyQT: public Utils::QT::SimpleQT
{
	Q_OBJECT

public slots:
	void sliderFocale_cb(int x);
	void color_cb();
	void pervertex_cb(bool val);

public:
	MyQT();
	~MyQT();

	void cb_redraw();

	Algo::Render::GL2::MapRender* m_render;

	Utils::VBO* positionVBO;
	Utils::VBO* normalVBO;
	Utils::VBO* colorVBO;

	Utils::ShaderPhong* shader1;
	Utils::GLSLShader* currentShader;
	void cb_initGL();
};

#endif
