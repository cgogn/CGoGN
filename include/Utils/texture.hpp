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

#include <vector>
#include <typeinfo>

#include "Utils/os_spec.h"

namespace CGoGN
{

namespace Utils
{

template<typename T>
Texture<T>::Texture()
{
	m_data = NULL;
	m_width = 0;
	m_height = 0;
	m_cpp = 0;
	m_ilName = 0;
	m_texName = 0;
}

template<typename T>
Texture<T>::Texture(std::string fileName)
{
	ilInit();
	iluInit();
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ilEnable(IL_ORIGIN_SET);

	ilGenImages(1,&m_ilName);
	ilBindImage(m_ilName);
	ilLoadImage(fileName.c_str());
	
//	get the info of image
	m_width = ilGetInteger(IL_IMAGE_WIDTH);
	m_height = ilGetInteger(IL_IMAGE_HEIGHT);
	m_cpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL) / sizeof(T);
	m_data = reinterpret_cast<T*>(ilGetData());
	m_texName = 0;
}

template<typename T>
Texture<T>::Texture( T* ptr, int w, int h, int cpp)
{
	m_data = new T[w*h*cpp];
	m_width = w;
	m_height = h;
	m_cpp = cpp;
	m_ilName = 0;
	m_texName = 0;
}

template<typename T>
void Texture<T>::load( std::string fileName)
{
	if (m_ilName !=0)
		ilDeleteImages(1,&m_ilName);
	else if(m_data !=0)	
		delete[] m_data;
	if (m_texName != 0)
		glDeleteTextures( 1, &m_texName );

	ilInit();
	iluInit();
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ilEnable(IL_ORIGIN_SET);

	ilGenImages(1,&m_ilName);
	ilBindImage(m_ilName);
	ilLoadImage(fileName.c_str());
	
//	get the info of image
	m_width = ilGetInteger(IL_IMAGE_WIDTH);
	m_height = ilGetInteger(IL_IMAGE_HEIGHT);
	m_cpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL) / sizeof(T);
	m_data = reinterpret_cast<T*>(ilGetData());
	m_texName = 0;
}

template<typename T>
void Texture<T>::setData( T* ptr, int w, int h, int cpp)
{
	if (m_ilName !=0)
		ilDeleteImages(1,&m_ilName);
	else if(m_data !=0)	
		delete[] m_data;
	if (m_texName != 0)
		glDeleteTextures( 1, &m_texName );

	m_data = new T[w*h*cpp];
	m_width = w;
	m_height = h;
	m_cpp = cpp;
	m_ilName = 0;
	m_texName = 0;
}

template<typename T>
Texture<T>::~Texture()
{
	if (m_ilName != 0)
		ilDeleteImages(1, &m_ilName);
	else if(m_data != 0)
		delete[] m_data;

	if (m_texName != 0)
		glDeleteTextures( 1, &m_texName );
}

template<typename T>
void Texture<T>::initGL(GLint filter, GLenum format, const char* uniformName, GLuint prg)
{
	// allocate a texture name
	glGenTextures(1, &m_texName );

	// select our current texture
	glBindTexture(GL_TEXTURE_2D, m_texName);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter );
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	GLenum ptype;
	if (typeid(T) == typeid(unsigned char))
		ptype = GL_UNSIGNED_BYTE;
    if (typeid(T) == typeid(unsigned short))
		ptype = GL_UNSIGNED_SHORT;
	if (typeid(T) == typeid(unsigned int))
		ptype = GL_UNSIGNED_INT;
	if (typeid(T) == typeid(float))
		ptype = GL_FLOAT;

	switch(format)
	{
	case GL_RGB:
		if (m_cpp != 3)
			CGoGNout << "Warning using GL_RGB with image not in RGB format"<<CGoGNendl;
		break;
	case GL_LUMINANCE:
		if (m_cpp != 1)
			CGoGNout << "Warning using GL_LUMINANCE with image not in grey level format"<<CGoGNendl;
		break;

	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
 	glTexImage2D (GL_TEXTURE_2D, 0, m_cpp, m_width, m_height, 0 , format, ptype, m_data);
//	glTexImage2D (GL_TEXTURE_2D, 0, m_cpp, m_width, m_height, 0 , GL_RGB, GL_UNSIGNED_BYTE , m_data);

	m_uniform_tex=glGetUniformLocationARB(prg,uniformName);
}

template<typename T>
void Texture<T>::useGL(unsigned int textureEngineIndex)
{
	glActiveTextureARB(GL_TEXTURE0_ARB + textureEngineIndex);
	glUniform1iARB(m_uniform_tex, textureEngineIndex);
	glBindTexture(GL_TEXTURE_2D, m_texName);
	glEnable(GL_TEXTURE_2D);
}

} // namespace Utils

} // namespace CGoGN
