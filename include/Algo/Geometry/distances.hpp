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

#include "Geometry/distances.h"
#include "Algo/Geometry/normal.h"
#include <limits>

namespace CGoGN
{

namespace Algo
{

namespace Geometry
{

template <typename PFP>
bool isPlanar(typename PFP::MAP& map, Dart d, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP::IMPL>& position)
{
	if (map.phi<111>(d)==d)
		return true;

	typename PFP::VEC3 No = Algo::Surface::Geometry::triangleNormal(map, d, position) ;

	Dart e = map.phi<11>(d);
	while (e != d)
	{
		typename PFP::VEC3 n = Algo::Surface::Geometry::triangleNormal(map, e, position) ;
		e = map.phi1(e);
		if (e!=d)
			e = map.phi1(e);
	}
}

template <typename PFP>
typename PFP::REAL squaredDistancePoint2FacePlane(typename PFP::MAP& map, Dart d, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP::IMPL>& position, const VEC3& P)
{
	const typename PFP::VEC3& A = position[d];
	d = map.phi1(d);
	const typename PFP::VEC3& B = position[d];
	d = map.phi1(d);
	const typename PFP::VEC3& C = position[d];
	return Geom::squaredDistancePoint2TrianglePlane(P,A,B,C);
}

template <typename PFP>
typename PFP::REAL squaredDistancePoint2Face(typename PFP::MAP& map, Dart d, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP::IMPL>& position, const VEC3& P)
{
	typedef typename PFP::REAL REAL;
	const typename PFP::VEC3& A = position[d];

	Dart e = map.phi1(d);
	Dart f = map.phi1(e);
	REAL dist2 = Geom::squaredDistancePoint2Triangle(P,A,position[e],position[f]);
	e = f;
	f = map.phi1(e);

	while (f != d)
	{
		REAL d2 = Geom::squaredDistancePoint2Triangle(P,A,position[e],position[f]);
		if (d2 < dist2)
			dist2 = d2;
		e = f;
		f = map.phi1(e);
	}
	return dist2;
}

template <typename PFP>
typename PFP::REAL squaredDistancePoint2Edge(typename PFP::MAP& map, Dart d, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP::IMPL>& position, const VEC3& P)
{
	const typename PFP::VEC3& A = position[d];
	typename PFP::VEC3& AB = position[map.phi1(d)]-A;
	typename PFP::REAL AB2 = AB*AB;
	return Geom::squaredDistanceSeg2Point(A,AB,AB2,P) ;
}

} // namespace Geometry

} // namespace Algo

} // namespace CGoGN
