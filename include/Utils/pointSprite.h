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

#ifndef __POINT_SPRITE_3D__
#define __POINT_SPRITE_3D__

#include "Utils/GLSLShader.h"
#include "Geometry/vector_gen.h"

namespace CGoGN { namespace Utils { class VBO; } }

namespace CGoGN
{

namespace Utils
{

class PointSprite: public Utils::GLSLShader
{
protected:
	static const unsigned int WIDTHSPRITE = 64;

	static std::string vertexShaderText;

	static std::string geometryShaderText;

	static std::string fragmentShaderText;

	static GLuint m_idTexture;

	static GLuint m_uniform_texture;

	static unsigned char* m_ptrSphere;

	// compute the sphere instead of storing it in image file.
	static void computeSphere();

	GLuint m_uniform_size;

	GLuint m_uniform_color;

public:
	/**
	 * init shaders, texture and variables
	 * @param withColorPerVertex if true use setAttributeColor for per vertex color, else use predraw(color) for global color
	 * @param radius of sphere
	 */

	PointSprite(bool withColorPerVertex=false, float radius=1.0f);

	/**
	 * clean shaders, texture and variables
	 */
	~PointSprite();

	/**
	 * call once before sending points to gpu
	 * @param color set global color of sprites
	 */
	void predraw(const Geom::Vec3f& color);

	/**
	 * call once before sending points to gpu
	 */
	void predraw();

	/**
	 * call once after sending points to gpu
	 */
	void postdraw();

	/**
	 * set the radius of sphere
	 * @param radius
	 */
	void setSize(float radius);

	/**
	 * set position attribute
	 */
	unsigned int setAttributePosition(VBO* vbo);

	/**
	 * set color attribute
	 */
	unsigned int setAttributeColor(VBO* vbo);
};

} // namespace Utils

} // namespace CGoGN
#endif
