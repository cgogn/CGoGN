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

#ifndef __CGOGN_SHADER_PHONG_
#define __CGOGN_SHADER_PHONG_

#include "Utils/GLSLShader3.h"
#include "Geometry/vector_gen.h"

#include <string>

namespace CGoGN
{
namespace Utils
{

class ShaderPhong : public GLSLShader
{
protected:
	// flag color per vertex or not
	bool m_with_color;

	// shader sources OGL3
    static std::string vertexShaderText;
    static std::string fragmentShaderText;


    // uniform locations
	GLuint m_unif_ambiant;
	GLuint m_unif_diffuse;
	GLuint m_unif_specular;
	GLuint m_unif_shininess;
	GLuint m_unif_lightPos;

	//values
	Geom::Vec4f m_ambiant;
	Geom::Vec4f m_diffuse;
	Geom::Vec4f m_specular;
	float m_shininess;
	Geom::Vec3f m_lightPos;

protected:

	void getLocations();

	void sendParams();

public:
	ShaderPhong();

	// inviduals parameter setting functions
	void setAmbiant(const Geom::Vec4f& ambiant);

	void setDiffuse(const Geom::Vec4f& diffuse);

	void setSpecular(const Geom::Vec4f& specular);

	void setShininess(float shininess);

	void setLightPosition( Geom::Vec3f lp);

	const Geom::Vec4f& getAmbiant() const { return m_ambiant;}

	const Geom::Vec4f& getDiffuse() const { return m_diffuse;}

	const Geom::Vec4f& getSpecular() const { return m_specular;}

	float getShininess() const { return m_shininess;}

	const Geom::Vec3f& getLightPosition() const { return m_lightPos;}

	/**
	 * set all parameter in on call (one bind also)
	 */
	void setParams(const Geom::Vec4f& ambiant, const Geom::Vec4f& diffuse, const Geom::Vec4f& specular, float shininess, const Geom::Vec3f& lightPos);


	// attributes
	void setAttributePosition(VBO& vbo)		{this->bindVA_VBO("VertexPosition", vbo);}
	void setAttributeNormal(VBO& vbo)		{this->bindVA_VBO("VertexNormal", vbo);}

	// optionnal attributes
	void setAttributeColor(VBO& vbo);
	void unsetAttributeColor();
};

}
}
#endif
