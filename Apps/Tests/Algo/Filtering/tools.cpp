#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Filtering/tools.h"

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


template PFP1::REAL Algo::Surface::Filtering::computeHaussdorf<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& originalPosition, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position2);
template void Algo::Surface::Filtering::computeNoise<PFP1>(PFP1::MAP& map, long amount, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position2, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& normal);
template void Algo::Surface::Filtering::computeUnfirmAdditiveNoise<PFP1>(PFP1::MAP& map, float noiseIntensity, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position2);
template void Algo::Surface::Filtering::computeGaussianAdditiveNoise<PFP1>(PFP1::MAP& map, float noiseIntensity, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position2);

template PFP2::REAL Algo::Surface::Filtering::computeHaussdorf<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& originalPosition, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position2);
template void Algo::Surface::Filtering::computeNoise<PFP2>(PFP2::MAP& map, long amount, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position2, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& normal);
template void Algo::Surface::Filtering::computeUnfirmAdditiveNoise<PFP2>(PFP2::MAP& map, float noiseIntensity, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position2);
template void Algo::Surface::Filtering::computeGaussianAdditiveNoise<PFP2>(PFP2::MAP& map, float noiseIntensity, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position2);

template PFP3::REAL Algo::Surface::Filtering::computeHaussdorf<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& originalPosition, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position2);
template void Algo::Surface::Filtering::computeNoise<PFP3>(PFP3::MAP& map, long amount, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, VertexAttribute<PFP3::VEC3, PFP3::MAP>& position2, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& normal);
template void Algo::Surface::Filtering::computeUnfirmAdditiveNoise<PFP3>(PFP3::MAP& map, float noiseIntensity, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, VertexAttribute<PFP3::VEC3, PFP3::MAP>& position2);
template void Algo::Surface::Filtering::computeGaussianAdditiveNoise<PFP3>(PFP3::MAP& map, float noiseIntensity, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, VertexAttribute<PFP3::VEC3, PFP3::MAP>& position2);


struct PFP4 : public PFP_STANDARD
{
	typedef EmbeddedMap3 MAP;
};

struct PFP5 : public PFP_DOUBLE
{
	typedef EmbeddedMap3 MAP;
};

template void Algo::Volume::Filtering::computeNoise<PFP4>(PFP4::MAP& map, long amount, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& position, VertexAttribute<PFP4::VEC3, PFP4::MAP>& position2);
template void Algo::Volume::Filtering::computeNoiseGaussian<PFP4>(PFP4::MAP& map, long amount, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& position, VertexAttribute<PFP4::VEC3, PFP4::MAP>& position2);

template void Algo::Volume::Filtering::computeNoise<PFP5>(PFP5::MAP& map, long amount, const VertexAttribute<PFP5::VEC3, PFP5::MAP>& position, VertexAttribute<PFP5::VEC3, PFP5::MAP>& position2);
template void Algo::Volume::Filtering::computeNoiseGaussian<PFP5>(PFP5::MAP& map, long amount, const VertexAttribute<PFP5::VEC3, PFP5::MAP>& position, VertexAttribute<PFP5::VEC3, PFP5::MAP>& position2);


int test_tools()
{

	return 0;
}