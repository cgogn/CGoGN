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
#include <string>
#include <sstream>
#include <vector>
#include <cmath>

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


	/***********************************************
	 *
	 * 		Plane Clipping
	 *
	 ***********************************************/

public:

	/**
	 * sets the clip planes count
	 * @param planesCount clip planes count to use
	 * @warning insertClippingCode must be called first
	 */
	void setClipPlanesCount(int planesCount);

	/// gets the clip planes count
	int getClipPlanesCount();

	/**
	 * sets all parameters for one clip plane
	 * @warning planeIndex starts at 0
	 * @param vec1 first basis vector
	 * @param vec2 second basis vector
	 * @param origin origin
	 * @param planeIndex index of the plane
	 */
	void setClipPlaneParamsAll(Geom::Vec3f vec1, Geom::Vec3f vec2, Geom::Vec3f origin, int planeIndex);

	/**
	 * sets first vector for one clip plane
	 * @warning planeIndex starts at 0
	 * @param vec1 first basis vector
	 * @param planeIndex index of the plane
	 */
	void setClipPlaneParamsFirstVec(Geom::Vec3f vec1, int planeIndex);

	/**
	 * sets second vector for one clip plane
	 * @warning planeIndex starts at 0
	 * @param vec2 second basis vector
	 * @param planeIndex index of the plane
	 */
	void setClipPlaneParamsSecondVec(Geom::Vec3f vec2, int planeIndex);
	
	/**
	 * sets origin for one clip plane
	 * @warning planeIndex starts at 0
	 * @param origin origin
	 * @param planeIndex index of the plane
	 */
	void setClipPlaneParamsOrigin(Geom::Vec3f origin, int planeIndex);

	/**
	 * gets first vector for one clip plane
	 * @warning planeIndex starts at 0
	 * @param planeIndex index of the plane
	 */
	Geom::Vec3f getClipPlaneParamsFirstVec(int planeIndex);

	/**
	 * gets second vector for one clip plane
	 * @warning planeIndex starts at 0
	 * @param planeIndex index of the plane
	 */
	Geom::Vec3f getClipPlaneParamsSecondVec(int planeIndex);

	/**
	 * gets origin for one clip plane
	 * @warning planeIndex starts at 0
	 * @param planeIndex index of the plane
	 */
	Geom::Vec3f getClipPlaneParamsOrigin(int planeIndex);

private:

	/**
	 * updates clip plane equation uniforms array
	 * @param planeIndex index of the plane
	 */
	void updateClipPlaneUniformsArray(int planeIndex);

	/// clip planes structure
	struct clipPlane
	{
		Geom::Vec3f firstVec, secondVec, origin;
	};

	/// clip planes array
	std::vector<clipPlane> m_clipPlanes;

	/// clip planes equations uniforms array
	std::vector<float> m_clipPlanesEquations;

	/// clip planes equations uniforms array id
	GLint m_unif_clipPlanesEquations;


	/***********************************************
	 *
	 * 		Sphere Clipping
	 *
	 ***********************************************/

public:

	/**
	 * sets the clip spheres count
	 * @param spheresCount clip spheres count to use
	 * @warning insertClippingCode must be called first
	 */
	void setClipSpheresCount(int spheresCount);

	/// gets the clip spheres count
	int getClipSpheresCount();

	/**
	 * sets all parameters for one clip sphere
	 * @warning sphereIndex starts at 0
	 * @param center center
	 * @param radius radius
	 * @param sphereIndex index of the sphere
	 */
	void setClipSphereParamsAll(Geom::Vec3f center, float radius, int sphereIndex);

	/**
	 * sets center for one clip sphere
	 * @warning sphereIndex starts at 0
	 * @param center center
	 * @param sphereIndex index of the sphere
	 */
	void setClipSphereParamsCenter(Geom::Vec3f center, int sphereIndex);

	/**
	 * sets radius for one clip sphere
	 * @warning sphereIndex starts at 0
	 * @param radius radius
	 * @param sphereIndex index of the sphere
	 */
	void setClipSphereParamsRadius(float radius, int sphereIndex);

	/**
	 * gets center for one clip sphere
	 * @warning sphereIndex starts at 0
	 * @param sphereIndex index of the sphere
	 */
	Geom::Vec3f getClipSphereParamsCenter(int sphereIndex);

	/**
	 * gets radius for one clip sphere
	 * @warning sphereIndex starts at 0
	 * @param sphereIndex index of the sphere
	 */
	float getClipSphereParamsRadius(int sphereIndex);

private:

	/**
	 * updates clip sphere center and radius uniforms array
	 * @param sphereIndex index of the sphere
	 */
	void updateClipSphereUniformsArray(int sphereIndex);

	/// clip spheres structure
	struct clipSphere
	{
		Geom::Vec3f center;
		float radius;
	};

	/// clip spheres array
	std::vector<clipSphere> m_clipSpheres;

	/// clip spheres centers and radiuses uniforms array
	std::vector<float> m_clipSpheresCentersAndRadiuses;

	/// clip spheres centers and radiuses uniforms array id
	GLint m_unif_clipSpheresCentersAndRadiuses;


	/***********************************************
	 *
	 * 		Global Clipping Stuff
	 *
	 ***********************************************/

public:

	/// enum used to choose clipping mode
	enum clippingMode { CLIPPING_MODE_AND, CLIPPING_MODE_OR };

	/**
	 * inserts clipping instructions into shader source code
	 * @warning this function is designed for shaders which *do not* use a geometry shader
	 * @return true if shader was processed successfully
	 */
	bool insertClippingCode();

	/**
	 * sets the color attenuation factor
	 * @param colorAttenuationFactor color attenuation factor
	 */
	void setClipColorAttenuationFactor(float colorAttenuationFactor);

	/// gets the color attenuation factor
	float getClipColorAttenuationFactor();

	/*
	 * sets the clipping mode
	 * @param clipMode clipping mode
	 */
	void setClipMode(clippingMode clipMode);

	/// gets the clipping mode
	clippingMode getClipMode();

private:

	/// used to control clipping code has been inserted before adding clipping objects
	bool m_hasClippingCodeBeenInserted;

	/// color attenuation factor
	float m_clipColorAttenuationFactor;

	/// color attenuation factor uniform id
	GLint m_unif_clipColorAttenuationFactor;

	/// clipping mode
	clippingMode m_clipMode;


	/***********************************************
	 *
	 * 		Clipping Uniforms Handling
	 *
	 ***********************************************/

public:

	/// updates uniforms (get their locations and send their values again)
	void updateClippingUniforms();

private:

	/// sends the clip planes equations uniforms array to shader
	void sendClipPlanesEquationsUniform();

	/// sends the clip spheres centers and radiuses uniforms array to shader
	void sendClipSpheresCentersAndRadiusesUniform();

	/// sends the color attenuation factor uniform to shader
	void sendClipColorAttenuationFactorUniform();


	/***********************************************
	 *
	 * 		Error Raising
	 *
	 ***********************************************/

private:

	/**
	 * Outputs a "shader mutator failure" error if the condition is satisfied
	 * @param condition condition to satisfy
	 * @param location name of the function where the error raising was done
	 * @return true if the error has been raised
	 */
	bool errorRaiseShaderMutatorFailure(bool condition, const std::string& location);

	/**
	 * Outputs a "parameter not positive" error if the condition is satisfied
	 * @param condition condition to satisfy
	 * @param location name of the function where the error raising was done
	 * @param paramName name of the parameter
	 * @return true if the error has been raised
	 */
	bool errorRaiseParameterIsNotPositive(bool condition, const std::string& location, const std::string& paramName);

	/**
	 * Outputs a "shader source is empty" error if the condition is satisfied
	 * @param condition condition to satisfy
	 * @param location name of the function where the error raising was done
	 * @param shaderType type of the shader
	 * @return true if the error has been raised
	 */
	bool errorRaiseShaderSourceIsEmpty(bool condition, const std::string& location, ShaderMutator::shaderSrcType shaderType);

	/**
	 * Outputs a "shader uses a geometry shader" error if the condition is satisfied
	 * @param condition condition to satisfy
	 * @param location name of the function where the error raising was done
	 * @return true if the error has been raised
	 */
	bool errorRaiseShaderUsesGeometryShader(bool condition, const std::string& location);

	/**
	 * Outputs a ".. not found in shader" error if the condition is satisfied
	 * @param condition condition to satisfy
	 * @param location name of the function where the error raising was done
	 * @param shaderType type of the shader
	 * @param varName name of the variable that may not be found
	 * @return true if the error has been raised
	 */
	bool errorRaiseVariableNotFoundInShader(bool condition, const std::string& location, ShaderMutator::shaderSrcType shaderType, const std::string& varName);

	/**
	 * Outputs a "parameter is out of range" error if the condition is satisfied
	 * @param condition condition to satisfy
	 * @param location name of the function where the error raising was done
	 * @param paramName name of the parameter
	 * @return true if the error has been raised
	 */
	bool errorRaiseParameterIsOutOfRange(bool condition, const std::string& location, const std::string& paramName);

	/**
	 * Outputs a "uniform .. not found in shader" error if the condition is satisfied
	 * @param condition condition to satisfy
	 * @param location name of the function where the error raising was done
	 * @param uniformName name of the uniform that may not be found
	 * @return true if the error has been raised
	 */
	bool errorRaiseUniformNotFoundInShader(bool condition, const std::string& location, const std::string& uniformName);

	/**
	 * Outputs a "clipping code was already inserted" error if the condition is satisfied
	 * @param condition condition to satisfy
	 * @param location name of the function where the error raising was done
	 * @return true if the error has been raised
	 */
	bool errorRaiseClippingCodeAlreadyInserted(bool condition, const std::string& location);

	/**
	 * Outputs a "clipping code has not been inserted yet" error if the condition is satisfied
	 * @param condition condition to satisfy
	 * @param location name of the function where the error raising was done
	 * @return true if the error has been raised
	 */
	bool errorRaiseClippingCodeNotInserted(bool condition, const std::string& location);

};


} // namespace Utils

} // namespace CGoGN

#endif
