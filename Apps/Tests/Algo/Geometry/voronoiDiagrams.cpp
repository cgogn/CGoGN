#include <iostream>
#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"

#include "Algo/Geometry/voronoiDiagrams.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

template class Algo::Surface::Geometry::VoronoiDiagram<PFP1>;
template class Algo::Surface::Geometry::CentroidalVoronoiDiagram<PFP1>;


struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

template class Algo::Surface::Geometry::VoronoiDiagram<PFP2>;
template class Algo::Surface::Geometry::CentroidalVoronoiDiagram<PFP2>;


struct PFP3 : public PFP_STANDARD
{
	typedef EmbeddedGMap2 MAP;
};

template class Algo::Surface::Geometry::VoronoiDiagram<PFP3>;
template class Algo::Surface::Geometry::CentroidalVoronoiDiagram<PFP3>;


int test_voronoiDiagrams()
{
	return 0;
}