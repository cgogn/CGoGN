#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Tiling/Surface/triangular.h"

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

template class Algo::Surface::Tilings::Triangular::Grid<PFP1>;
template class Algo::Surface::Tilings::Triangular::Cylinder<PFP1>;
template class Algo::Surface::Tilings::Triangular::Cube<PFP1>;
template class Algo::Surface::Tilings::Triangular::Tore<PFP1>;

template class Algo::Surface::Tilings::Triangular::Grid<PFP2>;
template class Algo::Surface::Tilings::Triangular::Cylinder<PFP2>;
template class Algo::Surface::Tilings::Triangular::Cube<PFP2>;
template class Algo::Surface::Tilings::Triangular::Tore<PFP2>;


template class Algo::Surface::Tilings::Triangular::Grid<PFP3>;
template class Algo::Surface::Tilings::Triangular::Cylinder<PFP3>;
template class Algo::Surface::Tilings::Triangular::Cube<PFP3>;
template class Algo::Surface::Tilings::Triangular::Tore<PFP3>;


int test_triangular()
{
	return 0;
}

