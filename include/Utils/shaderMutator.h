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

#ifndef _CGoGN_SHADERMUTATOR_H_
#define _CGoGN_SHADERMUTATOR_H_

#include "Utils/cgognStream.h"
#include <boost/regex.hpp>
#include <string>
#include <sstream>

namespace CGoGN
{

namespace Utils
{

class ShaderMutator
{
	
public:
	/**
	 * constructor
	 * @param vertShaderSrc the vertex shader source to store
	 * @param fragShaderSrc the fragment shader source to store
	 * @param geomShaderSrc the geometry shader source to store
	 */
	 ShaderMutator(const std::string& shaderName, const std::string& vertShaderSrc, const std::string& fragShaderSrc, const std::string& geomShaderSrc);
	 
	 /**
	  * check if a variable is declared in the vertex shader source
	  * @param variableName the variable to search for
	  */
	 bool VS_containsVariableDeclaration(const std::string& variableName);

	 /**
	  * check if a variable is declared in the fragment shader source
	  * @param variableName the variable to search for
	  */
	 bool FS_containsVariableDeclaration(const std::string& variableName);
	 
	 /**
	  * check if a variable is declared in the geometry shader source
	  * @param variableName the variable to search for
	  */
	 bool GS_containsVariableDeclaration(const std::string& variableName);
	 
	 /**
	  * set or change shading language version if the current version is lower
	  * @param version the version to set (110, 120, 150...)
	  */
	 void VS_FS_GS_setMinShadingLanguageVersion(int version);

	 /**
	  * insert code before main function into shader vertex source code
	  * @param insertedCode source code to insert into shader
	  */
	 void VS_insertCodeBeforeMainFunction(const std::string& insertedCode);
	 
	 /**
	  * insert code before main function into shader fragment source code
	  * @param insertedCode source code to insert into shader
	  */
	 void FS_insertCodeBeforeMainFunction(const std::string& insertedCode);
	 
	 /**
	  * insert code before main function into shader geometry source code
	  * @param insertedCode source code to insert into shader
	  */
	 void GS_insertCodeBeforeMainFunction(const std::string& insertedCode);
	 
	 /**
	  * insert code at the beginning of main function into shader vertex source code
	  * @param insertedCode source code to insert into shader
	  */
	 void VS_insertCodeAtMainFunctionBeginning(const std::string& insertedCode);
	 
	 /**
	  * insert code at the beginning of main function into shader fragment source code
	  * @param insertedCode source code to insert into shader
	  */
	 void FS_insertCodeAtMainFunctionBeginning(const std::string& insertedCode);
	 
	 /**
	  * insert code at the beginning of main function into shader geometry source code
	  * @param insertedCode source code to insert into shader
	  */
	 void GS_insertCodeAtMainFunctionBeginning(const std::string& insertedCode);
	 
	 /**
	  * insert code at the beginning of main function into shader vertex source code
	  * @warning takes the number of opening and closing braces of main function into account
	  * @param insertedCode source code to insert into shader
	  */
	 void VS_insertCodeAtMainFunctionEnd(const std::string& insertedCode);
	 
	 /**
	  * insert code at the beginning of main function into shader fragment source code
	  * @warning takes the number of opening and closing braces of main function into account
	  * @param insertedCode source code to insert into shader
	  */
	 void FS_insertCodeAtMainFunctionEnd(const std::string& insertedCode);
	 
	 /**
	  * insert code at the beginning of main function into shader geometry source code
	  * @warning takes the number of opening and closing braces of main function into account
	  * @param insertedCode source code to insert into shader
	  */
	 void GS_insertCodeAtMainFunctionEnd(const std::string& insertedCode);
	 
	 /**
	  * returns the modified vertex shader source code
	  */
	 std::string getModifiedVertexShaderSrc() { return m_vShaderMutation; }
	 
	 /**
	  * returns the modified fragment shader source code
	  */
	 std::string getModifiedFragmentShaderSrc() { return m_fShaderMutation; }
	 
	 /**
	  * returns the modified geometry shader source code
	  */
	 std::string getModifiedGeometryShaderSrc() { return m_gShaderMutation; }
	
private:

	 /**
	  * processed shader name stored for log purpose
	  */
	 std::string m_shaderName;
	
	/**
	 * modified version of the original vertex shader source code
	 */
	std::string m_vShaderMutation;
	
	/**
	 * modified version of the original fragment shader source code
	 */
	std::string m_fShaderMutation;
	
	/**
	 * modified version of the original geometry shader source code
	 */
	std::string m_gShaderMutation;
	
	/**
	 * verify if the given position in the string is commented or not
	 * @param pos the position
	 * @param str the string to analyze
	 */
	bool isCommented(size_t pos, const std::string& str);
	
	/**
	 * verify if the given position in the string is commented with a one-line comment or not
	 * @param pos the position
	 * @param str the string to analyze
	 */
	bool isOneLineCommented(size_t pos, const std::string& str);
	
	/**
	  * check if a variable is declared in a source code or not
	  * @param variable the variable to search for
	 */
	bool containsVariableDeclaration(const std::string& variableName, std::string& src);
	
	/**
	 * set or change shading language version if the current version is lower
	 * @param version the version to set (110, 120, 150...)
	 * @param modifiedSrc shader source code to modify
	 */
	bool setMinShadingLanguageVersion(int version, std::string& modifiedSrc);

	/**
	 * insert code before main function into source code
	 * @param insertedCode source code to insert into shader
	 * @param modifiedSrc shader source code to modify
	 */
	bool insertCodeBeforeMainFunction(const std::string& insertedCode, std::string& modifiedSrc);
	
	/**
	 * insert code at the beginning of main function into source code
	 * @param insertedCode source code to insert into shader
	 * @param modifiedSrc shader source code to modify
	 */
	bool insertCodeAtMainFunctionBeginning(const std::string& insertedCode, std::string& modifiedSrc);

	/**
	 * insert code at the end of main function into source code
	 * @param insertedCode source code to insert into shader
	 * @param modifiedSrc shader source code to modify
	 */
	bool insertCodeAtMainFunctionEnd(const std::string& insertedCode, std::string& modifiedSrc);
	
};

} // namespace Utils

} // namespace CGoGN

#endif
