/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009, IGG Team, LSIIT, University of Strasbourg                *
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
* Web site: https://iggservis.u-strasbg.fr/CGoGN/                              *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#ifndef __CGOGN_SHADER_PHONG__
#define __CGOGN_SHADER_PHONG__

#include <GL/glew.h>
#include "Utils/GLSLShader.h"
#include "Geometry/vector_gen.h"

#include <string>

namespace CGoGN
{

namespace Utils
{

class ShaderPTM : public GLSLShader
{
protected:

	// shader sources OGL3
    static std::string vertexShaderText;
    static std::string fragmentShaderText;

    // uniform locations
    GLuint m_unif_cameraPos ;
    GLuint m_unif_proj ;

	//values
	Geom::Vec3f m_cameraPos ;
	unsigned int m_proj ;

	VBO* m_vboPos ;
	VBO* m_vboFi ;
	VBO* m_vboFn ;
	VBO* m_vboA0 ;
	VBO* m_vboA3 ;
	VBO* m_vboA6 ;
	VBO* m_vboA9 ;
	VBO* m_vboA12 ;

	void getLocations();

	void sendParams();

	void restoreUniformsAttribs();

public:
	ShaderPTM();

	// inviduals parameter setting functions
	void setCameraPos(const Geom::Vec3f& camera) ;

	void setProjectionType(const unsigned int& proj) ;

	const Geom::Vec3f& getCameraPosition() const { return m_cameraPos ; }

	const unsigned int& getProjectionType() const { return m_proj ; }

	/**
	 * set all parameter in on call (one bind also)
	 */
	void setParams(const Geom::Vec3f& cameraPos, const unsigned int& proj) ;

	// attributes
	unsigned int setAttributePosition(VBO* vbo) ;
	unsigned int setAttributeFi(VBO* vbo) ;
	unsigned int setAttributeFn(VBO* vbo) ;
	unsigned int setAttributeA0(VBO* vbo) ;
	unsigned int setAttributeA3(VBO* vbo) ;
	unsigned int setAttributeA6(VBO* vbo) ;
	unsigned int setAttributeA9(VBO* vbo) ;
	unsigned int setAttributeA12(VBO* vbo) ;
} ;

} // namespace Utils

} // namespace CGoGN

#endif
