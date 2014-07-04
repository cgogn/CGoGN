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
#ifndef __SHOW_TRAVERSORS_
#define __SHOW_TRAVERSORS_

#include <iostream>

#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap3.h"

#include "Geometry/vector_gen.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Render/GL2/mapRender.h"
#include "Algo/Render/GL2/topoRender.h"
#include "Topology/generic/cellmarker.h"

#include "Utils/cgognStream.h"
#include "Utils/drawer.h"

#include "Utils/Qt/qtSimple.h"

#include "ui_show_traversors2.h"
// inclure qtui.h juste après le ui_xxx.h
#include "Utils/Qt/qtui.h"


using namespace CGoGN ;

struct PFP: public PFP_STANDARD
{
	// definition de la carte
	typedef EmbeddedMap2 MAP;
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

	Algo::Render::GL2::TopoRenderMap<PFP>* m_render_topo;
	bool m_showTopo;


	unsigned int m_first2;
	unsigned int m_ajd_or_inci2;
	unsigned int m_second2;
	float m_expl;
	unsigned int m_last;
public:
	MyQT():
		m_render_topo(NULL),
		m_showTopo(true),
		m_first2(0),
		m_ajd_or_inci2(0),
		m_second2(1),
		m_expl(0.8f),
		m_last(2),
		m_selected(NIL),
		m_dm_topo(NULL)
	{}

	Dart m_selected;
	std::vector<Dart> m_affDarts;

	Utils::Drawer m_drawer;

	DartMarker<MAP>* m_dm_topo;

	MAP myMap;

	VertexAttribute<VEC3, MAP> position ;

	Dart dglobal;

	void init(char *fname);

protected:
    void storeVerticesInfo();

	void cb_redraw();

	void cb_initGL();

	void cb_Save();

	void cb_mousePress(int button, int x, int y);

	template <unsigned int ORBIT>
	void colorizeCell(Cell<ORBIT> c, float r,float g, float b);

	void traverse2();
	void dynamicMarkOrbit(unsigned int orb);

// slots locaux
public slots:
	void cb_combo4(int x);
	void cb_combo5(int x);
	void cb_combo6(int x);

	void cb_checkTopo(bool b);
	void cb_explode(int x);
};

#endif
