#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"

#include "Algo/Decimation/edgeSelector.h"

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


template class Algo::Surface::Decimation::EdgeSelector_MapOrder<PFP1>;
template class Algo::Surface::Decimation::EdgeSelector_Random<PFP1>;
template class Algo::Surface::Decimation::EdgeSelector_Length<PFP1>;
template class Algo::Surface::Decimation::EdgeSelector_QEM<PFP1>;
template class Algo::Surface::Decimation::EdgeSelector_QEMml<PFP1>;
template class Algo::Surface::Decimation::EdgeSelector_NormalArea<PFP1>;
template class Algo::Surface::Decimation::EdgeSelector_Curvature<PFP1>;
template class Algo::Surface::Decimation::EdgeSelector_CurvatureTensor<PFP1>;
template class Algo::Surface::Decimation::EdgeSelector_MinDetail<PFP1>;
template class Algo::Surface::Decimation::EdgeSelector_ColorNaive<PFP1>;
template class Algo::Surface::Decimation::EdgeSelector_GeomColOptGradient<PFP1>;
template class Algo::Surface::Decimation::EdgeSelector_QEMextColor<PFP1>;



int test_edgeSelector()
{

	return 0;
}
