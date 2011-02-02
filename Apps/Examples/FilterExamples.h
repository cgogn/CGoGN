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
#include "Utils/os_spec.h"

#include "Utils/glutwin_atb.h"

#include "Topology/generic/parameters.h"
#include "Topology/map/map2.h"
#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"

#include "Algo/Import/import.h"
#include "Algo/Render/map_glRender.h"
#include "Algo/Render/vbo_MapRender.h"
#include "Algo/Geometry/boundingbox.h"
#include "Topology/generic/ecell.h"
//#include "Algo/Geometry/curvature.h"

#include "Algo/Geometry/normal.h"

#include "Algo/Filters2D/filters2D.h"

using namespace CGoGN ;

struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef Map2 MAP;
};


typedef PFP::MAP MAP ;


MAP myMap ;
SelectorTrue allDarts;


class MyGlutWin: public Utils::GlutWin_ATB
{
public:
	TwBar* viewer_bar ;

	enum renderMode { FLAT, GOURAUD, PHONG, NORMAL } ;

	Geom::Vec4f colDif ;
	Geom::Vec4f colSpec ;
	Geom::Vec4f colClear ;
	Geom::Vec4f colNormal ;

	float shininess ;

	Geom::Vec3f gPosObj ;
	float gWidthObj ;
	float normalBaseSize ;
	float normalScaleFactor ;
	float faceShrinkage ;

	int renderStyle ;
	bool renderObject ;
	bool renderLines ;
	bool renderNormals ;

	PFP::TVEC3 position ;
	PFP::TVEC3 position2 ;
	PFP::TVEC3 originalPosition ;
	PFP::TVEC3 normal ;

	PFP::TREAL faceArea ;
	PFP::TVEC3 faceNormal ;
	PFP::TVEC3 faceCentroid ;

	Algo::Render::VBO::MapRender_VBO* vbo_render ;
	GLuint dl_norm ;

	MyGlutWin(int* argc, char **argv, int winX, int winY) ;

	void init() ;
	void initGUI() ;

	void myRedraw() ;
//	void myKeyboard(unsigned char keycode, int x, int y) ;

	void updateVBOdata(int upType) ;
	void initDLNormals() ;
	void render(int renderMode) ;

	void loadOriginalPositions()
	{
		myMap.copyAttribute(position, originalPosition) ;
		updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS) ;
	}
	void addNoise()
	{
		Algo::Filters2D::computeNoise<PFP>(myMap, 33, position, position2, normal) ;
		myMap.swapAttributes(position, position2) ;
		updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS) ;
	}

	void filter(int type)
	{
		switch(type)
		{
			case Algo::Filters2D::AVERAGE :
				Algo::Filters2D::filterAverage<PFP>(myMap, position, position2) ;
				myMap.swapAttributes(position, position2) ;
				break ;
			case Algo::Filters2D::TAUBIN :
				Algo::Filters2D::filterTaubin<PFP>(myMap, position, position2) ;
				break ;
			case Algo::Filters2D::BILATERAL :
				Algo::Filters2D::filterBilateral<PFP>(myMap, position, position2, normal) ;
				myMap.swapAttributes(position, position2) ;
				break ;
			case Algo::Filters2D::AVERAGE_NORMAL :
				Algo::Filters2D::filterAverageNormals<PFP>(myMap, position, position2) ;
				myMap.swapAttributes(position, position2) ;
				break ;
			case Algo::Filters2D::MMSE :
				Algo::Filters2D::filterMMSE<PFP>(myMap, 0.1f, position, position2) ;
				myMap.swapAttributes(position, position2) ;
				break ;
			case Algo::Filters2D::SUSAN :
				Algo::Filters2D::filterSUSAN<PFP>(myMap, 0.35f, position, position2, normal) ;
				myMap.swapAttributes(position, position2) ;
				break ;
			case Algo::Filters2D::TNBA :
				Algo::Filters2D::filterTNBA<PFP>(myMap, 0.1f, 0.35f, position, position2) ;
				myMap.swapAttributes(position, position2) ;
				break ;
			case Algo::Filters2D::VNBA :
				Algo::Filters2D::filterVNBA<PFP>(myMap, 0.01f, 0.35f, position, position2, normal) ;
				myMap.swapAttributes(position, position2) ;
				break ;
		}
		updateVBOdata(Algo::Render::VBO::POSITIONS | Algo::Render::VBO::NORMALS) ;
	}
};
