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
* This library is distributed  in the hope that it will be useful, but WITHOUT *
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

#ifndef __STAGE_SHADER_H__
#define __STAGE_SHADER_H__

#include <iostream>

#include "Utils/qtSimple.h"

#include "ui_stage_shader.h"
// inclure qtui.h juste après le ui_xxx.h
#include "Utils/qtui.h"

#include "Topology/generic/parameters.h"
#include "Topology/map/map3.h"
#include "Topology/generic/embeddedMap3.h"

#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"
#include "Geometry/bounding_box.h"

#include "Algo/Import/import.h"
//#include "Algo/Export/export.h"

#include "Algo/Geometry/boundingbox.h"
#include "Algo/Render/GL2/mapRender.h"
#include "Algo/Render/GL2/topo3Render.h"
#include "Utils/Shaders/shaderSimpleColor.h"

#include "Utils/cgognStream.h"
#include "Utils/drawer.h"

#include <string>
#include <sstream>

using namespace CGoGN ;

struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef EmbeddedMap3<Map3> MAP;
};


class StageShader: public Utils::QT::SimpleQT
{
	Q_OBJECT

public:
	typedef PFP::MAP MAP;
	typedef PFP::TVEC3 TVEC3;

	//Manip Carte
	MAP myMap ;
	SelectorTrue allDarts ;
	TVEC3 position;
	Dart dglobal;

	//Render
	bool m_drawVertices;
	bool m_drawLines;
	bool m_drawFaces;
	bool m_drawTopo;

	Algo::Render::GL2::MapRender* m_render;
	Algo::Render::GL2::Topo3RenderMapD* m_render_topo;

	Utils::VBO* m_positionVBO;

	Utils::ShaderSimpleColor* m_shader;

	Utils::Drawer* influence_area;
	Utils::Drawer* chips_area;

	Geom::Vec3f m_coeffTopoExplod;
	Geom::Vec3f gPosObj ;
	float gWidthObj ;

	//QT
	Utils::QT::uiDockInterface dock;

	StageShader();

	void initGUI();
	void cb_Open();
	void updateVBOprimitives(int upType);
	void cb_initGL();
	void cb_redraw();
	void cb_mousePress(int button, int x, int y);
	void cb_keyPress(int code);
	void importMesh(std::string& filename);

public slots:
	void slot_drawVertices(bool b);
	void slot_drawLines(bool b);
	void slot_drawFaces(bool b);
	void slot_drawTopo(bool b);

	void slot_explodTopoPhi1(double c);
	void slot_explodTopoPhi2(double c);
	void slot_explodTopoPhi3(double c);

	void slot_pushButton_addPlane();
	void slot_pushButton_deletePlane();

	void slot_comboBox_PlaneIndexChanged(int newIndex);

	void slot_doubleSpinBox_Plane(double c);

	void slot_doubleSpinBox_ColorAttenuationFactor(double c);

	void button_compile();
};

#endif
