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
#ifndef __TUTO5_
#define __TUTO5_


#include <iostream>

//#define WITH_GMAP 1

#include "Topology/generic/parameters.h"
#ifdef WITH_GMAP
	#include "Topology/gmap/embeddedGMap3.h"
#else
	#include "Topology/map/embeddedMap3.h"
#endif

#include "Geometry/vector_gen.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Render/GL2/mapRender.h"
#include "Utils/Shaders/shaderSimpleColor.h"

#include "Algo/Render/GL2/topo3Render.h"

#include "Topology/generic/cellmarker.h"
#include "Utils/text3d.h"

#include "Utils/pointSprite.h"
#include "Utils/Shaders/shaderVectorPerVertex.h"
#include "Utils/cgognStream.h"

#include "Algo/Render/GL2/explodeVolumeRender.h"


#include "Utils/Qt/qtSimple.h"

#include "ui_tuto_orbits.h"
// inclure qtui.h juste après le ui_xxx.h
#include "Utils/qtui.h"


using namespace CGoGN ;

struct PFP: public PFP_STANDARD
{
	// definition de la carte
#ifdef WITH_GMAP
	typedef EmbeddedGMap3 MAP;
#else
	typedef EmbeddedMap3 MAP;
#endif
};

typedef PFP::MAP MAP ;
typedef PFP::VEC3 VEC3 ;


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
	Algo::Render::GL2::Topo3Render* m_render_topo;

	Utils::VBO* m_positionVBO;
	Utils::VBO* m_dataVBO;

	Utils::ShaderSimpleColor* m_shader;
	Utils::ShaderVectorPerVertex* m_lines;
	Utils::Strings3D* m_strings;
	Utils::PointSprite* m_sprite;

	Algo::Render::GL2::ExplodeVolumeRender* m_explodeRender;

	AttributeHandlerGen* m_att_orbits[9];

	QTimer *m_timer;
	unsigned int current_orbit;

	
public:
	MyQT():
		render_text(true),
		render_balls(true),
		render_vectors(true),
		render_topo(true),
		m_render(NULL),
		m_render_topo(NULL),
		m_positionVBO(NULL),
		m_dataVBO(NULL),
		m_shader(NULL),
		m_lines(NULL),
		m_strings(NULL),
		m_sprite(NULL),
		m_timer(NULL),
		current_orbit(0)
	{}

	std::vector<Dart> m_selected;
	Dart m_clicked;

	void initMap();

protected:
	template <unsigned int ORBIT>
	void storeVerticesInfo(const AttributeHandler<int, ORBIT>* attrib);

	void storeVerticesInfoGen(unsigned int orb, const AttributeHandlerGen* attrib);

	template <unsigned int ORB>
	void init_att_orb(AttributeHandlerGen* attg);

	void cb_redraw();
	void cb_initGL();
	void cb_mousePress(int button, int x, int y);

// slots locaux
public slots:
	void text_onoff(bool x);
	void slider_text(int x);
	void orbit_list(int x);
};

#endif
