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

#ifndef TP_MASTER_H_
#define TP_MASTER_H_


#include <iostream>


#include "Utils/qtSimple.h"

// forward definitions (minimize includes)
namespace CGoGN { namespace Algo { namespace Render { namespace GL2 { class MapRender; }}}}
namespace CGoGN { namespace Algo { namespace Render { namespace GL2 { class TopoRenderMapD; }}}}
namespace CGoGN { namespace Utils { class VBO; } }
namespace CGoGN { namespace Utils { class ShaderPhong; } }
namespace CGoGN { namespace Utils { class ShaderSimpleColor; } }

using namespace CGoGN ;

/**
 * A class for a little interface and rendering
 */

class MyQT: public Utils::QT::SimpleQT
{
	Q_OBJECT

protected:
	bool m_drawTopo;
	void drawSelected();
public:
	// render
	Algo::Render::GL2::MapRender* m_render;
	Algo::Render::GL2::TopoRenderMapD* m_render_topo;

	// VBO
	Utils::VBO* m_positionVBO;
	Utils::VBO* m_normalVBO;

	//2 shaders
	Utils::ShaderPhong* m_shader;
	Utils::ShaderSimpleColor* m_shader2;

	bool renderTopo;

	MyQT():
		m_render(NULL), m_render_topo(NULL),
		m_positionVBO(NULL), m_normalVBO(NULL),
		m_shader(NULL), m_shader2(NULL),
		renderTopo(true) {}

	// callbacks of simpleQT to overdefine:
	void cb_redraw();

	void cb_initGL();

	void cb_keyPress(int code);
};

#endif

