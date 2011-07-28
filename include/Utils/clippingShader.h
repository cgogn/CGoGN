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

	/*
	 * adds a clip plane
	 * @return clip plane id (positive value on success, else -1)
	 * @warning insertClippingCode must be called first
	 */
	int addClipPlane();

	/*
	 * deletes a clip plane
	 * @param id clip plane id
	 */
	void deleteClipPlane(unsigned int id);

	/// gets the clip planes count
	int getClipPlanesCount();

	/**
	 * sets all parameters for one clip plane
	 * @param id clip plane id
	 * @param vec1 first basis vector
	 * @param vec2 second basis vector
	 * @param origin origin
	 */
	void setClipPlaneParamsAll(unsigned int id, Geom::Vec3f vec1, Geom::Vec3f vec2, Geom::Vec3f origin);

	/**
	 * sets first vector for one clip plane
	 * @param id clip plane id
	 * @param vec1 first basis vector
	 */
	void setClipPlaneParamsFirstVec(unsigned int id, Geom::Vec3f vec1);

	/**
	 * sets second vector for one clip plane
	 * @param id clip plane id
	 * @param vec2 second basis vector
	 */
	void setClipPlaneParamsSecondVec(unsigned int id, Geom::Vec3f vec2);
	
	/**
	 * sets origin for one clip plane
	 * @param id clip plane id
	 * @param origin origin
	 */
	void setClipPlaneParamsOrigin(unsigned int id, Geom::Vec3f origin);

	/**
	 * gets first vector for one clip plane
	 * @param id clip plane id
	 */
	Geom::Vec3f getClipPlaneParamsFirstVec(unsigned int id);

	/**
	 * gets second vector for one clip plane
	 * @param id clip plane id
	 */
	Geom::Vec3f getClipPlaneParamsSecondVec(unsigned int id);

	/**
	 * gets origin for one clip plane
	 * @param id clip plane id
	 */
	Geom::Vec3f getClipPlaneParamsOrigin(unsigned int id);

private:

	/// gets a non used clip plane id
	unsigned int getFreeClipPlaneId();

	/**
	 * updates clip plane equation uniforms array
	 * @param id clip plane id
	 */
	void updateClipPlaneUniformsArray(unsigned int id);

	/// clip planes structure
	struct clipPlane
	{
		Geom::Vec3f firstVec, secondVec, origin;
	};

	/// clip planes array
	std::vector<clipPlane> m_clipPlanes;

	/// clip planes ids structure
	struct clipPlaneId
	{
		bool used;
		int index; // Corresponding index in the clip planes array
	};

	/// clip planes ids array
	std::vector<clipPlaneId> m_clipPlanesIds;

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

	/*
	 * adds a clip sphere
	 * @return clip sphere id (positive value on success, else -1)
	 * @warning insertClippingCode must be called first
	 */
	int addClipSphere();

	/*
	 * deletes a clip sphere
	 * @param id clip sphere id
	 */
	void deleteClipSphere(unsigned int id);

	/// gets the clip spheres count
	int getClipSpheresCount();

	/**
	 * sets all parameters for one clip sphere
	 * @param id clip sphere id
	 * @param center center
	 * @param radius radius
	 */
	void setClipSphereParamsAll(unsigned int id, Geom::Vec3f center, float radius);

	/**
	 * sets center for one clip sphere
	 * @param id clip sphere id
	 * @param center center
	 */
	void setClipSphereParamsCenter(unsigned int id, Geom::Vec3f center);

	/**
	 * sets radius for one clip sphere
	 * @param id clip sphere id
	 * @param radius radius
	 */
	void setClipSphereParamsRadius(unsigned int id, float radius);

	/**
	 * gets center for one clip sphere
	 * @param id clip sphere id
	 */
	Geom::Vec3f getClipSphereParamsCenter(unsigned int id);

	/**
	 * gets radius for one clip sphere
	 * @param id clip sphere id
	 */
	float getClipSphereParamsRadius(unsigned int id);

private:

	/// gets a non used clip sphere id
	unsigned int getFreeClipSphereId();

	/**
	 * updates clip sphere center and radius uniforms array
	 * @param id clip sphere id
	 */
	void updateClipSphereUniformsArray(unsigned int id);

	/// clip spheres structure
	struct clipSphere
	{
		Geom::Vec3f center;
		float radius;
	};

	/// clip spheres array
	std::vector<clipSphere> m_clipSpheres;

	/// clip spheres ids structure
	struct clipSphereId
	{
		bool used;
		int index; // Corresponding index in the clip spheres array
	};

	/// clip spheres ids array
	std::vector<clipSphereId> m_clipSpheresIds;

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
	 * Outputs a "wrong ID" error if the condition is satisfied
	 * @param condition condition to satisfy
	 * @param location name of the function where the error raising was done
	 * @return true if the error has been raised
	 */
	bool errorRaiseWrongId(bool condition, const std::string& location);

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
