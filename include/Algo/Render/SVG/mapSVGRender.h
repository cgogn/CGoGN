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
	std::vector<Geom::Vec3f> m_vertices;
	std::vector<Geom::Vec3f> m_vertices3D;
	Geom::Vec3f m_color;
	float m_width;
public:
	void addVertex(const Geom::Vec3f& v);

	void addVertex3D(const Geom::Vec3f& v);

	void setColor(const Geom::Vec3f& c);

	void setWidth(float w) { m_width=w;}

	void close();

	virtual void save(std::ofstream& out)=0;

	unsigned int nbv() const { return m_vertices3D.size();}

	const Geom::Vec3f& P(unsigned int i) const  { return m_vertices3D[i];}

	Geom::Vec3f normal();

};

class SvgPoints: public SvgObj
{
public:
	void save(std::ofstream& out);
};


class SvgPolyline: public SvgObj
{
public:
	void save(std::ofstream& out);
};


class SvgPolygon: public SvgObj
{
protected:
	Geom::Vec3f m_colorFill;
public:

	void setColorFill(const Geom::Vec3f& c);

	void save(std::ofstream& out);
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
	void renderLinesToSVG(typename PFP::MAP& map, const typename PFP::TVEC3& position, const FunctorSelect& good = allDarts, unsigned int thread=0);

	template <typename PFP>
	void renderFacesToSVG(typename PFP::MAP& map, const typename PFP::TVEC3& position, float shrink, bool cull = false, const FunctorSelect& good = allDarts, unsigned int thread=0);

	template <typename PFP>
	void renderPointsToSVG(typename PFP::MAP& map, const typename PFP::TVEC3& position, const FunctorSelect& good = allDarts, unsigned int thread=0);


	void orderPrimitives(std::list<SvgObj*>& primitives);
};


struct compSvgObj
{
	int points_plane (SvgPolygon* pol_points, SvgPolygon* pol_plane, float& averageZ);
	bool operator() (SvgObj* a, SvgObj*b);
};

struct compNormObj
{
//	int points_plane (SvgPolygon* pol_points, SvgPolygon* pol_plane);
	bool operator() (SvgObj* a, SvgObj*b);
};

//
//class BSP_SVG
//{
//protected:
//	std::list<BSP_SVG*> m_front;
//	std::list<BSP_SVG*> m_back;
//	std::list<BSP_SVG*> m_intersect;
//	SvgObj* m_obj;
//
//public:
//	void insert(SvgObj*)
//};



} // namespace SVG
} // namespace Render
} // namespace Algo
} // namespace CGoGN

#include "Algo/Render/SVG/mapSVGRender.hpp"

#endif
