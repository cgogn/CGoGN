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
	 * enum used to choose which shader src type to modify
	 */
	enum shaderSrcType { VERTEX_SHADER, FRAGMENT_SHADER, GEOMETRY_SHADER };

	/**
	 * constructor
	 * @param vertShaderSrc the vertex shader source to store
	 * @param fragShaderSrc the fragment shader source to store
	 * @param geomShaderSrc the geometry shader source to store
	 */
	 ShaderMutator(const std::string& shaderName, const std::string& vertShaderSrc, const std::string& fragShaderSrc, const std::string& geomShaderSrc = "");
	 
	 /**
	  * check if a variable is declared in the shader source
	  * @param srcType the shader source to use (vertex, fragment or geometry)
	  * @param variableName the variable to search for
	  */
	 bool containsVariableDeclaration(shaderSrcType srcType, const std::string& variableName);
	 
	 /**
	  * set or change shading language version in the shader source
	  * - only if the current version is lower
	  * @param srcType the shader source to use (vertex, fragment or geometry)
	  * @param version the version to set (110, 120, 150...)
	  */
	 void setMinShadingLanguageVersion(shaderSrcType srcType, int version);

	 /**
	  * change int constant value in the shader source
	  * @param srcType the shader source to use (vertex, fragment or geometry)
	  * @param newVal the new value
	  */
	 void changeIntConstantValue(shaderSrcType srcType, const std::string& constantName, int newVal);

	 /**
	  * insert code before main function into shader source
	  * @param srcType the shader source to use (vertex, fragment or geometry)
	  * @param insertedCode source code to insert into shader
	  */
	 void insertCodeBeforeMainFunction(shaderSrcType srcType, const std::string& insertedCode);
	 
	 /**
	  * insert code at the beginning of main function into shader source
	  * @param srcType the shader source to use (vertex, fragment or geometry)
	  * @param insertedCode source code to insert into shader
	  */
	 void insertCodeAtMainFunctionBeginning(shaderSrcType srcType, const std::string& insertedCode);
	 
	 /**
	  * insert code at the end of main function into shader source
	  * @warning takes the number of opening and closing braces of main function into account
	  * @param srcType the shader source to use (vertex, fragment or geometry)
	  * @param insertedCode source code to insert into shader
	  */
	 void insertCodeAtMainFunctionEnd(shaderSrcType srcType, const std::string& insertedCode);
	 
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
	 * check if the given position in the source is commented
	 * @param pos the position
	 * @param src the source to analyze
	 */
	bool srcIsCommented(size_t pos, const std::string& src);
	
	/**
	 * check if the given position in the source is commented with a one-line comment
	 * @param pos the position
	 * @param src the source to analyze
	 */
	bool srcIsOneLineCommented(size_t pos, const std::string& src);
	
	/**
	  * check if a variable is declared in a source code
	  * @param variable the variable to search for
	 */
	bool srcContainsVariableDeclaration(const std::string& variableName, std::string& src);
	
	/**
	 * set or change shading language version if the current version is lower
	 * @param version the version to set (110, 120, 150...)
	 * @param modifiedSrc shader source code to modify
	 */
	bool srcSetMinShadingLanguageVersion(int version, std::string& modifiedSrc);

	/**
	 * change int constant value
	 * @param newVal the new value
	 * @param modifiedSrc shader source code to modify
	 */
	bool srcChangeIntConstantValue(int newVal, const std::string& constantName, std::string& modifiedSrc);

	/**
	 * insert code before main function into source code
	 * @param insertedCode source code to insert into shader
	 * @param modifiedSrc shader source code to modify
	 */
	bool srcInsertCodeBeforeMainFunction(const std::string& insertedCode, std::string& modifiedSrc);
	
	/**
	 * insert code at the beginning of main function into source code
	 * @param insertedCode source code to insert into shader
	 * @param modifiedSrc shader source code to modify
	 */
	bool srcInsertCodeAtMainFunctionBeginning(const std::string& insertedCode, std::string& modifiedSrc);

	/**
	 * insert code at the end of main function into source code
	 * @param insertedCode source code to insert into shader
	 * @param modifiedSrc shader source code to modify
	 */
	bool srcInsertCodeAtMainFunctionEnd(const std::string& insertedCode, std::string& modifiedSrc);
	
};

} // namespace Utils

} // namespace CGoGN

#endif
