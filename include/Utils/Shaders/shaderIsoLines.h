/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2012, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#ifndef __CGOGN_SHADER_ISO_LINES__
#define __CGOGN_SHADER_ISO_LINES__

#include "Utils/GLSLShader.h"
#include "Geometry/vector_gen.h"

namespace CGoGN
{

namespace Utils
{

class ShaderIsoLines : public GLSLShader
{
protected:
	// shader sources
    static std::string vertexShaderText;
    static std::string fragmentShaderText;
    static std::string geometryShaderText;

    // uniform locations
	CGoGNGLuint m_unif_colorMin;
	CGoGNGLuint m_unif_colorMax;
	CGoGNGLuint m_unif_vmin;
	CGoGNGLuint m_unif_vmax;
	CGoGNGLuint m_unif_vnb;


	Geom::Vec4f m_colorMin;
	Geom::Vec4f m_colorMax;
	float m_vmin;
	float m_vmax;
	int m_vnb;

	VBO* m_vboPos;
	VBO* m_vboData;

	void getLocations();

//	void restoreUniformsAttribs();

public:
	ShaderIsoLines(int maxNbIsoPerTriangle=6);

	void setColors(const Geom::Vec4f& colorMin, const Geom::Vec4f& colorMax);

	void setDataBound(float attMin, float attMax);

	void setNbIso(int nb);

	/**
	 *
	 */
	void setAttributePosition(VBO* vbo);

	/**
	 * Data  attribute must be float
	 */
	void setAttributeData(VBO* vbo);
};

} // namespace Utils

} // namespace CGoGN

#endif
