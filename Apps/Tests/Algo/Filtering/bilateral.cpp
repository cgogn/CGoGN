#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Filtering/bilateral.h"

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


template void Algo::Surface::Filtering::filterBilateral<PFP1>( PFP1::MAP& map,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positionIn,VertexAttribute<PFP1::VEC3, PFP1::MAP>& positionOut,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& normal);

template void Algo::Surface::Filtering::filterSUSAN<PFP1>(PFP1::MAP& map, float SUSANthreshold,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,	VertexAttribute<PFP1::VEC3, PFP1::MAP>& position2,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& normal);



template void Algo::Surface::Filtering::filterBilateral<PFP2>(PFP2::MAP& map,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positionIn, VertexAttribute<PFP2::VEC3, PFP2::MAP>& positionOut,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& normal);

template void Algo::Surface::Filtering::filterSUSAN<PFP2>(PFP2::MAP& map, float SUSANthreshold,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position2,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& normal);



template void Algo::Surface::Filtering::filterBilateral<PFP3>(PFP3::MAP& map,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positionIn, VertexAttribute<PFP3::VEC3, PFP3::MAP>& positionOut,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& normal);

template void Algo::Surface::Filtering::filterSUSAN<PFP3>(PFP3::MAP& map, float SUSANthreshold,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, VertexAttribute<PFP3::VEC3, PFP3::MAP>& position2,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& normal);


int test_bilateral()
{

	return 0;
}
