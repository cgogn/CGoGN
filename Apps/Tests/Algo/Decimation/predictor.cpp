#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"

#include "Algo/Decimation/predictor.h"

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


template class Algo::Surface::Decimation::Predictor<PFP1, float>;
template class Algo::Surface::Decimation::Predictor<PFP2, float>;
template class Algo::Surface::Decimation::Predictor<PFP3, float>;


int test_predictor()
{

	return 0;
}

