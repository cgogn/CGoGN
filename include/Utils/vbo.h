/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2011, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.u-strasbg.fr/                                         *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#ifndef __CGoGN_GLSL_VBO__
#define __CGoGN_GLSL_VBO__

#include <vector>
#include <GL/glew.h>

#include "Topology/generic/attributeHandler.h"
#include "Container/convert.h"

namespace CGoGN
{

namespace Utils
{

class GLSLShader;

/**
 * Encapsulation of OpenGL Vertex Buffer Object
 * Manage
 * - alloc /release of GL buffer
 * - ref by Shaders
 * - size of data (invidual cells)
 */
class VBO
{
protected:
	// VBO id
	GLuint m_id;
	// size of data (in floats)
	unsigned int m_data_size;
	// shaders that ref this vbo
	std::vector<GLSLShader*> m_refs;
	unsigned int m_nbElts;
	mutable bool m_lock;

public:
	/**
	 * constructor: allocate the OGL VBO
	 */
	VBO();

	/**
	 * copy constructor, new VBO copy content
	 */
	VBO(const VBO& vbo);

	/**
	 * destructor: release the OGL VBO and clean references between VBO/Shaders
	 */
	~VBO();

	/**
	 * get id of vbo
	 */
	unsigned int id() const { return m_id; }

	/**
	 * get dataSize
	 */
	unsigned int dataSize() const { return m_data_size; }

	/**
	 * set the data size (in number of float)
	 */
	void setDataSize(unsigned int ds) { m_data_size = ds; }

	/**
	 * get nb element in vbo (vertices, colors ...)
	 */
	unsigned int nbElts() {return m_nbElts;}

	/**
	 * bind array vbo
	 */
	void bind() const  { glBindBuffer(GL_ARRAY_BUFFER,m_id); }

	/**
	 * alloc buffer of same size than parameter
	 */
	void sameAllocSameBufferSize(const VBO& vbo);

	/**
	 * update data from attribute handler to the vbo
	 */
	template <typename ATTR_HANDLER>
	void updateData(const ATTR_HANDLER& attrib);

	/**
	 * update data from attribute handler to the vbo, with conversion
	 */
	template <typename ATTR_HANDLER>
	void updateData(const ATTR_HANDLER& attrib, ConvertAttrib* conv);

	void* lockPtr();

	const void* lockPtr() const;

	void releasePtr() const;

	void copyData(void *ptr) const;



	void allocate(unsigned int nbElts);
};

} // namespace Utils

} // namespace CGoGN

#include "Utils/vbo.hpp"

#endif
