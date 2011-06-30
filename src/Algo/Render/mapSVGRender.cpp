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
#include "Algo/Render/SVG/mapSVGRender.h"
#include <algorithm>
#include <typeinfo>

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

void SvgObj::addVertex(const Geom::Vec3f& v)
{
	m_vertices.push_back(v);
}

void SvgObj::addVertex3D(const Geom::Vec3f& v)
{
	m_vertices3D.push_back(v);
}



void SvgObj::setColor(const Geom::Vec3f& c)
{
	m_color=c;
}


void SvgObj::close()
{
	m_vertices.push_back(m_vertices.front());
}


Geom::Vec3f SvgObj::normal()
{
	if (m_vertices.size()<3)
	{
		CGoGNerr << "Error SVG normal computing (not enough points)"<<CGoGNendl;
		return Geom::Vec3f(0.0f,0.0f,0.0f);
	}

	Geom::Vec3f U = m_vertices3D[2] - m_vertices3D[1];
	Geom::Vec3f V = m_vertices3D[0] - m_vertices3D[1];

	Geom::Vec3f N = U^V;

	N.normalize(); // TO DO verify that is necessary

	return N;
}


void SvgPoints::save(std::ofstream& out)
{
	std::stringstream ss;

	std::cout << "SAVE" << std::endl;

	for (std::vector<Geom::Vec3f>::iterator it =m_vertices.begin(); it != m_vertices.end(); ++it)
	{
		out << "<circle cx=\""<< (*it)[0];
		out << "\" cy=\""<< (*it)[1];
		out << "\" r=\""<< m_pointSize;
		out << "\" style=\"stroke: none; fill: #";

		out << std::hex;
		unsigned int wp = out.width(2);
		char prev = out.fill('0');
		out << int(m_color[0]*255);
		out.width(2); out.fill('0');
		out<< int(m_color[1]*255);
		out.width(2); out.fill('0');
		out << int(m_color[2]*255)<<std::dec;
		out.fill(prev);
		out.width(wp);

		out <<"\"/>"<< std::endl;
	}

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

void SvgPolygon::setColorFill(const Geom::Vec3f& c)
{
	m_colorFill=c;
}


void SvgPolygon::save(std::ofstream& out)
{
	std::stringstream ss;

	out << "<polyline fill=\"";
	out << std::hex;
	unsigned int wp = out.width(2);
	char prev = out.fill('0');
	out << int(m_colorFill[0]*255);
	out.width(2); out.fill('0');
	out<< int(m_colorFill[1]*255);
	out.width(2); out.fill('0');
	out << int(m_colorFill[2]*255);

	out << "none\" stroke=\"#";
	wp = out.width(2);
	prev = out.fill('0');
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
//	compNormObj cmp;
//	std::sort(m_objs.begin(),m_objs.end(),cmp);

	std::cout << "CLOSE"<< std::endl;
//	std::list<SvgObj*> primitives;

	for (std::vector<SvgObj*>::iterator it = m_objs.begin(); it != m_objs.end(); ++it)
	{
		(*it)->save(*m_out);
	}

	*m_out << "</g>" << std::endl;
	*m_out << "</svg>" << std::endl;
	m_out->close();
}



// all points behind the plane +1
// all points before the plane -1
// all points colinear to the plane 0
// undefined 999
int compSvgObj::points_plane (SvgPolygon* pol_points, SvgPolygon* pol_plane, float& averageZ)
{
	Geom::Vec3f N = pol_plane->normal();

	if (N[2] > 0.0f)
		N = -1.0f*N;

	unsigned int nb = pol_points->nbv();
	unsigned int nbback=0;
	unsigned int nbfront=0;
	unsigned int nb_col=0;
	averageZ=0.0f;
	for (unsigned int i=0; i< nb; ++i)
	{
		const Geom::Vec3f& Q = pol_points->P(i);
		averageZ += Q[2];
		Geom::Vec3f U = Q - pol_plane->P(0);

		float ps = U*N;

		if (fabs(ps) < 0.0001f)
			nb_col++;
		else
		{
			if (ps <0)
				nbback++;
			else
				nbfront++;
		}
	}

	averageZ /= float(nb);

	if (nbfront==0)
		return 1;

	if (nbback==0)
		return -1;

	if (nb_col==nb)
		return 0;

	return 999;
}


bool compSvgObj::operator() (SvgObj* a, SvgObj*b)
{
	SvgPolygon* p_a = dynamic_cast<SvgPolygon*>(a);
	SvgPolygon* p_b = dynamic_cast<SvgPolygon*>(b);

	if ((p_a!= NULL) && (p_b!=NULL)) // first case polygon/polygon
	{
		float avz_a;
		int  t1 = points_plane(p_a,p_b,avz_a);

		if (t1==0) // colinear choose farthest
		{
			float za = p_a->P(0)[2];
			float zb = p_b->P(0)[2];
			return za > zb;
		}

		float avz_b;
		int  t2 = points_plane(p_b,p_a,avz_b);

		// all point of a behind b
		if ((t1 == 1)&&(t2==999))
			return true;

		// all point of b infront of a
		if ((t2 == -1) && (t1==999))
			return true;

		if ((t1 == t2 )&& (t2!=999))
		{
			return avz_a > avz_b;
		}

		// all other cases ??
		return false;
	}

	std::cout << "Cas non traite !!"<< std::endl;
	return false;
}



bool compNormObj::operator() (SvgObj* a, SvgObj*b)
{
	SvgPolygon* p_a = dynamic_cast<SvgPolygon*>(a);
	SvgPolygon* p_b = dynamic_cast<SvgPolygon*>(b);


	if ((p_a!= NULL) && (p_b!=NULL)) // first case polygon/polygon
	{
		Geom::Vec3f Na = p_a->normal();
		Geom::Vec3f Nb = p_b->normal();
		return fabs(Na[2]) > fabs(Nb[2]);
	}

	if ((p_a!= NULL)) // second case polygon/other
	{
		return true;	// all polygon before segments.
	}

	std::cout << "Cas non traite !!"<< std::endl;
	return false;
}




} // namespace SVG
} // namespace Render
} // namespace Algo
} // namespace CGoGN


