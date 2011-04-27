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

#ifndef TUTO_MT_H
#define TUTO_MT_H

#include <iostream>

#include "Utils/qtSimple.h"
#include "Utils/cgognStream.h"

// forward definitions (minimize includes)
namespace CGoGN { namespace Algo { namespace Render { namespace GL1 { class MapRender; } } } }
namespace CGoGN { namespace Utils { class VBO; } }
namespace CGoGN { namespace Utils { class ShaderSimpleColor; } }

using namespace CGoGN ;

/**
 * A class for a little interface and rendering
 */
class MyQT: public Utils::QT::SimpleQT
{
	Q_OBJECT

public:
	MyQT()
	{}

	// callbacks of simpleQT to overdefine:
	void cb_redraw();

	void cb_initGL();

	void cb_Open();

	void cb_New();

// callbacks (slots) locally defined
public slots:
	void menu_slot1();
};

#endif
