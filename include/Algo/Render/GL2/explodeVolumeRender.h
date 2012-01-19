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

#ifndef _EXPLODE_VOLUME_VBO_RENDER
#define _EXPLODE_VOLUME_VBO_RENDER

#include <GL/glew.h>

#include <vector>
#include <list>

#include "Topology/generic/dart.h"
#include "Topology/generic/attributeHandler.h"
#include "Topology/generic/functor.h"
#include "Utils/vbo.h"
#include "Utils/Shaders/shaderExplodeVolumes.h"
#include "Utils/Shaders/shaderExplodeVolumesLines.h"

namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace GL2
{


class ExplodeVolumeRender
{
protected:

	Utils::ShaderExplodeVolumes* m_shader;

	Utils::ShaderExplodeVolumesLines* m_shaderL;

	Utils::VBO* m_vboPos;

	Utils::VBO* m_vboPosLine;

	/**
	*number of triangles to draw
	*/
	GLuint m_nbTris;

	GLuint m_nbLines;



public:
	/**
	* Constructor
	* @param map the map to draw
	* @param good functor that return true for darts of part to draw
	* @param type_vbo vbo to alloc ( VBO_P, VBO_PN, VBO_PNC, VBO_PC ..)
	*/
	ExplodeVolumeRender();

	/**
	* Destructor
	*/
	~ExplodeVolumeRender();


	/**
	 * return a ptr on used shader do not forgot to register
	 */
	Utils::GLSLShader* shaderFaces() { return m_shader;}

	/**
	 * return a ptr on used shader do not forgot to register
	 */
	Utils::GLSLShader* shaderLines() { return m_shaderL;}


	/**
	* update all drawing buffers
	* @param map the map
	* @param positions  attribute of position vertices
	* @param good selector
	*/
	template<typename PFP>
	void updateData(typename PFP::MAP& map, typename PFP::TVEC3& positions, const FunctorSelect& good = allDarts);

	/**
	 * draw
	 */
	void drawEdges();

	void drawFaces();


	void setExplodeVolumes(float explode) { m_shader->setExplodeVolumes(explode);m_shaderL->setExplodeVolumes(explode);}

	void setAmbiant(const Geom::Vec4f& ambiant) { m_shader->setAmbiant(ambiant);}

	void setDiffuse(const Geom::Vec4f& diffuse) { m_shader->setDiffuse(diffuse);}

	void setLightPosition(const Geom::Vec3f& lp) { m_shader->setLightPosition(lp);}
};

}//end namespace GL2

}//end namespace Algo

}//end namespace Render

}//end namespace CGoGN

#include "Algo/Render/GL2/explodeVolumeRender.hpp"

#endif
