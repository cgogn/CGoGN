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

#include <gmtl/gmtl.h>
#include "Utils/glutwin_atb.h"
#include "trackball.h"

#include "Topology/map/emap2.h"
#include "point3d.h"

#include "Algo/Selection/raySelector.h"
#include "Algo/Modelisation/subdivision.h"
#include "Algo/Modelisation/extrusion.h"

#include "Algo/Render/helpRender.h"
#include "Algo/Render/map_glRender.h"


using namespace CGoGN;


struct PFP
{
	typedef DartObj<DP::MAPD2_V0U1> DART ;
	typedef e0map2<DART> MAP ;
	typedef Emb::Point3D EMB ;
};

PFP::MAP myMap ;
Emb::Point3D P ;

unsigned int skipCount = 0 ;


class MyGlutWin : public Utils::GlutWin_ATB
{
public:
	gmtl::Vec4f colDif ;
	gmtl::Vec4f colSpec ;
	gmtl::Vec4f colClear ;
	gmtl::Vec4f colNormal ;
	float shininess ;
	gmtl::Vec3f gPosObj ;
	float gWidthObj ;
	float normalBaseSize ;
	float normalScaleFactor ;
	float selectionSize;
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
	int selectionStyle ;
	bool selecting ;
	bool deselecting ;
	Marker selectionMarker ;
	bool dragging ;
	GLuint dl_obj ;
	GLuint dl_line ;
	GLuint dl_norm ;

	//modeler options
	bool editMode;
	std::vector<gmtl::Vec3f> newFace;

	//topological options
	bool sewingMode;

	unsigned int remainingVertices ;
	bool quantize ;
	std::vector<gmtl::Vec3f> originalPos ;
	std::vector<gmtl::Vec3f*> dVectors_down ;
	std::vector<gmtl::Vec3f*> dVectors_up ;
	Marker inactiveMarker ;
	SelectorUnmarked<PFP::MAP>* activeDartsSelector ;

	TwBar* file_bar ;
	TwBar* viewer_bar ;
	TwBar* selection_bar ;
	TwBar* primitive_bar;
	TwBar* topology_bar;
	TwBar* transform_bar;

	std::string fileName;

	std::vector<PFP::MAP::Dart> selected_vertices ;

	MyGlutWin(int* argc, char **argv, int winX, int winY) :
		GlutWin_ATB(argc, argv, winX, winY),
		vNormals_dirty(true),
		invertedNormals(false),
		invertedObject(false),
		renderAxis(true),
		renderObject(true),
		renderNormals(false),
		renderLines(false),
		editMode(false),
		selecting(false),
		deselecting(false),
		dragging(false),
		renderHelp(true),
		renderStyle(FLAT),
		selectionStyle(VERTEX)
	{
		dl_obj = glGenLists(1) ;
		dl_line = glGenLists(1) ;
		dl_norm = glGenLists(1) ;
		if(this->shaderOk)
			shaders[0].loadShaders("phong_vs.txt", "phong_ps.txt") ;

		remainingVertices = 10 ;
		quantize = false ;
		inactiveMarker = myMap.getNewMarker() ;
		activeDartsSelector = new SelectorUnmarked<PFP::MAP>(myMap, inactiveMarker) ;
		selectionMarker = myMap.getNewMarker() ;
	}

	void initGUI() ;

	void myRedraw() ;
	void myKeyboard(unsigned char keycode, int x, int y) ;
	void myMouse(int button, int state, int x, int y) ;
	void myMotion(int x, int y) ;

	void updateDisplay() ;
	void initDL() ;
	void initDLNormals() ;
	void initDLLines() ;
	void render(int renderMode) ;
	void renderSelected() ;
	void renderDetailVectors() ;

	void selectVertices(PFP::MAP::Dart d, float dist) ;
	void selectVertex(PFP::MAP::Dart d) ;
	void deselectVertex(PFP::MAP::Dart d) ;
	void deselectAllVertices() ;
} ;
