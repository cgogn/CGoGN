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
bool isConvex(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& positions, bool CCW, unsigned int thread)
{
	//get all the dart of the volume
	std::vector<Dart> vStore;
	FunctorStore fs(vStore);
	map.foreach_dart_of_volume(d, fs,thread);

	bool convex = true;

	DartMarkerStore m(map);
	for (std::vector<Dart>::iterator it = vStore.begin() ; it != vStore.end() && convex ; ++it)
	{
		Dart e = *it;
		if (!m.isMarked(e))
		{
			m.markOrbit(EDGE_ORBIT, e) ;
			convex = isTetrahedronWellOriented<PFP>(map, e, positions, CCW) ;
		}
	}

	return convex;
}

template <typename PFP>
bool isPointInVolume(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& positions, const typename PFP::VEC3& point)
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
			bool interRes = Algo::Geometry::intersectionLineConvexFace<PFP>(map, *face, positions, point, dir, inter);
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
bool isPointInConvexVolume(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& positions, const typename PFP::VEC3& point, bool CCW)
{
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL;

	bool inside = true;
	std::list<Dart> visitedFaces;			// Faces that are traversed
	visitedFaces.push_back(d);				// Start with the face of d
	std::list<Dart>::iterator face;
	VEC3 N;
	float offset;

	DartMarkerStore mark(map);					// Lock a marker
	for (face = visitedFaces.begin(); inside && face != visitedFaces.end(); ++face)
	{
		if (!mark.isMarked(*face))
		{
			Geom::Plane3D<REAL> p = Geometry::facePlane<PFP>(map, *face, positions);
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
bool isPointInConvexFace(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& positions, const typename PFP::VEC3& point, bool CCW )
{
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL;

	bool inside = true;

	Geom::Plane3D<REAL> pl = Geometry::facePlane<PFP>(map,d,positions);
	Geom::Orientation3D o3d = pl.orient(point);
	if(o3d == Geom::ON)
	{
		Dart dd =d;
		do
		{
			VEC3 N = pl.normal();
			VEC3 v2(positions[map.phi1(dd)]-positions[dd]);
			VEC3 norm2 = N^v2;
			Geom::Plane3D<REAL> pl2(norm2,positions[dd]);
			o3d = pl2.orient(point);
			if(CCW)
			{
				if(o3d == Geom::UNDER)
					inside = false;
			}
			else if(o3d == Geom::OVER)
				inside =false;

			dd = map.phi1(dd);
		} while(dd != d);

		return inside;
	}

	return false;
}

template <typename PFP>
bool isPointInConvexFace2D(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& positions, const typename PFP::VEC3& point, bool CCW )
{
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL;

// 	CGoGNout << "point " << point << "d " << d << "faceDeg " << map.faceDegree(d) << CGoGNendl;

	bool convex=true;
	Geom::Orientation2D o2d;
	Dart dd = d;
	do
	{
// 		CGoGNout << "dd " << dd << " ";
		o2d = Geom::testOrientation2D(point, positions[dd], positions[map.phi1(dd)]);
		if(CCW)
		{
			if(o2d == Geom::RIGHT)
				convex= false;
		}
		else if(o2d == Geom::LEFT)
			return false;

		dd = map.phi1(dd);
	} while(dd != d);

	return convex;
}

template <typename PFP>
bool isPointOnEdge(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& positions, const typename PFP::VEC3& point)
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

	if((Algo::Geometry::isPointOnHalfEdge<PFP>(map,d,positions,point) && Algo::Geometry::isPointOnHalfEdge<PFP>(map,map.phi2(d),positions,point)) || Algo::Geometry::isPointOnVertex<PFP>(map,d,positions,point) || Algo::Geometry::isPointOnVertex<PFP>(map,map.phi1(d),positions,point))
		return true;
	else {
		CGoGNout << " point " << point << CGoGNendl;
		CGoGNout << " d1 " << positions[d] << " d2 " << positions[map.phi2(d)] << CGoGNendl;
		return false;
	}
}

template <typename PFP>
bool isPointOnHalfEdge(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& positions, const typename PFP::VEC3& point)
{
	typedef typename PFP::REAL REAL;
	typedef typename PFP::VEC3 VEC3;

	VEC3 v1 = Algo::Geometry::vectorOutOfDart<PFP>(map,d,positions);
	VEC3 v2(point - positions[d]);

	v1.normalize();
	v2.normalize();

	return (v1*v2) >= -REAL(0.00001);
}

template <typename PFP>
bool isPointOnVertex(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& positions, const typename PFP::VEC3& point)
{
// 	typedef typename PFP::REAL REAL;
// 	typedef typename PFP::VEC3 VEC3 ;
// 
// 	VEC3 v(point - positions[d]);
// 
// 	return v.norm2() < std::numeric_limits<REAL>::min()*std::numeric_limits<REAL>::min();

	return Geom::arePointsEquals(point,positions[d]);
}

template <typename PFP>
bool isConvexFaceInOrIntersectingTetrahedron(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& positions, const typename PFP::VEC3 points[4], bool CCW)
{
	typedef typename PFP::VEC3 VEC3 ;

	Dart it = d;
	do
	{
		if(Geom::isPointInTetrahedron(points, positions[d], CCW))
			return true;
		it = map.phi1(it);
	} while(it!=d);

	VEC3 inter;
	if( intersectionSegmentConvexFace(map,d,positions,points[0],points[1],inter)
	|| 	intersectionSegmentConvexFace(map,d,positions,points[1],points[2],inter)
	|| 	intersectionSegmentConvexFace(map,d,positions,points[2],points[0],inter)
	|| 	intersectionSegmentConvexFace(map,d,positions,points[0],points[3],inter)
	|| 	intersectionSegmentConvexFace(map,d,positions,points[1],points[3],inter)
	|| 	intersectionSegmentConvexFace(map,d,positions,points[2],points[3],inter)
	)
		return true;

	return false;
}

}

}

}
