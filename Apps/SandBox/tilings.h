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

#ifndef TUTO_TILINGS_H
#define TUTO_TILINGS_H

#include <iostream>

#include "Utils/Qt/qtSimple.h"

#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"

#include "Geometry/vector_gen.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Render/GL2/mapRender.h"

#include "Utils/Shaders/shaderSimpleColor.h"
#include "Utils/Shaders/shaderVectorPerVertex.h"
#include "Utils/vbo.h"
#include "Utils/cgognStream.h"

#include "Algo/Tiling/Surface/square.h"
#include "Algo/Tiling/Surface/triangular.h"
#include "Algo/Tiling/Surface/hexagonal.h"

// forward definitions (minimize includes)
namespace CGoGN { namespace Algo { namespace Render { namespace GL1 { class MapRender; } } } }
namespace CGoGN { namespace Utils { class VBO; } }
namespace CGoGN { namespace Utils { class ShaderSimpleColor; } }

using namespace CGoGN ;

/**
 * A class for a little interface and rendering
 */
class MyQT: public Utils::QT::SimpleQT
{
    Q_OBJECT

public:
    Algo::Render::GL2::MapRender* m_render;

    Utils::VBO* m_positionVBO;
    Utils::VBO* m_normalVBO;

    Utils::ShaderSimpleColor* m_shader;
    Utils::ShaderVectorPerVertex* m_lines;



    MyQT():
        m_render(NULL),
        m_positionVBO(NULL),
        m_normalVBO(NULL),
        m_shader(NULL),
        m_lines(NULL)
    {}

	void squareTiling(int code);
	void triangularTiling(int code);
    void hexagonalTiling(int code);

protected:

    void cb_redraw();

    void cb_initGL();

    void threadSimple();

    void threadAttrib();

    void threadStorage();



};


#endif // TUTO_TILINGS_H
