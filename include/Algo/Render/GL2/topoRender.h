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

#ifndef _GL2_TOPO_RENDER_
#define _GL2_TOPO_RENDER_

#include <GL/glew.h>

#include <vector>
#include <list>

#include "Topology/generic/dart.h"
#include "Topology/generic/attributeHandler.h"
#include "Topology/generic/functor.h"
#include "Geometry/vector_gen.h"

#include "Utils/vbo.h"

#include "Utils/svg.h"

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

class TopoRender
{
protected:
	/**
	* vbo buffers
	* 0: vertices darts
	* 1: vertices relation 1
	* 2: vertices relation 2
	* 3: color
	*/
	Utils::VBO* m_vbo0;
	Utils::VBO* m_vbo1;
	Utils::VBO* m_vbo2;
	Utils::VBO* m_vbo3;

	unsigned int m_vaId;

	/**
	*number of darts to draw
	*/
	GLuint m_nbDarts;

	/**
	* number of relations 1 to draw
	*/
	GLuint m_nbRel1;

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
	 * initial darts color (set in update)
	 */
	Geom::Vec3f m_dartsColor;


	float *m_color_save;

	/**
	 * attribut d'index dans le VBO
	 */
	AttributeHandler<unsigned int> m_attIndex;

	Utils::ShaderSimpleColor* m_shader1;
	Utils::ShaderColorPerVertex* m_shader2;


	/**
	 * compute color from dart index (for color picking)
	 */
	Dart colToDart(float* color);

	/**
	 * compute dart  from color (for color picking)
	 */
	void dartToCol(Dart d, float& r, float& g, float& b);

	/**
	 * pick the color in the rendered image
	 */
	Dart pickColor(unsigned int x, unsigned int y);

	/**
	 * affect a color to each dart
	 */
	template<typename PFP>
	void setDartsIdColor(typename PFP::MAP& map, const FunctorSelect& good);

	/**
	 * save colors before picking
	 */
	void pushColors();

	/**
	 * restore colors after picking
	 */
	void popColors();

public:
	/**
	* Constructor
	* @param map the map to draw
	* @param good functor that return true for darts of part to draw
	* @param type_vbo vbo to alloc ( VBO_P, VBO_PN, VBO_PNC, VBO_PC ..)
	*/	

	TopoRender();

	/**
	* Destructor
	*/
	~TopoRender();

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

	void setInitialDartsColor(float r, float g, float b);

	/**
	 * redraw one dart with specific width and color (not efficient use only for debug with small amount of call)
	 * @param d the dart
	 * @param width the drawing width
	 * @param r red !
	 * @param g green !
	 * @param b blue !
	 */
	void overdrawDart(Dart d, float width, float r, float g, float b);


	/**
	 * pick dart with color set by setDartsIdColor
	 * Do not forget to apply same transformation to scene before picking than before drawing !
	 * @param map the map in which we pick (same as drawn !)
	 * @param good the selector (same as used during drawing)
	 * @param x position of mouse (x)
	 * @param y position of mouse (pass H-y, classic pb of origin)
	 * @return the dart or NIL
	 */
	template<typename PFP>
	Dart picking(typename PFP::MAP& map, int x, int y, const FunctorSelect& good=allDarts);

	template <typename PFP>
	void updateData(typename PFP::MAP& map, const typename PFP::TVEC3& positions, float ke, float kf, const FunctorSelect& good = allDarts);

	template <typename PFP>
	void updateDataMap(typename PFP::MAP& map, const typename PFP::TVEC3& positions, float ke, float kf, const FunctorSelect& good = allDarts);

	template <typename PFP>
	void updateDataGMap(typename PFP::MAP& map, const typename PFP::TVEC3& positions, float ke, float kf, const FunctorSelect& good = allDarts);


	void svgout2D(const std::string& filename, const glm::mat4& model, const glm::mat4& proj);
};

// just for compatibility with old code
typedef TopoRender TopoRenderMapD;
typedef TopoRender TopoRenderGMap;


} // namespace GL2

} // namespace Render

} // namespace Algo

} // namespace CGoGN


#include "Algo/Render/GL2/topoRender.hpp"

#endif
