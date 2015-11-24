#include <iostream>
#include "Topology/generic/parameters.h"
#include "Topology/gmap/embeddedGMap2.h"
#include "Topology/map/embeddedMap2.h"

#include "Algo/Geometry/basic.h"


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


using namespace CGoGN;


/*****************************************
*		BASIC INSTANTIATION
*****************************************/


template PFP1::VEC3 Algo::Geometry::vectorOutOfDart<PFP1>(PFP1::MAP& map, Dart d, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template PFP1::REAL  Algo::Geometry::edgeLength<PFP1>(PFP1::MAP& map, Dart d, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template PFP1::REAL  Algo::Geometry::meanEdgeLength<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template PFP1::REAL  Algo::Geometry::angle<PFP1>(PFP1::MAP& map, Dart d1, Dart d2, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template bool  Algo::Geometry::isTriangleObtuse<PFP1>(PFP1::MAP& map, Dart d, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);

template PFP2::VEC3 Algo::Geometry::vectorOutOfDart<PFP2>(PFP2::MAP& map, Dart d, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template PFP2::REAL  Algo::Geometry::edgeLength<PFP2>(PFP2::MAP& map, Dart d, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template PFP2::REAL  Algo::Geometry::meanEdgeLength<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template PFP2::REAL  Algo::Geometry::angle<PFP2>(PFP2::MAP& map, Dart d1, Dart d2, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template bool  Algo::Geometry::isTriangleObtuse<PFP2>(PFP2::MAP& map, Dart d, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);

template PFP3::VEC3 Algo::Geometry::vectorOutOfDart<PFP3>(PFP3::MAP& map, Dart d, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
template PFP3::REAL  Algo::Geometry::edgeLength<PFP3>(PFP3::MAP& map, Dart d, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
template PFP3::REAL  Algo::Geometry::meanEdgeLength<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
template PFP3::REAL  Algo::Geometry::angle<PFP3>(PFP3::MAP& map, Dart d1, Dart d2, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
template bool  Algo::Geometry::isTriangleObtuse<PFP3>(PFP3::MAP& map, Dart d, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);


int test_basic()
{
	return 0;
}
