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

#ifndef __CGoGN_GLSL_SHADER__
#define __CGoGN_GLSL_SHADER__

#include "Utils/os_spec.h"
#include "Utils/vbo.h"

#include "glm/glm.hpp"
#include <GL/glew.h>

#include <stdlib.h>
#include <string>
#include <vector>
#include <set>


namespace CGoGN
{
namespace Utils
{


class GLSLShader
{


public:

	struct VAStr
	{
		int va_id;
		VBO* vbo_ptr;
//		GLuint vbo_id;
//		unsigned int size;
	};


	/**
	 * enum of supported shader type
	 */
	enum shaderType {VERTEX_SHADER = 1, FRAGMENT_SHADER = 2, GEOMETRY_SHADER = 3 };

	static unsigned int CURRENT_OGL_VERSION;

	static std::set< std::pair<void*, GLSLShader*> > m_registredRunning;

protected:

	static std::string DEFINES_GL2;

	static std::string DEFINES_GL3;

	static std::string* DEFINES_GL;

	static std::string defines_Geom(const std::string& primitivesIn, const std::string& primitivesOut, int maxVert);


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

	std::string m_nameVS;
	std::string m_nameFS;
	std::string m_nameGS;

	/**
	 * handle of program
	 */
	GLhandleARB m_program_object;


	GLint m_uniMat_Proj;
	GLint m_uniMat_Model;
	GLint m_uniMat_ModelProj;
	GLint m_uniMat_Normal;


	char* m_vertex_shader_source;
	char* m_fragment_shader_source;
	char* m_geom_shader_source;

	GLint m_geom_inputPrimitives;
	GLint m_geom_outputPrimitives;


	/**
	 * a set of pair VA_id / VBO_id
	 */
//	std::vector<pair<int,unsigned int> > m_va_vbo_binding;
	std::vector<VAStr> m_va_vbo_binding;


	static std::vector<std::string> m_pathes;

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

public:
	/**
	 * constructor
	 */
	GLSLShader();


	/**
	 * destructor
	 */
	virtual ~GLSLShader();


	static void setCurrentOGLVersion(unsigned int version);


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


	static void registerRunning(GLSLShader* ptr);

	static void unregisterRunning(GLSLShader* ptr);
//
//	static void updateMatricesRunningShaders(const glm::mat4& projection, const glm::mat4& modelview);

	/**
	 * load shaders (compile and link)
	 * @param vs vertex shader source file
	 * @param fs fragment shader source file
	 */
	bool loadShaders(const std::string& vs, const std::string& fs);

	/**
	 * load shaders (compile and link)
	 * @param vs vertex shader source file name
	 * @param fs fragment shader source file name
	 * @param gs geometry shader source file name
	 * @param inputGeometryPrimitive primitives used in geometry shader as input
	 * @param outputGeometryPrimitive primitives generated in geometry shader as output
	 */
	bool loadShaders(const std::string& vs, const std::string& fs, const std::string& gs, GLint inputGeometryPrimitive=GL_TRIANGLES,GLint outputGeometryPrimitive=GL_TRIANGLE_STRIP);


	/**
	 * load shaders (compile and link)
	 * @param vs vertex shader source char* prt
	 * @param fs fragment shader source char* prt

	 * @param outputGeometryPrimitive primitives generated in geometry shader as output
	 */
	bool loadShadersFromMemory(const char* vs, const char* fs);

	/**
	 * load shaders (compile and link)
	 * @param vs vertex shader source char* prt
	 * @param fs fragment shader source char* prt
	 * @param fs geometry shader source char* prt
	 * @param inputGeometryPrimitive primitives used in geometry shader as input
	 * @param outputGeometryPrimitive primitives generated in geometry shader as output
	 */
	bool loadShadersFromMemory(const char* vs, const char* fs, const char* gs, GLint inputGeometryPrimitive,GLint outputGeometryPrimitive);


	const char* getVertexShaderSrc() {return m_vertex_shader_source;}
	const char* getFragmentShaderSrc() {return m_fragment_shader_source;}
	const char* getGeometryShaderSrc() {return m_geom_shader_source;}


	bool reloadVertexShaderFromMemory(const char* vs);

	bool reloadFragmentShaderFromMemory(const char* fs);

	bool reloadGeometryShaderFromMemory(const char* gs);

	bool recompile();

	/**
	 * Link the shader do it just after binding the attributes
	 */
	bool link();



	inline bool		isCreated();

	bool			isBinded();

	virtual bool	bind() const;

	virtual void	unbind() const;

	/**
	 * restore all uniforms and vertex attributes after recompiling
	 */
	virtual void restoreUniformsAttribs() {CGoGNerr << "Warning restoreUniformsAttribs not implemented"<< CGoGNendl;}


	/**
	 *
	 */
//	GLuint 	getAttribIndex( char* attribName );

	
	/**
	 * get handler of program for external use og gl functions
	 */
	GLuint program_handler() { return m_program_object;}
	



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
	void setuniformf( const char* name, const float* val);

	/**
	 * set uniform shader int variable
	 * @warning practical but less efficient that storing id (get with glGetUniformLocation) and use glUniform*ivARB
	 * @param NB template size of variable to set
	 * @param name name in shader
	 * @param pointer on data to copy
	 */
	template<unsigned int NB>
	void setuniformi( const char* name, const int* val);

	/**
	 * add search path for file
	 * @param path to add
	 */
	void addPathFileSeach(const std::string& path);

	/**
	 * remove VBO index from binding
	 */
	void unbindVBO(VBO* ptr);

	/**
	 * remove VBO index from binding
	 */
	void unbindVA(const std::string& name);

	/**
	 * associate an attribute name of shader with a vbo
	 * @return the index in vector of pair binding, negative if fail
	 */
	unsigned int bindVA_VBO(const std::string& name, VBO* vbo);

	/**
	 * change the vbo of id case of binding vector
	 */
	void changeVA_VBO(unsigned int id, VBO* vbo);

	/**
	 * get binding VA VBO
	 */
	const std::vector<VAStr>& getVA_VBO_Bindings() { return m_va_vbo_binding;}




	void bindAttrib(unsigned int att, const char* name) const;

	/**
	 * update projection, modelview, ... matrices
	 */
	void updateMatrices(const glm::mat4& projection, const glm::mat4& modelview);

	/**
	 * bind, enable, and set all vertex attrib pointers
	 * @param stride: the stride parameter, number osf byte between two consecutive attributes
	 */
	void enableVertexAttribs(unsigned int stride=0) const;

	/**
	 * disenable all vertex attribs
	 */
	void disableVertexAttribs() const;
};


////////// INLINE FUNCTIONS //////////


inline bool GLSLShader::isCreated()
{
	return ( m_program_object != 0 );
}

template<unsigned int NB>
void GLSLShader::setuniformf( const char* name, const float* val)
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
void GLSLShader::setuniformi( const char* name, const int* val)
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




#endif
