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

#include "Utils/shaderMutator.h"

namespace CGoGN
{

namespace Utils
{

ShaderMutator::ShaderMutator(const std:: string& shaderName, const std::string& vertShaderSrc, const std::string& fragShaderSrc, const std::string& geomShaderSrc)
{
	// Store the shader name
	m_shaderName = shaderName;

	// Store the shader source codes
	m_vShaderMutation = vertShaderSrc;
	m_fShaderMutation = fragShaderSrc;
	m_gShaderMutation = geomShaderSrc;
}

bool ShaderMutator::VS_containsVariableDeclaration(const std::string& variableName)
{
	return containsVariableDeclaration(variableName, m_vShaderMutation);
}

bool ShaderMutator::FS_containsVariableDeclaration(const std::string& variableName)
{
	return containsVariableDeclaration(variableName, m_fShaderMutation);
}

bool ShaderMutator::GS_containsVariableDeclaration(const std::string& variableName)
{
	return containsVariableDeclaration(variableName, m_gShaderMutation);
}

void ShaderMutator::VS_insertCodeBeforeMainFunction(const std::string& insertedCode)
{
	if (!insertCodeBeforeMainFunction(insertedCode, m_vShaderMutation))
	{
		CGoGNerr
		<< "ERROR - "
		<< "ShaderMutator::VS_insertCodeBeforeMainFunction : "
		<< "Unable to insert source code in vertex shader of "
		<< m_shaderName
		<< ". You should check if the shader has a main function declaration"
		<< CGoGNendl;
	}
}

void ShaderMutator::FS_insertCodeBeforeMainFunction(const std::string& insertedCode)
{
	if (!insertCodeBeforeMainFunction(insertedCode, m_fShaderMutation))
	{
		CGoGNerr
		<< "ERROR - "
		<< "ShaderMutator::FS_insertCodeBeforeMainFunction : "
		<< "Unable to insert source code in fragment shader of "
		<< m_shaderName
		<< ". You should check if the shader has a main function declaration"
		<< CGoGNendl;
	}
}

void ShaderMutator::GS_insertCodeBeforeMainFunction(const std::string& insertedCode)
{
	if (!insertCodeBeforeMainFunction(insertedCode, m_gShaderMutation))
	{
		CGoGNerr
		<< "ERROR - "
		<< "ShaderMutator::GS_insertCodeBeforeMainFunction : "
		<< "Unable to insert source code in geometry shader of "
		<< m_shaderName
		<< ". You should check if the shader has a main function declaration"
		<< CGoGNendl;
	}
}

void ShaderMutator::VS_insertCodeAtMainFunctionBeginning(const std::string& insertedCode)
{
	if (!insertCodeAtMainFunctionBeginning(insertedCode, m_vShaderMutation))
	{
		CGoGNerr
		<< "ERROR - "
		<< "ShaderMutator::VS_insertCodeAtMainFunctionBeginnning : "
		<< "Unable to insert source code vertex shader of "
		<< m_shaderName
		<< ". You should check if the shader has a main function declaration"
		<< CGoGNendl;
	}
}

void ShaderMutator::FS_insertCodeAtMainFunctionBeginning(const std::string& insertedCode)
{
	if (!insertCodeAtMainFunctionBeginning(insertedCode, m_fShaderMutation))
	{
		CGoGNerr
		<< "ERROR - "
		<< "ShaderMutator::FS_insertCodeAtMainFunctionBeginnning : "
		<< "Unable to insert source code in fragment shader of "
		<< m_shaderName
		<< ". You should check if the shader has a main function declaration"
		<< CGoGNendl;
	}
}

void ShaderMutator::GS_insertCodeAtMainFunctionBeginning(const std::string& insertedCode)
{
	if (!insertCodeAtMainFunctionBeginning(insertedCode, m_gShaderMutation))
	{
		CGoGNerr
		<< "ERROR - "
		<< "ShaderMutator::GS_insertCodeAtMainFunctionBeginnning : "
		<< "Unable to insert source code in geometry shader of "
		<< m_shaderName
		<< ". You should check if the shader has a main function declaration"
		<< CGoGNendl;
	}
}

void ShaderMutator::VS_insertCodeAtMainFunctionEnd(const std::string& insertedCode)
{
	if (!insertCodeAtMainFunctionEnd(insertedCode, m_vShaderMutation))
	{
		CGoGNerr
		<< "ERROR - "
		<< "ShaderMutator::VS_insertCodeAtMainFunctionEnd : "
		<< "Unable to insert source code in vertex shader of "
		<< m_shaderName
		<< ". You should check if the shader has a main function declaration "
		<< "and as many '{' as '}'"
		<< CGoGNendl;
	}
}

void ShaderMutator::FS_insertCodeAtMainFunctionEnd(const std::string& insertedCode)
{
	if (!insertCodeAtMainFunctionEnd(insertedCode, m_fShaderMutation))
	{
		CGoGNerr
		<< "ERROR - "
		<< "ShaderMutator::FS_insertCodeAtMainFunctionEnd : "
		<< "Unable to insert source code in fragment shader of "
		<< m_shaderName
		<< ". You should check if the shader has a main function declaration "
		<< "and as many '{' as '}'"
		<< CGoGNendl;
	}
}

void ShaderMutator::GS_insertCodeAtMainFunctionEnd(const std::string& insertedCode)
{
	if (!insertCodeAtMainFunctionEnd(insertedCode, m_gShaderMutation))
	{
		CGoGNerr
		<< "ERROR - "
		<< "ShaderMutator::GS_insertCodeAtMainFunctionEnd : "
		<< "Unable to insert source code in geometry shader of "
		<< m_shaderName
		<< ". You should check if the shader has a main function declaration "
		<< "and as many '{' as '}'"
		<< CGoGNendl;
	}
}







bool ShaderMutator::isCommented(size_t pos, const std::string& str)
{
	// Verify that the given position is not out of the string
	if (pos >= str.length())
	{
		CGoGNerr
		<< "ERROR - "
		<< "ShaderMutator::isCommented : "
		<< "Given position is out of range"
		<< CGoGNendl;
		return false;
	}
	
	// Look backward in the string to see if there is any comment symbol (// or /* */)
	
	// First look for one-line comments
	if (isOneLineCommented(pos, str))
		return true;
	
	// Now look for multi-line comments 
	size_t i;
	for (i = pos; i > 0; i--)
	{	
		if (str[i] == '/')
		{
			// End of multi-line comment
			if (str[i-1] == '*')
			{
				// Verify that the end of multi-line comment is not one-line commented !
				if (!isOneLineCommented(i, str))
					return false;
			}
		}
		else if (str[i] == '*')
		{
			// Beginning of multi-line comment
			if (str[i-1] == '/')
			{
				// Verify that the beginning of multi-line comment is not one-line commented !
				if (!isOneLineCommented(i, str))
					return true;
			}
		}
	}
	
	// No one-line or multi-line comments were found
	return false;
	
}

bool ShaderMutator::isOneLineCommented(size_t pos, const std::string& str)
{
	// Verify that the given position is not out of the string
	if (pos >= str.length())
	{
		CGoGNerr
		<< "ERROR - "
		<< "ShaderMutator::isOneLineCommented : "
		<< "Given position is out of range"
		<< CGoGNendl;
		return false;
	}
	
	// Look backward in the string to see if there is any "//"
	size_t i;
	for (i = pos; i > 0; i--)
	{
		// As soon as a '\n' is found, any other "//" will not affect this line anymore
		if (str[i] == '\n')
			return false;
		else if (str[i] == '/')
			if (str[i-1] == '/')
				return true;
	}
	
	// No one-line comments were found
	return false;
}

bool ShaderMutator::containsVariableDeclaration(const std::string& variableName, std::string& src)
{
	// Regular expression for variable declaration
	// <',' OR white-space[1 or more times]> <variableName> <',' OR ';' OR white-space>
	boost::regex var_re("(,|\\s+)" + variableName + "(,|;|\\s)");
	
	// Matches results
	boost::match_results <std::string::iterator> matches;
	
	// Search for the first expression that matches and isn't commented
	std::string::iterator start = src.begin();
	std::string::iterator end = src.end();
	while (regex_search(start, end, matches, var_re, boost::format_first_only))
	{
		// Start position of the match
		size_t startPosition = std::distance(src.begin(), matches[0].first);
		
		// Finish if the matched variable is the good one (i.e. not commented)
		if (!isCommented(startPosition, src))
			return true;
		// Else continue to search for it after last match
		else
			start = matches[0].second;
	}
	
	// At this point no correct match was found
	return false;
}

bool ShaderMutator::insertCodeBeforeMainFunction(const std::string& insertedCode, std::string& modifiedSrc)
{
	// Regular expression for main function
	// <void> <white-space>[1 or more times] <main> <white-space>[0 or more times] <'('>
	boost::regex main_re("(void)\\s+(main)\\s*\\(");
	
	// Matches results
	boost::match_results <std::string::iterator> matches;
	
	// Search for the first expression that matches and isn't commented
	std::string::iterator start = modifiedSrc.begin();
	std::string::iterator end = modifiedSrc.end();
	while (regex_search(start, end, matches, main_re, boost::format_first_only))
	{
		// Start position of the match
		size_t startPosition = std::distance(modifiedSrc.begin(), matches[0].first);
		
		// Insert and finish if the matched "main" is the good one (i.e. not commented)
		if (!isCommented(startPosition, modifiedSrc))
		{
			modifiedSrc.insert(startPosition, insertedCode);
			return true;
		}
		// Else continue to search for it after last match
		else
		{
			start = matches[0].second;
		}
	}
	
	// At this point no correct match was found
	return false;
}

bool ShaderMutator::insertCodeAtMainFunctionBeginning(const std::string& insertedCode, std::string& modifiedSrc)
{
	// Regular expression for main function
	// <void> <white-space>[1 or more times] <main> <white-space>[0 or more times]
	// <'('> <white-space>[0 or more times] <')'>
	// <white-space>[0 or more times] <'{'>
	boost::regex main_re("(void)\\s+(main)\\s*\\(\\s*\\)\\s*\\{");
	
	// Matches results
	boost::match_results <std::string::iterator> matches;
	
	// Search for the first expression that matches and isn't commented
	std::string::iterator start = modifiedSrc.begin();
	std::string::iterator end = modifiedSrc.end();
	while (regex_search(start, end, matches, main_re, boost::format_first_only))
	{
		// Start position of the match
		size_t startPosition = std::distance(modifiedSrc.begin(), matches[0].first);
		
		// End position of the match
		size_t endPosition = std::distance(modifiedSrc.begin(), matches[0].second);
		
		// Insert and finish if the matched "main" is the good one (i.e. not commented)
		if (!isCommented(startPosition, modifiedSrc))
		{
			modifiedSrc.insert(endPosition, insertedCode);
			return true;
		}
		// Else continue to search for it after last match
		else
		{
			start = matches[0].second;
		}
	}
	
	// At this point no correct match was found
	return false;
}

bool ShaderMutator::insertCodeAtMainFunctionEnd(const std::string& insertedCode, std::string& modifiedSrc)
{
	// Regular expression for main function
	// <void> <white-space>[1 or more times] <main> <white-space>[0 or more times]
	// <'('> <white-space>[0 or more times] <')'>
	// <white-space>[0 or more times] <'{'>
	boost::regex main_re("(void)\\s+(main)\\s*\\(\\s*\\)\\s*\\{");
	
	// Matches results
	boost::match_results <std::string::iterator> matches;
	
	// Search for the first expression that matches and isn't commented
	std::string::iterator start = modifiedSrc.begin();
	std::string::iterator end = modifiedSrc.end();
	size_t mainFirstBracePos = 0;  // The aim is to find this position
	while (regex_search(start, end, matches, main_re, boost::format_first_only) && (mainFirstBracePos == 0))
	{
		// Start position of the match
		size_t startPosition = std::distance(modifiedSrc.begin(), matches[0].first);
		
		// End position of the match
		size_t endPosition = std::distance(modifiedSrc.begin(), matches[0].second);
		
		// Get the main first brace position if the matched "main" is the good one (i.e. not commented)
		if (!isCommented(startPosition, modifiedSrc))
			mainFirstBracePos = endPosition;
		// Else continue to search for it after last match
		else
			start = matches[0].second;
	}
	
	// If mainFirstBracePos is still zero, no correct match was found
	if (mainFirstBracePos == 0)
		return false;
	
	// Else, it is now possible to count the opening and closing braces till the final closing brace of the main function is reached
	int bracesCounter = 1;  // =1 since the first opening brace is counted in, it will be =0 when the final closing brace is reached
	size_t closestBracePos = mainFirstBracePos;
	size_t closestOpeningBracePos;
	size_t closestClosingBracePos;
	while (bracesCounter != 0)
	{
		closestOpeningBracePos = modifiedSrc.find_first_of('{', closestBracePos + 1);
		// If this brace appears to be commented, try to get the next one
		while ((closestOpeningBracePos != std::string::npos) && isCommented(closestOpeningBracePos, modifiedSrc))
			closestOpeningBracePos = modifiedSrc.find_first_of('{', closestOpeningBracePos + 1);
		
		closestClosingBracePos = modifiedSrc.find_first_of('}', closestBracePos + 1);
		// If this brace appears to be commented, try to get the next one
		while ((closestClosingBracePos != std::string::npos) && isCommented(closestClosingBracePos, modifiedSrc))
			closestClosingBracePos = modifiedSrc.find_first_of('}', closestClosingBracePos + 1);

		// Happens if there is not enough "}" for the corresponding "{"
		if (closestClosingBracePos == std::string::npos)
			return false;

		// Refresh the closest brace position, and increment or decrement the counter
		if (closestClosingBracePos < closestOpeningBracePos)
		{
			closestBracePos = closestClosingBracePos;
			bracesCounter -= 1;
		}
		else
		{
			closestBracePos = closestOpeningBracePos;
			bracesCounter += 1;
		}
	}

	// We should now have the final '}' of the main function
	size_t mainLastBracePos = closestBracePos;
	
	// Insert the source there
	modifiedSrc.insert(mainLastBracePos, insertedCode);
	
	return true;
}

} // namespace Utils

} // namespace CGoGN
