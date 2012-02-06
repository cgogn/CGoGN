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
#ifndef __SHOW_TRAVERSORS_
#define __SHOW_TRAVERSORS_


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
#include "Utils/drawer.h"


#include "Utils/Qt/qtSimple.h"

#include "ui_show_traversors.h"
// inclure qtui.h juste après le ui_xxx.h
#include "Utils/Qt/qtui.h"


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

	Algo::Render::GL2::Topo3Render* m_render_topo;
	bool m_showTopo;

	
	unsigned int m_val_combo1;
	unsigned int m_val_combo2;
	unsigned int m_val_combo3;
	unsigned int m_val_combo4;
	unsigned int m_val_combo5;
	unsigned int m_val_combo6;
public:
	MyQT():
		m_render_topo(NULL),
		m_showTopo(true),
		m_val_combo1(1),
		m_val_combo2(1),
		m_val_combo3(2),
		m_val_combo4(1),
		m_val_combo5(1),
		m_val_combo6(2),
		m_selected(NIL)

	{}

	Dart m_selected;
	std::vector<Dart> m_affDarts;

	Utils::Drawer m_drawer;

protected:
    void storeVerticesInfo();

	void cb_redraw();

	void cb_initGL();

	void cb_mousePress(int button, int x, int y);

	void colorizeCell(Dart d, unsigned int orbit, float r,float g, float b);

	void traverse2();
	void traverse3();

// slots locaux
public slots:
	void cb_combo1(int x);
	void cb_combo2(int x);
	void cb_combo3(int x);

	void cb_combo4(int x);
	void cb_combo5(int x);
	void cb_combo6(int x);
	void cb_checkTopo(bool b);
};

#endif
