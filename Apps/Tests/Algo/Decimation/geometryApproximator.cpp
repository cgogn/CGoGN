#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"

#include "Algo/Decimation/geometryApproximator.h"

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



template class Algo::Surface::Decimation::Approximator_QEM<PFP1>;
template class Algo::Surface::Decimation::Approximator_QEMhalfEdge<PFP1>;
template class Algo::Surface::Decimation::Approximator_InterpolateAlongEdge<PFP1,float>;
template class Algo::Surface::Decimation::Approximator_HalfCollapse<PFP1,float>;
template class Algo::Surface::Decimation::Approximator_CornerCutting<PFP1>;
template class Algo::Surface::Decimation::Approximator_NormalArea<PFP1>;



int test_geometryApproximator()
{

	return 0;
}


