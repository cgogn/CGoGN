#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap3.h"


#include "Algo/Render/GL2/topo3Render.h"


using namespace CGoGN;


struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap3 MAP;
};

template class Algo::Render::GL2::Topo3RenderMap<PFP1>;


struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap3 MAP;
};

template class Algo::Render::GL2::Topo3RenderMap<PFP2>;



struct PFP3 : public PFP_STANDARD
{
	typedef EmbeddedGMap3 MAP;
};

template class Algo::Render::GL2::Topo3RenderGMap<PFP3>;


struct PFP4 : public PFP_DOUBLE
{
	typedef EmbeddedGMap3 MAP;
};

template class Algo::Render::GL2::Topo3RenderGMap<PFP4>;



int test_topo3Render()
{

	return 0;
}

