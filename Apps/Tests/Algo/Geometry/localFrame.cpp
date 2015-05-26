#include <iostream>
#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"

#include "Algo/Geometry/localFrame.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};


template void Algo::Geometry::vertexLocalFrame<PFP1>(PFP1::MAP& map, Dart d, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, PFP1::VEC3& X, PFP1::VEC3& Y, PFP1::VEC3& Z);
template PFP1::MATRIX33 Algo::Geometry::vertexLocalFrame<PFP1>(PFP1::MAP& map, Dart d, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template void Algo::Geometry::vertexLocalFrame<PFP1>(PFP1::MAP& map, Dart d, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, PFP1::VEC3& normal, PFP1::VEC3& X, PFP1::VEC3& Y, PFP1::VEC3& Z);
template PFP1::MATRIX33 Algo::Geometry::vertexLocalFrame<PFP1>(PFP1::MAP& map, Dart d, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, PFP1::VEC3& normal);


struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};


template void Algo::Geometry::vertexLocalFrame<PFP2>(PFP2::MAP& map, Dart d, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, PFP2::VEC3& X, PFP2::VEC3& Y, PFP2::VEC3& Z);
template PFP2::MATRIX33 Algo::Geometry::vertexLocalFrame<PFP2>(PFP2::MAP& map, Dart d, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template void Algo::Geometry::vertexLocalFrame<PFP2>(PFP2::MAP& map, Dart d, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, PFP2::VEC3& normal, PFP2::VEC3& X, PFP2::VEC3& Y, PFP2::VEC3& Z);
template PFP2::MATRIX33 Algo::Geometry::vertexLocalFrame<PFP2>(PFP2::MAP& map, Dart d, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, PFP2::VEC3& normal);


struct PFP3 : public PFP_STANDARD
{
	typedef EmbeddedGMap2 MAP;
};


template void Algo::Geometry::vertexLocalFrame<PFP3>(PFP3::MAP& map, Dart d, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, PFP3::VEC3& X, PFP3::VEC3& Y, PFP3::VEC3& Z);
template PFP3::MATRIX33 Algo::Geometry::vertexLocalFrame<PFP3>(PFP3::MAP& map, Dart d, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
template void Algo::Geometry::vertexLocalFrame<PFP3>(PFP3::MAP& map, Dart d, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, PFP3::VEC3& normal, PFP3::VEC3& X, PFP3::VEC3& Y, PFP3::VEC3& Z);
template PFP3::MATRIX33 Algo::Geometry::vertexLocalFrame<PFP3>(PFP3::MAP& map, Dart d, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, PFP3::VEC3& normal);


int test_localFrame()
{
	return 0;
}