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

#include <GL/gl.h>

#include <vector>
#include <list>

#include "Topology/generic/dart.h"
#include "Topology/generic/attributeHandler.h"
#include "Topology/generic/functor.h"

namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace GL2
{


class explodeVolume_VBORender
{
protected:


	/**
	* vbo buffers
	* 0: vertices
	* 1: normals
	*/
	GLuint m_VBOBuffers[2];

	/**
	*number of triangles to draw
	*/
	GLuint m_nbTris;

public:
	/**
	* Constructor
	* @param map the map to draw
	* @param good functor that return true for darts of part to draw
	* @param type_vbo vbo to alloc ( VBO_P, VBO_PN, VBO_PNC, VBO_PC ..)
	*/
	explodeVolume_VBORender();

	/**
	* Destructor
	*/
	~explodeVolume_VBORender();

	/**
	* update all drawing buffers
	* @param map the map
	* @param good selector
	* @param positions  attribute of position vertices
	* @param kf exploding coef for face
 	* @param kv exploding coef for face
	*/
	template<typename PFP>
	void updateData(typename PFP::MAP& map, const FunctorSelect& good, const typename PFP::TVEC3& positions, float kf, float kv);

	/**
	 * draw all topo
	 */
	void drawFaces();
};

}//end namespace GL2

}//end namespace Algo

}//end namespace Render

}//end namespace CGoGN

#include "Algo/Render/GL2/explodeVolumeRender.hpp"

#endif
