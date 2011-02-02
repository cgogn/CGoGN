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

#include <iostream>

#include "Utils/glutwin_atb.h"

#include "Utils/os_spec.h"

#include "Utils/GLSLShader.h"
// #include "Utils/glutwin.h"

#include "simulator.h"
#include "env_render.h"

#include "Algo/Export/exportPov.h"
#include "Algo/Render/map_glRender.h"
#include "Algo/Render/vbo_MapRender.h"
// #include "Algo/Render/test_map_RenderVBO.h"
// #include "Algo/Render/test_vbo_pointsColCB.hpp"
// #include "Algo/Render/topo_vboRender.h"
#include "Algo/Render/topo_render.h"

using namespace CGoGN ;

typedef PFP::MAP MAP;

class MyGlutWin : public Utils::GlutWin_ATB
{
public:
	Simulator * sim;
	SelectorTrue allDarts;

	bool b_animate;
	bool render_anim;
	int visu;
	int nbGenerated;

	PFP::VEC3 posToReach;

	MyGlutWin(int* argc, char **argv, int winX, int winY) ;

	bool reachedGoal(Simulator* sim);

	float computeSelectRadius(int x, int y, int pixelRadius);

	bool exportScenePov(PFP::MAP& map, PFP::TVEC3& position, const std::string& filename, PFP::VEC3 cameraPos, PFP::VEC3 cameraLook, PFP::VEC3 translate, float angle_X, float angle_Y, float angle_Z,const FunctorSelect& good = SelectorTrue());

	void animate(void);
	void myRedraw();
	void myKeyboard(unsigned char keycode, int x, int y);
	void myMouse(int button, int state, int x, int y);

	Algo::Render::VBO::MapRender_VBO* m_render;
	Algo::Render::VBO::MapRender_VBO* m_render_line ;
// 	Algo::Render::VBO::topo_MD2_VBORender<PFP>* topo_render ;
// 	Algo::Render::VBO::topo_VBORenderMapD* topo_render ;

	//to count fps
	int frames;
	clock_t nextUpdate;
	bool displayFps;

	TwBar* viewer_bar ;

	void updateUI();
	void init() ;
	void initGUI() ;
} ;
