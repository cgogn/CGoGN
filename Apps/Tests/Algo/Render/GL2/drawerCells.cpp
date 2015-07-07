#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Render/GL2/drawerCells.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

template void Algo::Render::drawerCells<PFP1>(unsigned int cell, Utils::Drawer& dr, PFP1::MAP& map, std::vector<Dart>& vd, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions,float k);
template void Algo::Render::drawerCell<PFP1>(unsigned int cell, Utils::Drawer& dr, PFP1::MAP& map, Dart d, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, float k);
template void Algo::Render::drawerVertices<PFP1>(Utils::Drawer& dr, PFP1::MAP& map, std::vector<Dart>& vd, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions);
template void Algo::Render::drawerEdges<PFP1>(Utils::Drawer& dr, PFP1::MAP& map, std::vector<Dart>& vd, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, float k);
template void Algo::Render::drawerFaces<PFP1>(Utils::Drawer& dr, PFP1::MAP& map, std::vector<Dart>& vd, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, float k);
template void Algo::Render::drawerVolumes<PFP1>(Utils::Drawer& dr, PFP1::MAP& map, std::vector<Dart>& vd, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, float k);
template void Algo::Render::drawerVertex<PFP1>(Utils::Drawer& dr, PFP1::MAP& map, Dart d, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions);
template void Algo::Render::drawerEdge<PFP1>(Utils::Drawer& dr, PFP1::MAP& map, Dart d, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, float k);
template void Algo::Render::drawerFace<PFP1>(Utils::Drawer& dr, PFP1::MAP& map, Dart d, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, float k);
template void Algo::Render::drawerVolume<PFP1>(Utils::Drawer& dr, PFP1::MAP& map, Dart d, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, float k);
template void Algo::Render::drawerAddEdge<PFP1>(Utils::Drawer& dr, PFP1::MAP& map, Dart d, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, float k);
template void Algo::Render::drawerAddEdgeShrink<PFP1>(Utils::Drawer& dr, PFP1::MAP& map, Dart d, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, const PFP1::VEC3& C, float k);
template void Algo::Render::drawerAddFace<PFP1>(Utils::Drawer& dr, PFP1::MAP& map, Dart d, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, float k);
template void Algo::Render::drawerAddVolume<PFP1>(Utils::Drawer& dr, PFP1::MAP& map, Dart d, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, float k);


struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

template void Algo::Render::drawerCells<PFP2>(unsigned int cell, Utils::Drawer& dr, PFP2::MAP& map, std::vector<Dart>& vd, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, float k);
template void Algo::Render::drawerCell<PFP2>(unsigned int cell, Utils::Drawer& dr, PFP2::MAP& map, Dart d, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, float k);
template void Algo::Render::drawerVertices<PFP2>(Utils::Drawer& dr, PFP2::MAP& map, std::vector<Dart>& vd, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions);
template void Algo::Render::drawerEdges<PFP2>(Utils::Drawer& dr, PFP2::MAP& map, std::vector<Dart>& vd, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, float k);
template void Algo::Render::drawerFaces<PFP2>(Utils::Drawer& dr, PFP2::MAP& map, std::vector<Dart>& vd, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, float k);
template void Algo::Render::drawerVolumes<PFP2>(Utils::Drawer& dr, PFP2::MAP& map, std::vector<Dart>& vd, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, float k);
template void Algo::Render::drawerVertex<PFP2>(Utils::Drawer& dr, PFP2::MAP& map, Dart d, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions);
template void Algo::Render::drawerEdge<PFP2>(Utils::Drawer& dr, PFP2::MAP& map, Dart d, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, float k);
template void Algo::Render::drawerFace<PFP2>(Utils::Drawer& dr, PFP2::MAP& map, Dart d, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, float k);
template void Algo::Render::drawerVolume<PFP2>(Utils::Drawer& dr, PFP2::MAP& map, Dart d, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, float k);
template void Algo::Render::drawerAddEdge<PFP2>(Utils::Drawer& dr, PFP2::MAP& map, Dart d, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, float k);
template void Algo::Render::drawerAddEdgeShrink<PFP2>(Utils::Drawer& dr, PFP2::MAP& map, Dart d, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, const PFP2::VEC3& C, float k);
template void Algo::Render::drawerAddFace<PFP2>(Utils::Drawer& dr, PFP2::MAP& map, Dart d, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, float k);
template void Algo::Render::drawerAddVolume<PFP2>(Utils::Drawer& dr, PFP2::MAP& map, Dart d, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, float k);



struct PFP3 : public PFP_DOUBLE
{
	typedef EmbeddedGMap2 MAP;
};

template void Algo::Render::drawerCells<PFP3>(unsigned int cell, Utils::Drawer& dr, PFP3::MAP& map, std::vector<Dart>& vd, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, float k);
template void Algo::Render::drawerCell<PFP3>(unsigned int cell, Utils::Drawer& dr, PFP3::MAP& map, Dart d, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, float k);
template void Algo::Render::drawerVertices<PFP3>(Utils::Drawer& dr, PFP3::MAP& map, std::vector<Dart>& vd, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions);
template void Algo::Render::drawerEdges<PFP3>(Utils::Drawer& dr, PFP3::MAP& map, std::vector<Dart>& vd, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, float k);
template void Algo::Render::drawerFaces<PFP3>(Utils::Drawer& dr, PFP3::MAP& map, std::vector<Dart>& vd, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, float k);
template void Algo::Render::drawerVolumes<PFP3>(Utils::Drawer& dr, PFP3::MAP& map, std::vector<Dart>& vd, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, float k);
template void Algo::Render::drawerVertex<PFP3>(Utils::Drawer& dr, PFP3::MAP& map, Dart d, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions);
template void Algo::Render::drawerEdge<PFP3>(Utils::Drawer& dr, PFP3::MAP& map, Dart d, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, float k);
template void Algo::Render::drawerFace<PFP3>(Utils::Drawer& dr, PFP3::MAP& map, Dart d, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, float k);
template void Algo::Render::drawerVolume<PFP3>(Utils::Drawer& dr, PFP3::MAP& map, Dart d, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, float k);
template void Algo::Render::drawerAddEdge<PFP3>(Utils::Drawer& dr, PFP3::MAP& map, Dart d, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, float k);
template void Algo::Render::drawerAddEdgeShrink<PFP3>(Utils::Drawer& dr, PFP3::MAP& map, Dart d, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, const PFP3::VEC3& C, float k);
template void Algo::Render::drawerAddFace<PFP3>(Utils::Drawer& dr, PFP3::MAP& map, Dart d, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, float k);
template void Algo::Render::drawerAddVolume<PFP3>(Utils::Drawer& dr, PFP3::MAP& map, Dart d, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, float k);


struct PFP4 : public PFP_DOUBLE
{
	typedef EmbeddedMap3 MAP;
};

template void Algo::Render::drawerCells<PFP4>(unsigned int cell, Utils::Drawer& dr, PFP4::MAP& map, std::vector<Dart>& vd, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& positions, float k);
template void Algo::Render::drawerCell<PFP4>(unsigned int cell, Utils::Drawer& dr, PFP4::MAP& map, Dart d, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& positions, float k);
template void Algo::Render::drawerVertices<PFP4>(Utils::Drawer& dr, PFP4::MAP& map, std::vector<Dart>& vd, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& positions);
template void Algo::Render::drawerEdges<PFP4>(Utils::Drawer& dr, PFP4::MAP& map, std::vector<Dart>& vd, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& positions, float k);
template void Algo::Render::drawerFaces<PFP4>(Utils::Drawer& dr, PFP4::MAP& map, std::vector<Dart>& vd, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& positions, float k);
template void Algo::Render::drawerVolumes<PFP4>(Utils::Drawer& dr, PFP4::MAP& map, std::vector<Dart>& vd, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& positions, float k);
template void Algo::Render::drawerVertex<PFP4>(Utils::Drawer& dr, PFP4::MAP& map, Dart d, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& positions);
template void Algo::Render::drawerEdge<PFP4>(Utils::Drawer& dr, PFP4::MAP& map, Dart d, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& positions, float k);
template void Algo::Render::drawerFace<PFP4>(Utils::Drawer& dr, PFP4::MAP& map, Dart d, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& positions, float k);
template void Algo::Render::drawerVolume<PFP4>(Utils::Drawer& dr, PFP4::MAP& map, Dart d, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& positions, float k);
template void Algo::Render::drawerAddEdge<PFP4>(Utils::Drawer& dr, PFP4::MAP& map, Dart d, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& positions, float k);
template void Algo::Render::drawerAddEdgeShrink<PFP4>(Utils::Drawer& dr, PFP4::MAP& map, Dart d, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& positions, const PFP4::VEC3& C, float k);
template void Algo::Render::drawerAddFace<PFP4>(Utils::Drawer& dr, PFP4::MAP& map, Dart d, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& positions, float k);
template void Algo::Render::drawerAddVolume<PFP4>(Utils::Drawer& dr, PFP4::MAP& map, Dart d, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& positions, float k);


int test_drawerCells()
{

	return 0;
}
