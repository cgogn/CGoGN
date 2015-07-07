#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Render/GL2/colorPerEdgeRender.h"

using namespace CGoGN;


struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};
typedef EdgeAttribute<Geom::Vec3d,PFP1::MAP> ATT1;

template void Algo::Render::GL2::ColorPerEdgeRender::updateVBO<PFP1, ATT1>(Utils::VBO& vboPosition, Utils::VBO& vboColor, PFP1::MAP& map,
	const VertexAttribute<PFP1::VEC3,PFP1::MAP>& positions, const ATT1& colorPerXXX);



struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap3 MAP;
};
typedef AttributeHandler<PFP2::VEC3, VERTEX2, PFP2::MAP> ATT2;

template void Algo::Render::GL2::ColorPerEdgeRender::updateVBO<PFP2, ATT2>(Utils::VBO& vboPosition, Utils::VBO& vboColor, PFP2::MAP& map,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, const ATT2& colorPerXXX);



int test_colorPerEdgeRender()
{

	return 0;
}
