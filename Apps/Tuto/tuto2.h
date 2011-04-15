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
#ifndef __TUTO2_
#define __TUTO2_


#include <iostream>


#include "Utils/qtSimple.h"
#include "ui_tuto2.h"
#include "Utils/qtui.h"

#include "Geometry/vector_gen.h"

// forward definitions (minimize includes)
namespace CGoGN { namespace Algo { namespace Render { namespace GL2 { class MapRender; }}}}
namespace CGoGN { namespace Utils { class VBO; } }
namespace CGoGN { namespace Utils { class ShaderSimpleColor; } }

using namespace CGoGN ;

/**
 * Utilisation de designer-qt4:
 * Faire un DockWiget (laisser le nom par defaut
 * dans le Contents ajouter le layout choisi (vertical classiquement)
 * Ajouter les widgets necessaires, mettre des noms clairs pour
 * les utiliser dans le .cpp (pour les call back principalement)
 */

class MyQT: public Utils::QT::SimpleQT
{
	Q_OBJECT

	bool m_with_lines;
	Geom::Vec4f m_color;
	int m_line_width;
public:

	// render
	Algo::Render::GL2::MapRender* m_render;

	// VBO
	Utils::VBO* m_positionVBO;

	// shader basic
	Utils::ShaderSimpleColor* m_shader;

	MyQT():m_render(NULL), m_positionVBO(NULL), m_shader(NULL) {}


	void cb_redraw();

	void cb_initGL();

// slots locaux
public slots:
	void lines_slot(bool x);
	void line_width_slot(int x);
	void color_slot();


};

#endif

