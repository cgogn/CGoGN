#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Render/GL2/mapRender.h"


using namespace CGoGN;


struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

template void Algo::Render::GL2::MapRender::initPrimitives<PFP1>(PFP1::MAP& map, int prim, bool optimized);
template void Algo::Render::GL2::MapRender::initPrimitives<PFP1>(PFP1::MAP& map, int prim, const VertexAttribute<PFP1::VEC3, PFP1::MAP>* position, bool optimized);
template void Algo::Render::GL2::MapRender::addPrimitives<PFP1>(PFP1::MAP& map, int prim, const VertexAttribute<PFP1::VEC3, PFP1::MAP>* position, bool optimized);



struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

template void Algo::Render::GL2::MapRender::initPrimitives<PFP2>(PFP2::MAP& map, int prim, bool optimized);
template void Algo::Render::GL2::MapRender::initPrimitives<PFP2>(PFP2::MAP& map, int prim, const VertexAttribute<PFP2::VEC3, PFP2::MAP>* position, bool optimized);
template void Algo::Render::GL2::MapRender::addPrimitives<PFP2>(PFP2::MAP& map, int prim, const VertexAttribute<PFP2::VEC3, PFP2::MAP>* position, bool optimized);


struct PFP3 : public PFP_DOUBLE
{
	typedef EmbeddedMap3 MAP;
};

template void Algo::Render::GL2::MapRender::initPrimitives<PFP3>(PFP3::MAP& map, int prim, bool optimized);
template void Algo::Render::GL2::MapRender::initPrimitives<PFP3>(PFP3::MAP& map, int prim, const VertexAttribute<PFP3::VEC3, PFP3::MAP>* position, bool optimized);
template void Algo::Render::GL2::MapRender::addPrimitives<PFP3>(PFP3::MAP& map, int prim, const VertexAttribute<PFP3::VEC3, PFP3::MAP>* position, bool optimized);



int test_mapRender()
{

	return 0;
}