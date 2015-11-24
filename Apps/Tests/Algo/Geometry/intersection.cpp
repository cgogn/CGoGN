#include <iostream>
#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"

#include "Algo/Geometry/intersection.h"

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


template bool Algo::Surface::Geometry::intersectionLineConvexFace<PFP1>(PFP1::MAP& map, Face f, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, const PFP1::VEC3& P, const PFP1::VEC3& Dir, PFP1::VEC3& Inter);
template bool Algo::Surface::Geometry::intersectionSegmentConvexFace<PFP1>(PFP1::MAP& map, Face f, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, const PFP1::VEC3& PA, const PFP1::VEC3& PB, PFP1::VEC3& Inter);
template bool Algo::Surface::Geometry::areTrianglesInIntersection<PFP1>(PFP1::MAP& map, Face t1, Face t2, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template bool Algo::Surface::Geometry::intersectionSphereEdge<PFP1>(PFP1::MAP& map, const PFP1::VEC3& center, PFP1::REAL radius, Edge e, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, PFP1::REAL& alpha);

// double

template bool Algo::Surface::Geometry::intersectionLineConvexFace<PFP2>(PFP2::MAP& map, Face f, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, const PFP2::VEC3& P, const PFP2::VEC3& Dir, PFP2::VEC3& Inter);
template bool Algo::Surface::Geometry::intersectionSegmentConvexFace<PFP2>(PFP2::MAP& map, Face f, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, const PFP2::VEC3& PA, const PFP2::VEC3& PB, PFP2::VEC3& Inter);
template bool Algo::Surface::Geometry::areTrianglesInIntersection<PFP2>(PFP2::MAP& map, Face t1, Face t2, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template bool Algo::Surface::Geometry::intersectionSphereEdge<PFP2>(PFP2::MAP& map, const PFP2::VEC3& center, PFP2::REAL radius, Edge e, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, PFP2::REAL& alpha);

// GMap

template bool Algo::Surface::Geometry::intersectionLineConvexFace<PFP3>(PFP3::MAP& map, Face f, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, const PFP3::VEC3& P, const PFP3::VEC3& Dir, PFP3::VEC3& Inter);
template bool Algo::Surface::Geometry::intersectionSegmentConvexFace<PFP3>(PFP3::MAP& map, Face f, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, const PFP3::VEC3& PA, const PFP3::VEC3& PB, PFP3::VEC3& Inter);
template bool Algo::Surface::Geometry::areTrianglesInIntersection<PFP3>(PFP3::MAP& map, Face t1, Face t2, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
template bool Algo::Surface::Geometry::intersectionSphereEdge<PFP3>(PFP3::MAP& map, const PFP3::VEC3& center, PFP3::REAL radius, Edge e, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, PFP3::REAL& alpha);



int test_intersection()
{
	return 0;
}