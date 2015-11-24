#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Render/GL2/topoRender.h"


using namespace CGoGN;


struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};
typedef PFP1::MAP MAP1;

template void Algo::Render::GL2::TopoRender::overdrawDart<MAP1>(MAP1& map, Dart d, float width, float r, float g, float b);
template Dart Algo::Render::GL2::TopoRender::picking<MAP1>(MAP1& map, int x, int y, bool withBoundary);
template Dart Algo::Render::GL2::TopoRender::coneSelection<MAP1>(MAP1& map, const Geom::Vec3f& rayA, const Geom::Vec3f& rayAB, float angle);
template Dart Algo::Render::GL2::TopoRender::raySelection<MAP1>(MAP1& map, const Geom::Vec3f& rayA, const Geom::Vec3f& rayAB, float distmax);
template void Algo::Render::GL2::TopoRender::updateData<PFP1>(PFP1::MAP &map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, float ke, float kf, bool withBoundary, bool onlyBoundary);
template void Algo::Render::GL2::TopoRender::updateData<PFP1>(PFP1::MAP &map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, bool onlyBoundary);
template void Algo::Render::GL2::TopoRender::updateDataBoundary<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, float ke, float kf, float ns);
template void Algo::Render::GL2::TopoRender::updateDataBoundary<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions);
//template void Algo::Render::GL2::TopoRender::updateDataGMap<PFP1>(PFP1::MAP &map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, float ke, float kf, bool withBoundary, bool onlyBoundary);
//template void Algo::Render::GL2::TopoRender::updateDataGMap<PFP1>(PFP1::MAP &map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, bool onlyBoundary);
template void Algo::Render::GL2::TopoRender::drawColoredDarts<MAP1>(MAP1& map);



struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};
template void Algo::Render::GL2::TopoRender::updateData<PFP2>(PFP2::MAP &map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, float ke, float kf, bool withBoundary, bool onlyBoundary);
template void Algo::Render::GL2::TopoRender::updateData<PFP2>(PFP2::MAP &map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, bool onlyBoundary);
template void Algo::Render::GL2::TopoRender::updateDataBoundary<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, float ke, float kf, float ns);
template void Algo::Render::GL2::TopoRender::updateDataBoundary<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions);


struct PFPG1 : public PFP_STANDARD
{
	typedef EmbeddedGMap2 MAP;
};
typedef PFPG1::MAP GMAP1;

template void Algo::Render::GL2::TopoRender::overdrawDart<GMAP1>(GMAP1& map, Dart d, float width, float r, float g, float b);
template Dart Algo::Render::GL2::TopoRender::picking<GMAP1>(GMAP1& map, int x, int y, bool withBoundary);
template Dart Algo::Render::GL2::TopoRender::coneSelection<GMAP1>(GMAP1& map, const Geom::Vec3f& rayA, const Geom::Vec3f& rayAB, float angle);
template Dart Algo::Render::GL2::TopoRender::raySelection<GMAP1>(GMAP1& map, const Geom::Vec3f& rayA, const Geom::Vec3f& rayAB, float distmax);
template void Algo::Render::GL2::TopoRender::updateDataGMap<PFPG1>(PFPG1::MAP &map, const VertexAttribute<PFPG1::VEC3, PFPG1::MAP>& positions, float ke, float kf, bool withBoundary, bool onlyBoundary);
template void Algo::Render::GL2::TopoRender::updateDataGMap<PFPG1>(PFPG1::MAP &map, const VertexAttribute<PFPG1::VEC3, PFPG1::MAP>& positions, bool onlyBoundary);
template void Algo::Render::GL2::TopoRender::drawColoredDarts<GMAP1>(GMAP1& map);



struct PFPG2 : public PFP_DOUBLE
{
	typedef EmbeddedGMap2 MAP;
};
typedef PFPG2::MAP GMAP2;
template void Algo::Render::GL2::TopoRender::updateDataGMap<PFPG2>(PFPG2::MAP &map, const VertexAttribute<PFPG2::VEC3, PFPG2::MAP>& positions, float ke, float kf, bool withBoundary, bool onlyBoundary);
template void Algo::Render::GL2::TopoRender::updateDataGMap<PFPG2>(PFPG2::MAP &map, const VertexAttribute<PFPG2::VEC3, PFPG2::MAP>& positions, bool onlyBoundary);



int test_topoRender()
{

	return 0;
}
