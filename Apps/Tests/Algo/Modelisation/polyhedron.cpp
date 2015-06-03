#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Modelisation/polyhedron.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

template void Algo::Surface::Modelisation::quads2TrianglesCC<PFP1>(PFP1::MAP& the_map, Dart primd);
//template void Algo::Surface::Modelisation::explodPolyhedron<PFP1>(PFP1::MAP& map, Dart d, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template Dart Algo::Surface::Modelisation::createPyramid<PFP1>(PFP1::MAP& map, unsigned int nbSides, bool withBoundary);
template Dart Algo::Surface::Modelisation::createPrism<PFP1>(PFP1::MAP& map, unsigned int nbSides, bool withBoundary);
template Dart Algo::Surface::Modelisation::createDiamond<PFP1>(PFP1::MAP& map, unsigned int nbSides, bool withBoundary);
template Dart Algo::Surface::Modelisation::createTetrahedron<PFP1>(PFP1::MAP& map, bool withBoundary);
template Dart Algo::Surface::Modelisation::createHexahedron<PFP1>(PFP1::MAP& map, bool withBoundary);
template Dart Algo::Surface::Modelisation::createTriangularPrism<PFP1>(PFP1::MAP& map, bool withBoundary);
template Dart Algo::Surface::Modelisation::createQuadrangularPyramid<PFP1>(PFP1::MAP& map, bool withBoundary);
template Dart Algo::Surface::Modelisation::createOctahedron<PFP1>(PFP1::MAP& map, bool withBoundary);
template bool Algo::Surface::Modelisation::isPyra<PFP1>(PFP1::MAP& map, Dart d);
template bool Algo::Surface::Modelisation::isPrism<PFP1>(PFP1::MAP& map, Dart d);
template bool Algo::Surface::Modelisation::isHexahedron<PFP1>(PFP1::MAP& the_map, Dart d);
template Dart Algo::Surface::Modelisation::embedPrism<PFP1>(PFP1::MAP& map, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, unsigned int n, bool withBoundary, float bottom_radius, float top_radius, float height);
template Dart Algo::Surface::Modelisation::embedPyramid<PFP1>(PFP1::MAP& map, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, unsigned int n, bool withBoundary, float radius, float height);


struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

template void Algo::Surface::Modelisation::quads2TrianglesCC<PFP2>(PFP2::MAP& the_map, Dart primd);
//template void Algo::Surface::Modelisation::explodPolyhedron<PFP2>(PFP2::MAP& map, Dart d, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template Dart Algo::Surface::Modelisation::createPyramid<PFP2>(PFP2::MAP& map, unsigned int nbSides, bool withBoundary);
template Dart Algo::Surface::Modelisation::createPrism<PFP2>(PFP2::MAP& map, unsigned int nbSides, bool withBoundary);
template Dart Algo::Surface::Modelisation::createDiamond<PFP2>(PFP2::MAP& map, unsigned int nbSides, bool withBoundary);
template Dart Algo::Surface::Modelisation::createTetrahedron<PFP2>(PFP2::MAP& map, bool withBoundary);
template Dart Algo::Surface::Modelisation::createHexahedron<PFP2>(PFP2::MAP& map, bool withBoundary);
template Dart Algo::Surface::Modelisation::createTriangularPrism<PFP2>(PFP2::MAP& map, bool withBoundary);
template Dart Algo::Surface::Modelisation::createQuadrangularPyramid<PFP2>(PFP2::MAP& map, bool withBoundary);
template Dart Algo::Surface::Modelisation::createOctahedron<PFP2>(PFP2::MAP& map, bool withBoundary);
template bool Algo::Surface::Modelisation::isPyra<PFP2>(PFP2::MAP& map, Dart d);
template bool Algo::Surface::Modelisation::isPrism<PFP2>(PFP2::MAP& map, Dart d);
template bool Algo::Surface::Modelisation::isHexahedron<PFP2>(PFP2::MAP& the_map, Dart d);
template Dart Algo::Surface::Modelisation::embedPrism<PFP2>(PFP2::MAP& map, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, unsigned int n, bool withBoundary, float bottom_radius, float top_radius, float height);
template Dart Algo::Surface::Modelisation::embedPyramid<PFP2>(PFP2::MAP& map, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, unsigned int n, bool withBoundary, float radius, float height);


//struct PFP3 : public PFP_DOUBLE
//{
//	typedef EmbeddedGMap2 MAP;
//};
//
//template void Algo::Surface::Modelisation::quads2TrianglesCC<PFP3>(PFP3::MAP& the_map, Dart primd);
//template void Algo::Surface::Modelisation::explodPolyhedron<PFP3>(PFP3::MAP& map, Dart d, VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
//template Dart Algo::Surface::Modelisation::createPyramid<PFP3>(PFP3::MAP& map, unsigned int nbSides, bool withBoundary);
//template Dart Algo::Surface::Modelisation::createPrism<PFP3>(PFP3::MAP& map, unsigned int nbSides, bool withBoundary);
//template Dart Algo::Surface::Modelisation::createDiamond<PFP3>(PFP3::MAP& map, unsigned int nbSides, bool withBoundary);
//template Dart Algo::Surface::Modelisation::createTetrahedron<PFP3>(PFP3::MAP& map, bool withBoundary);
//template Dart Algo::Surface::Modelisation::createHexahedron<PFP3>(PFP3::MAP& map, bool withBoundary);
//template Dart Algo::Surface::Modelisation::createTriangularPrism<PFP3>(PFP3::MAP& map, bool withBoundary);
//template Dart Algo::Surface::Modelisation::createQuadrangularPyramid<PFP3>(PFP3::MAP& map, bool withBoundary);
//template Dart Algo::Surface::Modelisation::createOctahedron<PFP3>(PFP3::MAP& map, bool withBoundary);
//template bool Algo::Surface::Modelisation::isPyra<PFP3>(PFP3::MAP& map, Dart d);
//template bool Algo::Surface::Modelisation::isPrism<PFP3>(PFP3::MAP& map, Dart d);
//template bool Algo::Surface::Modelisation::isHexahedron<PFP3>(PFP3::MAP& the_map, Dart d);
//template Dart Algo::Surface::Modelisation::embedPrism<PFP3>(PFP3::MAP& map, VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, unsigned int n, bool withBoundary, float bottom_radius, float top_radius, float height);
//template Dart Algo::Surface::Modelisation::embedPyramid<PFP3>(PFP3::MAP& map, VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, unsigned int n, bool withBoundary, float radius, float height);


int test_polyhedron()
{

	return 0;
}
