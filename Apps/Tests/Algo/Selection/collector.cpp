#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Selection/collector.h"

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


template class Algo::Surface::Selection::Collector_OneRing<PFP1>;
template class Algo::Surface::Selection::Collector_OneRing_AroundEdge<PFP1>;
template class Algo::Surface::Selection::Collector_WithinSphere<PFP1>;
template class Algo::Surface::Selection::Collector_NormalAngle<PFP1>;
template class Algo::Surface::Selection::Collector_NormalAngle_Triangles<PFP1>;
template class Algo::Surface::Selection::CollectorCriterion_VertexNormalAngle<PFP1>;
template class Algo::Surface::Selection::CollectorCriterion_TriangleNormalAngle<PFP1>;
template class Algo::Surface::Selection::CollectorCriterion_VertexWithinSphere<PFP1>;
template class Algo::Surface::Selection::Collector_Vertices<PFP1>;
template class Algo::Surface::Selection::Collector_Triangles<PFP1>;
template class Algo::Surface::Selection::Collector_Dijkstra_Vertices<PFP1>;
template class Algo::Surface::Selection::Collector_Dijkstra<PFP1>;

template class Algo::Surface::Selection::Collector_OneRing<PFP2>;
template class Algo::Surface::Selection::Collector_OneRing_AroundEdge<PFP2>;
template class Algo::Surface::Selection::Collector_WithinSphere<PFP2>;
template class Algo::Surface::Selection::Collector_NormalAngle<PFP2>;
template class Algo::Surface::Selection::Collector_NormalAngle_Triangles<PFP2>;
template class Algo::Surface::Selection::CollectorCriterion_VertexNormalAngle<PFP2>;
template class Algo::Surface::Selection::CollectorCriterion_TriangleNormalAngle<PFP2>;
template class Algo::Surface::Selection::CollectorCriterion_VertexWithinSphere<PFP2>;
template class Algo::Surface::Selection::Collector_Vertices<PFP2>;
template class Algo::Surface::Selection::Collector_Triangles<PFP2>;
template class Algo::Surface::Selection::Collector_Dijkstra_Vertices<PFP2>;
template class Algo::Surface::Selection::Collector_Dijkstra<PFP2>;


template class Algo::Surface::Selection::Collector_OneRing<PFP3>;
template class Algo::Surface::Selection::Collector_OneRing_AroundEdge<PFP3>;
template class Algo::Surface::Selection::Collector_WithinSphere<PFP3>;
template class Algo::Surface::Selection::Collector_NormalAngle<PFP3>;
template class Algo::Surface::Selection::Collector_NormalAngle_Triangles<PFP3>;
template class Algo::Surface::Selection::CollectorCriterion_VertexNormalAngle<PFP3>;
template class Algo::Surface::Selection::CollectorCriterion_TriangleNormalAngle<PFP3>;
template class Algo::Surface::Selection::CollectorCriterion_VertexWithinSphere<PFP3>;
template class Algo::Surface::Selection::Collector_Vertices<PFP3>;
template class Algo::Surface::Selection::Collector_Triangles<PFP3>;
template class Algo::Surface::Selection::Collector_Dijkstra_Vertices<PFP3>;
template class Algo::Surface::Selection::Collector_Dijkstra<PFP3>;


int test_collector()
{

	return 0;
}


