#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Export/export.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

template bool Algo::Surface::Export::exportPLY<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, const char* filename, const bool binary);
template bool Algo::Surface::Export::exportPLYnew<PFP1>(PFP1::MAP& map, const std::vector<VertexAttribute<PFP1::VEC3, PFP1::MAP>* >& attributeHandlers, const char* filename, const bool binary);
template bool Algo::Surface::Export::exportOFF<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, const char* filename);
template bool Algo::Surface::Export::exportOBJ<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, const char* filename);
template bool Algo::Surface::Export::exportChoupi<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, const char* filename);


struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

template bool Algo::Surface::Export::exportPLY<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, const char* filename, const bool binary);
template bool Algo::Surface::Export::exportPLYnew<PFP2>(PFP2::MAP& map, const std::vector<VertexAttribute<PFP2::VEC3, PFP2::MAP>* >& attributeHandlers, const char* filename, const bool binary);
template bool Algo::Surface::Export::exportOFF<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, const char* filename);
template bool Algo::Surface::Export::exportOBJ<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, const char* filename);
template bool Algo::Surface::Export::exportChoupi<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, const char* filename);


struct PFP3 : public PFP_DOUBLE
{
	typedef EmbeddedGMap2 MAP;
};

template bool Algo::Surface::Export::exportPLY<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, const char* filename, const bool binary);
template bool Algo::Surface::Export::exportPLYnew<PFP3>(PFP3::MAP& map, const std::vector<VertexAttribute<PFP3::VEC3, PFP3::MAP>* >& attributeHandlers, const char* filename, const bool binary);
template bool Algo::Surface::Export::exportOFF<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, const char* filename);
template bool Algo::Surface::Export::exportOBJ<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, const char* filename);
template bool Algo::Surface::Export::exportChoupi<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, const char* filename);


int test_export()
{

	return 0;
}
