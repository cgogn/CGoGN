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
#define EXPORTING 1

#include "Utils/GLSLShader.h"
#include <iostream>
#include <fstream>
#include <vector>

#include "glm/gtx/inverse_transpose.hpp"


namespace CGoGN
{

namespace Utils
{
unsigned int GLSLShader::CURRENT_OGL_VERSION=2;

std::string GLSLShader::DEFINES_GL2=\
"#version 110\n"
"#define PRECISON\n"
"#define ATTRIBUTE attribute\n"
"#define VARYING_VERT varying\n"
"#define VARYING_FRAG varying\n"
"#define FRAG_OUT_DEF \n"
"#define INVARIANT_POS \n";


std::string GLSLShader::DEFINES_GL3=\
"#version 150\n"
"#define PRECISON precision highp float;\n"
"#define ATTRIBUTE in\n"
"#define VARYING_VERT smooth out\n"
"#define VARYING_FRAG smooth in\n"
"#define FRAG_OUT_DEF out vec4 gl_FragColor\n"
"#define INVARIANT_POS \n";

std::string* GLSLShader::DEFINES_GL =NULL;

std::vector<std::string> GLSLShader::m_pathes;

std::set< std::pair<void*, GLSLShader*> > GLSLShader::m_registredRunning;

GLSLShader::GLSLShader():
			m_vertex_shader_object(0),
			m_fragment_shader_object(0),
			m_geom_shader_object(0),
			m_program_object(0),
			m_uniMat_Proj(-1),
			m_uniMat_Model(-1),
			m_uniMat_ModelProj(-1),
			m_uniMat_Normal(-1),
			m_vertex_shader_source(NULL),
			m_fragment_shader_source(NULL),
			m_geom_shader_source(NULL)
{
	if (DEFINES_GL==NULL)
		DEFINES_GL = &DEFINES_GL3;
}


void GLSLShader::registerRunning(GLSLShader* ptr)
{
	m_registredRunning.insert(std::pair<void*,GLSLShader*>(NULL,ptr));
}

void GLSLShader::unregisterRunning(GLSLShader* ptr)
{
	m_registredRunning.erase(std::pair<void*,GLSLShader*>(NULL,ptr));
}


std::string GLSLShader::defines_Geom(const std::string& primitivesIn, const std::string& primitivesOut, int maxVert)
{
	if (CURRENT_OGL_VERSION==3)
	{
		std::string str("#version 150\n");
		str.append("precision highp float;\n");
		str.append("layout (");
		str.append(primitivesIn);
		str.append(") in;\n");

		str.append("layout (");
		str.append(primitivesOut);
		str.append(", max_vertices = ");
		std::stringstream ss;
		ss << maxVert;
		str.append(ss.str());
		str.append(") out;\n");
		str.append("#define VARYING_IN in\n");
		str.append("#define VARYING_OUT smooth out\n");
		str.append("#define POSITION_IN(X) gl_in[X].gl_Position\n");
		return str;
	}
	else
	{
		std::string str("#extension GL_EXT_geometry_shader4 : enable\n");
		str.append("#define PRECISION\n");
		str.append("#define ATTRIBUTE attribute\n");
		str.append("#define VARYING_IN varying in\n");
		str.append("#define VARYING_OUT varying out\n");
		str.append("#define POSITION_IN(X) gl_PositionIn[X]\n");
		return str;
	}
}



bool GLSLShader::areGeometryShadersSupported()
{
	if ( ! glewGetExtension("GL_EXT_geometry_shader4")) return false;
	return true;
}

bool GLSLShader::areShadersSupported()
{
	if ( ! glewGetExtension("GL_ARB_vertex_shader")) return false;
	if ( ! glewGetExtension("GL_ARB_fragment_shader")) return false;
	if ( ! glewGetExtension("GL_ARB_shader_objects")) return false;
	if ( ! glewGetExtension("GL_ARB_shading_language_100")) return false;

	return true;
}


bool GLSLShader::areVBOSupported()
{
	if (!glewGetExtension("GL_ARB_vertex_buffer_object"))
		return false;
	return true;
}



char* GLSLShader::loadSourceFile(  const std::string& filename)
{
	std::ifstream	file;
	int				file_size;
	char*			shader_source;


	/*** opening file ***/
	file.open( filename.c_str() , std::ios::in | std::ios::binary );

	if( !file.good() )
	{
		std::cerr << "ERROR - GLSLShader::loadSourceFile() - unable to open the file " << filename << "." << std::endl;
		return NULL;
	}


	/*** reading file ***/
	try
	{
		/* get file size */
		file.seekg( 0, std::ios::end );
		file_size = file.tellg();
		file.seekg( 0, std::ios::beg );

		/* allocate shader source table */
		shader_source = new char [ file_size+1 ];

		/* read source file */
		file.read( shader_source, file_size );
		shader_source[ file_size ] = '\0';
	}
	catch( std::exception& io_exception )
	{
		std::cerr << "ERROR - GLSLShader::loadSourceFile() - " << io_exception.what() << std::endl;
		file.close();
		return NULL;
	}


	/*** termination ***/
	file.close();
	return shader_source;
}


bool GLSLShader::loadVertexShader(  const std::string& filename )
{
	bool	flag;
//	char	*vertex_shader_source;

	if (m_vertex_shader_source)
		delete [] m_vertex_shader_source;

	m_vertex_shader_source = loadSourceFile( filename );

	if( !m_vertex_shader_source )
	{
		std::cerr << "ERROR - GLSLShader::loadVertexShader() - error occured while loading source file." << std::endl;
		return false;
	}


	flag = loadVertexShaderSourceString( m_vertex_shader_source );
//	delete [] vertex_shader_source;

	return flag;
}


bool GLSLShader::loadFragmentShader(const std::string& filename )
{
	bool	flag;
//	char	*fragment_shader_source;

	if (m_fragment_shader_source)
		delete [] m_fragment_shader_source;

	m_fragment_shader_source = loadSourceFile( filename );

	if( !m_fragment_shader_source )
	{
		std::cerr << "ERROR - GLSLShader::loadFragmentShader() - error occured while loading source file." << std::endl;
		return false;
	}


	flag = loadFragmentShaderSourceString( m_fragment_shader_source );
//	delete [] fragment_shader_source;


	return flag;
}


bool GLSLShader::loadGeometryShader(const std::string& filename )
{
	bool	flag;
//	char	*geom_shader_source;


	if (m_geom_shader_source)
		delete [] m_geom_shader_source;

	m_geom_shader_source = loadSourceFile( filename );



	if( !m_geom_shader_source )
	{
		std::cerr << "ERROR - GLSLShader::loadGeometryShader() - error occured while loading source file." << std::endl;
		return false;
	}

	flag = loadGeometryShaderSourceString( m_geom_shader_source );
//	delete [] geom_shader_source;


	return flag;
}


bool GLSLShader::loadVertexShaderSourceString( const char *vertex_shader_source )
{
	int		status;
	char	*info_log;

	if (m_vertex_shader_object==0)
	{
		glDeleteShader(m_vertex_shader_object);
		m_vertex_shader_object=0;
	}

	/*** create shader object ***/
	m_vertex_shader_object = glCreateShader( GL_VERTEX_SHADER );

	if( !m_vertex_shader_object )
	{
		std::cerr << "ERROR - GLSLShader::loadVertexShader() - unable to create shader object." << std::endl;
		return false;
	}


	/*** load source file ***/
	if( !vertex_shader_source )
	{
		std::cerr << "ERROR - GLSLShader::loadVertexShader() - source string is empty." << std::endl;

		glDeleteObjectARB( m_vertex_shader_object );
		m_vertex_shader_object = 0;

		return false;
	}

	glShaderSourceARB( m_vertex_shader_object, 1, (const char**)&vertex_shader_source, NULL );


	/*** compile shader object ***/
	glCompileShaderARB( m_vertex_shader_object );

	glGetObjectParameterivARB( m_vertex_shader_object, GL_OBJECT_COMPILE_STATUS_ARB, &status );
	if( !status )
	{
		std::cerr << "ERROR - GLshader::loadVertexShader() - error occured while compiling shader." << std::endl;
		info_log = getInfoLog( m_vertex_shader_object );
		std::cerr << "  COMPILATION " << info_log << std::endl;
		delete [] info_log;

		glDeleteObjectARB( m_vertex_shader_object );
		m_vertex_shader_object = 0;

		return false;
	}


	/*** termination ***/
	return true;
}


bool GLSLShader::loadFragmentShaderSourceString( const char *fragment_shader_source )
{
	int		status;
	char	*info_log;

	if (m_fragment_shader_object==0)
	{
		glDeleteShader(m_fragment_shader_object);
		m_fragment_shader_object=0;
	}

	/*** create shader object ***/
	m_fragment_shader_object = glCreateShader( GL_FRAGMENT_SHADER );

	if( !m_fragment_shader_object )
	{
		std::cerr << "ERROR - GLSLShader::loadFragmentShader() - unable to create shader object." << std::endl;
		return false;
	}


	/*** load source file ***/
	if( !fragment_shader_source )
	{
		std::cerr << "ERROR - GLSLShader::loadFragmentShader() - source string is empty." << std::endl;

		glDeleteObjectARB( m_fragment_shader_object );
		m_fragment_shader_object = 0;

		return false;
	}

	glShaderSourceARB( m_fragment_shader_object, 1, (const char**)&fragment_shader_source, NULL );


	/*** compile shader object ***/
	glCompileShaderARB( m_fragment_shader_object );

	glGetObjectParameterivARB( m_fragment_shader_object, GL_OBJECT_COMPILE_STATUS_ARB, &status );
	if( !status )
	{
		std::cerr << "ERROR - GLshader::loadFragmentShader() - error occured while compiling shader." << std::endl;
		info_log = getInfoLog( m_fragment_shader_object );
		std::cerr << "  COMPILATION " << info_log << std::endl;
		delete [] info_log;

		glDeleteObjectARB( m_fragment_shader_object );
		m_fragment_shader_object = 0;

		return false;
	}


	/*** termination ***/
	return true;
}



bool GLSLShader::loadGeometryShaderSourceString( const char *geom_shader_source )
{
	int		status;
	char	*info_log;

	if (m_geom_shader_object==0)
	{
		glDeleteShader(m_geom_shader_object);
		m_geom_shader_object=0;
	}
	/*** create shader object ***/
	m_geom_shader_object = glCreateShader(GL_GEOMETRY_SHADER_EXT);


	if( !m_geom_shader_object )
	{
		std::cerr << "ERROR - GLSLShader::loadGeometryShader() - unable to create shader object." << std::endl;
		return false;
	}


	/*** load source file ***/
	if( !geom_shader_source )
	{
		std::cerr << "ERROR - GLSLShader::loadGeometryShader() - source string is empty." << std::endl;

		glDeleteObjectARB( m_geom_shader_object );
		m_geom_shader_object = 0;

		return false;
	}

	glShaderSourceARB( m_geom_shader_object, 1, (const char**)&geom_shader_source, NULL );


	/*** compile shader object ***/
	glCompileShaderARB( m_geom_shader_object );

	glGetObjectParameterivARB( m_geom_shader_object, GL_OBJECT_COMPILE_STATUS_ARB, &status );
	if( !status )
	{
		std::cerr << "ERROR - GLshader::loadGeometryShader() - error occured while compiling shader." << std::endl;
		info_log = getInfoLog( m_geom_shader_object );
		std::cerr << "  COMPILATION " << info_log << std::endl;
		delete [] info_log;

		glDeleteObjectARB( m_geom_shader_object );
		m_geom_shader_object = 0;

		return false;
	}


	/*** termination ***/
	return true;
}


char* GLSLShader::getInfoLog( GLhandleARB obj )
{
	char	*info_log;
	int		info_log_length;
	int		length;


	glGetObjectParameterivARB( obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &info_log_length );

	info_log = new char [info_log_length];
	glGetInfoLogARB( obj, info_log_length, &length, info_log );


	return info_log;
}


bool GLSLShader::create(GLint inputGeometryPrimitive,GLint outputGeometryPrimitive)
{
	int		status;
	char	*info_log;

	m_geom_inputPrimitives = inputGeometryPrimitive;
	m_geom_outputPrimitives = outputGeometryPrimitive;

	/*** check if shaders are loaded ***/
	if( !m_vertex_shader_object || !m_fragment_shader_object )
	{
		std::cerr << "ERROR - GLSLShader::create() - shaders are not defined." << std::endl;
		return false;
	}


	/*** create program object ***/
	m_program_object = glCreateProgramObjectARB();

	if( !m_program_object )
	{
		std::cerr << "ERROR - GLSLShader::create() - unable to create program object." << std::endl;
		return false;
	}


	/*** attach shaders to program object ***/
	glAttachObjectARB( m_program_object, m_vertex_shader_object );
	glAttachObjectARB( m_program_object, m_fragment_shader_object );
	if (m_geom_shader_object)
	{
		glAttachObjectARB( m_program_object, m_geom_shader_object );

		glProgramParameteriEXT(m_program_object,GL_GEOMETRY_INPUT_TYPE_EXT,inputGeometryPrimitive);
		glProgramParameteriEXT(m_program_object,GL_GEOMETRY_OUTPUT_TYPE_EXT,outputGeometryPrimitive);
		int temp;
		glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT,&temp);
		glProgramParameteriEXT(m_program_object,GL_GEOMETRY_VERTICES_OUT_EXT,temp);
	}


	/*** link program object ***/
	glLinkProgramARB( m_program_object );

	glGetObjectParameterivARB( m_program_object, GL_OBJECT_LINK_STATUS_ARB, &status );
	if( !status )
	{
		std::cerr << "ERROR - GLSLShader::create() - error occured while linking shader program." << std::endl;
		info_log = getInfoLog( m_program_object );
		std::cerr << "  LINK " << info_log << std::endl;
		delete [] info_log;

		glDetachObjectARB( m_program_object, m_vertex_shader_object );
		glDetachObjectARB( m_program_object, m_fragment_shader_object );
		if (m_geom_shader_object)
			glDetachObjectARB( m_program_object, m_geom_shader_object );
		glDeleteObjectARB( m_program_object );
		m_program_object = 0;

		return false;
	}

	m_uniMat_Proj		= glGetUniformLocation(m_program_object,"ProjectionMatrix");
	m_uniMat_Model		= glGetUniformLocation(m_program_object,"ModelViewMatrix");
	m_uniMat_ModelProj	= glGetUniformLocation(m_program_object,"ModelViewProjectionMatrix");
	m_uniMat_Normal		= glGetUniformLocation(m_program_object,"NormalMatrix");

	return true;
}


bool GLSLShader::link()
{
	int		status;
	char	*info_log;


	/*** link program object ***/
	glLinkProgramARB( m_program_object );

	glGetObjectParameterivARB( m_program_object, GL_OBJECT_LINK_STATUS_ARB, &status );
	if( !status )
	{
		std::cerr << "ERROR - GLSLShader::create() - error occured while linking shader program." << std::endl;
		info_log = getInfoLog( m_program_object );
		std::cerr << "  LINK " << info_log << std::endl;
		delete [] info_log;

		glDetachObjectARB( m_program_object, m_vertex_shader_object );
		glDetachObjectARB( m_program_object, m_fragment_shader_object );
		if (m_geom_shader_object)
			glDetachObjectARB( m_program_object, m_geom_shader_object );
		glDeleteObjectARB( m_program_object );
		m_program_object = 0;

		return false;
	}

	return true;
}




bool GLSLShader::bind() const
{
	if( m_program_object )
	{
		glUseProgramObjectARB( m_program_object );

		return true;
	}
	else  return false;
}


void GLSLShader::unbind() const
{
	if( m_program_object )
	{
		glUseProgramObjectARB( 0 );
	}
}


bool GLSLShader::isBinded()
{
	return ( m_program_object && m_program_object == glGetHandleARB(GL_PROGRAM_OBJECT_ARB) );
}


GLSLShader::~GLSLShader()
{
	if( m_program_object )
	{
		unbind();

		if( m_vertex_shader_object )
		{
			glDetachObjectARB( m_program_object, m_vertex_shader_object );
			glDeleteObjectARB( m_vertex_shader_object );
		}
		if( m_fragment_shader_object )
		{
			glDetachObjectARB( m_program_object, m_fragment_shader_object );
			glDeleteObjectARB( m_fragment_shader_object );
		}
		if (m_geom_shader_object)
		{
			glDetachObjectARB( m_program_object, m_geom_shader_object );
			glDeleteObjectARB( m_geom_shader_object );
		}

		glDeleteObjectARB( m_program_object );
	}

//	m_registredRunning.erase(this);
}


std::string GLSLShader::findFile(const std::string filename)
{
// cherche d'abord dans le repertoire courant
	std::ifstream file;
	file.open(filename.c_str(),std::ios::in );
	if (!file.fail())
	{
		file.close();
		return filename;
	}
	file.close();

	for (std::vector<std::string>::const_iterator ipath = m_pathes.begin(); ipath != m_pathes.end(); ++ipath)
	{
		std::string st(*ipath);
		st.append(filename);

		std::ifstream file2;
		file2.open(st.c_str(),std::ios::in);
		if (!file2.fail())
		{
			file2.close();
			return st;
		}
	}

// LA MACRO SHADERPATH contient le chemin du repertoire qui contient les fichiers textes
	std::string st(SHADERPATH);
	st.append(filename);

	std::ifstream file2; // on ne peut pas réutiliser file ????
	file2.open(st.c_str(),std::ios::in);
	if (!file2.fail())
	{
		file2.close();
		return st;
	}

	return filename;
}


bool GLSLShader::init()
{
	GLenum error = glewInit();

	if (error != GLEW_OK)
		std::cerr << "Error: " << glewGetErrorString(error) << std::endl;
	else
		std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

	if (!areVBOSupported())
		std::cout << "VBO not supported !" << std::endl;

	if(!areShadersSupported()) {
		std::cout << "Shaders not supported !" << std::endl;
		return false;
	}
	return true; 
}

bool GLSLShader::loadShaders(const std::string& vs, const std::string& ps)
{

	std::string vss = findFile(vs);
	if(!loadVertexShader(vss)) return false;
	
	std::string pss = findFile(ps);
	if(!loadFragmentShader(pss)) return false;

	if(!create()) {
		std::cout << "Unable to create the shaders !" << std::endl;
		return false;
	}
	std::cout << "Shaders loaded (" << vs << "," << ps << ")" << std::endl;
	return true; 
}

bool GLSLShader::loadShaders(const std::string& vs, const std::string& ps, const std::string& gs, GLint inputGeometryPrimitive,GLint outputGeometryPrimitive)
{
	std::string vss = findFile(vs);
	if(!loadVertexShader(vss)) return false;

	std::string pss = findFile(ps);
	if(!loadFragmentShader(pss)) return false;

	std::string gss = findFile(gs);
	bool geomShaderLoaded = loadGeometryShader(gss);

	if (!geomShaderLoaded)
	{
		std::cerr << "Error while loading geometry shader" << std::endl;
	}


	if(!create(inputGeometryPrimitive,outputGeometryPrimitive))
	{
		std::cout << "Unable to create the shaders !" << std::endl;
		return false;
	}

	std::cout << "Shaders loaded (" << vs << "," << ps << "," << gs <<")" << std::endl;
	return true;
}



bool GLSLShader::loadShadersFromMemory(const char* vs, const char* fs)
{
	if (m_vertex_shader_source)
		delete [] m_vertex_shader_source;

	unsigned int sz = strlen(vs);
	m_vertex_shader_source = new char[sz+1];
	strcpy(m_vertex_shader_source,vs);


	if (m_fragment_shader_source)
		delete [] m_fragment_shader_source;

	sz = strlen(fs);
	m_fragment_shader_source = new char[sz+1];
	strcpy(m_fragment_shader_source,fs);


	if(!loadVertexShaderSourceString(vs)) return false;

	if(!loadFragmentShaderSourceString(fs)) return false;

	if(!create())
	{
		std::cout << "Unable to create the shaders !" << std::endl;
		return false;
	}
	return true;
}

bool GLSLShader::loadShadersFromMemory(const char* vs, const char* fs, const char* gs, GLint inputGeometryPrimitive,GLint outputGeometryPrimitive)
{

	if (m_vertex_shader_source)
		delete [] m_vertex_shader_source;

	unsigned int sz = strlen(vs);
	m_vertex_shader_source = new char[sz+1];
	strcpy(m_vertex_shader_source,vs);

	if (m_fragment_shader_source)
		delete [] m_fragment_shader_source;

	sz = strlen(fs);
	m_fragment_shader_source = new char[sz+1];
	strcpy(m_fragment_shader_source,fs);

	if (m_geom_shader_source)
		delete [] m_geom_shader_source;

	sz = strlen(gs);
	m_geom_shader_source = new char[sz+1];
	strcpy(m_geom_shader_source,gs);

	if(!loadVertexShaderSourceString(vs)) return false;

	if(!loadFragmentShaderSourceString(fs)) return false;

	if(!loadGeometryShaderSourceString(gs)) return false;

	if(!create(inputGeometryPrimitive,outputGeometryPrimitive))
	{
		std::cout << "Unable to create the shaders !" << std::endl;
		return false;
	}

	return true;
}


bool GLSLShader::reloadVertexShaderFromMemory(const char* vs)
{

	if (m_vertex_shader_source)
		delete [] m_vertex_shader_source;

	unsigned int sz = strlen(vs);
	m_vertex_shader_source = new char[sz+1];
	strcpy(m_vertex_shader_source,vs);

	if(!loadVertexShaderSourceString(vs)) return false;
	return true;
}

bool GLSLShader::reloadFragmentShaderFromMemory(const char* fs)
{
	if (m_fragment_shader_source)
		delete [] m_fragment_shader_source;

	unsigned int sz = strlen(fs);
	m_fragment_shader_source = new char[sz+1];
	strcpy(m_fragment_shader_source,fs);

	if(!loadFragmentShaderSourceString(fs)) return false;
	return true;
}

bool GLSLShader::reloadGeometryShaderFromMemory(const char* gs)
{
	if (m_geom_shader_source)
		delete [] m_geom_shader_source;

	unsigned int sz = strlen(gs);
	m_geom_shader_source = new char[sz+1];
	strcpy(m_geom_shader_source,gs);

	if(!loadGeometryShaderSourceString(gs)) return false;
	return true;
}

bool GLSLShader::recompile()
{
	std::cout << "Recomp: "<<  m_vertex_shader_source << std::endl;

	if (m_vertex_shader_source)
		if(!loadVertexShaderSourceString(m_vertex_shader_source)) return false;

	if (m_fragment_shader_source)
		if(!loadFragmentShaderSourceString(m_fragment_shader_source)) return false;

	if (m_geom_shader_source)
		if(!loadGeometryShaderSourceString(m_geom_shader_source)) return false;

	if(!create(m_geom_inputPrimitives,m_geom_outputPrimitives))
	{
		std::cout << "Unable to create the shaders !" << std::endl;
		return false;
	}

	m_uniMat_Proj		= glGetUniformLocation(m_program_object,"ProjectionMatrix");
	m_uniMat_Model		= glGetUniformLocation(m_program_object,"ModelViewMatrix");
	m_uniMat_ModelProj	= glGetUniformLocation(m_program_object,"ModelViewProjectionMatrix");
	m_uniMat_Normal		= glGetUniformLocation(m_program_object,"NormalMatrix");

	restoreUniformsAttribs();

	return true;
}


bool GLSLShader::validateProgram()
{
	if(!m_program_object)
		return false;

	glValidateProgram(m_program_object);
	GLint Result = GL_FALSE;
	glGetProgramiv(m_program_object, GL_VALIDATE_STATUS, &Result);

	if(Result == GL_FALSE)
	{
		std::cout << "Validate program:" << std::endl;
		int InfoLogLength;
		glGetProgramiv(m_program_object, GL_INFO_LOG_LENGTH, &InfoLogLength);
		std::vector<char> Buffer(InfoLogLength);
		glGetProgramInfoLog(m_program_object, InfoLogLength, NULL, &Buffer[0]);
		std::cout <<  &(Buffer[0]) << std::endl;
		return false;
	}

	return true;
}

bool GLSLShader::checkProgram()
{
	GLint Result = GL_FALSE;
	glGetProgramiv(m_program_object, GL_LINK_STATUS, &Result);

	int InfoLogLength;
	glGetProgramiv(m_program_object, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> Buffer(std::max(InfoLogLength, int(1)));
	glGetProgramInfoLog(m_program_object, InfoLogLength, NULL, &Buffer[0]);
	std::cout << &Buffer[0] << std::endl;

	return Result == GL_TRUE;
}

bool GLSLShader::checkShader(int shaderType)
{
	GLint Result = GL_FALSE;
	int InfoLogLength;
	GLhandleARB id;

	switch(shaderType)
	{
	case VERTEX_SHADER:
		id = m_vertex_shader_object;
		break;
	case FRAGMENT_SHADER:
		id = m_fragment_shader_object;
		break;
	case GEOMETRY_SHADER:
		id = m_geom_shader_object;
		break;
	default:
		std::cerr << "Error unkown shader type"<< std::endl;
		return false;
		break;
	}

	glGetShaderiv(id, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> Buffer(InfoLogLength);
	glGetShaderInfoLog(id, InfoLogLength, NULL, &Buffer[0]);
	std::cout << &Buffer[0] << std::endl;

	return Result == GL_TRUE;
}



void GLSLShader::bindAttrib(unsigned int att, const char* name) const
{
	glBindAttribLocation(m_program_object,att,name);
}


void GLSLShader::addPathFileSeach(const std::string& path)
{
	m_pathes.push_back(path);
}

void GLSLShader::unbindVBO(VBO* ptr)
{
	unsigned int nb = m_va_vbo_binding.size();
	for (unsigned int i =0; i<nb; ++i)
	{
		if (m_va_vbo_binding[i].vbo_ptr == ptr)
		{
			if (i != (nb-1))
				m_va_vbo_binding[i] = m_va_vbo_binding[nb-1];
			m_va_vbo_binding.pop_back();
			return;
		}
	}
}

unsigned int GLSLShader::bindVA_VBO(const std::string& name, VBO* vbo)
{
	vbo->ref(this);

	GLint idVA = glGetAttribLocation(this->m_program_object, name.c_str());
	//valid ?
	if (idVA < 0)
	{
		std::cerr << "GLSLShader: Attribute "<<name<< " does not exist in shader"<< std::endl;
		return idVA;
	}
	// search if name already exist
	for (std::vector<VAStr>::iterator it = m_va_vbo_binding.begin(); it != m_va_vbo_binding.end(); ++it)
	{
		if (it->va_id == idVA)
		{
			it->vbo_ptr = vbo;
			return (it - m_va_vbo_binding.begin());
		}
	}
	// new one:
	VAStr temp;
	temp.va_id = idVA;
	temp.vbo_ptr = vbo;
	m_va_vbo_binding.push_back(temp);
	return (m_va_vbo_binding.size() -1);
}

void GLSLShader::changeVA_VBO(unsigned int id, VBO* vbo)
{
	m_va_vbo_binding[id].vbo_ptr = vbo;
}


void GLSLShader::unbindVA(const std::string& name)
{
	GLint idVA = glGetAttribLocation(this->m_program_object, name.c_str());
	//valid ?
	if (idVA < 0)
	{
		std::cerr << "GLSLShader: Attribute "<<name<< " does not exist in shader, not unbinded"<< std::endl;
		return;
	}
	// search if name already exist
	unsigned int nb = m_va_vbo_binding.size();
	for (unsigned int i =0; i<nb; ++i)
	{
		if (m_va_vbo_binding[i].va_id == idVA)
		{
			if (i != (nb-1))
				m_va_vbo_binding[i] = m_va_vbo_binding[nb-1];
			m_va_vbo_binding.pop_back();
			return;
		}
	}
	std::cerr << "GLSLShader: Attribute "<<name<< " not binded"<< std::endl;
}

void GLSLShader::setCurrentOGLVersion(unsigned int version)
{
	CURRENT_OGL_VERSION=version;
	switch(version)
	{
	case 2:
		DEFINES_GL = &DEFINES_GL2;
		break;
	case 3:
		DEFINES_GL = &DEFINES_GL3;
		break;
	}
}

/**
 * update projection, modelview, ... matrices
 */
void GLSLShader::updateMatrices(const glm::mat4& projection, const glm::mat4& modelview)
{
	this->bind();
	glm::mat4 PMV = projection*modelview;
	glm::mat4 normalMatrix = glm::gtx::inverse_transpose::inverseTranspose(modelview);


	if (m_uniMat_Proj>=0)
		glUniformMatrix4fv(m_uniMat_Proj,		1 , false, &projection[0][0]);
	if (m_uniMat_Model>=0)
		glUniformMatrix4fv(m_uniMat_Model,		1 , false, &modelview[0][0]);
	if (m_uniMat_ModelProj>=0)
		glUniformMatrix4fv(m_uniMat_ModelProj,	1 , false, &PMV[0][0]);
	if (m_uniMat_Normal>=0)
		glUniformMatrix4fv(m_uniMat_Normal, 	1 , false, &normalMatrix[0][0]);

}

void GLSLShader::enableVertexAttribs(unsigned int stride) const
{
	this->bind();
	for (std::vector<Utils::GLSLShader::VAStr>::const_iterator it= m_va_vbo_binding.begin(); it != m_va_vbo_binding.end(); ++it)
	{
		glBindBuffer(GL_ARRAY_BUFFER, it->vbo_ptr->id());
		glEnableVertexAttribArray(it->va_id);
		glVertexAttribPointer(it->va_id ,it->vbo_ptr->dataSize(), GL_FLOAT, false, stride, 0);

	}
}

void GLSLShader::disableVertexAttribs() const
{
	for (std::vector<Utils::GLSLShader::VAStr>::const_iterator it= m_va_vbo_binding.begin(); it != m_va_vbo_binding.end(); ++it)
		glEnableVertexAttribArray(it->va_id);
}

} // namespace Utils
} // namespace CGoGN
