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


#ifndef __GL_MATRICES_H_
#define __GL_MATRICES_H_

#include "glm/glm.hpp"
#include <stack>

namespace CGoGN
{
namespace Utils
{

class GL_Matrices
{
public:
	/// 0: projection / 1: modelView /2: transfo
	glm::mat4 m_matrices[3];
	/// stack of transfo matrix
	std::stack<glm::mat4> m_stack;

	void pushTransfo() { m_stack.push(m_matrices[2]); }
	void popTransfo() { if (m_stack.empty()) return;  m_matrices[2] = m_stack.top(); m_stack.pop(); }
};


}
}

#endif
