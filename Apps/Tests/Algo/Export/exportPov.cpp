#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Export/exportPov.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

template void Algo::Surface::ExportPov::exportTriangleWire<PFP1>(std::ofstream& out, PFP1::VEC3& p1, PFP1::VEC3& p2, PFP1::VEC3& p3, float width);
template void Algo::Surface::ExportPov::exportTrianglePlain<PFP1>(std::ofstream& out, PFP1::VEC3& p1, PFP1::VEC3& p2, PFP1::VEC3& p3);
template void Algo::Surface::ExportPov::exportMeshPlain<PFP1>(std::ofstream& out, PFP1::MAP& map, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, const std::string& meshName);
template void Algo::Surface::ExportPov::exportMeshWire<PFP1>(std::ofstream& out, PFP1::MAP& map, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, const std::string& meshName);
template bool Algo::Surface::ExportPov::exportScenePov<PFP1>(PFP1::MAP& map, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, const std::string& filename, PFP1::VEC3 cameraPos, PFP1::VEC3 cameraLook, PFP1::VEC3 translate, float angle_X, float angle_Y, float angle_Z);


struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

template void Algo::Surface::ExportPov::exportTriangleWire<PFP2>(std::ofstream& out, PFP2::VEC3& p1, PFP2::VEC3& p2, PFP2::VEC3& p3, float width);
template void Algo::Surface::ExportPov::exportTrianglePlain<PFP2>(std::ofstream& out, PFP2::VEC3& p1, PFP2::VEC3& p2, PFP2::VEC3& p3);
template void Algo::Surface::ExportPov::exportMeshPlain<PFP2>(std::ofstream& out, PFP2::MAP& map, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, const std::string& meshName);
template void Algo::Surface::ExportPov::exportMeshWire<PFP2>(std::ofstream& out, PFP2::MAP& map, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, const std::string& meshName);
template bool Algo::Surface::ExportPov::exportScenePov<PFP2>(PFP2::MAP& map, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, const std::string& filename, PFP2::VEC3 cameraPos, PFP2::VEC3 cameraLook, PFP2::VEC3 translate, float angle_X, float angle_Y, float angle_Z);



struct PFP3 : public PFP_DOUBLE
{
	typedef EmbeddedMap3 MAP;
};

template void Algo::Surface::ExportPov::export3MeshPlainSmooth<PFP3>(std::ofstream& out, PFP3::MAP& map, VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, const std::string& meshName);
template bool Algo::Surface::ExportPov::exportScenePovSmooth<PFP3>(PFP3::MAP& map, VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, const std::string& filename, PFP3::VEC3 cameraPos, PFP3::VEC3 cameraLook, PFP3::VEC3 translate, float angle_X, float angle_Y, float angle_Z);


int test_exportPov()
{

	return 0;
}