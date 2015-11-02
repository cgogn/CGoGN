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

#include <vector>
#include <list>

#include "Topology/generic/dart.h"
#include "Topology/generic/attributeHandler.h"
#include "Topology/generic/functor.h"
#include "Geometry/vector_gen.h"

#include "Utils/vbo_base.h"
#include "Utils/svg.h"

//#include "Utils/Shaders/shaderSimpleColor.h"

#include "Utils/Shaders/shaderBoldLines.h"
#include "Utils/Shaders/shaderDarts.h"


#ifdef WIN32
#ifndef CGoGN_ALGO_API
#if defined CGoGN_ALGO_DLL_EXPORT
#define CGoGN_ALGO_API __declspec(dllexport)
#else
#define CGoGN_ALGO_API __declspec(dllimport)
#endif
#endif
#else
#define CGoGN_ALGO_API
#endif

namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace GL2
{


class CGoGN_ALGO_API TopoGenRender
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


	unsigned int m_vaId1;
	unsigned int m_vaId2;

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

	/// shifting along normals for 3-map boundary drawing
	float m_normalShift;

	float m_boundShift;

	/**
	 * initial darts color (set in update)
	 */
	Geom::Vec4f m_dartsColor;
	Geom::Vec4f m_colorPhi1;
	Geom::Vec4f m_colorPhi2;
	Geom::Vec4f m_dartsBoundaryColor;

	Geom::Vec3f* m_bufferDartPosition;

	//Utils::ShaderSimpleColor* m_shader1;
	//Utils::ShaderPointsLines* m_shader2;

	Utils::ShaderDarts* m_shader1;
	Utils::ShaderBoldLines* m_shader2;

	bool m_sharedShaders;

	std::string m_nameIndex;


	float m_ke;
	float m_kf;
	bool m_wb;
	float m_ns;

	Geom::Vec4f colorPhi1;
	Geom::Vec4f colorPhi2;


	/**
	 * compute color from dart index (for color picking)
	 */
	Dart colToDart(float* color);

	/**
	 * compute dart  from color (for color picking)
	 */
	void dartToCol(Dart d, float& r, float& g, float& b);

	class ColoredDart
	{
	public:
		 Dart d;
		 float r;
		 float g;
		 float b;
		 ColoredDart(Dart dd, float rr, float gg, float bb):
			 d(dd),r(rr),g(gg),b(bb) {}

	};

	std::vector<ColoredDart> m_coloredDarts;



public:

	static void createShaders(std::vector<Utils::GLSLShader*>& shaders);

	/**
	* Constructor
	* @param bs shift for boundary drawing
	*/
	TopoGenRender(float bs = 0.01f);

	TopoGenRender(const std::vector<Utils::GLSLShader*>& shaders, float bs = 0.01f);

	/**
	* Destructor
	*/
	~TopoGenRender();

	void setClippingPlane(const Geom::Vec4f& plane);
	void setNoClippingPlane();


	/**
	 * set the with of line use to draw darts (default val is 2)
	 * @param dw width
	 */
	void setDartWidth(float dw);

	/**
	 * set the with of line use to draw phi (default val is 3)
	 * @param pw width
	 */
	void setRelationWidth(float pw);

	void setColorPhi1(const Geom::Vec4f& col);

	void setColorPhi2(const Geom::Vec4f& col);

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
	 * @param code bit 0: draw darts/ bit 1 drawRelation1 / bit 2 drawRelation2();
	 */
	void drawTopo(int code=0xf);

	/**
	 * get shader objects
	 */
	//Utils::GLSLShader* shader1() { return static_cast<Utils::GLSLShader*>(m_shader1); }
	std::vector<Utils::GLSLShader*> shaders();



	/**
	 * change all darts drawing color
	 * @param d the dart
	 * @param r red !
	 * @param g green !
	 * @param b blue !
	 */
	void setAllDartsColor(float r, float g, float b);

	void setInitialDartsColor(float r, float g, float b);

	void setInitialBoundaryDartsColor(float r, float g, float b);


	/**
	 * pick the color in the rendered image
	 */
	Dart pickColor(unsigned int x, unsigned int y);


	/**
	 * render to svg struct
	 */
	void toSVG(Utils::SVG::SVGOut& svg);

	/**
	 * render svg into svg file
	 */
	void svgout2D(const std::string& filename, const glm::mat4& model, const glm::mat4& proj);

	/**
	 * @brief set normal shift for boundary of dim 3 drawing
	 * @param ns distance shift along normals (use BB.diagSize()/100 is good approximation)
	 */
	void setNormalShift(float ns);

	/**
	 * @brief set boundary shift for boundary of dim 2 drawing
	 * @param ns distance shift
	 */
	void setBoundaryShift(float bs);

	void setExplodeEdge(float ke);

	void setExplodeFace(float kf);

	void setWithBoundary(bool wb);


	/**
	 * @brief add dart to set of colored dart to draw
	 * @param d
	 * @param r
	 * @param g
	 * @param b
	 */
	void addColoredDart(Dart d, float r, float g, float b);

	/**
	 * @brief clear the ColoredDarts set
	 * @param d
	 */
	inline void clearColoredDarts() { m_coloredDarts.clear();}

	/**
	 * @brief remove a dart the ColoredDarts set
	 * @param d
	 */
	void removeColoredDart(Dart d);

};



//template <typename PFP>
class TopoRender : public TopoGenRender
{
public:
	/**
	* Constructor
	* @param bs shift for boundary drawing
	*/
	TopoRender(float bs = 0.01f): TopoGenRender(bs) {}

	TopoRender(const std::vector<Utils::GLSLShader*>& shaders, float bs = 0.01f) : TopoGenRender(shaders, bs) {}

	/**
	* Destructor
	*/
	~TopoRender() {}

	/**
	 * redraw one dart with specific width and color (not efficient use only for debug with small amount of call)
	 * @param d the dart
	 * @param width the drawing width
	 * @param r red !
	 * @param g green !
	 * @param b blue !
	 */
	template<typename MAP>
	void overdrawDart(MAP& map, Dart d, float width, float r, float g, float b);

	/**
	 * pick dart with color set by setDartsIdColor
	 * Do not forget to apply same transformation to scene before picking than before drawing !
	 * @param map the map in which we pick (same as drawn !)
	 * @param x position of mouse (x)
	 * @param y position of mouse (pass H-y, classic pb of origin)
	 * @return the dart or NIL
	 */
	template<typename MAP>
	Dart picking(MAP& map, int x, int y, bool withBoundary=false);

	template<typename MAP>
	Dart coneSelection(MAP& map, const Geom::Vec3f& rayA, const Geom::Vec3f& rayAB, float angle);

	template<typename MAP>
	Dart raySelection(MAP& map, const Geom::Vec3f& rayA, const Geom::Vec3f& rayAB, float distmax);


	template <typename PFP>
	void updateData(typename PFP::MAP &map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& positions, float ke, float kf, bool withBoundary = false, bool onlyBoundary=false);

	template <typename PFP>
	void updateData(typename PFP::MAP &map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& positions, bool onlyBoundary=false);


	/**
	 * Special update function used to draw boundary of map3
	 */
	template<typename PFP>
	void updateDataBoundary(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& positions, float ke, float kf, float ns);

	template<typename PFP>
	void updateDataBoundary(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& positions);


	template <typename PFP>
	void updateDataGMap(typename PFP::MAP &map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& positions, float ke, float kf, bool withBoundary = false, bool onlyBoundary=false);

	template <typename PFP>
	void updateDataGMap(typename PFP::MAP &map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& positions, bool onlyBoundary=false);


	template<typename MAP>
	void drawColoredDarts(MAP& map);

	/**
	 * Get back middle position of drawn darts
	 * @param map the map
	 * @param posExpl the output positions
	 */
	template<typename PFP>
	void computeDartMiddlePositions(typename PFP::MAP& map, DartAttribute< typename PFP::VEC3, typename PFP::MAP>& posExpl);
};


} // namespace GL2

} // namespace Render

} // namespace Algo

} // namespace CGoGN

#include "Algo/Render/GL2/topoRender.hpp"

#endif
