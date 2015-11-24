#include <iostream>
#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"

#include "Algo/Geometry/laplacian.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};
typedef float ATTR;

template ATTR Algo::Surface::Geometry::computeLaplacianTopoVertex<PFP1,ATTR> (
	PFP1::MAP& map,
	Dart d,
	const VertexAttribute<ATTR, PFP1::MAP>& attr) ;

template ATTR Algo::Surface::Geometry::computeLaplacianCotanVertex<PFP1, ATTR>(
	PFP1::MAP& map,
	Dart d,
	const EdgeAttribute<PFP1::REAL, PFP1::MAP>& edgeWeight,
	const VertexAttribute<PFP1::REAL, PFP1::MAP>& vertexArea,
	const VertexAttribute<ATTR, PFP1::MAP>& attr) ;

template void Algo::Surface::Geometry::computeLaplacianTopoVertices<PFP1, ATTR>(
	PFP1::MAP& map,
	const VertexAttribute<ATTR, PFP1::MAP>& attr,
	VertexAttribute<ATTR, PFP1::MAP>& laplacian) ;

template void Algo::Surface::Geometry::computeLaplacianCotanVertices<PFP1, ATTR>(
	PFP1::MAP& map,
	const EdgeAttribute<PFP1::REAL, PFP1::MAP>& edgeWeight,
	const VertexAttribute<PFP1::REAL, PFP1::MAP>& vertexArea,
	const VertexAttribute<ATTR, PFP1::MAP>& attr,
	VertexAttribute<ATTR, PFP1::MAP>& laplacian) ;

template PFP1::REAL Algo::Surface::Geometry::computeCotanWeightEdge<PFP1>(
	PFP1::MAP& map,
	Dart d,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position) ;

template void Algo::Surface::Geometry::computeCotanWeightEdges<PFP1>(
	PFP1::MAP& map,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
	EdgeAttribute<PFP1::REAL, PFP1::MAP>& edgeWeight) ;


struct PFP3: public PFP_STANDARD
{
	typedef EmbeddedMap3 MAP;
};

template ATTR Algo::Volume::Geometry::computeLaplacianTopoVertex<PFP3,ATTR>(
	PFP3::MAP& map,
	Dart d,
	const VertexAttribute<ATTR, PFP3::MAP>& attr) ;

template void Algo::Volume::Geometry::computeLaplacianTopoVertices<PFP3, ATTR>(
	PFP3::MAP& map,
	const VertexAttribute<ATTR, PFP3::MAP>& attr,
	VertexAttribute<ATTR, PFP3::MAP>& laplacian) ;


int test_laplacian()
{
	return 0;
}