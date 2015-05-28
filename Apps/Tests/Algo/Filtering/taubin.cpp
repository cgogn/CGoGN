#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Filtering/taubin.h"

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



template void Algo::Surface::Filtering::filterTaubin<PFP1>(PFP1::MAP& map,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position2);

template void Algo::Surface::Filtering::filterTaubin_modified<PFP1>(PFP1::MAP& map,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position2, PFP1::REAL radius);


template void Algo::Surface::Filtering::filterTaubin<PFP2>(PFP2::MAP& map,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position2);

template void Algo::Surface::Filtering::filterTaubin_modified<PFP2>(PFP2::MAP& map,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position2, PFP2::REAL radius);


template void Algo::Surface::Filtering::filterTaubin<PFP3>(PFP3::MAP& map,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, VertexAttribute<PFP3::VEC3, PFP3::MAP>& position2);

template void Algo::Surface::Filtering::filterTaubin_modified<PFP3>(PFP3::MAP& map,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, VertexAttribute<PFP3::VEC3, PFP3::MAP>& position2, PFP3::REAL radius);


int test_taubin()
{

		return 0;
}