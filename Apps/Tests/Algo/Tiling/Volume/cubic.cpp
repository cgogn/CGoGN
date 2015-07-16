#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap3.h"


#include "Algo/Tiling/Volume/cubic.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap3 MAP;
};

struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap3 MAP;
};


template class Algo::Volume::Tilings::Cubic::Grid<PFP1>;
template class Algo::Volume::Tilings::Cubic::Grid<PFP2>;


int test_cubic()
{
	return 0;
}
