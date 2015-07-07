#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Tiling/Surface/hexagonal.h"

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


// TODO implementation !!!
template class Algo::Surface::Tilings::Hexagonal::Grid<PFP1>;
template class Algo::Surface::Tilings::Hexagonal::Grid<PFP2>;
template class Algo::Surface::Tilings::Hexagonal::Grid<PFP3>;



int test_hexagonal()
{
	return 0;
}
