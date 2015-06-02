
#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap3.h"


#include "Algo/Render/GL2/topo3PrimalRender.h"


using namespace CGoGN;


struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap3 MAP;
};

template class Algo::Render::GL2::Topo3PrimalRender<PFP1>;



struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap3 MAP;
};

template class Algo::Render::GL2::Topo3PrimalRender<PFP2>;


int test_topo3PrimalRender()
{

	return 0;
}
