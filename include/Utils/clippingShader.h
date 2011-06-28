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

public:
	/**
	 * constructor
	 */
	ClippingShader();

	/**
	 * destructor
	 */
	~ClippingShader();

	/**
	 * set the plane equation for plane clipping
	 * @warning planeIndex starts at 0
	 * @param clipPlane plane equation
	 * @param planeIndex index of plane to modify
	 */
	void setClippingPlaneEquation(Geom::Vec4f clipPlane, int planeIndex = 0);
	
	/**
	 * get the plane equation for plane clipping
	 * @warning planeIndex starts at 0
	 * @param planeIndex index of plane to get
	 */
	Geom::Vec4f getClippingPlaneEquation(int planeIndex = 0);

	/**
	 * set the plane quaternion for plane clipping
	 * @param quat plane quaternion
	 */
	//void setClippingPlaneQuaternion(float quat[4]);

	/**
	 * get the plane quaternion for plane clipping
	 */
	//Geom::Vec4f getClippingPlaneQuaternion();

	/**
	 * set the color attenuation factor for clipping
	 * @param colorAttenuationFactor color attenuation factor
	 */
	void setClippingColorAttenuationFactor(float colorAttenuationFactor);

	/**
	 * get the color attenuation factor for clipping
	 */
	float getClippingColorAttenuationFactor();

	/**
	 * insert plane clipping instructions into vertex and fragment shader source code
	 * - does not modify the geometry shader source code
	 * @param planesCount the clipping planes count to use
	 * @warning this function is designed for shaders which *do not* use a geometry shader
	 */
	void setPlaneClipping(int planesCount);
	
	/**
	 * get the clipping planes count used for plane clipping
	 */
	int getClippingPlanesCount() { return (int)m_clipPlanesEquations.size() / 4; }

	/**
	 * update uniforms (get their locations and resend their values) for clipping
	 */
	void updateClippingUniforms();

	/**
	 * displays a quad corresponding to the current clipping plane position and orientation
	 */
	void displayClippingPlane();


private:

	/**
	 * sends the clipping planes uniform to shader
	 */
	void sendClippingPlanesUniform();

	/**
	 * sends the color attenuation factor uniform to shader
	 */
	void sendColorAttenuationFactorUniform();

	/**
	 * stored original vertex shader source code (without clipping)
	 */
	std::string originalVertShaderSrc;

	/**
	 * stored original fragment shader source code (without clipping)
	 */
	std::string originalFragShaderSrc;

	/**
	 * clip planes equations (a, b, c, d)*planes count
	 */
	std::vector<float> m_clipPlanesEquations;
	
	/**
	 * clip plane quaternion
	 */
	//float m_clipPlaneQuaternion[4];

	/**
	 * clip planes vector uniform id
	 */
	GLint m_unif_clipPlanes;
	
	/**
	 * color attenuation factor
	 */
	float m_colorAttenuationFactor;
	
	/**
	 * color attenuation factor uniform id
	 */
	GLint m_unif_colorAttenuationFactor;

	/**
	 * plane drawer
	 */
	Drawer* m_planeDrawer;

};


} // namespace Utils

} // namespace CGoGN

#endif
