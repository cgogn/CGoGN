#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"

#include "Algo/Render/GL2/topoPrimalRender.h"


using namespace CGoGN;


struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

template class Algo::Render::GL2::TopoPrimalRender<PFP1>;


struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

template class Algo::Render::GL2::TopoPrimalRender<PFP2>;



int test_topoPrimalRender()
{

	return 0;
}
