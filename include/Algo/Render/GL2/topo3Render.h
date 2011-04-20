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

#ifndef _TOPO3_VBO_RENDER
#define _TOPO3_VBO_RENDER

#include <GL/glew.h>

#include <vector>
#include <list>

#include "Topology/generic/dart.h"
#include "Topology/generic/attributeHandler.h"
#include "Topology/generic/functor.h"

#include "Utils/vbo.h"

// forward
namespace CGoGN { namespace Utils {  class ShaderSimpleColor; } }
namespace CGoGN { namespace Utils {  class ShaderColorPerVertex; } }

namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace GL2
{


class Topo3Render
{
protected:

	/**
	* vbo buffers
	* 0: vertices darts
	* 1: vertices phi1
	* 2: vertices phi2
	* 3: vertices phi3
	* 4: colors
	*/
	Utils::VBO* m_vbo0;
	Utils::VBO* m_vbo1;
	Utils::VBO* m_vbo2;
	Utils::VBO* m_vbo3;
	Utils::VBO* m_vbo4;

	unsigned int m_vaId;

	Utils::ShaderSimpleColor* m_shader1;
	Utils::ShaderColorPerVertex* m_shader2;


	/**
	*number of darts to draw
	*/
	GLuint m_nbDarts;

	/**
	* number of relations 2 to draw
	*/
	GLuint m_nbRel2;

	/**
	* number of relations 3 to draw
	*/
	GLuint m_nbRel3;

	/**
	 * width of lines use to draw darts
	 */
	float m_topo_dart_width;

	/**
	 * width of lines use to draw phi
	 */
	float m_topo_relation_width;


	float *m_color_save;

	AttributeHandler<unsigned int> m_attIndex;

	/**
	* update all drawing buffers
	* @param map the map
	* @param good selector
	* @param positions  attribute of position vertices
	* @param ke exploding coef for edge
	* @param kf exploding coef for face
 	* @param kv exploding coef for volume
	*/
	template<typename PFP>
	void updateMapD3(typename PFP::MAP& map, const FunctorSelect& good, const typename PFP::TVEC3& positions, float ke, float kf, float kv);


	/**
	* update all drawing buffers
	* @param map the map
	* @param good selector
	* @param positions  attribute of position vertices
	* @param ke exploding coef for edge
	* @param kf exploding coef for face
 	* @param kv exploding coef for volume
	*/
	template<typename PFP>
	void updateGMap3(typename PFP::MAP& map, const FunctorSelect& good, const typename PFP::TVEC3& positions, float ke, float kf, float kv);


	/**
	 * save colors
	 */
	void pushColors();


	/**
	 * restore colors
	 */
	void popColors();

	/**
	 * pick dart with color set bey setDartsIdColor
	 * @param x position of mouse (x)
	 * @param y position of mouse (pass H-y, classic pb of origin)
	 * @return the dart or NIL
	 */
	Dart pickColor(unsigned int x, unsigned int y);

public:

	Dart colToDart(float* color);

	void dartToCol(Dart d, float& r, float& g, float& b);

	/**
	* Constructor
	* @param map the map to draw
	* @param good functor that return true for darts of part to draw
	* @param type_vbo vbo to alloc ( VBO_P, VBO_PN, VBO_PNC, VBO_PC ..)
	*/
	Topo3Render();

	/**
	* Destructor
	*/
	~Topo3Render();

	/**
	* update all drawing buffers
	* @param map the map
	* @param good selector
	* @param positions  attribute of position vertices
	* @param ke exploding coef for edge
	* @param kf exploding coef for face
 	* @param kv exploding coef for face
	*/
//	template<typename PFP>
//	void updateData(typename PFP::MAP& map, const FunctorSelect& good, const typename PFP::TVEC3& positions, float ke, float kf, float kv);


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
	* Drawing function for phi2 only
	*/
	void drawRelation3();

	/**
	 * draw all topo
	 * \warning DO NOT FORGET TO DISABLE CULLFACE BEFORE CALLING
	 */
	void drawTopo();

//	void drawDart(Dart d, float R, float G, float B, float width);

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
	 * overdraw a dart with given width and color
	 * @param d the dart
	 * @param width drawing width
	 * @param r red !
	 * @param g green !
	 * @param b blue !
	 */
	void overdrawDart(Dart d, float width, float r, float g, float b);



	/*
	 * store darts in color for picking
	 * @param map the map (must be the same than during updating data)
	 * @param good the selector (must be the same than during updating data)
	 */
	template<typename PFP>
	void setDartsIdColor(typename PFP::MAP& map, const FunctorSelect& good);



	/**
	 * pick dart with color set bey setDartsIdColor
	 * Do not forget to apply same transformation to scene before picking than before drawing !
	 * @param map the map
	 * @param good a dart selector
	 * @param x position of mouse (x)
	 * @param y position of mouse (pass H-y, classic pb of origin)
	 * @return the dart or NIL
	 */
	template<typename PFP>
	Dart picking(typename PFP::MAP& map, const FunctorSelect& good, int x, int y);

};

//template<typename MAP>
//class topo3_VBORenderMap: public topo3_VBORender
//{
//public:
//	/**
//	* update all drawing buffers
//	* @param map the map
//	* @param good selector
//	* @param positions  attribute of position vertices
//	* @param ke exploding coef for edge
//	* @param kf exploding coef for face
// 	* @param kv exploding coef for face
//	*/
//	template<typename PFP>
//	void updateData(typename PFP::MAP& map, const FunctorSelect& good, const typename PFP::TVEC3& positions, float ke, float kf, float kv) {}
//};




class Topo3RenderMapD: public Topo3Render
{
public:
	/**
	* update all drawing buffers
	* @param map the map
	* @param good selector
	* @param positions  attribute of position vertices
	* @param ke exploding coef for edge
	* @param kf exploding coef for face
 	* @param kv exploding coef for face
	*/
	template<typename PFP>
	void updateData(typename PFP::MAP& map, const FunctorSelect& good, const typename PFP::TVEC3& positions, float ke, float kf, float kv);
};
//
//
//class Topo3RenderGMap: public Topo3Render
//{
//public:
//	/**
//	* update all drawing buffers
//	* @param map the map
//	* @param good selector
//	* @param positions  attribute of position vertices
//	* @param ke exploding coef for edge
//	* @param kf exploding coef for face
// 	* @param kv exploding coef for face
//	*/
//	template<typename PFP>
//	void updateData(typename PFP::MAP& map, const FunctorSelect& good, const typename PFP::TVEC3& positions, float ke, float kf, float kv);
//};

}//end namespace VBO

}//end namespace Algo

}//end namespace Render

}//end namespace CGoGN

#include "Algo/Render/GL2/topo3Render.hpp"

#endif
