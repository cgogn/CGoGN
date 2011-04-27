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

#ifndef _TUTO4_H_
#define _TUTO4_H_

#include "Utils/qtSimple.h"

#include "ui_tuto4.h"
// inclure qtui.h juste après le ui_xxx.h
#include "Utils/qtui.h"

// forward definitions (minimize includes) syntax a little bit tricky !!
namespace CGoGN { namespace Algo { namespace Render { namespace GL2 { class MapRender; class TopoRenderMapD; } } } }
namespace CGoGN { namespace Utils { class VBO; } }
namespace CGoGN { namespace Utils { class ShaderSimpleColor; } }
namespace CGoGN { namespace Utils { class ShaderFlat; } }

using namespace CGoGN ;
/**
 * A class for a little interface and rendering
 */

class MyQT: public Utils::QT::SimpleQT
{
	Q_OBJECT

public:
	// render
	Algo::Render::GL2::MapRender* m_render;
	Algo::Render::GL2::TopoRenderMapD* m_render_topo;

	// VBO
	Utils::VBO* m_positionVBO;

	// shader basic
	Utils::ShaderSimpleColor* m_shader;
	Utils::ShaderFlat* m_shader2;

	bool render_obj;
	bool render_topo;

	MyQT():m_render(NULL), m_positionVBO(NULL), m_shader(NULL), render_obj(true), render_topo(false) {}

	void cb_redraw();

	void cb_initGL();

	void cb_keyPress(int code);

public slots:
	void button_compile();
	void slider_explode(int x);
};

#endif
