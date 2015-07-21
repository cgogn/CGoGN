#include <iostream>
#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"

#include "Algo/Geometry/orientation.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

template bool Algo::Surface::Geometry::isTetrahedronWellOriented<PFP1>(PFP1::MAP& map, Dart d, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, bool CCW);

struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedGMap2 MAP;
};

template bool Algo::Surface::Geometry::isTetrahedronWellOriented<PFP2>(PFP2::MAP& map, Dart d, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, bool CCW);


int test_orientation()
{
	return 0;
}

