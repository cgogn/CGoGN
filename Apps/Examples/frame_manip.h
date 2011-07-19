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
#ifndef FRAME_MANIP_EX_H_
#define FRAME_MANIP_EX_H_

#include <iostream>


#include "Utils/qtSimple.h"
#include "Utils/frameManipulator.h"
#include "Utils/drawer.h"

// forward definitions (minimize includes)
namespace CGoGN { namespace Algo { namespace Render { namespace GL2 { class MapRender; }}}}
namespace CGoGN { namespace Utils { class VBO; } }
namespace CGoGN { namespace Utils { class ShaderFlat; } }
namespace CGoGN { namespace Utils { class ShaderSimpleColor; } }

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

	// VBO
	Utils::VBO* m_positionVBO;

	Utils::FrameManipulator* m_frame;
	unsigned int m_pickedAxis;

	///vector of six drawables
	std::vector<Utils::LineDrawable*> m_ld;

	/// precision of drawing (number of subdivisions)
	unsigned int m_precDraw;

	std::vector<Utils::Pickable*> m_pickables;
	Utils::Pickable* m_lastPickedObject;

	// width of cube of pickable
	unsigned int NBP;

	MyQT():m_render(NULL), m_positionVBO(NULL),NBP(2){}

	// callbacks of simpleQT to overdefine:
	void cb_redraw();

	void cb_initGL();

	void cb_mousePress(int button, int x, int y);

	void  cb_wheelEvent(int delta, int x, int y);

	void cb_keyPress(int code);

	void cb_mouseMove(int button, int x, int y);

	int m_begX;
	int m_begY;
	Geom::Vec3f m_projAxis;
	Geom::Vec3f m_projCenter;
};

#endif

