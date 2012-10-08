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
#include "Utils/fbo.h"

namespace CGoGN
{

namespace Utils
{

FBO::FBO()
{
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &m_maxColorAttachments);
    *m_attachmentPoints = new GLenum[m_maxColorAttachments];

    glGenFramebuffers(1, &(*m_fboID));
}

FBO::FBO(unsigned int width, unsigned int height)
{
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &m_maxColorAttachments);
    *m_attachmentPoints = new GLenum[m_maxColorAttachments];

    glGenFramebuffers(1, &(*m_fboID));

    m_width = width;
    m_height = height;
}

FBO::~FBO()
{
    GLuint tex_id;

    for( unsigned int i = 0; i < m_texID.size(); ++i ) {
        tex_id = *(m_texID.at(i));
        glDeleteTextures(1, &tex_id);
    }

    glDeleteFramebuffers(1, &(*m_fboID));
    delete[] *m_attachmentPoints;
}

void FBO::attachRender(GLenum internalformat)
{
    GLenum attachment;
    GLuint renderID;

    if( internalformat == GL_DEPTH_COMPONENT ) {
        attachment = GL_DEPTH_ATTACHMENT;
    }

    else if( internalformat == GL_STENCIL_INDEX ) {
        attachment = GL_STENCIL_ATTACHMENT;
    }

    else if( internalformat == GL_DEPTH_STENCIL ) {
        attachment = GL_DEPTH_STENCIL_ATTACHMENT;
    }

    else {
        std::cout << "FBO::attachRender : Bad Internal Format" << std::endl;
        return;
    }

    glGenRenderbuffers(1, &renderID);
    glBindFramebuffer(GL_FRAMEBUFFER, *m_fboID);
    glBindRenderbuffer(GL_RENDERBUFFER, renderID);
    glRenderbufferStorage(GL_RENDERBUFFER, internalformat, m_width, m_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment,
                              GL_RENDERBUFFER, renderID);

    *m_renderID = renderID;
}

void FBO::attachTexture(GLenum internalformat, GLint filter)
{
    GLenum attachment;
    GLenum format;
    GLenum type;
    GLuint texID;

    if( int(m_texID.size()) == m_maxColorAttachments ) {
        std::cout << "FBO::attachTexture : The number of texture has been exceeded" << std::endl;
        return;
    }

    attachment = GL_COLOR_ATTACHMENT0 + m_texID.size();

    if( internalformat == GL_RGBA ) {
        format = GL_RGBA;
        type = GL_FLOAT;
    }

    if( internalformat == GL_RGB ) {
        format = GL_RGB;
        type = GL_FLOAT;
    }

    if( internalformat == GL_RGBA32F ) {
        format = GL_RGBA;
        type = GL_FLOAT;
    }

    if( internalformat == GL_RGB32F ) {
        format = GL_RGB;
        type = GL_FLOAT;
    }

    glGenTextures(1, &texID);
    glBindFramebuffer(GL_FRAMEBUFFER, *m_fboID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, m_width,
                 m_height, 0, format, type, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);

    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment,
                           GL_TEXTURE_2D, texID, 0);

    m_texID.push_back(CGoGNGLuint(texID));
    (*m_attachmentPoints)[m_texID.size() - 1] = attachment;
}

void FBO::bindTexInput()
{
    for( unsigned int i = 0; i < m_texID.size(); ++i ) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, *(m_texID[i]));
    }
}

void FBO::bindTexInput(int num)
{
    glBindTexture(GL_TEXTURE_2D, *(m_texID[num]));
}

void FBO::bindTexOutput()
{
    glBindFramebuffer(GL_FRAMEBUFFER, *m_fboID);

    if( m_texID.size() == 1 ) {
        glDrawBuffer((*m_attachmentPoints)[0]);
    }

    else {
        glDrawBuffers(m_texID.size(), *m_attachmentPoints);
    }
}

void FBO::bindTexOutput(int num)
{
    glBindFramebuffer(GL_FRAMEBUFFER, *m_fboID);
    glDrawBuffer((*m_attachmentPoints)[num]);
}

void FBO::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::checkFBO()
{
    GLenum status;

    glBindFramebuffer(GL_FRAMEBUFFER, *m_fboID);
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if(status != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "FBO STATUS ERROR : " << status << std::endl;
    }
}

}
}

