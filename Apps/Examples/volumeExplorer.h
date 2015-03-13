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

#ifndef __VOLUME_EXPLORER__
#define __VOLUME_EXPLORER__

#include <iostream>

#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap3.h"

#include "Geometry/vector_gen.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Render/GL2/topo3Render.h"
#include "Algo/Render/GL2/explodeVolumeRender.h"

#include "Utils/cgognStream.h"
#include "Utils/Qt/qtQGLV.h"
#include "Utils/frameManipulator.h"

#include "ui_volumeExplorer.h"
#include "Utils/Qt/qtui.h"

#include "Algo/Export/exportVol.h"
#include "Utils/chrono.h"

using namespace CGoGN ;

struct PFP: public PFP_STANDARD
{
	// definition de la carte
	typedef EmbeddedMap3 MAP;
};

typedef PFP::MAP MAP;
typedef PFP::VEC3 VEC3;
typedef PFP::REAL REAL;

using namespace CGoGN ;

/**
 * Utilisation de designer-qt4:
 * Faire un DockWiget (laisser le nom par defaut
 * dans le Contents ajouter le layout choisi (vertical classiquement)
 * Ajouter les widgets necessaires, mettre des noms clairs pour
 * les utiliser dans le .cpp (pour les call back principalement)
 */
class MyQT: public Utils::QT::SimpleQGLV
{
	Q_OBJECT

    bool render_volumes;
	bool render_edges;
    bool render_topo;
    bool render_topoTemp;
    bool clip_volume;
    bool hide_clipping;

	Algo::Render::GL2::Topo3RenderMap<PFP>* m_topo_render;
	Algo::Render::GL2::ExplodeVolumeRender* m_explode_render;

	float m_explode_factor;
	float m_explode_factorf;

	// for clipping plane manipulation
	Utils::Pickable* m_PlanePick;
	Utils::FrameManipulator* m_frame;
	unsigned int m_pickedAxis;
	int m_begX;
	int m_begY;
	int clip_id1;
	int clip_id2;

	// shader of toporender3
//	Utils::ClippingShader* m_sh1;
//	Utils::ClippingShader* m_sh2;
	Utils::Chrono m_frame_ch;
	unsigned int m_nbFrames;

public:
	float m_WidthObj;
	Geom::Vec3f m_PosObj;

public:
	MyQT():
		render_volumes(true),
		render_edges(true),
		render_topo(true),
		render_topoTemp(true),
		clip_volume(true),
		hide_clipping(false),
		m_topo_render(NULL),
		m_explode_render(NULL),
		m_explode_factor(0.8f),
		m_nbFrames(0)
	{}

protected:
	void cb_redraw();
	void cb_initGL();
	void cb_mouseMove(int buttons, int x, int y);
	void cb_mousePress(int button, int x, int y);
	void cb_mouseRelease(int button, int x, int y);
	void cb_Open();
	void cb_Save();

// slots locaux
public slots:
	void volumes_onoff(bool x);
	void edges_onoff(bool x);
	void topo_onoff(bool x);
	void clipping_onoff(bool x);
	void hide_onoff(bool x);
	void slider_explode(int x);
	void slider_pressed();
	void slider_released();
	void slider_explodeF(int x);
};

#endif // __VOLUME_EXPLORER__
