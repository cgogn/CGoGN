#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Remeshing/pliant.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

struct PFP3 : public PFP_DOUBLE
{
	typedef EmbeddedGMap2 MAP;
};


template void  Algo::Surface::Remeshing::pliantRemeshing<PFP1>(PFP1::MAP& map, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, VertexAttribute<PFP1::VEC3, PFP1::MAP>& normal);
template void  Algo::Surface::Remeshing::pliantRemeshing<PFP2>(PFP2::MAP& map, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, VertexAttribute<PFP2::VEC3, PFP2::MAP>& normal);
template void  Algo::Surface::Remeshing::pliantRemeshing<PFP3>(PFP3::MAP& map, VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, VertexAttribute<PFP3::VEC3, PFP3::MAP>& normal);


int test_pliant()
{

	return 0;
}
