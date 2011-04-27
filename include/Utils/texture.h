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

#ifndef TEXTURE_RENDER
#define TEXTURE_RENDER

#include <GL/gl.h>
#include <IL/ilu.h>

/**
* A set of function that allow the creation of rendering
* object using Vertex-Buffer-Object.
* Function are made for dual-2-map and can be used on
* any subset of a dual-N-map which is a 2-map
*/
namespace CGoGN
{

namespace Utils
{

template <typename T>
class Texture
{
protected:

	/**
	* buffer of texture image
	*/
	T *m_data;
	
	/**
	* width of texture
	*/
	int m_width;

	/**
	* height of image
	*/
	int m_height;
	
	/**
	* number of component by pixel
	*/
	int m_cpp;
	
	/**
	* name of DevIL image
	*/
	ILuint m_ilName;
	
	/**
	* name of OGL texture
	*/
	GLuint m_texName;
	
	/**
	* uniform variable for texture name in shader
	*/
	GLuint m_uniform_tex;	

public:

	/**
	* constructor 
	*/
	Texture();

	/**
	* constructor 
	*/
	~Texture();

	/**
	* constructor from image filename
	* @param filename name of an image file (all format accepted: png, jpg, ....)
	*/
	Texture( std::string fileName);
		
	/**
	* constructor from data
	* @param prt pointer on data
	* @param w width in pixels
	* @param h height in pixels
	* @param cpp cnumber of components per pixel
	*/
	Texture( T* ptr, int w, int h, int cpp);
		
	/**
	* loaf an image filename
	* @param filename name of an image file (all format accepted: png, jpg, ....)
	*/
	void load( std::string fileName);

	/**
	* set data
	* @param prt pointer on data
	* @param w width in pixels
	* @param h height in pixels
	* @param cpp cnumber of components per pixel
	*/
	void setData( T* ptr, int w, int h, int cpp);

	/**
	* get the width of texture
	* @return width of texture
	*/
	int getWidth() { return m_width; }

	/**
	* get the height of texture
	* @return height of texture
	*/
	int getHeight() { return m_height; }

	/**
	* get the number of component per pixel
	* @return component per pixel
	*/
	int getCPP() { return m_cpp; }

	/**
	* get a pointer on texture image
	*/
	T* getPtr() { return m_data; }

	/**
	* get texture name
	*/
	GLuint getTextureName() { return m_texName; }

	/**
	* initialisation GL
	* @param filter filter parameter GL_NEAREST,GL_LINEAR,...
	* @param format format of texture GL_RGB,GL_RGBA,GL_LUMINANCE,... 
	* @param prg shader program handler
	*/
	void initGL(GLint filter, GLenum format, const char* uniformName, GLuint prg);

	/**
	* fonction to call before drawing
	* @param textureEngineIndex index of multitexture engine (0,1, ...
	*/
	void useGL(unsigned int textureEngineIndex);
};

} // namespace utils

} // namespace CGoGN

#include "texture.hpp"

#endif
