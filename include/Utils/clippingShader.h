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

	/// destructor
	~ClippingShader();


	/***********************************************
	 *
	 * 		Plane Clipping
	 *
	 ***********************************************/

public:

	/**
	 * set the clip planes count
	 * @param planesCount the clip planes count to use
	 * @warning insertClippingCode must be called first
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
	 * @param planeIndex index of the plane
	 */
	Geom::Vec3f getClipPlaneParamsFirstVec(int planeIndex);

	/**
	 * get second vector for one clip plane
	 * @warning planeIndex starts at 0
	 * @param planeIndex index of the plane
	 */
	Geom::Vec3f getClipPlaneParamsSecondVec(int planeIndex);

	/**
	 * get origin for one clip plane
	 * @warning planeIndex starts at 0
	 * @param planeIndex index of the plane
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

	/// clip planes x grid display resolution
	size_t m_clipPlanesDisplayXRes;

	/// clip planes y grid display resolution
	size_t m_clipPlanesDisplayYRes;

	/// clip planes display size
	float m_clipPlanesDisplaySize;


	/***********************************************
	 *
	 * 		Sphere Clipping
	 *
	 ***********************************************/

public:

	/**
	 * set the clip spheres count
	 * @param spheresCount the clip spheres count to use
	 * @warning insertClippingCode must be called first
	 */
	void setClipSpheresCount(int spheresCount);

	/// get the clip spheres count
	int getClipSpheresCount();

	/**
	 * set all parameters for one clip sphere
	 * @warning sphereIndex starts at 0
	 * @param center center
	 * @param radius radius
	 * @param sphereIndex index of the sphere to modify
	 */
	void setClipSphereParamsAll(Geom::Vec3f center, float radius, int sphereIndex);

	/**
	 * set center for one clip sphere
	 * @warning sphereIndex starts at 0
	 * @param center center
	 * @param sphereIndex index of the sphere to modify
	 */
	void setClipSphereParamsCenter(Geom::Vec3f center, int sphereIndex);

	/**
	 * set radius for one clip sphere
	 * @warning sphereIndex starts at 0
	 * @param radius radius
	 * @param sphereIndex index of the sphere to modify
	 */
	void setClipSphereParamsRadius(float radius, int sphereIndex);

	/**
	 * get center for one clip sphere
	 * @warning sphereIndex starts at 0
	 * @param sphereIndex index of the sphere
	 */
	Geom::Vec3f getClipSphereParamsCenter(int sphereIndex);

	/**
	 * get radius for one clip sphere
	 * @warning sphereIndex starts at 0
	 * @param sphereIndex index of the sphere
	 */
	float getClipSphereParamsRadius(int sphereIndex);

private:

	/**
	 * update clip sphere center and radius array
	 * @param sphereIndex index of the sphere
	 */
	void updateClipSphereArray(int sphereIndex);

	/// clip spheres structure
	struct clipSphere
	{
		Geom::Vec3f center;
		float radius;
	};

	/// clip spheres array
	std::vector<clipSphere> m_clipSpheres;

	/**
	 * clip spheres centers and radiuses array (size = 4*(spheres count))
	 * - ** only used for sending sphere data to shader **
	 */
	std::vector<float> m_clipSpheresCentersAndRadiuses;

	/// clip spheres equations vector uniform id
	GLint m_unif_clipSpheresCentersAndRadiuses;


	/***********************************************
	 *
	 * 		Sphere Clipping Display
	 *
	 ***********************************************/

public:

	/// display all clipping spheres
	void displayClipSpheres();

	/**
	 * set the spheres display color
	 * @param color the new color
	 */
	void setClipSpheresDisplayColor(Geom::Vec3f color);

	/// get the spheres display color
	Geom::Vec3f getClipSpheresDisplayColor();

	/**
	 * set the spheres display grid x resolution
	 * @param res the new resolution
	 */
	void setClipSpheresDisplayXRes(size_t res);

	/// get the spheres display grid x resolution
	size_t getClipSpheresDisplayXRes();

	/**
	 * set the spheres display grid y resolution
	 * @param res the new resolution
	 */
	void setClipSpheresDisplayYRes(size_t res);

	/// get the spheres display grid y resolution
	size_t getClipSpheresDisplayYRes();

private:

	/**
	 * update VBO for one sphere
	 * @param sphereIndex index of the sphere
	 */
	void updateClipSphereVBO(int sphereIndex);

	/// update VBOs for all spheres
	void updateClipSpheresVBOs();

	/// clip spheres drawers array
	std::vector<Drawer*> m_clipSpheresDrawers;

	/// clip spheres display color
	Geom::Vec3f m_clipSpheresDisplayColor;

	/// clip spheres x grid display resolution
	size_t m_clipSpheresDisplayXRes;

	/// clip spheres y grid display resolution
	size_t m_clipSpheresDisplayYRes;


	/***********************************************
	 *
	 * 		Global Clipping Stuff
	 *
	 ***********************************************/

public:

	/**
	 * insert clipping instructions into shader source code
	 * @warning this function is designed for shaders which *do not* use a geometry shader
	 */
	bool insertClippingCode();

	/**
	 * set the color attenuation factor for clipping
	 * @param colorAttenuationFactor color attenuation factor
	 */
	void setClipColorAttenuationFactor(float colorAttenuationFactor);

	/// get the color attenuation factor for clipping
	float getClipColorAttenuationFactor();

private:

	/// to control clipping code has been inserted before clipping objects have been added
	bool m_hasClippingCodeBeenInserted;

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

	/// sends the clip spheres centers and radiuses array to shader
	void sendClipSpheresCentersAndRadiusesUniform();

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
	 * Output a "shader source is empty" error if the condition is satisfied
	 * @param condition condition to satisfy
	 * @param location name of the function where the error raising is done
	 * @param shaderType type of the shader
	 */
	bool errorRaiseShaderSourceIsEmpty(bool condition, const std::string& location, ShaderMutator::shaderSrcType shaderType);

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

	/**
	 * Output a "clipping code was already inserted" error if the condition is satisfied
	 * @param condition condition to satisfy
	 * @param location name of the function where the error raising is done
	 */
	bool errorRaiseClippingCodeAlreadyInserted(bool condition, const std::string& location);

	/**
	 * Output a "clipping code has not been inserted yet" error if the condition is satisfied
	 * @param condition condition to satisfy
	 * @param location name of the function where the error raising is done
	 */
	bool errorRaiseClippingCodeNotInserted(bool condition, const std::string& location);

};


} // namespace Utils

} // namespace CGoGN

#endif
