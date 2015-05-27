#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"


using namespace CGoGN;
#include "Algo/Simulation/ShapeMatching/shapeMatchingLinear.h"


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


template class Algo::Surface::Simulation::ShapeMatching::ShapeMatchingLinear<PFP1>;
template class Algo::Surface::Simulation::ShapeMatching::ShapeMatchingLinear<PFP2>;
template class Algo::Surface::Simulation::ShapeMatching::ShapeMatchingLinear<PFP3>;


int test_shapeMatchingLinear()
{

	return 0;
}

