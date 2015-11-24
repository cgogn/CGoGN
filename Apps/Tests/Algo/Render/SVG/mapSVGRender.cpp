#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap2.h"
#include "Topology/gmap/embeddedGMap3.h"


#include "Algo/Render/SVG/mapSVGRender.h"


using namespace CGoGN;


struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

template void Algo::Render::SVG::renderVertices<PFP1>(Utils::SVG::SVGOut& svg, PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template void Algo::Render::SVG::renderVertices<PFP1>(Utils::SVG::SVGOut& svg, PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& color);
template void Algo::Render::SVG::renderEdges<PFP1>(Utils::SVG::SVGOut& svg, PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template void Algo::Render::SVG::renderEdges<PFP1>(Utils::SVG::SVGOut& svg, PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& color);


struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

template void Algo::Render::SVG::renderVertices<PFP2>(Utils::SVG::SVGOut& svg, PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template void Algo::Render::SVG::renderVertices<PFP2>(Utils::SVG::SVGOut& svg, PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& color);
template void Algo::Render::SVG::renderEdges<PFP2>(Utils::SVG::SVGOut& svg, PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template void Algo::Render::SVG::renderEdges<PFP2>(Utils::SVG::SVGOut& svg, PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& color);


struct PFP3 : public PFP_DOUBLE
{
	typedef EmbeddedMap3 MAP;
};

template void Algo::Render::SVG::renderVertices<PFP3>(Utils::SVG::SVGOut& svg, PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
template void Algo::Render::SVG::renderVertices<PFP3>(Utils::SVG::SVGOut& svg, PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& color);
template void Algo::Render::SVG::renderEdges<PFP3>(Utils::SVG::SVGOut& svg, PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
template void Algo::Render::SVG::renderEdges<PFP3>(Utils::SVG::SVGOut& svg, PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& color);


struct PFP4 : public PFP_DOUBLE
{
	typedef EmbeddedGMap2 MAP;
};

template void Algo::Render::SVG::renderVertices<PFP4>(Utils::SVG::SVGOut& svg, PFP4::MAP& map, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& position);
template void Algo::Render::SVG::renderVertices<PFP4>(Utils::SVG::SVGOut& svg, PFP4::MAP& map, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& position, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& color);
template void Algo::Render::SVG::renderEdges<PFP4>(Utils::SVG::SVGOut& svg, PFP4::MAP& map, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& position);
template void Algo::Render::SVG::renderEdges<PFP4>(Utils::SVG::SVGOut& svg, PFP4::MAP& map, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& position, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& color);


struct PFP5 : public PFP_STANDARD
{
	typedef EmbeddedGMap3 MAP;
};

template void Algo::Render::SVG::renderVertices<PFP5>(Utils::SVG::SVGOut& svg, PFP5::MAP& map, const VertexAttribute<PFP5::VEC3, PFP5::MAP>& position);
template void Algo::Render::SVG::renderVertices<PFP5>(Utils::SVG::SVGOut& svg, PFP5::MAP& map, const VertexAttribute<PFP5::VEC3, PFP5::MAP>& position, const VertexAttribute<PFP5::VEC3, PFP5::MAP>& color);
template void Algo::Render::SVG::renderEdges<PFP5>(Utils::SVG::SVGOut& svg, PFP5::MAP& map, const VertexAttribute<PFP5::VEC3, PFP5::MAP>& position);
template void Algo::Render::SVG::renderEdges<PFP5>(Utils::SVG::SVGOut& svg, PFP5::MAP& map, const VertexAttribute<PFP5::VEC3, PFP5::MAP>& position, const VertexAttribute<PFP5::VEC3, PFP5::MAP>& color);



int test_mapSVGRender()
{

	return 0;
}