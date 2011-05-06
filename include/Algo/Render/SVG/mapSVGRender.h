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

#ifndef _MAP_SVG_RENDER
#define _MAP_SVG_RENDER

#include <vector>
#include <fstream>
#include <sstream>

#include "Geometry/vector_gen.h"
#include "Topology/generic/functor.h"
#include "Topology/generic/dartmarker.h"


#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_precision.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_projection.hpp"
#include "glm/gtc/matrix_transform.hpp"


/**
* A set of functions that allow the creation of rendering
* object using Vertex-Buffer-Object.
* Function are made for dual-2-map and can be used on
* any subset of a dual-N-map which is a 2-map
*/
namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace SVG
{

class SvgObj
{
protected:
	Geom::Vec3f m_color;
public:
	virtual void save(std::ofstream& out)=0;
};

class SvgPolyline: public SvgObj
{
protected:
	std::vector<Geom::Vec3f> m_vertices;
	float m_width;
public:
	void addVertex(const Geom::Vec3f& v);

	void close();

	void setColor(const Geom::Vec3f& c) { m_color=c;}

	void setWidth(float w) { m_width=w;}

	void save(std::ofstream& out);

};


class Svg_Polygon: public SvgObj
{
protected:
	std::vector<Geom::Vec3f> m_vertices;
public:
	void addVertex(const Geom::Vec3f& v) {}
	void close() {}
	void save(std::ofstream& out) {}
};


class SVGOut
{
protected:
	std::ofstream* m_out;

	const glm::mat4& m_model;
	const glm::mat4& m_proj;
	glm::i32vec4 m_viewport;

	Geom::Vec3f global_color;
	float global_width;

	std::vector<SvgObj*> m_objs;

public:

	/**
	 * Object that allow the rendering/exporting in svg file
	 * @param filename file name ended by .svg
	 * @param model the modelview matrix
	 * @param proj the projection matrix
	 */
	SVGOut(const std::string& filename, const glm::mat4& model, const glm::mat4& proj);

	/**
	 * destructor
	 * flush and close the file
	 */
	~SVGOut();

	void setColor(const Geom::Vec3f& col);

	void setWidth(float w);

	void closeFile();

	template <typename PFP>
	void renderLinesToSVG(typename PFP::MAP& map, const typename PFP::TVEC3& position, const FunctorSelect& good = SelectorTrue(), unsigned int thread=0);

	template <typename PFP>
	void renderFacesToSVG(typename PFP::MAP& map, const typename PFP::TVEC3& position, float shrink, const FunctorSelect& good = SelectorTrue(), unsigned int thread=0);

};



} // namespace SVG
} // namespace Render
} // namespace Algo
} // namespace CGoGN

#include "Algo/Render/SVG/mapSVGRender.hpp"

#endif
