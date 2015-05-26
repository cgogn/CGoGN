#include <iostream>
#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"

#include "Algo/Geometry/inclusion.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

struct PFP3 : public PFP_STANDARD
{
	typedef EmbeddedGMap2 MAP;
};



template bool Algo::Surface::Geometry::isConvex<PFP1>(PFP1::MAP& map, Vol v, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, bool CCW);
template bool Algo::Surface::Geometry::isPointInVolume<PFP1>(PFP1::MAP& map, Vol v, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, const PFP1::VEC3& point);
template bool Algo::Surface::Geometry::isPointInConvexVolume<PFP1>(PFP1::MAP& map, Vol v, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, const PFP1::VEC3& point, bool CCW);
template bool Algo::Surface::Geometry::isPointInConvexFace<PFP1>(PFP1::MAP& map, Face f, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, const PFP1::VEC3& point, bool CCW);
template bool Algo::Surface::Geometry::isPointInConvexFace2D<PFP1>(PFP1::MAP& map, Face f, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, const PFP1::VEC3& point, bool CCW);
template bool Algo::Surface::Geometry::isPointOnEdge<PFP1>(PFP1::MAP& map, Edge e, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, const PFP1::VEC3& point);
template bool Algo::Surface::Geometry::isPointOnHalfEdge<PFP1>(PFP1::MAP& map, Dart d, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, const PFP1::VEC3& point);
template bool Algo::Surface::Geometry::isPointOnVertex<PFP1>(PFP1::MAP& map, Vertex v, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, const PFP1::VEC3& point);
template bool Algo::Surface::Geometry::isConvexFaceInOrIntersectingTetrahedron<PFP1>(PFP1::MAP& map, Face f, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, const PFP1::VEC3 points[4], bool CCW);

// Double

template bool Algo::Surface::Geometry::isConvex<PFP2>(PFP2::MAP& map, Vol v, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, bool CCW);
template bool Algo::Surface::Geometry::isPointInVolume<PFP2>(PFP2::MAP& map, Vol v, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, const PFP2::VEC3& point);
template bool Algo::Surface::Geometry::isPointInConvexVolume<PFP2>(PFP2::MAP& map, Vol v, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, const PFP2::VEC3& point, bool CCW );
template bool Algo::Surface::Geometry::isPointInConvexFace<PFP2>(PFP2::MAP& map, Face f, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, const PFP2::VEC3& point, bool CCW);
template bool Algo::Surface::Geometry::isPointInConvexFace2D<PFP2>(PFP2::MAP& map, Face f, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, const PFP2::VEC3& point, bool CCW );
template bool Algo::Surface::Geometry::isPointOnEdge<PFP2>(PFP2::MAP& map, Edge e, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, const PFP2::VEC3& point);
template bool Algo::Surface::Geometry::isPointOnHalfEdge<PFP2>(PFP2::MAP& map, Dart d, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, const PFP2::VEC3& point);
template bool Algo::Surface::Geometry::isPointOnVertex<PFP2>(PFP2::MAP& map, Vertex v, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, const PFP2::VEC3& point);
template bool Algo::Surface::Geometry::isConvexFaceInOrIntersectingTetrahedron<PFP2>(PFP2::MAP& map, Face f, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, const PFP2::VEC3 points[4], bool CCW);


//GMAP

template bool Algo::Surface::Geometry::isConvex<PFP3>(PFP3::MAP& map, Vol v, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, bool CCW);
template bool Algo::Surface::Geometry::isPointInVolume<PFP3>(PFP3::MAP& map, Vol v, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, const PFP3::VEC3& point);
template bool Algo::Surface::Geometry::isPointInConvexVolume<PFP3>(PFP3::MAP& map, Vol v, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, const PFP3::VEC3& point, bool CCW );
template bool Algo::Surface::Geometry::isPointInConvexFace<PFP3>(PFP3::MAP& map, Face f, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, const PFP3::VEC3& point, bool CCW);
template bool Algo::Surface::Geometry::isPointInConvexFace2D<PFP3>(PFP3::MAP& map, Face f, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, const PFP3::VEC3& point, bool CCW );
template bool Algo::Surface::Geometry::isPointOnEdge<PFP3>(PFP3::MAP& map, Edge e, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, const PFP3::VEC3& point);
template bool Algo::Surface::Geometry::isPointOnHalfEdge<PFP3>(PFP3::MAP& map, Dart d, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, const PFP3::VEC3& point);
template bool Algo::Surface::Geometry::isPointOnVertex<PFP3>(PFP3::MAP& map, Vertex v, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, const PFP3::VEC3& point);
template bool Algo::Surface::Geometry::isConvexFaceInOrIntersectingTetrahedron<PFP3>(PFP3::MAP& map, Face f, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, const PFP3::VEC3 points[4], bool CCW);


int test_inclusion()
{
	return 0;
}