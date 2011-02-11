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


namespace CGoGN
{

namespace Utils
{

GLSLShader::GLSLShader()
{
	vertex_shader_object = 0;
	fragment_shader_object = 0;
	geom_shader_object = 0;

	program_object = 0;
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
	char	*vertex_shader_source;


	vertex_shader_source = loadSourceFile( filename );

	if( !vertex_shader_source )
	{
		std::cerr << "ERROR - GLSLShader::loadVertexShader() - error occured while loading source file." << std::endl;
		return false;
	}


	flag = loadVertexShaderSourceString( vertex_shader_source );
	delete [] vertex_shader_source;


	return flag;
}


bool GLSLShader::loadPixelShader(const std::string& filename )
{
	bool	flag;
	char	*pixel_shader_source;


	pixel_shader_source = loadSourceFile( filename );

	if( !pixel_shader_source )
	{
		std::cerr << "ERROR - GLSLShader::loadPixelShader() - error occured while loading source file." << std::endl;
		return false;
	}


	flag = loadPixelShaderSourceString( pixel_shader_source );
	delete [] pixel_shader_source;


	return flag;
}


bool GLSLShader::loadGeometryShader(const std::string& filename )
{
	bool	flag;
	char	*geom_shader_source;

	geom_shader_source = loadSourceFile( filename );



	if( !geom_shader_source )
	{
		std::cerr << "ERROR - GLSLShader::loadGeometryShader() - error occured while loading source file." << std::endl;
		return false;
	}

	flag = loadGeometryShaderSourceString( geom_shader_source );
	delete [] geom_shader_source;


	return flag;
}


bool GLSLShader::loadVertexShaderSourceString( const char *vertex_shader_source )
{
	int		status;
	char	*info_log;


	/*** create shader object ***/
	vertex_shader_object = glCreateShaderObjectARB( GL_VERTEX_SHADER_ARB );

	if( !vertex_shader_object )
	{
		std::cerr << "ERROR - GLSLShader::loadVertexShader() - unable to create shader object." << std::endl;
		return false;
	}


	/*** load source file ***/
	if( !vertex_shader_source )
	{
		std::cerr << "ERROR - GLSLShader::loadVertexShader() - source string is empty." << std::endl;

		glDeleteObjectARB( vertex_shader_object );
		vertex_shader_object = 0;

		return false;
	}

	glShaderSourceARB( vertex_shader_object, 1, (const char**)&vertex_shader_source, NULL );


	/*** compile shader object ***/
	glCompileShaderARB( vertex_shader_object );

	glGetObjectParameterivARB( vertex_shader_object, GL_OBJECT_COMPILE_STATUS_ARB, &status );
	if( !status )
	{
		std::cerr << "ERROR - GLshader::loadVertexShader() - error occured while compiling shader." << std::endl;
		info_log = getInfoLog( vertex_shader_object );
		std::cerr << "  COMPILATION " << info_log << std::endl;
		delete [] info_log;

		glDeleteObjectARB( vertex_shader_object );
		vertex_shader_object = 0;

		return false;
	}


	/*** termination ***/
	return true;
}


bool GLSLShader::loadPixelShaderSourceString( const char *pixel_shader_source )
{
	int		status;
	char	*info_log;


	/*** create shader object ***/
	fragment_shader_object = glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB );

	if( !fragment_shader_object )
	{
		std::cerr << "ERROR - GLSLShader::loadPixelShader() - unable to create shader object." << std::endl;
		return false;
	}


	/*** load source file ***/
	if( !pixel_shader_source )
	{
		std::cerr << "ERROR - GLSLShader::loadPixelShader() - source string is empty." << std::endl;

		glDeleteObjectARB( fragment_shader_object );
		fragment_shader_object = 0;

		return false;
	}

	glShaderSourceARB( fragment_shader_object, 1, (const char**)&pixel_shader_source, NULL );


	/*** compile shader object ***/
	glCompileShaderARB( fragment_shader_object );

	glGetObjectParameterivARB( fragment_shader_object, GL_OBJECT_COMPILE_STATUS_ARB, &status );
	if( !status )
	{
		std::cerr << "ERROR - GLshader::loadPixelShader() - error occured while compiling shader." << std::endl;
		info_log = getInfoLog( fragment_shader_object );
		std::cerr << "  COMPILATION " << info_log << std::endl;
		delete [] info_log;

		glDeleteObjectARB( fragment_shader_object );
		fragment_shader_object = 0;

		return false;
	}


	/*** termination ***/
	return true;
}



bool GLSLShader::loadGeometryShaderSourceString( const char *geom_shader_source )
{
	int		status;
	char	*info_log;


	/*** create shader object ***/
	// glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB );
	geom_shader_object = glCreateShader(GL_GEOMETRY_SHADER_EXT);


	if( !geom_shader_object )
	{
		std::cerr << "ERROR - GLSLShader::loadGeometryShader() - unable to create shader object." << std::endl;
		return false;
	}


	/*** load source file ***/
	if( !geom_shader_source )
	{
		std::cerr << "ERROR - GLSLShader::loadGeometryShader() - source string is empty." << std::endl;

		glDeleteObjectARB( geom_shader_object );
		geom_shader_object = 0;

		return false;
	}

	glShaderSourceARB( geom_shader_object, 1, (const char**)&geom_shader_source, NULL );


	/*** compile shader object ***/
	glCompileShaderARB( geom_shader_object );

	glGetObjectParameterivARB( geom_shader_object, GL_OBJECT_COMPILE_STATUS_ARB, &status );
	if( !status )
	{
		std::cerr << "ERROR - GLshader::loadGeometryShader() - error occured while compiling shader." << std::endl;
		info_log = getInfoLog( geom_shader_object );
		std::cerr << "  COMPILATION " << info_log << std::endl;
		delete [] info_log;

		glDeleteObjectARB( geom_shader_object );
		geom_shader_object = 0;

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


	/*** check if shaders are loaded ***/
	if( !vertex_shader_object || !vertex_shader_object )
	{
		std::cerr << "ERROR - GLSLShader::create() - at least one shader is not defined." << std::endl;
		return false;
	}


	/*** create program object ***/
	program_object = glCreateProgramObjectARB();

	if( !program_object )
	{
		std::cerr << "ERROR - GLSLShader::create() - unable to create program object." << std::endl;
		return false;
	}


	/*** attach shaders to program object ***/
	glAttachObjectARB( program_object, vertex_shader_object );
	glAttachObjectARB( program_object, fragment_shader_object );
	if (geom_shader_object)
	{
		glAttachObjectARB( program_object, geom_shader_object );

		glProgramParameteriEXT(program_object,GL_GEOMETRY_INPUT_TYPE_EXT,inputGeometryPrimitive);
		glProgramParameteriEXT(program_object,GL_GEOMETRY_OUTPUT_TYPE_EXT,outputGeometryPrimitive);
		int temp;
		glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT,&temp);
		glProgramParameteriEXT(program_object,GL_GEOMETRY_VERTICES_OUT_EXT,temp);
	}


	/*** link program object ***/
	glLinkProgramARB( program_object );

	glGetObjectParameterivARB( program_object, GL_OBJECT_LINK_STATUS_ARB, &status );
	if( !status )
	{
		std::cerr << "ERROR - GLSLShader::create() - error occured while linking shader program." << std::endl;
		info_log = getInfoLog( program_object );
		std::cerr << "  LINK " << info_log << std::endl;
		delete [] info_log;

		glDetachObjectARB( program_object, vertex_shader_object );
		glDetachObjectARB( program_object, fragment_shader_object );
		if (geom_shader_object)
			glDetachObjectARB( program_object, geom_shader_object );
		glDeleteObjectARB( program_object );
		program_object = 0;

		return false;
	}


	/*** creates the list of uniform variables and samplers ***/
//	GLint		active_uniform_count;
//	GLcharARB	name_char[256];
//	int			length;
//	int			size;
//	int			type;
//
//	glGetObjectParameterivARB( program_object, GL_OBJECT_ACTIVE_UNIFORMS_ARB, &active_uniform_count );
//
//	for( GLuint i=0; i<(GLuint)active_uniform_count; i++ )
//	{
//		glGetActiveUniformARB( program_object, i, 256, &length, &size, (GLenum*)&type, name_char );
//		std::string		name = name_char;
//
//		if( name.find( std::string("gl_"), 0 ) != std::string::npos )
//			continue;
//		else if( type<GL_SAMPLER_1D_ARB || type>GL_SAMPLER_2D_RECT_SHADOW_ARB )
//		{
//			unif_list[name].location = glGetUniformLocationARB( program_object, name_char );
//			unif_list[name].size = size;
//			unif_list[name].type = type;
//		}
//		else
//		{
//			sampler_list[name].location = glGetUniformLocationARB( program_object, name_char );
//		}
//	}


	/*** creates the list of vertex attributes */
//	GLint	activeAttribCount;
//	glGetObjectParameterivARB( program_object, GL_OBJECT_ACTIVE_ATTRIBUTES_ARB, &activeAttribCount );
//
//	for( GLuint i=0; i<(GLuint)activeAttribCount; i++ )
//	{
//		glGetActiveAttribARB( program_object, i, 256, &length, &size, (GLenum*)&type, name_char );
//		attrib_list[ name_char ] = i;
//	}


	/*** termination ***/
	return true;
}


bool GLSLShader::bind()
{
	if( program_object )
	{
		glUseProgramObjectARB( program_object );

		return true;
	}
	else  return false;
}


void GLSLShader::unbind()
{
	if( program_object )
	{
		glUseProgramObjectARB( 0 );
	}
}


bool GLSLShader::isBinded()
{
	return ( program_object && program_object == glGetHandleARB(GL_PROGRAM_OBJECT_ARB) );
}

//
//bool GLSLShader::setSampler( char* sampler_name, int tex_unit )
//{
//	GLuint		pgObjBackup;
//	std::string	name = sampler_name;
//
//
//	if( sampler_list.find( name ) != sampler_list.end() )
//	{
//		pgObjBackup = glGetHandleARB( GL_PROGRAM_OBJECT_ARB );
//		glUseProgramObjectARB( program_object );
//
//		sampler_list[name].tex_unit = tex_unit - GL_TEXTURE0_ARB;
//		glUniform1iARB( sampler_list[name].location, sampler_list[name].tex_unit );
//
//		glUseProgramObjectARB( pgObjBackup );
//
//		return true;
//	}
//
//
//	return false;
//}
//
//
//bool GLSLShader::setUniform( char* unif_name, void* value )
//{
//	GLuint		pgObjBackup;
//	std::string	name = unif_name;
//
//
//	if( unif_list.find( name ) != unif_list.end() )
//	{
//		Uniform	u = unif_list[name];
//
//		pgObjBackup = glGetHandleARB( GL_PROGRAM_OBJECT_ARB );
//		glUseProgramObjectARB( program_object );
//
//		switch( u.type )
//		{
//			case GL_FLOAT:
//				glUniform1fvARB( u.location, u.size, (GLfloat*) value ); break;
//			case GL_FLOAT_VEC2_ARB:
//				glUniform2fvARB( u.location, u.size, (GLfloat*) value ); break;
//			case GL_FLOAT_VEC3_ARB:
//				glUniform3fvARB( u.location, u.size, (GLfloat*) value ); break;
//			case GL_FLOAT_VEC4_ARB:
//				glUniform4fvARB( u.location, u.size, (GLfloat*) value ); break;
//			case GL_INT:
//				glUniform1ivARB( u.location, u.size, (GLint*) value ); break;
//			case GL_INT_VEC2_ARB:
//				glUniform2ivARB( u.location, u.size, (GLint*) value ); break;
//			case GL_INT_VEC3_ARB:
//				glUniform3ivARB( u.location, u.size, (GLint*) value ); break;
//			case GL_INT_VEC4_ARB:
//				glUniform4ivARB( u.location, u.size, (GLint*) value ); break;
//			case GL_BOOL_ARB:
//				glUniform1ivARB( u.location, u.size, (GLint*) value ); break;
//			case GL_BOOL_VEC2_ARB:
//				glUniform2ivARB( u.location, u.size, (GLint*) value ); break;
//			case GL_BOOL_VEC3_ARB:
//				glUniform3ivARB( u.location, u.size, (GLint*) value ); break;
//			case GL_BOOL_VEC4_ARB:
//				glUniform4ivARB( u.location, u.size, (GLint*) value ); break;
//			case GL_FLOAT_MAT2_ARB:
//				glUniformMatrix2fvARB( u.location, u.size, GL_FALSE, (GLfloat*) value ); break;
//			case GL_FLOAT_MAT3_ARB:
//				glUniformMatrix3fvARB( u.location, u.size, GL_FALSE, (GLfloat*) value ); break;
//			case GL_FLOAT_MAT4_ARB:
//				glUniformMatrix4fvARB( u.location, u.size, GL_FALSE, (GLfloat*) value ); break;
//		}
//
//		glUseProgramObjectARB( pgObjBackup );
//
//		return true;
//	}
//
//
//	return false;
//}


//GLuint GLSLShader::getAttribIndex( char* attribName )
//{
//	return attrib_list[ attribName ];
//}


GLSLShader::~GLSLShader()
{
	if( program_object )
	{
		unbind();

		if( vertex_shader_object )
		{
			glDetachObjectARB( program_object, vertex_shader_object );
			glDeleteObjectARB( vertex_shader_object );
		}
		if( fragment_shader_object )
		{
			glDetachObjectARB( program_object, fragment_shader_object );
			glDeleteObjectARB( fragment_shader_object );
		}
		if (geom_shader_object)
		{
			glDetachObjectARB( program_object, geom_shader_object );
			glDeleteObjectARB( geom_shader_object );
		}

		glDeleteObjectARB( program_object );
	}
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
	if(!loadPixelShader(pss)) return false;

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
	if(!loadPixelShader(pss)) return false;

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


void GLSLShader::bindAttrib(unsigned int att, const char* name)
{
	glBindAttribLocation(program_object,att,name);
}




} // namespace Utils
} // namespace CGoGN
