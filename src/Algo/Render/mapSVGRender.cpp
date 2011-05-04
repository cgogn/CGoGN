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
#include "Algo/Render/SVG/mapSVGRender.h"

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

void SvgPolyline::addVertex(const Geom::Vec3f& v)
{
	m_vertices.push_back(v);
}

void SvgPolyline::close()
{
	m_vertices.push_back(m_vertices.front());
}

void SvgPolyline::save(std::ofstream& out)
{
	std::stringstream ss;

	out << "<polyline fill=\"none\" stroke=\"#";
	out << std::hex;
	unsigned int wp = out.width(2);
	char prev = out.fill('0');
	out << int(m_color[0]*255);
	out.width(2); out.fill('0');
	out<< int(m_color[1]*255);
	out.width(2); out.fill('0');
	out << int(m_color[2]*255)<<std::dec;
	out <<"\" stroke-width=\""<<m_width<<"\" points=\"";
	out.fill(prev);
	out.width(wp);
	for (std::vector<Geom::Vec3f>::iterator it =m_vertices.begin(); it != m_vertices.end(); ++it)
	{
		out << (*it)[0] << ","<< (*it)[1]<< " ";
	}
	out <<"\"/>"<< std::endl;

}





SVGOut::SVGOut(const std::string& filename, const glm::mat4& model, const glm::mat4& proj):
		m_model(model),m_proj(proj),global_color(Geom::Vec3f(0.0f,0.0f,0.0f)), global_width(2.0f)
{
	m_objs.reserve(1000);

	m_out = new std::ofstream(filename.c_str()) ;
	if (!m_out->good())
	{
		CGoGNerr << "Unable to open file " << CGoGNendl ;
		// ????
	}

	glGetIntegerv(GL_VIEWPORT, &(m_viewport[0]));

	*m_out << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"<< std::endl;
	*m_out << "<svg xmlns=\"http://www.w3.org/2000/svg\""<< std::endl;
	*m_out << " xmlns:xlink=\"http://www.w3.org/1999/xlink\""<< std::endl;
	*m_out << " width=\""<<m_viewport[2]<<"px\" height=\""<<m_viewport[3]<<"px\" viewBox=\"0 0 "<<m_viewport[2]<<" "<<m_viewport[3]<<"\">"<< std::endl;
	*m_out << "<title>test</title>"<< std::endl;
	*m_out << "<desc>"<< std::endl;
	*m_out << "Rendered from CGoGN"<< std::endl;

	*m_out << "</desc>"<< std::endl;
	*m_out << "<defs>"<< std::endl;
	*m_out << "</defs>"<< std::endl;
	*m_out << "<g shape-rendering=\"crispEdges\">" << std::endl;
}

SVGOut::~SVGOut()
{
	if (m_out->good())
	{
		closeFile();
	}
	delete m_out;

	for (std::vector<SvgObj*>::iterator it = m_objs.begin(); it != m_objs.end(); ++it)
		delete (*it);
}

void SVGOut::setColor(const Geom::Vec3f& col)
{
	global_color = col;
}

void SVGOut::setWidth(float w)
{
	global_width = w;
}

void SVGOut::closeFile()
{
	// here do the sort in necessary
	for (std::vector<SvgObj*>::iterator it = m_objs.begin(); it != m_objs.end(); ++it)
	{
		(*it)->save(*m_out);
	}

	*m_out << "</g>" << std::endl;
	*m_out << "</svg>" << std::endl;
	m_out->close();
}



} // namespace SVG
} // namespace Render
} // namespace Algo
} // namespace CGoGN


