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
	 * enum used to choose which type of clip plane grid to display
	 */
	enum clipPlaneDisplayGridType { STRAIGHT_GRID, RADIAL_GRID };

	/**
	 * constructor
	 */
	ClippingShader();

	/**
	 * destructor
	 */
	~ClippingShader();

	/**
	 * set all parameters for one clipping plane
	 * @warning planeIndex starts at 0
	 * @param vec1 first basis vector
	 * @param vec2 second basis vector
	 * @param origin point of the plane that will be used as origin to display it
	 * @param planeIndex index of the plane to modify
	 */
	void setClippingPlane(Geom::Vec3f vec1, Geom::Vec3f vec2, Geom::Vec3f origin, int planeIndex = 0);

	/**
	 * set first vector parameter for one clipping plane
	 * @warning planeIndex starts at 0
	 * @param vec1 first basis vector
	 * @param planeIndex index of the plane to modify
	 */
	void setClippingPlaneFirstVec(Geom::Vec3f vec1, int planeIndex = 0);

	/**
	 * set second vector parameter for one clipping plane
	 * @warning planeIndex starts at 0
	 * @param vec2 second basis vector
	 * @param planeIndex index of the plane to modify
	 */
	void setClippingPlaneSecondVec(Geom::Vec3f vec2, int planeIndex = 0);
	
	/**
	 * set origin parameter for one clipping plane
	 * @warning planeIndex starts at 0
	 * @param origin point of the plane that will be used as origin to display it
	 * @param planeIndex index of the plane to modify
	 */
	void setClippingPlaneOrigin(Geom::Vec3f origin, int planeIndex = 0);

	/**
	 * get first parameter for one clipping plane
	 * @warning planeIndex starts at 0
	 * @param planeIndex index of the plane to modify
	 */
	Geom::Vec3f getClippingPlaneFirstVec(int planeIndex = 0);

	/**
	 * get second vector parameter for one clipping plane
	 * @warning planeIndex starts at 0
	 * @param planeIndex index of the plane to modify
	 */
	Geom::Vec3f getClippingPlaneSecondVec(int planeIndex = 0);

	/**
	 * get origin parameter for one clipping plane
	 * @warning planeIndex starts at 0
	 * @param planeIndex index of the plane to modify
	 */
	Geom::Vec3f getClippingPlaneOrigin(int planeIndex = 0);

	/**
	 * set the color attenuation factor for clipping
	 * @param colorAttenuationFactor color attenuation factor
	 */
	void setClippingColorAttenuationFactor(float colorAttenuationFactor);

	/**
	 * get the color attenuation factor for clipping
	 */
	float getClippingColorAttenuationFactor() { return m_colorAttenuationFactor; }

	/**
	 * set the clipping planes count used for plane clipping
	 * - insert plane clipping instructions into vertex and fragment shader source code
	 * but does not modify the geometry shader source code
	 * @param planesCount the clipping planes count to use
	 * @warning this function is designed for shaders which *do not* use a geometry shader
	 */
	void setClippingPlanesCount(int planesCount);
	
	/**
	 * get the clipping planes count used for plane clipping
	 */
	int getClippingPlanesCount() { return (int)m_clipPlanes.size(); }

	/**
	 * update uniforms (get their locations and send their values again) for clipping
	 */
	void updateClippingUniforms();

	/**
	 * display all clipping planes
	 */
	void displayClippingPlanes();

	/**
	 * set the planes display color
	 * @param color the new color
	 */
	void setPlaneDisplayColor(Geom::Vec3f color) { m_clipPlanesDisplayColor = color; updateAllClippingPlanesVBOs(); }

	/**
	 * get the planes display color
	 */
	Geom::Vec3f getPlaneDisplayColor() { return m_clipPlanesDisplayColor; }

	/**
	 * set the planes display grids type
	 * @param gridType the new grid type
	 */
	void setPlaneDisplayType(clipPlaneDisplayGridType gridType) { m_clipPlanesDisplayType = gridType; updateAllClippingPlanesVBOs(); }

	/**
	 * get the planes display grids type
	 */
	clipPlaneDisplayGridType getPlaneDisplayType() { return m_clipPlanesDisplayType; }

	/**
	 * set the planes display grid x resolution
	 * @param res the new resolution
	 */
	void setPlaneDisplayXRes(size_t res) { m_clipPlanesDisplayXRes = res; updateAllClippingPlanesVBOs(); }

	/**
	 * get the planes display grid x resolution
	 */
	size_t getPlaneDisplayXRes() { return m_clipPlanesDisplayXRes; }

	/**
	 * set the planes display grid y resolution
	 * @param res the new resolution
	 */
	void setPlaneDisplayYRes(size_t res) { m_clipPlanesDisplayYRes = res; updateAllClippingPlanesVBOs(); }

	/**
	 * get the planes display grid y resolution
	 */
	size_t getPlaneDisplayYRes() { return m_clipPlanesDisplayYRes; }

	/**
	 * set the planes display size
	 * @param size the new size
	 */
	void setPlaneDisplaySize(float size) { m_clipPlanesDisplaySize = size; updateAllClippingPlanesVBOs(); }

	/**
	 * get the planes display size
	 */
	float getPlaneDisplaySize() { return m_clipPlanesDisplaySize; }


private:

	/**
	 * clipping planes structure
	 */
	struct clipPlane
	{
		Geom::Vec3f firstVec, secondVec, origin;
	};

	/**
	 * sends the clipping planes uniform to shader
	 */
	void sendClippingPlanesUniform();

	/**
	 * sends the color attenuation factor uniform to shader
	 */
	void sendColorAttenuationFactorUniform();

	/**
	 * update VBO for one plane
	 * @param planeIndex index of the plane
	 */
	void updateClippingPlaneVBO(int planeIndex);

	/**
	 * update VBOs for all planes
	 */
	void updateAllClippingPlanesVBOs();

	/**
	 * original vertex shader source code (without clipping)
	 */
	std::string originalVertShaderSrc;

	/**
	 * original fragment shader source code (without clipping)
	 */
	std::string originalFragShaderSrc;

	/**
	 * clip planes array
	 */
	std::vector<clipPlane> m_clipPlanes;

	/**
	 * clip planes equations array (size = 4*(planes count))
	 * - ** only used for sending planes data to shader **
	 */
	std::vector<float> m_clipPlanesEquations;

	/**
	 * clip planes first vectors array (size = 3*(planes count))
	 * - ** only used for sending planes data to shader **
	 */
	std::vector<float> m_clipPlanesFirstVectors;

	/**
	 * clip planes second vectors array (size = 3*(planes count))
	 * - ** only used for sending planes data to shader **
	 */
	std::vector<float> m_clipPlanesSecondVectors;

	/**
	 * clip planes origins array (size = 3*(planes count))
	 * - ** only used for sending planes data to shader **
	 */
	std::vector<float> m_clipPlanesOrigins;

	/**
	 * clip planes equations vector uniform id
	 */
	GLint m_unif_clipPlanesEquations;
	
	/**
	 * color attenuation factor
	 */
	float m_colorAttenuationFactor;
	
	/**
	 * color attenuation factor uniform id
	 */
	GLint m_unif_colorAttenuationFactor;

	/**
	 * clip planes drawers array
	 */
	std::vector<Drawer*> m_clipPlanesDrawers;

	/**
	 * clip planes display color
	 */
	Geom::Vec3f m_clipPlanesDisplayColor;

	/**
	 * clip planes display grid type
	 */
	clipPlaneDisplayGridType m_clipPlanesDisplayType;

	/**
	 * clip planes x grid display resolution
	 */
	size_t m_clipPlanesDisplayXRes;

	/**
	 * clip planes y grid display resolution
	 */
	size_t m_clipPlanesDisplayYRes;

	/**
	 * clip planes display size
	 */
	float m_clipPlanesDisplaySize;

};


} // namespace Utils

} // namespace CGoGN

#endif
