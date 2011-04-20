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
#ifndef __TUTO5_
#define __TUTO5_


#include <iostream>


#include "Utils/qtSimple.h"

#include "ui_tuto5.h"
// inclure qtui.h juste après le ui_xxx.h
#include "Utils/qtui.h"

// forward definitions (minimize includes)
namespace CGoGN { namespace Algo { namespace Render { namespace GL2 { class MapRender; }}}}
namespace CGoGN { namespace Algo { namespace Render { namespace GL2 { class Topo3RenderMapD; }}}}
namespace CGoGN { namespace Utils { class VBO; } }
namespace CGoGN { namespace Utils { class ShaderSimpleColor; } }
namespace CGoGN { namespace Utils { class Strings3D; } }
namespace CGoGN { namespace Utils { class PointSprite; } }
namespace CGoGN { namespace Utils { class PointLine; } }

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

    bool render_text;
	bool render_balls;
	bool render_vectors;
    bool render_topo;

	Algo::Render::GL2::MapRender* m_render;
	Algo::Render::GL2::Topo3RenderMapD* m_render_topo;

	Utils::VBO* m_positionVBO;
	Utils::VBO* m_dataVBO;

	Utils::ShaderSimpleColor* m_shader;
	Utils::Strings3D* m_strings;
	Utils::PointSprite* m_sprite;
	Utils::PointLine* m_lines;
public:



	MyQT():
		render_text(true),render_balls(true),render_vectors(true),render_topo(true),
		m_render(NULL), m_positionVBO(NULL), m_shader(NULL)
		{}


protected:
//    void updateVBO();

    void storeVerticesInfo();

	void cb_redraw();

	void cb_initGL();

	void cb_mousePress(int button, int x, int y);

// slots locaux
public slots:
	void balls_onoff(bool x);
	void vectors_onoff(bool x);
	void text_onoff(bool x);
	void topo_onoff(bool x);

	void slider_balls(int x);
	void slider_vectors(int x);
	void slider_text(int x);



};

#endif

