#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Import/AHEMImporter.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

template class Algo::Import::AHEMImporter<PFP1>;



struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

template class Algo::Import::AHEMImporter<PFP2>;


int test_AHEMImporter()
{

	return 0;
}


