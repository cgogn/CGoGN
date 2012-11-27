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

#ifndef __CGoGN_GLSL_FBO__
#define __CGoGN_GLSL_FBO__

#include "Utils/gl_def.h"

#include <iostream>
#include <vector>

namespace CGoGN
{

namespace Utils
{

class FBO
{
public:
	/// default constructor
    FBO();

    /**
     * constructor with size of frame buffer
     * @param width width in pixel
     * @param height heigt in pixel
     */
    FBO(unsigned int width, unsigned int height);

    /// destructor
    ~FBO();

    /**
     *
     */
    void attachRender(GLenum internalformat);

    /**
     *
     */
    void attachTexture(GLenum internalformat, GLint filter = GL_LINEAR);

    void bindTexInput();
    void bindTexInput(int num);

    void bindTexOutput();
    void bindTexOutput(int num);

    void unbind();

    void setSize(unsigned int width, unsigned int height)
    {
        m_width = width;
        m_height = height;
    }

    void checkFBO();

protected:
    unsigned int m_width;
    unsigned int m_height;

    int m_maxColorAttachments;

    CGoGNGLuint m_fboID;
    CGoGNGLuint m_renderID;
    std::vector<CGoGNGLuint> m_texID;


    CGoGNGLenumTable m_attachmentPoints;

};

}
}

#endif	/* FRAMEBUFFER_HPP */
