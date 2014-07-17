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

#ifndef __CGoGN_GLSL_VBO__
#define __CGoGN_GLSL_VBO__


#include <vector>
#include "Utils/gl_def.h"
#include "Container/convert.h"
#include "Topology/generic/attributeHandler.h"

namespace CGoGN
{

namespace Utils
{

class GLSLShader;

/**
 * Encapsulation of OpenGL Vertex Buffer Object
 * Manage
 * - alloc / release of GL buffer
 * - ref by Shaders
 * - size of data (invidual cells)
 */
class VBO
{
protected:
	// VBO id
	CGoGNGLuint m_id;

	// size of data (in floats)
	unsigned int m_data_size;

	// shaders that ref this vbo
	std::vector<GLSLShader*> m_refs;

	unsigned int m_nbElts;
	mutable bool m_lock;

	/// name of the last attribute used to fill the VBO
	std::string m_name;

	/// type name of the last attribute used to fill the VBO
	std::string m_typeName;

	/// pointer to buffer converter
	ConvertBuffer* m_conv;


	/**
	 * update data from AttributeMultiVectorGen to the vbo, with conversion
	 */
	void updateData_withConversion(const AttributeMultiVectorGen* attrib, ConvertBuffer* conv);

public:
	/**
	 * constructor: allocate the OGL VBO
	 */
	VBO(const std::string& name = "");

	/**
	 * constructor with buffer converter: allocate the OGL VBO
	 */
	VBO(ConvertBuffer* conv, const std::string& name = "");

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
	GLuint id() const { return *m_id; }

	/**
	 * get dataSize
	 */
	unsigned int dataSize() const { return m_data_size; }

	/**
	 * get name
	 */
	const std::string& name() const { return m_name; }

	/**
	 * get type name
	 */
	const std::string& typeName() const { return m_typeName; }

	/**
	 * set the data size (in number of float)
	 */
	void setDataSize(unsigned int ds) { m_data_size = ds; }

	/**
	 * get nb element in vbo (vertices, colors ...)
	 */
	unsigned int nbElts() { return m_nbElts; }

	/**
	 * bind array vbo
	 */
	void bind() const  { glBindBuffer(GL_ARRAY_BUFFER, *m_id); }

	/**
	 * alloc buffer of same size than parameter
	 */
	void sameAllocSameBufferSize(const VBO& vbo);

	/**
	 * update data from attribute handler to the vbo
	 */
	inline void updateData(const AttributeHandlerGen& attrib)
	{
		updateData(attrib.getDataVectorGen()) ;
	}

	void updateData(const AttributeMultiVectorGen* attrib);

//	template <typename T_IN, typename T_OUT, unsigned int VS, typename CONVFUNC>
//	void updateData_withLambdaConversion(const AttributeHandlerGen& attribHG, CONVFUNC conv);

	/**
	 * set the converter that convert buffer to float *
	 */
	void setBufferConverter(ConvertBuffer* conv);

	/**
	 * update data from given data vector
	 * @warning use only with include vbo.h (not vbo_base.h)
	 */
	template <typename T>
	void updateData(std::vector<T>& data);

	void* lockPtr();

	const void* lockPtr() const;

	void releasePtr() const;

	void copyData(void *ptr) const;

	void allocate(unsigned int nbElts);


	template <typename T_IN, typename T_OUT, unsigned int VS, typename CONVFUNC>
	void updateData_withLambdaConversion(const AttributeHandlerGen& attribHG, CONVFUNC conv)
	{
		const AttributeMultiVectorGen* attrib = attribHG.getDataVectorGen();

		m_name = attrib->getName();
		m_typeName = attrib->getTypeName();
		m_data_size = VS;

		// alloue la memoire pour le buffer et initialise le conv
		T_OUT* typedBuffer = new T_OUT[_BLOCKSIZE_];

		std::vector<void*> addr;
		unsigned int byteTableSize;
		unsigned int nbb = attrib->getBlocksPointers(addr, byteTableSize);

		m_nbElts = nbb * _BLOCKSIZE_/(VS*sizeof(T_OUT));

		// bind buffer to update
		glBindBuffer(GL_ARRAY_BUFFER, *m_id);
		glBufferData(GL_ARRAY_BUFFER, nbb * _BLOCKSIZE_, 0, GL_STREAM_DRAW);

		unsigned int offset = 0;
		unsigned int szb = _BLOCKSIZE_*sizeof(T_OUT);

		for (unsigned int i = 0; i < nbb; ++i)
		{
			// convertit les donnees dans le buffer de conv
			const T_IN* typedIn = reinterpret_cast<const T_IN*>(addr[i]);
			T_OUT* typedOut = typedBuffer;
			// compute conversion
			for (unsigned int i = 0; i < _BLOCKSIZE_; ++i)
			{
//				*typedOut++ = conv(*typedIn++);
				std::cout << *typedIn << " -> " ;
				*typedOut = conv(*typedIn++);
				std::cout << *typedOut++ << std::endl;
			}

			// update sub-vbo
			glBufferSubDataARB(GL_ARRAY_BUFFER, offset, szb, typedBuffer);
			// block suivant
			offset += szb;
		}

		// libere la memoire de la conversion
		delete[] typedBuffer;
	}


};



} // namespace Utils

} // namespace CGoGN

#endif
