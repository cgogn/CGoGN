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


#include "GL/glew.h"
#include "Algo/Geometry/centroid.h"

namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace SVG
{


template <typename PFP>
void SVGOut::renderPointsToSVG(typename PFP::MAP& map, const typename PFP::TVEC3& position, const FunctorSelect& good, unsigned int thread)
{
	SvgPoints* points = new SvgPoints();
	points->setColor(global_color);
	points->setWidth(global_width);

	TraversorV<typename PFP::MAP> trav(map,good,thread);

	for(Dart d = trav.begin(); d != trav.end(); d=trav.next())
	{
		const Geom::Vec3f& P = position[d];
		glm::vec3 Q = glm::project(glm::vec3(P[0],P[1],P[2]),m_model,m_proj,m_viewport);
		glm::vec3 R = glm::project(glm::vec3(P[0],P[1],P[2]),m_model,glm::mat4(1.0),m_viewport);
		points->addVertex(Geom::Vec3f(Q[0],float(m_viewport[3])-Q[1],Q[2]));
	}
	m_objs.push_back(points);
}





template <typename PFP>
void SVGOut::renderLinesToSVG(typename PFP::MAP& map, const typename PFP::TVEC3& position, const FunctorSelect& good, unsigned int thread)
{
	TraversorE<typename PFP::MAP> trav(map,good,thread);

	for(Dart d = trav.begin(); d != trav.end(); d=trav.next())
	{
		const Geom::Vec3f& P = position[d];
		glm::vec3 Q = glm::project(glm::vec3(P[0],P[1],P[2]),m_model,m_proj,m_viewport);
		glm::vec3 R = glm::project(glm::vec3(P[0],P[1],P[2]),m_model,glm::mat4(1.0),m_viewport);

		const Geom::Vec3f& P2 = position[map.phi1(d)];
		glm::vec3 Q2 = glm::project(glm::vec3(P2[0],P2[1],P2[2]),m_model,m_proj,m_viewport);
		glm::vec3 R2 = glm::project(glm::vec3(P2[0],P2[1],P2[2]),m_model,glm::mat4(1.0),m_viewport);

		SvgPolyline* pol = new SvgPolyline();
		pol->addVertex(Geom::Vec3f(Q[0],float(m_viewport[3])-Q[1],Q[2]));
		pol->addVertex(Geom::Vec3f(Q2[0],float(m_viewport[3])-Q2[1],Q2[2]));

		pol->addVertex3D(Geom::Vec3f(R[0],float(m_viewport[3])-R[1],R[2]));
		pol->addVertex3D(Geom::Vec3f(R2[0],float(m_viewport[3])-R2[1],R2[2]));

		pol->setColor(global_color);
		pol->setWidth(global_width);
		m_objs.push_back(pol);
	}
}


template <typename PFP>
void SVGOut::renderFacesToSVG(typename PFP::MAP& map, const typename PFP::TVEC3& position, float shrink, bool cull, const FunctorSelect& good, unsigned int thread)
{
	TraversorF<typename PFP::MAP> trav(map,good,thread);

	for(Dart d = trav.begin(); d != trav.end(); d=trav.next())
	{
		bool cullFace=false;
		if (cull)
		{
			const Geom::Vec3f& P = position[d];

			glm::vec3 Q = glm::project(glm::vec3(P[0],P[1],P[2]),m_model,m_proj,m_viewport);
			const Geom::Vec3f& P2 = position[map.phi1(d)];
			glm::vec3 R = glm::project(glm::vec3(P2[0],P2[1],P2[2]),m_model,m_proj,m_viewport);
			const Geom::Vec3f& P3 = position[map.phi1(map.phi1(d))];
			glm::vec3 S = glm::project(glm::vec3(P3[0],P3[1],P3[2]),m_model,m_proj,m_viewport);
			glm::vec3 N = glm::cross(S-R,Q-R);
			if (N[2]<0.0f)
				cullFace=true;
		}

		if (!cullFace)
		{
			typename PFP::VEC3 center = Algo::Geometry::faceCentroid<PFP>(map,d,position);
			SvgPolygon* pol = new SvgPolygon();
			Dart dd = d;
			do
			{
				Geom::Vec3f P = position[d];
				P = P*shrink + center*(1.0f-shrink);
				glm::vec3 Q = glm::project(glm::vec3(P[0],P[1],P[2]),m_model,m_proj,m_viewport);
				glm::vec3 R = glm::project(glm::vec3(P[0],P[1],P[2]),m_model,glm::mat4(1.0),m_viewport);
				pol->addVertex(Geom::Vec3f(Q[0],float(m_viewport[3])-Q[1],Q[2]));
				pol->addVertex3D(Geom::Vec3f(R[0],R[1],R[2]));
				d = map.phi1(d);
			}while (d!=dd);

			pol->close();
			pol->setColor(global_color);
			pol->setWidth(global_width);
			m_objs.push_back(pol);
		}
	}

}


} // namespace SVG
} // namespace Render
} // namespace Algo
} // namespace CGoGN
