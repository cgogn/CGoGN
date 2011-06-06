/*
 * stage_shader.h
 *
 *  Created on: 6 juin 2011
 *      Author: untereiner
 */
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
#include "Utils/shaderSimpleColor.h"

#include "Utils/cgognStream.h"
#include "Utils/drawer.h"

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

	void dockWidget_topLevelChanged(bool isFloating);

	void button_compile();
};

#endif
