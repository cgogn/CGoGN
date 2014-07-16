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

#ifndef TUTO_WALLPAPER_H
#define TUTO_WALLPAPER_H

#include <iostream>

#include "Utils/Qt/qtQGLV.h"

#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"

#include "Geometry/vector_gen.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Render/GL2/mapRender.h"

#include "Utils/Shaders/shaderSimpleColor.h"
#include "Utils/vbo.h"
#include "Utils/cgognStream.h"


#include "Utils/Shaders/shaderWallPaper.h"

using namespace CGoGN ;


struct PFP: public PFP_DOUBLE
{
	// definition of the map
	typedef EmbeddedMap2 MAP ;
};

typedef PFP::MAP MAP;
typedef PFP::VEC3 VEC3;

/**
 * A class for a little interface and rendering
 */
class MyQT: public Utils::QT::SimpleQGLV
{
    Q_OBJECT

public:
	MAP myMap ;
	VertexAttribute<VEC3, MAP> position ;

    Algo::Render::GL2::MapRender* m_render;

	// converter for automatic conversion when updating VBOs
	ConvertVec3dToVec3f converterDF;
    Utils::VBO* m_positionVBO;

    Utils::ShaderSimpleColor* m_shader;

	// FOR WALL PAPER
	Utils::Texture<2,Geom::Vec3uc>* m_textureWP;
	Utils::ShaderWallPaper* m_shaderWP;

    MyQT():
        m_render(NULL),
        m_positionVBO(NULL),

        m_shader(NULL),
		m_shaderWP(NULL)
    {}


protected:

    void cb_redraw();

	void cb_initGL();

	void cb_keyPress(int keycode);

	void computeTexture();
};

#endif // TUTO_TILINGS_H
