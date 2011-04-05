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

#ifndef __CGOGN_SHADER_FLAT_
#define __CGOGN_SHADER_FLAT_

#include "Utils/GLSLShader.h"
#include "Geometry/vector_gen.h"

namespace CGoGN
{
namespace Utils
{

class ShaderFlat : public GLSLShader
{
protected:
	// shader sources
    static std::string vertexShaderText;
    static std::string fragmentShaderText;
    static std::string geometryShaderText;
    // uniform locations
	GLuint m_unif_ambiant;
	GLuint m_unif_diffuse;
	GLuint m_unif_lightPos;
	GLuint m_unif_explode;

public:
	ShaderFlat();
	void setAmbiant(float explode) 				 {this->bind(); glUniform1f (m_unif_explode, explode);}
	void setAmbiant(const Geom::Vec4f& ambiant)  {this->bind(); glUniform4fv(m_unif_ambiant,1, ambiant.data());}
	void setDiffuse(const Geom::Vec4f& diffuse)  {this->bind(); glUniform4fv(m_unif_diffuse,1, diffuse.data());}
	void setLightPosition( Geom::Vec3f lp)		 {this->bind(); glUniform3fv(m_unif_lightPos,1,lp.data());}

	void setParams(float explode, const Geom::Vec4f& ambiant, const Geom::Vec4f& diffuse, const Geom::Vec3f& lightPos);

	void setAttributePosition(VBO& vbo)	{this->bindVA_VBO("VertexPosition", vbo);}

};

}
}
#endif
