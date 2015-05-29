#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Export/exportVTU.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

template bool Algo::Surface::Export::exportVTU<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, const char* filename);
template bool Algo::Surface::Export::exportVTUBinary<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, const char* filename);
template class Algo::Surface::Export::VTUExporter<PFP1>;


struct PFP2 : public PFP_STANDARD
{
	typedef EmbeddedMap3 MAP;
};

template class Algo::Volume::Export::VTUExporter<PFP2>;

int test_exportVTU()
{

	return 0;
}