#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap3.h"

#include "Algo/Export/exportVol.hpp"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap3 MAP;
};


template bool Algo::Volume::Export::exportMesh<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, const std::string& filename) ;

int test_exportVol()
{

	return 0;
}