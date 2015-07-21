#include <iostream>
#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"

#include "Algo/Geometry/volume.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

template PFP1::REAL Algo::Geometry::tetrahedronSignedVolume<PFP1>(PFP1::MAP& map, Vol v, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template PFP1::REAL Algo::Geometry::tetrahedronVolume<PFP1>(PFP1::MAP& map, Vol v, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template PFP1::REAL Algo::Geometry::convexPolyhedronVolume<PFP1>(PFP1::MAP& map, Vol v, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template PFP1::REAL Algo::Geometry::totalVolume<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);

struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

template PFP2::REAL Algo::Geometry::tetrahedronSignedVolume<PFP2>(PFP2::MAP& map, Vol v, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template PFP2::REAL Algo::Geometry::tetrahedronVolume<PFP2>(PFP2::MAP& map, Vol v, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template PFP2::REAL Algo::Geometry::convexPolyhedronVolume<PFP2>(PFP2::MAP& map, Vol v, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template PFP2::REAL Algo::Geometry::totalVolume<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);


struct PFP3 : public PFP_STANDARD
{
	typedef EmbeddedGMap2 MAP;
};

template PFP3::REAL Algo::Geometry::tetrahedronSignedVolume<PFP3>(PFP3::MAP& map, Vol v, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
template PFP3::REAL Algo::Geometry::tetrahedronVolume<PFP3>(PFP3::MAP& map, Vol v, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
template PFP3::REAL Algo::Geometry::convexPolyhedronVolume<PFP3>(PFP3::MAP& map, Vol v, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
template PFP3::REAL Algo::Geometry::totalVolume<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);



int test_volume()
{
	return 0;
}

