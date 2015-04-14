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
* Web site: http://cgogn.unistra.fr                                            *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include <iostream>

#include "Utils/Qt/qtSimple.h"
#include "ui_mcmesh.h"
#include "Utils/Qt/qtui.h"

#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"

#include "Algo/MC/marchingcube.h"

#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"

#include "Algo/Render/GL2/mapRender.h"
#include "Utils/Shaders/shaderFlat.h"
#include "Utils/Shaders/shaderSimpleColor.h"
#include "Utils/Shaders/shaderBoldLines.h"
#include "Utils/vbo.h"
#include "Algo/Geometry/boundingbox.h"

#include "Utils/drawer.h"

using namespace CGoGN ;

namespace SAlgo = ::CGoGN::Algo::Surface;

struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef EmbeddedMap2 MAP ;
};

typedef PFP::MAP MAP ;
typedef PFP::VEC3 VEC3 ;

typedef unsigned char DATATYPE;

class MCMesh : public Utils::QT::SimpleQT
{
	Q_OBJECT

public:
	MAP myMap ;

    Utils::QT::uiDockInterface dock ;

	float shininess ;

	Geom::BoundingBox<PFP::VEC3> bb ;

	bool m_drawEdges ;
	bool m_drawFaces ;

	VertexAttribute<VEC3, MAP> position ;

	Algo::Render::GL2::MapRender* m_render ;

	Utils::VBO* m_positionVBO ;
	Utils::VBO* m_colorVBO ;
	Utils::ShaderFlat* m_flatShader ;
	Utils::ShaderSimpleColor* m_simpleColorShader ;

	Utils::ShaderBoldLines* m_linesShader ;

	Utils::Drawer* m_dr;

	DATATYPE valLabel;
	SAlgo::MC::Image<DATATYPE>* myImg;

	MCMesh() ;

	void initGUI();
	void updateRender();
	void MC();

	void cb_initGL() ;
	void cb_redraw() ;
	void cb_Open() ;
	
	void fromFile(char* fname);
	void sphere();

public slots:
	void slot_drawEdges(bool b) ;
	void slot_drawFaces(bool b) ;
};
