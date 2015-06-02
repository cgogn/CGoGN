#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Render/GL2/dataPerFaceRender.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};
typedef double SCAL;

template void Algo::Render::GL2::DataPerFaceRender::updateVBO<PFP1, FACE, SCAL>(Utils::VBO& vboPosition, Utils::VBO& vboData, PFP1::MAP& map,
const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, const AttributeHandler<SCAL, FACE, PFP1::MAP>& dataPerXXX);

template void Algo::Render::GL2::DataPerFaceRender::updateVBO<PFP1, VERTEX2, SCAL>(Utils::VBO& vboPosition, Utils::VBO& vboData, PFP1::MAP& map,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, const AttributeHandler<SCAL, VERTEX2, PFP1::MAP>& dataPerXXX);


typedef Geom::Vec4d VEC;

template void Algo::Render::GL2::DataPerFaceRender::updateVBO<PFP1, FACE, VEC>(Utils::VBO& vboPosition, Utils::VBO& vboData, PFP1::MAP& map,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, const AttributeHandler<VEC, FACE, PFP1::MAP>& dataPerXXX);

template void Algo::Render::GL2::DataPerFaceRender::updateVBO<PFP1, VERTEX2, VEC>(Utils::VBO& vboPosition, Utils::VBO& vboData, PFP1::MAP& map,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, const AttributeHandler<VEC, VERTEX2, PFP1::MAP>& dataPerXXX);


struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap3 MAP;
};
typedef float SCAL2;

template void Algo::Render::GL2::DataPerFaceRender::updateVBO<PFP2, FACE, SCAL2>(Utils::VBO& vboPosition, Utils::VBO& vboData, PFP2::MAP& map,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, const AttributeHandler<SCAL2, FACE, PFP2::MAP>& dataPerXXX);

template void Algo::Render::GL2::DataPerFaceRender::updateVBO<PFP2, VERTEX2, SCAL2>(Utils::VBO& vboPosition, Utils::VBO& vboData, PFP2::MAP& map,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, const AttributeHandler<SCAL2, VERTEX2, PFP2::MAP>& dataPerXXX);


typedef Geom::Vec4f VEC2;

template void Algo::Render::GL2::DataPerFaceRender::updateVBO<PFP2, FACE, VEC2>(Utils::VBO& vboPosition, Utils::VBO& vboData, PFP2::MAP& map,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, const AttributeHandler<VEC2, FACE, PFP2::MAP>& dataPerXXX);

template void Algo::Render::GL2::DataPerFaceRender::updateVBO<PFP2, VERTEX2, VEC2>(Utils::VBO& vboPosition, Utils::VBO& vboData, PFP2::MAP& map,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, const AttributeHandler<VEC2, VERTEX2, PFP2::MAP>& dataPerXXX);



int test_dataPerFaceRender()
{

	return 0;
}
