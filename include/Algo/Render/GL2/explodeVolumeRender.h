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

#ifndef _EXPLODE_VOLUME_VBO_RENDER
#define _EXPLODE_VOLUME_VBO_RENDER

#include <vector>
#include <list>

#include "Topology/generic/dart.h"
#include "Topology/generic/attributeHandler.h"
#include "Topology/generic/functor.h"
#include "Utils/vbo_base.h"
#include "Utils/Shaders/shaderExplodeSmoothVolumes.h"
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

	Utils::ShaderExplodeSmoothVolumes* m_shaderS;

	bool m_cpf;

	bool m_ef;

	bool m_smooth;

	Utils::ShaderExplodeVolumesLines* m_shaderL;

	Utils::VBO* m_vboPos;

	Utils::VBO* m_vboColors;

	Utils::VBO* m_vboNormals;

	Utils::VBO* m_vboPosLine;

	/**
	*number of triangles to draw
	*/
	GLuint m_nbTris;

	GLuint m_nbLines;

	Geom::Vec3f m_globalColor;

	template<typename PFP>
	void updateSmooth(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& positions, const VolumeAttribute<typename PFP::VEC3>& colorPerFace, const FunctorSelect& good = allDarts) ;

	template<typename PFP>
	void updateSmooth(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& positions, const FunctorSelect& good = allDarts) ;


public:
	/**
	* Constructor
	* @param withColorPerFace affect a color per face
	* @param withExplodeFace shrinj each face
	*/
	ExplodeVolumeRender(bool withColorPerFace = false, bool withExplodeFace = false, bool withSmoothFaces = false) ;

	/**
	* Destructor
	*/
	~ExplodeVolumeRender() ;

	/**
	 * return a ptr on used shader do not forgot to register
	 */
	Utils::GLSLShader* shaderFaces() ;

	/**
	 * return a ptr on used shader do not forgot to register
	 */
	Utils::GLSLShader* shaderLines() ;

	/**
	* update all drawing buffers
	* @param map the map
	* @param positions  attribute of position vertices
	* @param good selector
	*/
	template<typename PFP>
	void updateData(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& positions, const FunctorSelect& good = allDarts) ;

	/**
	* update all drawing buffers
	* @param map the map
	* @param positions attribute of position vertices
	* @param colorPerFace attribute of color (per face)
	* @param good selector
	*/
	template<typename PFP>
	void updateData(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& positions, const VolumeAttribute<typename PFP::VEC3>& colorPerFace, const FunctorSelect& good = allDarts) ;


	/**
	 * draw edges
	 */
	void drawEdges() ;

	/**
	 * draw edges
	 */
	void drawFaces() ;

	/**
	 * set exploding volume coefficient parameter
	 */
	void setExplodeVolumes(float explode) ;

	/**
	 * set exploding volume coefficient parameter
	 */
	void setExplodeFaces(float explode) ;

	/**
	 * set clipping plane
	 */
	void setClippingPlane(const Geom::Vec4f& p) ;

	/**
	 * unset clipping plane
	 */
	void setNoClippingPlane() ;

	/**
	 * set ambiant color parameter
	 */
	void setAmbiant(const Geom::Vec4f& ambiant) ;

	/**
	 * set back color parameter
	 */
	void setBackColor(const Geom::Vec4f& color) ;

	/**
	 * set light position parameter
	 */
	void setLightPosition(const Geom::Vec3f& lp) ;

	/**
	 * set color parameter for edge drawing
	 */
	void setColorLine(const Geom::Vec4f& col) ;
};

}//end namespace GL2

}//end namespace Algo

}//end namespace Render

}//end namespace CGoGN

#include "Algo/Render/GL2/explodeVolumeRender.hpp"

#endif
