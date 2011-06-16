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
	 * set the plane equation for plane clipping
	 * @param newClipPlane plane equation
	 */
	void setPlaneClippingParams(Geom::Vec4f clipPlane);
	
	/**
	 * set the color attenuation factor for clipping
	 * @param newColorAttenuationFactor color attenuation factor
	 */
	void setClippingColorAttenuationFactor(float colorAttenuationFactor);

	/**
	 * insert plane clipping instructions into vertex and fragment shader source code
	 * - does not modify the geometry shader source code
	 * @warning this function is designed for shaders which do not use a geometry shader
	 */
	void addPlaneClippingToShaderSource();
	
	/**
	 * update uniforms (get their locations and resent their values) for clipping
	 */
	void updateClippingUniforms();


private:

	/**
	 * clip plane vector (a, b, c, d)
	 */
	Geom::Vec4f m_clipPlane;
	
	/**
	 * clip plane vector uniform id
	 */
	GLint m_unif_clipPlane;
	
	/**
	 * color attenuation factor
	 */
	float m_colorAttenuationFactor;
	
	/**
	 * color attenuation factor uniform id
	 */
	GLint m_unif_colorAttenuationFactor;

};


} // namespace Utils

} // namespace CGoGN

#endif
