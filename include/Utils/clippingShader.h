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

#ifndef _CGoGN_CLIPPINGSHADER_H_
#define _CGoGN_CLIPPINGSHADER_H_

#include "Utils/GLSLShader.h"
#include "Geometry/vector_gen.h"
#include "Utils/cgognStream.h"
#include "Utils/shaderMutator.h"
#include "Utils/drawer.h"
#include <string>
#include <sstream>
#include <vector>

namespace CGoGN
{

namespace Utils
{

class ClippingShader : public GLSLShader
{


	/***********************************************
	 *
	 * 		Constructor / Destructor
	 *
	 ***********************************************/

public :

	/// constructor
	ClippingShader();

	/// destructor
	~ClippingShader();


	/***********************************************
	 *
	 * 		Original Shader Sources
	 *
	 ***********************************************/

private:

	/// original vertex shader source code (without clipping)
	std::string originalVertShaderSrc;

	/// original fragment shader source code (without clipping)
	std::string originalFragShaderSrc;


	/***********************************************
	 *
	 * 		Plane Clipping
	 *
	 ***********************************************/

public:

	/**
	 * set the clip planes count
	 * - inserts plane clipping instructions into vertex and fragment shader source code
	 * - does not modify the geometry shader source code
	 * @param planesCount the clip planes count to use
	 * @warning this function is designed for shaders which *do not* use a geometry shader
	 */
	void setClipPlanesCount(int planesCount);

	/// get the clip planes count
	int getClipPlanesCount();

	/**
	 * set all parameters for one clip plane
	 * @warning planeIndex starts at 0
	 * @param vec1 first basis vector
	 * @param vec2 second basis vector
	 * @param origin point of the plane that will be used as origin to display it
	 * @param planeIndex index of the plane to modify
	 */
	void setClipPlaneParamsAll(Geom::Vec3f vec1, Geom::Vec3f vec2, Geom::Vec3f origin, int planeIndex);

	/**
	 * set first vector for one clip plane
	 * @warning planeIndex starts at 0
	 * @param vec1 first basis vector
	 * @param planeIndex index of the plane to modify
	 */
	void setClipPlaneParamsFirstVec(Geom::Vec3f vec1, int planeIndex);

	/**
	 * set second vector for one clip plane
	 * @warning planeIndex starts at 0
	 * @param vec2 second basis vector
	 * @param planeIndex index of the plane to modify
	 */
	void setClipPlaneParamsSecondVec(Geom::Vec3f vec2, int planeIndex);
	
	/**
	 * set origin for one clip plane
	 * @warning planeIndex starts at 0
	 * @param origin point of the plane that will be used as origin to display it
	 * @param planeIndex index of the plane to modify
	 */
	void setClipPlaneParamsOrigin(Geom::Vec3f origin, int planeIndex);

	/**
	 * get first vector for one clip plane
	 * @warning planeIndex starts at 0
	 * @param planeIndex index of the plane to modify
	 */
	Geom::Vec3f getClipPlaneParamsFirstVec(int planeIndex);

	/**
	 * get second vector for one clip plane
	 * @warning planeIndex starts at 0
	 * @param planeIndex index of the plane to modify
	 */
	Geom::Vec3f getClipPlaneParamsSecondVec(int planeIndex);

	/**
	 * get origin for one clip plane
	 * @warning planeIndex starts at 0
	 * @param planeIndex index of the plane to modify
	 */
	Geom::Vec3f getClipPlaneParamsOrigin(int planeIndex);

private:

	/**
	 * update clip plane equation array
	 * @param planeIndex index of the plane
	 */
	void updateClipPlaneArray(int planeIndex);

	/// clip planes structure
	struct clipPlane
	{
		Geom::Vec3f firstVec, secondVec, origin;
	};

	/// clip planes array
	std::vector<clipPlane> m_clipPlanes;

	/**
	 * clip planes equations array (size = 4*(planes count))
	 * - ** only used for sending planes data to shader **
	 */
	std::vector<float> m_clipPlanesEquations;

	/// clip planes equations vector uniform id
	GLint m_unif_clipPlanesEquations;


	/***********************************************
	 *
	 * 		Plane Clipping Display
	 *
	 ***********************************************/

public:

	/// enum used to choose which type of clip plane grid to display
	enum clipPlaneDisplayGridType { STRAIGHT_GRID, RADIAL_GRID };

	/// display all clipping planes
	void displayClipPlanes();

	/**
	 * set the planes display color
	 * @param color the new color
	 */
	void setClipPlanesDisplayColor(Geom::Vec3f color);

	/// get the planes display color
	Geom::Vec3f getClipPlanesDisplayColor();

	/**
	 * set the planes display grids type
	 * @param gridType the new grid type
	 */
	void setClipPlanesDisplayType(clipPlaneDisplayGridType gridType);

	/// get the planes display grids type
	clipPlaneDisplayGridType getClipPlanesDisplayType();

	/**
	 * set the planes display grid x resolution
	 * @param res the new resolution
	 */
	void setClipPlanesDisplayXRes(size_t res);

	/// get the planes display grid x resolution
	size_t getClipPlanesDisplayXRes();

	/**
	 * set the planes display grid y resolution
	 * @param res the new resolution
	 */
	void setClipPlanesDisplayYRes(size_t res);

	/// get the planes display grid y resolution
	size_t getClipPlanesDisplayYRes();

	/**
	 * set the planes display size
	 * @param size the new size
	 */
	void setClipPlanesDisplaySize(float size);

	/// get the planes display size
	float getClipPlanesDisplaySize();

private:

	/**
	 * update VBO for one plane
	 * @param planeIndex index of the plane
	 */
	void updateClipPlaneVBO(int planeIndex);

	/// update VBOs for all planes
	void updateClipPlanesVBOs();

	/// clip planes drawers array
	std::vector<Drawer*> m_clipPlanesDrawers;

	/// clip planes display color
	Geom::Vec3f m_clipPlanesDisplayColor;

	/// clip planes display grid type
	clipPlaneDisplayGridType m_clipPlanesDisplayType;

	/// clip planes x grid display resolution
	size_t m_clipPlanesDisplayXRes;

	/// clip planes y grid display resolution
	size_t m_clipPlanesDisplayYRes;

	/// clip planes display size
	float m_clipPlanesDisplaySize;


	/***********************************************
	 *
	 * 		Global Clipping Stuff
	 *
	 ***********************************************/

public:

	/**
	 * set the color attenuation factor for clipping
	 * @param colorAttenuationFactor color attenuation factor
	 */
	void setClipColorAttenuationFactor(float colorAttenuationFactor);

	/// get the color attenuation factor for clipping
	float getClipColorAttenuationFactor();

private:

	/// color attenuation factor
	float m_clipColorAttenuationFactor;

	/// color attenuation factor uniform id
	GLint m_unif_clipColorAttenuationFactor;


	/***********************************************
	 *
	 * 		Clipping Uniforms Handling
	 *
	 ***********************************************/

public:

	/// update uniforms (get their locations and send their values again) for clipping
	void updateClippingUniforms();

private:

	/// sends the clip planes equations array to shader
	void sendClipPlanesEquationsUniform();

	/// sends the color attenuation factor to shader
	void sendClipColorAttenuationFactorUniform();


	/***********************************************
	 *
	 * 		Error Raising
	 *
	 ***********************************************/

private:

	/**
	 * Output a "parameter not positive" error if the condition is satisfied
	 * @param condition condition to satisfy
	 * @param location name of the function where the error raising is done
	 * @param paramName name of the parameter
	 */
	bool errorRaiseParameterIsNotPositive(bool condition, const std::string& location, const std::string& paramName);

	/**
	 * Output a "shader has not been well created" error if the condition is satisfied
	 * @param condition condition to satisfy
	 * @param location name of the function where the error raising is done
	 */
	bool errorRaiseShaderHasNotBeenWellCreated(bool condition, const std::string& location);

	/**
	 * Output a "shader uses a geometry shader" error if the condition is satisfied
	 * @param condition condition to satisfy
	 * @param location name of the function where the error raising is done
	 */
	bool errorRaiseShaderUsesGeometryShader(bool condition, const std::string& location);

	/**
	 * Output a ".. not found in shader" error if the condition is satisfied
	 * @param condition condition to satisfy
	 * @param location name of the function where the error raising is done
	 * @param shaderType type of the shader
	 * @param varName name of the variable that may not be found
	 */
	bool errorRaiseVariableNotFoundInShader(bool condition, const std::string& location, ShaderMutator::shaderSrcType shaderType, const std::string& varName);

	/**
	 * Output a "parameter is out of range" error if the condition is satisfied
	 * @param condition condition to satisfy
	 * @param location name of the function where the error raising is done
	 * @param paramName name of the parameter
	 */
	bool errorRaiseParameterIsOutOfRange(bool condition, const std::string& location, const std::string& paramName);

	/**
	 * Output a "uniform .. not found in shader" error if the condition is satisfied
	 * @param condition condition to satisfy
	 * @param location name of the function where the error raising is done
	 * @param uniformName name of the uniform that may not be found
	 */
	bool errorRaiseUniformNotFoundInShader(bool condition, const std::string& location, const std::string& uniformName);

};


} // namespace Utils

} // namespace CGoGN

#endif
