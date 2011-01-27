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

#include "xmap/exmap.h"
#include "point3d.h"
#include "export/export.h"

#include "Algo/Render/helpRender.h"
#include "Algo/Render/map_glRender.h"

using namespace CGoGN ;

class XMAP_V0 : public DP::DefaultDartParameters
{
public:
	static const unsigned nbInvolutions = 2 ;
	static const unsigned nbPermutations = 2 ;
} ;

struct PFP
{
	typedef DartObj<XMAP_V0> DART ;
	typedef e0xmap<DART> MAP ;
	typedef Emb::Point3D EMB ;
};

PFP::MAP myMap ;
PFP::EMB P ;

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
	std::vector<gmtl::Vec3f> vNormals ;
	bool vNormals_dirty ;

	enum { FLAT, GOURAUD, PHONG, LINE, NORMAL } ;
	bool renderObject ;
	bool renderNormals ;
	bool renderLines ;
	bool renderHelp ;
	int renderStyle ;
	bool selecting ;
	bool deselecting ;
	Marker selectionMarker ;
	std::vector<PFP::MAP::Dart> selected_vertices ;
	bool dragging ;
	GLuint dl_obj ;
	GLuint dl_line ;
	GLuint dl_norm ;

	TwBar* viewer_bar ;

	MyGlutWin(int* argc, char **argv, int winX, int winY) ;

	void initGUI() ;

	void myRedraw() ;
	void myKeyboard(unsigned char keycode, int x, int y) ;

	void initDL() ;
	void initDLNormals() ;
	void initDLLines() ;
	void render(int renderMode) ;

	void selectVertices(PFP::MAP::Dart d, float dist) ;
	void selectVertex(PFP::MAP::Dart d) ;
	void deselectVertex(PFP::MAP::Dart d) ;
	void deselectAllVertices() ;

	void saveOriginalPositions() ;
	void restoreOriginalPositions() ;
	void updateVerticesDistance2() ;
	void updateVerticesColor(float min, float max) ;
	void computeDistanceToOriginal() ;
} ;
