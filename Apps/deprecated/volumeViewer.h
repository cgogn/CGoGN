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

#ifndef __VOLUMEVIEWER_H__
#define __VOLUMEVIEWER_H__

#include <iostream>

#include "Utils/glutwin_atb.h"

#include "Topology/map/map3.h"

#include "Algo/Selection/raySelector.h"

#include "Algo/Render/vbo_MapRender.h"


using namespace CGoGN;

struct PFP: public PFP_STANDARD
{
	// definition de la carte
	typedef Map3 MAP;

};



PFP::MAP myMap;
PFP::TVEC3 position ;


unsigned int skipCount = 0 ;


class MyGlutWin : public Utils::GlutWin_ATB
{
public:
	gmtl::Vec4f * colDif ;
	gmtl::Vec4f colSpec ;
	gmtl::Vec4f colClear ;
	gmtl::Vec4f colNormal ;
	float shininess ;
	gmtl::Vec3f gPosObj ;
	float gWidthObj ;
	float normalBaseSize ;
	float normalScaleFactor ;
	std::vector<gmtl::Vec3f> vNormals ;
	bool vNormals_dirty ;

	//view and select
	enum { FLAT, GOURAUD, PHONG, LINE, NORMAL } ;
	enum { VERTEX, EDGE, FACE} ;
	bool invertedNormals ;
	bool invertedObject ;
	bool renderAxis ;
	bool renderObject ;
	bool renderNormals ;
	bool renderLines ;
	bool renderHelp ;
	int renderStyle ;
	bool viewVisible;
	GLuint dl_obj ;
	GLuint dl_line ;
	GLuint dl_norm ;

	TwBar* viewer_bar ;
	TwBar* object_bar ;

	int nbVolumes;
	bool * volumeVisible;

	std::string fileName;

	SelectorUnmarked<PFP::MAP>* activeDartsSelector ;

	int objetCourant;

	MyGlutWin(int* argc, char **argv, int winX, int winY) :
		GlutWin_ATB(argc, argv, winX, winY),
		vNormals_dirty(true),
		invertedNormals(false),
		invertedObject(false),
		renderAxis(true),
		renderObject(true),
		renderNormals(false),
		renderLines(false),
		renderHelp(true),
		renderStyle(FLAT),
		viewVisible(true),
		nbVolumes(0)
	{
		dl_obj = glGenLists(1) ;
		dl_line = glGenLists(1) ;
		dl_norm = glGenLists(1) ;
		if(this->shaderOk)
			shaders[0].loadShaders("phong_vs.txt", "phong_ps.txt") ;

		objetCourant=-1;
		object_bar=NULL;
	}

	void initGUI() ;

	void myRedraw() ;
	void myKeyboard(unsigned char keycode, int x, int y) ;
	void myMouse(int button, int state, int x, int y) ;
	void myMotion(int x, int y) ;

	void updateDisplay() ;
	void initDL(PFP::MAP::Dart d, int volCur) ;
	void initDLNormals(PFP::MAP::Dart d, int volCur) ;
	void initDLLines(PFP::MAP::Dart d, int volCur) ;
	void render(int renderMode, PFP::MAP::Dart d) ;
	void renderEff(int renderMode, PFP::MAP::Dart d) ;

	gmtl::Vec3f computeFaceNormale(PFP::MAP::Dart d, bool inverted);
	gmtl::Vec3f computeVertexNormale(PFP::MAP::Dart d, bool inverted);
	void processObject() ;
	void selectObject(PFP::MAP::Dart d);
	void updateObjectBar();
	void changeCurrentObjectVisibility();
} ;

#endif /* __VOLUMEVIEWER_H__ */
