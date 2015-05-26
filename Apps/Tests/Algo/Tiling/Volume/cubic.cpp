#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap3.h"


using namespace CGoGN;
#include "Algo/Tiling/Volume/cubic.h"


struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap3 MAP;
};

struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap3 MAP;
};


template Algo::Volume::Tilings::Cubic::Grid<PFP1>;
template Algo::Volume::Tilings::Cubic::Grid<PFP2>;


int test_cubic()
{
	return 0;
}
