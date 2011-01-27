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

#ifndef _TOPO_VBO_RENDER
#define _TOPO_VBO_RENDER

#include <GL/glew.h>

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

namespace VBO
{

class topo_VBORender
{
protected:

	/**
	* vbo buffers
	* 0: vertices darts
	* 1: vertices relation 1
	* 2: vertices relation 2
	* 3: color
	*/
	GLuint m_VBOBuffers[4];

	/**
	*number of darts to draw
	*/
	GLuint m_nbDarts;

	/**
	* number of relations 2 to draw
	*/
	GLuint m_nbRel2;
	/**
	 * width of lines use to draw darts
	 */
	float m_topo_dart_width;

	/**
	 * width of lines use to draw phi
	 */
	float m_topo_relation_width;


	/**
	 * attribut d'index dans le VBO
	 */
	AttributeHandler<unsigned int> m_attIndex;

public:
	/**
	* Constructor
	* @param map the map to draw
	* @param good functor that return true for darts of part to draw
	* @param type_vbo vbo to alloc ( VBO_P, VBO_PN, VBO_PNC, VBO_PC ..)
	*/	

	topo_VBORender();

	/**
	* Destructor
	*/
	~topo_VBORender();

	/**
	 * set the with of line use to draw darts (default val is 2)
	 * @param dw width
	 */
	void setDartWidth(float dw);

	/**
	 * set the with of line use to draw phi (default val is ")
	 * @param pw width
	 */
	void setRelationWidth(float pw);

	/**
	* Drawing function for darts only
	*/
	void drawDarts();

	/**
	* Drawing function for phi1 only
	*/
	void drawRelation1();

	/**
	* Drawing function for phi2 only
	*/
	void drawRelation2();

	/**
	 * draw all topo
	 */
	void drawTopo();
	/**
	 * change dart drawing color
	 * @param d the dart
	 * @param r red !
	 * @param g green !
	 * @param b blue !
	 */
	void setDartColor(Dart d, float r, float g, float b);

	/**
	 * change all darts drawing color
	 * @param d the dart
	 * @param r red !
	 * @param g green !
	 * @param b blue !
	 */
	void setAllDartsColor(float r, float g, float b);

	/**
	 * redraw one dart with specific width and color (not efficient use only for debug with small amount of call)
	 * @param d the dart
	 * @param width the drawing width
	 * @param r red !
	 * @param g green !
	 * @param b blue !
	 */
	void overdrawDart(Dart d, float width, float r, float g, float b);


};

class topo_VBORenderMapD:public topo_VBORender
{
public:
	/**
	* update all drawing buffers
	* @param map the map
	* @param positions  attribute of position vertices
	* @param ke exploding coef for edge
	* @param kf exploding coef for face
	* @param good selector
	*/
	template <typename PFP>
	void updateData(typename PFP::MAP& map, const typename PFP::TVEC3& positions, float ke, float kf, const FunctorSelect& good = SelectorTrue());
};

class topo_VBORenderGMap: public topo_VBORender
{
public:
	/**
	* update all drawing buffers
	* @param map the map
	* @param positions  attribute of position vertices
	* @param ke exploding coef for edge
	* @param kf exploding coef for face
	* @param good selector
	*/
	template <typename PFP>
	void updateData(typename PFP::MAP& map, const typename PFP::TVEC3& positions, float ke, float kf, const FunctorSelect& good = SelectorTrue());
};

} // namespace VBO

} // namespace Render

} // namespace Algo

} // namespace CGoGN


#include "Algo/Render/topo_vboRender.hpp"

#endif
