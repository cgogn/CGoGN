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

template<typename PFP>
typename PFP::VEC3 newellNormal(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position)
{
	Dart e = d;
	typename PFP::VEC3 normal(0);
	do
	{
		const typename PFP::VEC3& P = position[e];
		e = map.phi1(e);
		const typename PFP::VEC3& Q = position[e];
		normal[0] += (P[1] - Q[1]) * (P[2] + Q[2]);
		normal[1] += (P[2] - Q[2]) * (P[0] + Q[0]);
		normal[2] += (P[0] - Q[0]) * (P[1] + Q[1]);
	} while (e != d);

	normal.normalize();
	return normal;
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
			//if(!std::isnan(n[0]) && !std::isnan(n[1]) && !std::isnan(n[2]))
			if(!n.hasNan())
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
	CellMarker marker(map, FACE,thread);
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
void computeNormalVertices(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TVEC3& normal, const FunctorSelect& select, unsigned int thread)
{
	CellMarker marker(map, VERTEX, thread);
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !marker.isMarked(d))
		{
			marker.mark(d);
			normal[d] = vertexNormal<PFP>(map, d, position) ;
		}
	}
}

template <typename PFP>
typename PFP::REAL computeAngleBetweenNormalsOnEdge(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position)
{
	typedef typename PFP::VEC3 VEC3 ;

	Dart dd = map.phi2(d) ;
	if(dd == d)
		return 0 ;

	const VEC3 n1 = faceNormal<PFP>(map, d, position) ;
	const VEC3 n2 = faceNormal<PFP>(map, dd, position) ;
	VEC3 e = position[dd] - position[d] ;
	e.normalize() ;
	typename PFP::REAL s = e * (n1 ^ n2) ;
	typename PFP::REAL c = n1 * n2 ;
	typename PFP::REAL a(0) ;
	// the following trick is useful for avoiding NaNs (due to floating point errors)
	if (c > 0.5) a = asin(s) ;
	else
	{
		if(c < -1) c = -1 ;
		if (s >= 0) a = acos(c) ;
		else a = -acos(c) ;
	}
//	if (isnan(a))
	if(a != a)
		std::cerr<< "Warning : computeAngleBetweenNormalsOnEdge returns NaN on edge " << d << "-" << dd << std::endl ;
	return a ;
}

template <typename PFP>
void computeAnglesBetweenNormalsOnEdges(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TREAL& angles, const FunctorSelect& select, unsigned int thread)
{
	CellMarker me(map, EDGE, thread) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !me.isMarked(d))
		{
			me.mark(d) ;
			angles[d] = computeAngleBetweenNormalsOnEdge<PFP>(map, d, position) ;
		}
	}
}

} // namespace Geometry

} // namespace Algo

} // namespace CGoGN
