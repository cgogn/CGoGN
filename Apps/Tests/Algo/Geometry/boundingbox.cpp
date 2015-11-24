#include <iostream>
#include "Topology/generic/parameters.h"
#include "Topology/gmap/embeddedGMap2.h"
#include "Topology/map/embeddedMap2.h"

#include "Algo/Geometry/boundingbox.h"


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
*		 INSTANTIATION
*****************************************/

template Geom::BoundingBox<PFP1::VEC3> Algo::Geometry::computeBoundingBox<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template Geom::BoundingBox<PFP2::VEC3> Algo::Geometry::computeBoundingBox<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template Geom::BoundingBox<PFP3::VEC3> Algo::Geometry::computeBoundingBox<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);


int test_boundingbox()
{
	return 0;
}
