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

#ifndef TUTO_MT_H
#define TUTO_MT_H

#include <iostream>


#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap3.h"

#include "Geometry/vector_gen.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Render/GL2/mapRender.h"

#include "Utils/Shaders/shaderSimpleColor.h"
#include "Utils/Shaders/shaderVectorPerVertex.h"
#include "Utils/cgognStream.h"
#include "Utils/Qt/qtSimple.h"



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
	Algo::Render::GL2::MapRender* m_render;

	Utils::VBO* m_positionVBO;
	Utils::VBO* m_normalVBO;

	Utils::ShaderSimpleColor* m_shader;
	Utils::ShaderVectorPerVertex* m_lines;



	MyQT():
		m_render(NULL),
		m_positionVBO(NULL),
		m_normalVBO(NULL),
		m_shader(NULL),
		m_lines(NULL)
	{}

protected:

	void cb_redraw();

	void cb_initGL();

	void threadSimple();

	void threadAttrib();

	void threadStorage();

	void cb_keyPress(int code);

};








//class MyQT: public Utils::QT::SimpleQT
//{
//	Q_OBJECT
//
//public:
//	MyQT()
//	{}
//
//	// callbacks of simpleQT to overdefine:
//	void cb_redraw();
//
//	void cb_initGL();
//
//	void cb_Open();
//
//	void cb_New();
//
//// callbacks (slots) locally defined
//public slots:
//	void menu_slot1();
//};

#endif
