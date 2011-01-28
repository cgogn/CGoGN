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

#include "Topology/map/map2.h"
#include "Topology/generic/embeddedMap2.h"

#include "Algo/Export/export.h"
#include "Algo/Import/import.h"

#include "Algo/Geometry/boundingbox.h"
#include "Algo/Render/map_glRender.h"
#include "Algo/Render/vbo_MapRender.h"
#include "Topology/generic/ecell.h"
#include "Algo/ProgressiveMesh/pmesh.h"

using namespace CGoGN ;

struct PFP
{
	// definition of the map
	typedef EmbeddedMap2<Map2> MAP;

	// definition of the type of real value
	typedef float REAL;

	// other types definitions
	typedef Geom::Vector<3,REAL> VEC3;
	typedef Geom::Vector<6,REAL> VEC6;
	typedef Geom::Matrix<3,3,REAL> MATRIX33;
	typedef Geom::Matrix<4,4,REAL> MATRIX44;
	typedef Geom::Matrix<3,6,REAL> MATRIX36;

	typedef AttributeHandler<VEC3> TVEC3;
	typedef AttributeHandler<REAL> TREAL;
};

typedef PFP::MAP MAP;

MAP myMap ;
SelectorTrue allDarts ;

class MyGlutWin : public Utils::GlutWin_ATB
{
public:
	TwBar* viewer_bar ;

	enum renderMode { FLAT, GOURAUD, PHONG, LINES, NORMALS } ;

	Geom::Vec4f colDif ;
	Geom::Vec4f colSpec ;
	Geom::Vec4f colClear ;
	Geom::Vec4f colNormal ;

	float shininess ;

	PFP::VEC3 gPosObj ;
	float gWidthObj ;
	float normalBaseSize ;
	float normalScaleFactor ;
	float faceShrinkage ;

	int renderStyle ;
	bool renderObject ;
	bool renderLines ;
	bool renderNormals ;

	PFP::TVEC3 position ;
	PFP::TVEC3 originalPosition ;
	PFP::TVEC3 normal ;
	AttributeHandler<Geom::Vec4f> color ;

	Algo::Render::VBO::MapRender_VBO<PFP>* vbo_render ;
	bool usePrecomputedTableIndices ;
	std::vector<GLuint> triangleTableIndices[101] ;
	std::vector<GLuint> lineTableIndices[101] ;
	GLuint dl_norm ;

	CellMarker* selectionMarker ;
	bool selecting ;
	bool deselecting ;
	bool dragging ;
	std::vector<unsigned int> selected_vertices ;

	Algo::PMesh::ProgressiveMesh<PFP>* m_pmesh ;
	unsigned int remainingVertices, currentLevel, nbQuantizationClasses ;

	Algo::Decimation::SelectorType selector ;
	Algo::Decimation::ApproximatorType approximator ;

	DartMarker inactiveMarker ;
	SelectorUnmarked* activeDartsSelector ;

	CellMarker* origResVertexMarker ;
	PFP::REAL meshDistanceToOriginal ;
	bool distanceColoredMesh ;

	MyGlutWin(int* argc, char **argv, int winX, int winY) ;

	void init() ;
	void initGUI() ;

	void start_PM() ;
	void stop_PM() ;

	void myRedraw() ;
	void myKeyboard(unsigned char keycode, int x, int y) ;

//	void evaluateScheme(Algo::PMesh::ApproximatorType type, const char* filename) ;

	void computeVBOprimitives() ;
	void resetVBOprimitives() ;
	void updateVBOprimitives(int upType) ;
	void updateVBOdata(int upType, bool recompute = true) ;
	void initDLNormals() ;
	void render(int renderMode) ;
	void renderDetailVectors() ;

	void saveOriginalPositions()
	{
		myMap.copyAttribute(originalPosition, position) ;
	}
	void restoreOriginalPositions()
	{
		myMap.swapAttributes(originalPosition, position) ;
	}
} ;
