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

#ifndef _UTIL_SVG_
#define _UTIL_SVG_

#include <vector>
#include <fstream>
#include <sstream>

#include "Geometry/vector_gen.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_precision.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_projection.hpp"
#include "glm/gtc/matrix_transform.hpp"


namespace CGoGN
{

namespace Utils
{

namespace SVG
{


class SvgObj
{
protected:
	std::vector<Geom::Vec3f> m_vertices;
	std::vector<Geom::Vec3f> m_colors;
	std::vector<Geom::Vec3f> m_vertices3D;
	Geom::Vec3f m_color;
	float m_width;
public:
	virtual ~SvgObj() {}

	void addVertex(const Geom::Vec3f& v);

	void addVertex3D(const Geom::Vec3f& v);

	void addVertex(const Geom::Vec3f& v, const Geom::Vec3f& C);

	void addVertex3D(const Geom::Vec3f& v, const Geom::Vec3f& C);

	void setColor(const Geom::Vec3f& c);

	void setWidth(float w);

	void close();

	virtual void save(std::ofstream& out)=0;

	unsigned int nbv() const;

	const Geom::Vec3f& P(unsigned int i) const;

	Geom::Vec3f normal();

};

class SvgPoints: public SvgObj
{
public:
	void save(std::ofstream& out);
};

class SvgLines: public SvgObj
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
	SvgObj* m_current;

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

	void beginPoints();
	void endPoints();
	void addPoint(const Geom::Vec3f& P);
	void addPoint(const Geom::Vec3f& P, const Geom::Vec3f& C);


	void beginLines();
	void endLines();
	void addLine(const Geom::Vec3f& P, const Geom::Vec3f& P2);
	void addLine(const Geom::Vec3f& P, const Geom::Vec3f& P2, const Geom::Vec3f& C);

};




} // namespace SVG
} // namespace Utils
} // namespace CGogN


#endif
