#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Render/GL2/colorPerFaceRender.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};


template void Algo::Render::GL2::ColorPerFaceRender::updateVBO<PFP1,FACE>(Utils::VBO& vboPosition, Utils::VBO& vboColor, PFP1::MAP& map,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, const AttributeHandler<PFP1::VEC3, FACE, PFP1::MAP>& colorPerXXX);

template void Algo::Render::GL2::ColorPerFaceRender::updateVBO<PFP1, FACE>(Utils::VBO& vboPosition, Utils::VBO& vboNormal, Utils::VBO& vboColor, PFP1::MAP& map,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& normals,
	const AttributeHandler<PFP1::VEC3, FACE, PFP1::MAP>& colorPerXXX);


template void Algo::Render::GL2::ColorPerFaceRender::updateVBO<PFP1, VERTEX2>(Utils::VBO& vboPosition, Utils::VBO& vboColor, PFP1::MAP& map,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, const AttributeHandler<PFP1::VEC3, VERTEX2, PFP1::MAP>& colorPerXXX);

template void Algo::Render::GL2::ColorPerFaceRender::updateVBO<PFP1, VERTEX2>(Utils::VBO& vboPosition, Utils::VBO& vboNormal, Utils::VBO& vboColor, PFP1::MAP& map,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& normals,
	const AttributeHandler<PFP1::VEC3, VERTEX2, PFP1::MAP>& colorPerXXX);


struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

template void Algo::Render::GL2::ColorPerFaceRender::updateVBO<PFP2, FACE>(Utils::VBO& vboPosition, Utils::VBO& vboColor, PFP2::MAP& map,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, const AttributeHandler<PFP2::VEC3, FACE, PFP2::MAP>& colorPerXXX);

template void Algo::Render::GL2::ColorPerFaceRender::updateVBO<PFP2, FACE>(Utils::VBO& vboPosition, Utils::VBO& vboNormal, Utils::VBO& vboColor, PFP2::MAP& map,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& normals,
	const AttributeHandler<PFP2::VEC3, FACE, PFP2::MAP>& colorPerXXX);


template void Algo::Render::GL2::ColorPerFaceRender::updateVBO<PFP2, VERTEX2>(Utils::VBO& vboPosition, Utils::VBO& vboColor, PFP2::MAP& map,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, const AttributeHandler<PFP2::VEC3, VERTEX2, PFP2::MAP>& colorPerXXX);

template void Algo::Render::GL2::ColorPerFaceRender::updateVBO<PFP2, VERTEX2>(Utils::VBO& vboPosition, Utils::VBO& vboNormal, Utils::VBO& vboColor, PFP2::MAP& map,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& normals,
	const AttributeHandler<PFP2::VEC3, VERTEX2, PFP2::MAP>& colorPerXXX);



struct PFP3 : public PFP_DOUBLE
{
	typedef EmbeddedGMap2 MAP;
};

template void Algo::Render::GL2::ColorPerFaceRender::updateVBO<PFP3, FACE>(Utils::VBO& vboPosition, Utils::VBO& vboColor, PFP3::MAP& map,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, const AttributeHandler<PFP3::VEC3, FACE, PFP3::MAP>& colorPerXXX);

template void Algo::Render::GL2::ColorPerFaceRender::updateVBO<PFP3, FACE>(Utils::VBO& vboPosition, Utils::VBO& vboNormal, Utils::VBO& vboColor, PFP3::MAP& map,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& normals,
	const AttributeHandler<PFP3::VEC3, FACE, PFP3::MAP>& colorPerXXX);


template void Algo::Render::GL2::ColorPerFaceRender::updateVBO<PFP3, VERTEX2>(Utils::VBO& vboPosition, Utils::VBO& vboColor, PFP3::MAP& map,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, const AttributeHandler<PFP3::VEC3, VERTEX2, PFP3::MAP>& colorPerXXX);

template void Algo::Render::GL2::ColorPerFaceRender::updateVBO<PFP3, VERTEX2>(Utils::VBO& vboPosition, Utils::VBO& vboNormal, Utils::VBO& vboColor, PFP3::MAP& map,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& normals,
	const AttributeHandler<PFP3::VEC3, VERTEX2, PFP3::MAP>& colorPerXXX);




struct PFP4 : public PFP_DOUBLE
{
	typedef EmbeddedMap3 MAP;
};

template void Algo::Render::GL2::ColorPerFaceRender::updateVBO<PFP4, FACE>(Utils::VBO& vboPosition, Utils::VBO& vboColor, PFP4::MAP& map,
	const VertexAttribute<PFP4::VEC3, PFP4::MAP>& positions, const AttributeHandler<PFP4::VEC3, FACE, PFP4::MAP>& colorPerXXX);

template void Algo::Render::GL2::ColorPerFaceRender::updateVBO<PFP4, FACE>(Utils::VBO& vboPosition, Utils::VBO& vboNormal, Utils::VBO& vboColor, PFP4::MAP& map,
	const VertexAttribute<PFP4::VEC3, PFP4::MAP>& positions, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& normals,
	const AttributeHandler<PFP4::VEC3, FACE, PFP4::MAP>& colorPerXXX);


template void Algo::Render::GL2::ColorPerFaceRender::updateVBO<PFP4, VERTEX2>(Utils::VBO& vboPosition, Utils::VBO& vboColor, PFP4::MAP& map,
	const VertexAttribute<PFP4::VEC3, PFP4::MAP>& positions, const AttributeHandler<PFP4::VEC3, VERTEX2, PFP4::MAP>& colorPerXXX);

template void Algo::Render::GL2::ColorPerFaceRender::updateVBO<PFP4, VERTEX2>(Utils::VBO& vboPosition, Utils::VBO& vboNormal, Utils::VBO& vboColor, PFP4::MAP& map,
	const VertexAttribute<PFP4::VEC3, PFP4::MAP>& positions, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& normals,
	const AttributeHandler<PFP4::VEC3, VERTEX2, PFP4::MAP>& colorPerXXX);


int test_colorPerFaceRender()
{

	return 0;
}
