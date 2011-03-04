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

#include "Algo/Geometry/basic.h"
#include "Algo/Geometry/area.h"
#include <cmath>

namespace CGoGN
{

namespace Algo
{

namespace Geometry
{

template <typename PFP>
typename PFP::VEC3 triangleNormal(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position)
{
	typedef typename PFP::VEC3 VEC3 ;

	const VEC3& p1 = position[d];
	const VEC3& p2 = position[map.phi1(d)];
	const VEC3& p3 = position[map.phi_1(d)];

	VEC3 N = Geom::triangleNormal(p1, p2, p3) ;
	N.normalize() ;

	return N ;
}

template <typename PFP>
typename PFP::VEC3 faceNormal(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position)
{
	typedef typename PFP::VEC3 VEC3 ;

	if(map.isFaceTriangle(d))
		return triangleNormal<PFP>(map, d, position) ;
	else
	{
		VEC3 N(0) ;
		Dart it = d ;
		do
		{
			VEC3 n = triangleNormal<PFP>(map, it, position) ;
			//if(!std::isnan(n[0]))
			if (n[0] == n[0])
				N += n ;
			it = map.phi1(it) ;
		} while (it != d) ;
		N.normalize() ;
		return N ;
	}
}

template <typename PFP>
typename PFP::VEC3 vertexNormal(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position)
{
	typedef typename PFP::VEC3 VEC3 ;

	VEC3 N(0) ;
	Dart it = d ;
	do
	{
		VEC3 n = faceNormal<PFP>(map, it, position) ;
		if(!n.hasNan())
		{
			VEC3 v1 = vectorOutOfDart<PFP>(map, it, position) ;
			VEC3 v2 = vectorOutOfDart<PFP>(map, map.phi_1(it), position) ;
			n *= convexFaceArea<PFP>(map, it, position) / (v1.norm2() * v2.norm2()) ;
			N += n ;
		}
		it = map.phi1(map.phi2(it)) ;
	} while (it != d) ;
	N.normalize() ;
	return N ;
}

template <typename PFP>
void computeNormalFaces(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TVEC3& face_normal, const FunctorSelect& select, unsigned int thread)
{
	CellMarker marker(map, FACE_CELL,thread);
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !marker.isMarked(d))
		{
			marker.mark(d);
			face_normal[d] = faceNormal<PFP>(map, d, position) ;
		}
	}
}

template <typename PFP>
class computeNormalVerticesFunctor: public FunctorMap<typename PFP::MAP>
{
protected:
	typename PFP::MAP& m_map;
	const typename PFP::TVEC3& m_position;
	typename PFP::TVEC3& m_normal;
public:
	computeNormalVerticesFunctor(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TVEC3& normal):
		m_map(map), m_position(position), m_normal(normal) {}

	bool operator()(Dart d)
	{
		m_normal[d] = vertexNormal<PFP>(m_map, d, m_position) ;
		return false;
	}
};



template <typename PFP>
void computeNormalVertices(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TVEC3& normal, const FunctorSelect& select, unsigned int thread)
{
	CellMarker marker(map, VERTEX_CELL,thread);
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !marker.isMarked(d))
		{
			marker.mark(d);
			normal[d] = vertexNormal<PFP>(map, d, position) ;
		}
	}
}

} // namespace Geometry

} // namespace Algo

} // namespace CGoGN
