#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Tiling/Surface/square.h"

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


template class Algo::Surface::Tilings::Square::Grid<PFP1>;
template class Algo::Surface::Tilings::Square::Cylinder<PFP1>;
template class Algo::Surface::Tilings::Square::Cube<PFP1>;
template class Algo::Surface::Tilings::Square::Tore<PFP1>;

template class Algo::Surface::Tilings::Square::Grid<PFP2>;
template class Algo::Surface::Tilings::Square::Cylinder<PFP2>;
template class Algo::Surface::Tilings::Square::Cube<PFP2>;
template class Algo::Surface::Tilings::Square::Tore<PFP2>;

template class Algo::Surface::Tilings::Square::Grid<PFP3>;
template class Algo::Surface::Tilings::Square::Cylinder<PFP3>;
template class Algo::Surface::Tilings::Square::Cube<PFP3>;
template class Algo::Surface::Tilings::Square::Tore<PFP3>;


int test_square()
{
	return 0;
}

