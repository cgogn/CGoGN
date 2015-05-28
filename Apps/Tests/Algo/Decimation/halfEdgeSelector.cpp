#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Decimation/halfEdgeSelector.h"

using namespace CGoGN;


struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};



template class Algo::Surface::Decimation::HalfEdgeSelector_QEMml<PFP1>;
template class Algo::Surface::Decimation::HalfEdgeSelector_QEMextColor<PFP1>;
template class Algo::Surface::Decimation::HalfEdgeSelector_QEMextColorNormal<PFP1>;
template class Algo::Surface::Decimation::HalfEdgeSelector_ColorGradient<PFP1>;

template class Algo::Surface::Decimation::HalfEdgeSelector_QEMml<PFP2>;
template class Algo::Surface::Decimation::HalfEdgeSelector_QEMextColor<PFP2>;
template class Algo::Surface::Decimation::HalfEdgeSelector_QEMextColorNormal<PFP2>;
template class Algo::Surface::Decimation::HalfEdgeSelector_ColorGradient<PFP2>;


int test_halfEdgeSelector()
{

	return 0;
}
