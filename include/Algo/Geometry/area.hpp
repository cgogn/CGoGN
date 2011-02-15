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

#include "Geometry/basic.h"
#include "Algo/Geometry/centroid.h"

namespace CGoGN
{

namespace Algo
{

namespace Geometry
{

template <typename PFP>
typename PFP::REAL triangleArea(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position)
{
	typename PFP::VEC3 p1 = position[d];
	typename PFP::VEC3 p2 = position[map.phi1(d)];
	typename PFP::VEC3 p3 = position[map.phi_1(d)];

	return Geom::triangleArea(p1, p2, p3) ;
}

template <typename PFP>
typename PFP::REAL convexFaceArea(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position)
{
	typedef typename PFP::VEC3 VEC3;

	if(map.isFaceTriangle(d))
		return triangleArea<PFP>(map, d, position) ;
	else
	{
		float area = 0.0f ;
		VEC3 centroid = Algo::Geometry::faceCentroid<PFP>(map, d, position) ;
		Dart it = d ;
		do
		{
			VEC3 p1 = position[it];
			VEC3 p2 = position[map.phi1(it)];
			area += Geom::triangleArea(p1, p2, centroid) ;
			it = map.phi1(it) ;
		} while (it != d) ;
		return area ;
	}
}

template <typename PFP>
typename PFP::REAL totalArea(typename PFP::MAP& map, const typename PFP::TVEC3& position, const FunctorSelect& select, unsigned int th)
{
	float area = 0.0f ;
	DartMarker mark(map,th) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !mark.isMarked(d))
		{
			mark.markOrbit(FACE_ORBIT, d) ;
			area += convexFaceArea<PFP>(map, d, position) ;
		}
	}
	return area ;
}

template <typename PFP>
void computeAreaFaces(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TREAL& face_area, const FunctorSelect& select)
{
	CellMarker marker(map, FACE_CELL);
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !marker.isMarked(d))
		{
			marker.mark(d);
			face_area[d] = convexFaceArea<PFP>(map, d, position) ;
		}
	}
}

} // namespace Geometry

} // namespace Algo

} // namespace CGoGN
