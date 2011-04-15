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

#ifndef __POINT_LINE_3D__
#define __POINT_LINE_3D__

#include "Utils/GLSLShader.h"
#include "Geometry/vector_gen.h"

namespace CGoGN
{
namespace Utils
{

class PointLine : public GLSLShader
{
protected:
    static std::string vertexShaderText;

    static std::string geometryShaderText;

    static std::string fragmentShaderText;

    GLuint m_uniform_scale;

    GLuint m_uniform_color;

public:
    /**
     * init shaders and variables
     */
    PointLine(float scale=1.0f, const Geom::Vec3f& color = Geom::Vec3f(1.0f,1.0f,1.0f));

    /**
	 * set the radius of sphere
	 * @param radius
	 */
	void setScale(float scale);

	/**
	 * set the color
	 */
	void setColor(const Geom::Vec3f& color);

	/**
	 * set attribute for position
	 */
	unsigned int setAttributePosition(VBO* vbo);

	/**
	 * set attribute for data (drawn vector)
	 */
	unsigned int setAttributeData(VBO* vbo);

};
}
}
#endif
