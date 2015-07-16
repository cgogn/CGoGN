#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"

#include "Algo/Decimation/geometryPredictor.h"

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



template class Algo::Surface::Decimation::Predictor_HalfCollapse<PFP1>;
template class Algo::Surface::Decimation::Predictor_TangentPredict1<PFP1>;
template class Algo::Surface::Decimation::Predictor_TangentPredict2<PFP1>;

template class Algo::Surface::Decimation::Predictor_HalfCollapse<PFP2>;
template class Algo::Surface::Decimation::Predictor_TangentPredict1<PFP2>;
template class Algo::Surface::Decimation::Predictor_TangentPredict2<PFP2>;

template class Algo::Surface::Decimation::Predictor_HalfCollapse<PFP3>;
template class Algo::Surface::Decimation::Predictor_TangentPredict1<PFP3>;
template class Algo::Surface::Decimation::Predictor_TangentPredict2<PFP3>;


int test_geometryPredictor()
{

	return 0;
}