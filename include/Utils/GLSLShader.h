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
#include <vector>

namespace CGoGN
{

namespace Utils
{


////////// CLASS DECLARATION //////////

class GLSLShader
{

public:
	/**
	 * enum of supported shader type
	 */
	enum shaderType {VERTEX_SHADER = 1, FRAGMENT_SHADER = 2, GEOMETRY_SHADER = 3 };


protected:
	/**
	 * handle of vertex shader
	 */
	GLhandleARB	m_vertex_shader_object;

	/**
	 * handle of fragment shader
	 */
	GLhandleARB	m_fragment_shader_object;

	/**
	 * handle of geometry shader
	 */
	GLhandleARB	m_geom_shader_object;

	/**
	 * handle of program
	 */
	GLhandleARB m_program_object;


	/**
	 * load vertex shader
	 * @param vertex_shader_source src text shader
	 */
	bool loadVertexShaderSourceString( const char* vertex_shader_source );

	/**
	 * load fragment shader
	 * @param fragment_shader_source src text shader
	 */
	bool loadFragmentShaderSourceString( const char* fragment_shader_source );

	/**
	 * load geometry shader
	 * @param geom_shader_source src text shader
	 */
	bool loadGeometryShaderSourceString( const char* geom_shader_source );

	/**
	 * load vertex shader
	 * @param filename file name
	 */
	bool loadVertexShader( const std::string& filename );

	/**
	 * load fragment shader
	 * @param filename file name
	 */
	bool loadFragmentShader( const std::string& filename );

	/**
	 * load geometry shader
	 * @param filename file name
	 */
	bool  loadGeometryShader( const std::string& filename );

	/**
	 * Load of source file in a char buffer
	 * @param source_file file name
	 */
	char* loadSourceFile( const std::string& source_file );

	/**
	 * create the shader (attach and link shaders into program)
	 */
	bool create(GLint inputGeometryPrimitive=GL_TRIANGLES,GLint outputGeometryPrimitive=GL_TRIANGLES);



	/**
	 * get log after compiling
	 * @param obj what log do you want ?
	 * @return the log
	 */
	char* getInfoLog( GLhandleARB obj );


	static std::vector<std::string> m_pathes;

public:
	/**
	 * constructor
	 */
	GLSLShader();


	/**
	 * destructor
	 */
	virtual ~GLSLShader();

	/*
	 * search file in different path
	 */
	static std::string findFile(const std::string filename);


	/**
	 * test support of shader
	 */
	static bool	areShadersSupported();

	/**
	 * test support of Vertex Buffer Object
	 */
	static bool	areVBOSupported();

	/**
	 * test support of geometry shader
	 */
	static bool	areGeometryShadersSupported();

	/**
	 * test support of gl3
	 */
	static bool	isGL3Supported();


	static bool init();


	/**
	 * load shaders (compile and link)
	 * @param vs vertex shader source file
	 * @param fs fragment shader source file
	 */
	bool loadShaders(const std::string& vs, const std::string& fs);

	/**
	 * load shaders (compile and link)
	 * @param vs vertex shader source file
	 * @param fs fragment shader source file
	 * @param fs fragment shader source file
	 * @param inputGeometryPrimitive primitives used in geometry shader as input
	 * @param outputGeometryPrimitive primitives generated in geometry shader as output
	 */
	bool loadShaders(const std::string& vs, const std::string& fs, const std::string& gs, GLint inputGeometryPrimitive=GL_TRIANGLES,GLint outputGeometryPrimitive=GL_TRIANGLES);


	/**
	 * Link the shader do it just after binding the attributes
	 */
	bool link();

	inline bool		isCreated();

	bool			isBinded();

	virtual bool	bind();

	virtual void	unbind();


	/**
	 *
	 */
	GLuint 	getAttribIndex( char* attribName );

	
	/**
	 * get handler of program for external use og gl functions
	 */
	GLuint program_handler() { return m_program_object;}
	

	/**
	 * bind vertex attribute with its name in shaders
	 */
	void bindAttrib(unsigned int att, const char* name) const;


	/**
	 * check shader validity width official GLSL syntax
	 */
	bool validateProgram();

	/**
	 * check program link status
	 */
	bool checkProgram();

	/**
	 * check shader compile status
	 */
	bool checkShader(int shaderType);

public:

	/**
	 * set uniform shader float variable
	 * @warning practical but less efficient that storing id (get with glGetUniformLocation) and use glUniform*fvARB
	 * @param NB template size of variable to set
	 * @param name name in shader
	 * @param pointer on data to copy
	 */
	template<unsigned int NB>
	void setuniformf( const char* name , float* val);

	/**
	 * set uniform shader int variable
	 * @warning practical but less efficient that storing id (get with glGetUniformLocation) and use glUniform*ivARB
	 * @param NB template size of variable to set
	 * @param name name in shader
	 * @param pointer on data to copy
	 */
	template<unsigned int NB>
	void setuniformi( const char* name , int* val);

	/**
	 * add search path for file
	 * @param path to add
	 */
	void addPathFileSeach(const std::string& path);

};


////////// INLINE FUNCTIONS //////////


inline bool GLSLShader::isCreated()
{
	return ( m_program_object != 0 );
}

template<unsigned int NB>
void GLSLShader::setuniformf( const char* name , float* val)
{
	GLint uni = glGetUniformLocationARB(m_program_object,name);
	if (uni>=0)
	{
		switch(NB)
		{
		case 1:
			glUniform1fvARB( uni, 1, val) ;
			break;
		case 2:
			glUniform2fvARB( uni, 1, val) ;
			break;
		case 3:
			glUniform3fvARB( uni, 1, val) ;
			break;
		case 4:
			glUniform4fvARB( uni, 1, val) ;
			break;
		case 16:
			glUniformMatrix4fv(uni, 1 , false, val);
			break;
		}
	}
}

template<unsigned int NB>
void GLSLShader::setuniformi( const char* name , int* val)
{
	GLint uni = glGetUniformLocationARB(m_program_object,name);
	if (uni>=0)
	{
		switch(NB)
		{
		case 1:
			glUniform1ivARB( uni, 1, val) ;
			break;
		case 2:
			glUniform2ivARB( uni, 1, val) ;
			break;
		case 3:
			glUniform3ivARB( uni, 1, val) ;
			break;
		case 4:
			glUniform4ivARB( uni, 1, val) ;
			break;
		}
	}
}






} //namespace Utils
} //namespace CGoGN




#endif /*__KLIB__GLSHADER__12052005__*/
