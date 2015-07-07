#include <iostream>
#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"

#include "Algo/Geometry/distances.h"

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
	typedef EmbeddedMap3 MAP;
};



template PFP1::REAL Algo::Geometry::squaredDistancePoint2FacePlane<PFP1>(PFP1::MAP& map, Face f, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, const PFP1::VEC3& P);
template PFP1::REAL Algo::Geometry::squaredDistancePoint2Face<PFP1>(PFP1::MAP& map, Face f, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, const PFP1::VEC3& P);
template PFP1::REAL Algo::Geometry::squaredDistancePoint2Edge<PFP1>(PFP1::MAP& map, Edge e, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, const PFP1::VEC3& P);

template PFP2::REAL Algo::Geometry::squaredDistancePoint2FacePlane<PFP2>(PFP2::MAP& map, Face f, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, const PFP2::VEC3& P);
template PFP2::REAL Algo::Geometry::squaredDistancePoint2Face<PFP2>(PFP2::MAP& map, Face f, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, const PFP2::VEC3& P);
template PFP2::REAL Algo::Geometry::squaredDistancePoint2Edge<PFP2>(PFP2::MAP& map, Edge e, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, const PFP2::VEC3& P);

template PFP3::REAL Algo::Geometry::squaredDistancePoint2FacePlane<PFP3>(PFP3::MAP& map, Face f, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, const PFP3::VEC3& P);
template PFP3::REAL Algo::Geometry::squaredDistancePoint2Face<PFP3>(PFP3::MAP& map, Face f, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, const PFP3::VEC3& P);
template PFP3::REAL Algo::Geometry::squaredDistancePoint2Edge<PFP3>(PFP3::MAP& map, Edge e, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, const PFP3::VEC3& P);



int test_distances()
{
	return 0;
}