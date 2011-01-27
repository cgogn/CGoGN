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

/***********************************************
*  Thanks to Frederic Larue for this class
***********************************************/

#ifndef __KLIB__GLSHADER__12052005__
#define __KLIB__GLSHADER__12052005__

#include "Utils/os_spec.h"

#include <GL/glew.h>


//#include <GL/glut.h>

#include <map>
#include <stdlib.h>
#include <string>

namespace CGoGN
{

namespace Utils
{


////////// CLASS DECLARATION //////////

APIEXPORT class GLSLShader
{
	/*********\
	| TYPE(S) |
	\*********/
protected:
	struct Sampler
	{
		int		location;
		int		tex_unit;
	};

	struct Uniform
	{
		int		location;
		int		size;
		int		type;
	};


	/**************\
	| ATTRIBUTE(S) |
	\**************/
protected:
	GLhandleARB						vertex_shader_object;
	GLhandleARB						fragment_shader_object;
	GLhandleARB						geom_shader_object;
	GLhandleARB						program_object;

	std::map<std::string,Sampler>	sampler_list;
	std::map<std::string,Uniform>	unif_list;
	std::map<std::string,GLuint>	attrib_list;


	/****************\
	| CONSTRUCTOR(S) |
	\****************/
public:
	GLSLShader();


	/***********\
	| METHOD(S) |
	\***********/
protected:
	char*			getInfoLog( GLhandleARB obj );


public:
	static bool		areShadersSupported();

	static bool		areVBOSupported();

	static bool		areGeometryShadersSupported();

	bool			loadVertexShaderSourceString( const char* vertex_shader_source );
	bool			loadPixelShaderSourceString( const char* pixel_shader_source );
	bool			loadGeometryShaderSourceString( const char* pixel_shader_source );


	bool			loadVertexShader( const std::string& filename );
	bool			loadPixelShader( const std::string& filename );
	bool 			loadGeometryShader( const std::string& filename );


	bool			create(GLint inputGeometryPrimitive=GL_TRIANGLES,GLint outputGeometryPrimitive=GL_TRIANGLES);

	inline bool		isCreated();
	bool			isBinded();

	virtual bool	bind();
	virtual void	unbind();

	bool			setSampler( char* sampler_name, int tex_unit );
	bool			setUniform( char* unif_name, void* value );
	GLuint			getAttribIndex( char* attribName );

	bool 			loadShaders(const std::string& vs, const std::string& ps);

	bool 			loadShaders(const std::string& vs, const std::string& ps, const std::string& gs,GLint inputGeometryPrimitive=GL_TRIANGLES,GLint outputGeometryPrimitive=GL_TRIANGLES);
	
	char*			loadSourceFile( const std::string& source_file );
	
	std::string findFile(const std::string filename);
	

	GLuint program_handler() { return program_object;}
	
	static bool init();


	/************\
	| DESTRUCTOR |
	\************/
public:
	virtual ~GLSLShader();
};


////////// INLINE FUNCTIONS //////////


inline bool GLSLShader::isCreated()
{
	return ( program_object != 0 );
}


} //namespace Utils

} //namespace CGoGN




#endif /*__KLIB__GLSHADER__12052005__*/
