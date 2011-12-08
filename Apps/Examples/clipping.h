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

#ifndef __CLIPPING_H__
#define __CLIPPING_H__

#include <iostream>

#include "Utils/Qt/qtSimple.h"
#include "Utils/Qt/qtInputs.h"

#include "ui_clipping.h"
// inclure qtui.h juste après le ui_xxx.h
#include "Utils/Qt/qtui.h"

#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap3.h"

#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"
#include "Geometry/bounding_box.h"

#include "Algo/Import/import.h"
//#include "Algo/Export/export.h"

#include "Algo/Geometry/boundingbox.h"
#include "Algo/Render/GL2/mapRender.h"
#include "Algo/Render/GL2/topo3Render.h"
#include "Utils/Shaders/shaderSimpleColor.h"

#include "Utils/frameManipulator.h"

#include "Utils/clippingPresets.h"
#include "Utils/clippingPresetsAnimated.h"

#include "Utils/cgognStream.h"
#include "Utils/drawer.h"

#include "Algo/Modelisation/primitives3d.h"

#include <string>
#include <sstream>

using namespace CGoGN ;

struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef EmbeddedMap3 MAP;
};


class Clipping: public Utils::QT::SimpleQT
{
	Q_OBJECT

public:
	typedef PFP::MAP MAP;
	typedef PFP::TVEC3 TVEC3;

	//Manip Carte
	MAP myMap ;
	TVEC3 position;
	Dart dglobal;

	//Render
	bool m_drawVertices;
	bool m_drawLines;
	bool m_drawFaces;
	bool m_drawTopo;

	Algo::Render::GL2::MapRender* m_render;
	Algo::Render::GL2::Topo3RenderMapD* m_render_topo;

	Geom::BoundingBox<PFP::VEC3> m_bb;

	Utils::VBO* m_positionVBO;

	Utils::ShaderSimpleColor* m_shader;

	Utils::Drawer* influence_area;
	Utils::Drawer* chips_area;

	Geom::Vec3f m_coeffTopoExplod;
	Geom::Vec3f gPosObj ;
	float gWidthObj ;

	//QT
	Utils::QT::uiDockInterface dock;
	QTimer *m_timer;

	// Picking
	Utils::LineDrawable* m_planeDrawable;
	Utils::LineDrawable* m_sphereDrawable;
	Utils::FrameManipulator* m_frameManipulator;
	unsigned int m_frameManipulatorPickedAxis;
	std::vector<Utils::Pickable*> m_pickablePlanes;
	std::vector<Utils::Pickable*> m_pickableSpheres;
	Utils::Pickable* m_lastPickedObject;
	int m_lastClickedX, m_lastClickedY;

	// Clipping
	Utils::ClippingPresetAnimated *m_lastAnimatedClippingPreset;

	Clipping();

	void initGUI();
	void cb_Open();
	void updateVBOprimitives(int upType);
	void cb_initGL();
	void cb_redraw();
	void cb_mousePress(int button, int x, int y);
	void cb_mouseMove(int buttons, int x, int y);
	void cb_keyPress(int code);
	void importMesh(std::string& filename);
	void updatePickables();

public slots:
	void slot_drawVertices(bool b);
	void slot_drawLines(bool b);
	void slot_drawFaces(bool b);
	void slot_drawTopo(bool b);

	void slot_explodTopoPhi1(double c);
	void slot_explodTopoPhi2(double c);
	void slot_explodTopoPhi3(double c);

	void slot_pushButton_addPlane();

	void slot_spinBox_GridResolution(int i);
	void slot_pushButton_changePlanesColor();

	void slot_pushButton_addSphere();

	void slot_spinBox_SphereResolution(int i);
	void slot_pushButton_changeSpheresColor();

	void slot_doubleSpinBox_ColorAttenuationFactor(double c);
	void slot_radioButton_ClippingMode(bool b);
	void slot_radioButton_ColorAttenuationMode(bool b);

	void slot_pushButton_deleteSelectedObject();

	void slot_pushButton_applyStaticClippingPreset();

	void slot_pushButton_applyAnimatedClippingPreset();
	void slot_pushButton_StopAnimation();
	void slot_doubleSpinBox_AnimatedClippingPresetSpeed(double c);

	void slot_setAnimationState(bool state);

	void slot_animationTimer();

	void button_compile();
};

#endif
