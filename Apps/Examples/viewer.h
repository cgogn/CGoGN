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
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include <iostream>

//#include "Utils/Qt/qtSimple.h"
#include "Utils/Qt/qtQGLV.h"
#include "ui_viewer.h"
#include "Utils/Qt/qtui.h"

#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"

#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"

#include "Algo/Import/import.h"
#include "Algo/Export/export.h"

#include "Algo/Render/GL2/mapRender.h"
#include "Algo/Render/GL2/topoRender.h"

#include "Utils/Shaders/shaderPhong.h"
#include "Utils/Shaders/shaderSimpleFlat.h"
//#include "Utils/Shaders/shaderFlat.h"
#include "Utils/Shaders/shaderSimpleColor.h"
#include "Utils/Shaders/shaderVectorPerVertex.h"
#include "Utils/pointSprite.h"
#include "Utils/text3d.h"
#include "Utils/vbo.h"
#include "Utils/chrono.h"

#include "Utils/Qt/qtInputs.h"

#include "Algo/Geometry/boundingbox.h"
#include "Algo/Geometry/normal.h"
#include "Algo/Geometry/convexity.h"

using namespace CGoGN ;

struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef EmbeddedMap2 MAP ;
};

typedef PFP::MAP MAP ;
typedef PFP::VEC3 VEC3 ;

//class Viewer : public Utils::QT::SimpleQT
class Viewer : public Utils::QT::SimpleQGLV
{
	Q_OBJECT

public:
	MAP myMap ;

    Utils::QT::uiDockInterface dock ;

	enum renderMode { FLAT, PHONG } ;

	Geom::Vec4f colDif ;
	Geom::Vec4f colSpec ;
	Geom::Vec4f colClear ;
	Geom::Vec4f colNormal ;

	float shininess ;

	Geom::BoundingBox<PFP::VEC3> bb ;
	float normalBaseSize ;
	float normalScaleFactor ;
	float vertexBaseSize ;
	float vertexScaleFactor ;
	float faceShrinkage ;

	int m_renderStyle ;
	bool m_drawVertices ;
	bool m_drawEdges ;
	bool m_drawFaces ;
	bool m_drawNormals ;
	bool m_drawTopo ;
	bool m_drawBoundaryTopo;

	VertexAttribute<VEC3, MAP> position ;
	VertexAttribute<VEC3, MAP> normal ;

	Algo::Render::GL2::MapRender* m_render ;
	Algo::Render::GL2::TopoRender* m_topoRender ;

	Utils::VBO* m_positionVBO ;
	Utils::VBO* m_normalVBO;

	Utils::ShaderPhong* m_phongShader ;
	Utils::ShaderSimpleFlat* m_flatShader ;
	Utils::ShaderVectorPerVertex* m_vectorShader ;
	Utils::ShaderSimpleColor* m_simpleColorShader ;
	Utils::PointSprite* m_pointSprite ;

	Utils::Chrono m_frame_ch;
	unsigned int m_nbFrames;

	Viewer() ;

	void initGUI() ;

	void cb_initGL() ;
	void cb_redraw() ;
	void cb_Open() ;
	void cb_Save() ;

	void cb_keyPress(int keycode);

	void importMesh(std::string& filename) ;
	void exportMesh(std::string& filename, bool askExportMode = true);

public slots:
	void slot_drawVertices(bool b) ;
	void slot_verticesSize(int i) ;
	void slot_drawEdges(bool b) ;
	void slot_drawFaces(bool b) ;
	void slot_faceLighting(int i) ;
	void slot_drawTopo(bool b) ;
	void slot_drawNormals(bool b) ;
	void slot_normalsSize(int i) ;
};
