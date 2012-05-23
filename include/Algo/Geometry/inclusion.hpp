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

#include "Algo/Geometry/intersection.h"
#include "Algo/Geometry/orientation.h"
#include "Algo/Geometry/basic.h"
#include "Algo/Geometry/plane.h"

#include <limits>

namespace CGoGN
{

namespace Algo
{

namespace Geometry
{

template <typename PFP>
bool isConvex(typename PFP::MAP& map, Dart d, const AttributeHandler<typename PFP::VEC3, VERTEX>& position, bool CCW, unsigned int thread)
{
	//get all the dart of the volume
	std::vector<Dart> vStore;
	FunctorStore fs(vStore);
	map.foreach_dart_of_volume(d, fs, thread);

	bool convex = true;

	DartMarkerStore m(map, thread);
	for (std::vector<Dart>::iterator it = vStore.begin() ; it != vStore.end() && convex ; ++it)
	{
		Dart e = *it;
		if (!m.isMarked(e))
		{
			m.markOrbit<EDGE>(e) ;
			convex = isTetrahedronWellOriented<PFP>(map, e, position, CCW) ;
		}
	}

	return convex;
}

template <typename PFP>
bool isPointInVolume(typename PFP::MAP& map, Dart d, const AttributeHandler<typename PFP::VEC3, VERTEX>& position, const typename PFP::VEC3& point)
{
	typedef typename PFP::VEC3 VEC3;

	//number of intersection between a ray and the volume must be odd
	int countInter = 0;

// 	if(isConvex<PFP>(map,d)) {
// 		CGoGNout << "optimize point in volume" << CGoGNendl;
// 	}

	std::list<Dart> visitedFaces;			// Faces that are traversed
	visitedFaces.push_back(d);				// Start with the face of d
	std::list<Dart>::iterator face;
	VEC3 dir(0.5f,0.5f,0.5f);
	VEC3 inter;
	std::vector<VEC3> interPrec;

	DartMarkerStore mark(map);					// Lock a marker
	// For every face added to the list
	// search the number of faces intersected by a ray whose origin is the tested point
	for (face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		if (!mark.isMarked(*face))			// Face has not been visited yet
		{
			bool alreadyfound = false;
			bool interRes = Algo::Geometry::intersectionLineConvexFace<PFP>(map, *face, position, point, dir, inter);
			if(interRes && (dir * (inter-point)) >= 0.0f)
			{
				if(interPrec.size() > 0)
				{
					for(typename std::vector<VEC3>::iterator it = interPrec.begin(); !alreadyfound && it != interPrec.end(); ++it)
					{
						if((*it)[0] == inter[0])
							alreadyfound = true;
					}
				}
				if(!alreadyfound)
				{
					++countInter;
					interPrec.push_back(inter);
				}

				// add non visited adjacent faces to the list of face
				Dart dNext = *face ;
				do
				{
					mark.mark(dNext);					// Mark
					Dart adj = map.phi2(dNext);			// Get adjacent face
					if (adj != dNext && !mark.isMarked(adj))
						visitedFaces.push_back(adj);	// Add it
					dNext = map.phi1(dNext) ;
				} while(dNext != *face) ;
			}
		}
	}

	//if the point is in the volume there is an odd number of intersection with all faces with any direction
	return (countInter % 2) == 1;
}

template <typename PFP>
bool isPointInConvexVolume(typename PFP::MAP& map, Dart d, const AttributeHandler<typename PFP::VEC3, VERTEX>& position, const typename PFP::VEC3& point, bool CCW)
{
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL;

	bool inside = true;
	std::list<Dart> visitedFaces;			// Faces that are traversed
	visitedFaces.push_back(d);				// Start with the face of d
	std::list<Dart>::iterator face;
	VEC3 N;

	DartMarkerStore mark(map);					// Lock a marker
	for (face = visitedFaces.begin(); inside && face != visitedFaces.end(); ++face)
	{
		if (!mark.isMarked(*face))
		{
			Geom::Plane3D<REAL> p = Geometry::facePlane<PFP>(map, *face, position);
			Geom::Orientation3D o3d = p.orient(point);
			if(CCW)
			{
				if(o3d == Geom::OVER)
					inside = false;
			}
			else if(o3d == Geom::UNDER)
				inside = false;

			Dart dNext = *face ;
			do
			{
				mark.mark(dNext);					// Mark
				Dart adj = map.phi2(dNext);			// Get adjacent face
				if (adj != dNext && !mark.isMarked(adj))
					visitedFaces.push_back(adj);	// Add it
				dNext = map.phi1(dNext) ;
			} while(dNext != *face) ;
		}
	}

	//if the point is in the volume there is an odd number of intersection with all faces with any direction
	return inside;
}

template <typename PFP>
bool isPointInConvexFace(typename PFP::MAP& map, Dart d, const AttributeHandler<typename PFP::VEC3, VERTEX>& position, const typename PFP::VEC3& point, bool CCW)
{
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL;

	bool inside = true;

	Geom::Plane3D<REAL> pl = Geometry::facePlane<PFP>(map, d, position);
	Geom::Orientation3D o3d = pl.orient(point);
	if(o3d == Geom::ON)
	{
		Traversor2FV<typename PFP::MAP> tfv(map, d) ;
		for(Dart it = tfv.begin(); it != tfv.end(); it = tfv.next())
		{
			VEC3 N = pl.normal();
			VEC3 v2(position[map.phi1(it)] - position[it]);
			VEC3 norm2 = N^v2;
			Geom::Plane3D<REAL> pl2(norm2, position[it]);
			o3d = pl2.orient(point);
			if(CCW)
			{
				if(o3d == Geom::UNDER)
					inside = false;
			}
			else if(o3d == Geom::OVER)
				inside = false;
		}
		return inside;
	}

	return false;
}

template <typename PFP>
bool isPointInConvexFace2D(typename PFP::MAP& map, Dart d, const AttributeHandler<typename PFP::VEC3, VERTEX>& position, const typename PFP::VEC3& point, bool CCW )
{
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL;

// 	CGoGNout << "point " << point << "d " << d << "faceDeg " << map.faceDegree(d) << CGoGNendl;

	bool convex=true;
	Geom::Orientation2D o2d;

	Traversor2FV<typename PFP::MAP> tfv(map, d) ;
	for(Dart it = tfv.begin(); it != tfv.end(); it = tfv.next())
	{
		o2d = Geom::testOrientation2D(point, position[it], position[map.phi1(it)]);
		if(CCW)
		{
			if(o2d == Geom::RIGHT)
				convex= false;
		}
		else if(o2d == Geom::LEFT)
			return false;
	}

	return convex;
}

template <typename PFP>
bool isPointOnEdge(typename PFP::MAP& map, Dart d, const AttributeHandler<typename PFP::VEC3, VERTEX>& position, const typename PFP::VEC3& point)
{
// 	typedef typename PFP::REAL REAL;
// 	typedef typename PFP::VEC3 VEC3 ;
// 
// 	VEC3 v1 = Algo::Geometry::vectorOutOfDart<PFP>(map, d, positions);
// 	VEC3 v2(point - positions[d]);
// 
// 	v1.normalize();
// 	v2.normalize();
// 
// 	return fabs(REAL(1) - (v1*v2)) < std::numeric_limits<REAL>::min();

	if(
		( Algo::Geometry::isPointOnHalfEdge<PFP>(map,d,position,point) && Algo::Geometry::isPointOnHalfEdge<PFP>(map,map.phi2(d),position,point) ) ||
		Algo::Geometry::isPointOnVertex<PFP>(map,d,position,point) ||
		Algo::Geometry::isPointOnVertex<PFP>(map,map.phi1(d),position,point)
	)
		return true;
	else
	{
		CGoGNout << " point " << point << CGoGNendl;
		CGoGNout << " d1 " << position[d] << " d2 " << position[map.phi2(d)] << CGoGNendl;
		return false;
	}
}

template <typename PFP>
bool isPointOnHalfEdge(typename PFP::MAP& map, Dart d, const AttributeHandler<typename PFP::VEC3, VERTEX>& position, const typename PFP::VEC3& point)
{
	typedef typename PFP::REAL REAL;
	typedef typename PFP::VEC3 VEC3;

	VEC3 v1 = Algo::Geometry::vectorOutOfDart<PFP>(map, d, position);
	VEC3 v2(point - position[d]);

	v1.normalize();
	v2.normalize();

	return (v1*v2) >= -REAL(0.00001);
}

template <typename PFP>
bool isPointOnVertex(typename PFP::MAP& map, Dart d, const AttributeHandler<typename PFP::VEC3, VERTEX>& position, const typename PFP::VEC3& point)
{
	return Geom::arePointsEquals(point, position[d]);
}

template <typename PFP>
bool isConvexFaceInOrIntersectingTetrahedron(typename PFP::MAP& map, Dart d, const AttributeHandler<typename PFP::VEC3, VERTEX>& position, const typename PFP::VEC3 points[4], bool CCW)
{
	typedef typename PFP::VEC3 VEC3 ;

	Traversor2FV<typename PFP::MAP> tfv(map, d) ;
	for(Dart it = tfv.begin(); it != tfv.end(); it = tfv.next())
	{
		if(Geom::isPointInTetrahedron(points, position[it], CCW))
			return true;
	}

	VEC3 inter;
	if( intersectionSegmentConvexFace(map, d, position, points[0], points[1], inter)
	|| 	intersectionSegmentConvexFace(map, d, position, points[1], points[2], inter)
	|| 	intersectionSegmentConvexFace(map, d, position, points[2], points[0], inter)
	|| 	intersectionSegmentConvexFace(map, d, position, points[0], points[3], inter)
	|| 	intersectionSegmentConvexFace(map, d, position, points[1], points[3], inter)
	|| 	intersectionSegmentConvexFace(map, d, position, points[2], points[3], inter)
	)
		return true;

	return false;
}

} // namespace Geometry

} // namspace Algo

} // namespace CGoGN
