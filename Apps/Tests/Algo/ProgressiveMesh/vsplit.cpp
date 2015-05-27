#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"


using namespace CGoGN;
#include "Algo/ProgressiveMesh/vsplit.h"


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


template class Algo::Surface::PMesh::VSplit<PFP1>;
template class Algo::Surface::PMesh::VSplit<PFP2>;
template class Algo::Surface::PMesh::VSplit<PFP3>;


int test_vsplit()
{

	return 0;
}

