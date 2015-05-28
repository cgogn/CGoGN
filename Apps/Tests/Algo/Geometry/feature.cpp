#include <iostream>
#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"

#include "Algo/Geometry/feature.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};



template void Algo::Surface::Geometry::featureEdgeDetection<PFP1>(
	PFP1::MAP& map,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
	CellMarker<PFP1::MAP, EDGE>& featureEdge) ;

template void Algo::Surface::Geometry::computeFaceGradient<PFP1>(
	PFP1::MAP& map,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
	const FaceAttribute<PFP1::VEC3, PFP1::MAP>& face_normal,
	const VertexAttribute<PFP1::REAL, PFP1::MAP>& scalar,
	const FaceAttribute<PFP1::REAL, PFP1::MAP>& face_area,
	FaceAttribute<PFP1::VEC3, PFP1::MAP>& face_gradient) ;

template PFP1::VEC3 Algo::Surface::Geometry::faceGradient<PFP1>(
	PFP1::MAP& map,
	Dart d,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
	const FaceAttribute<PFP1::VEC3, PFP1::MAP>& face_normal,
	const VertexAttribute<PFP1::REAL, PFP1::MAP>& scalar,
	const FaceAttribute<PFP1::REAL, PFP1::MAP>& face_area) ;

template void Algo::Surface::Geometry::computeVertexGradient<PFP1>(
	PFP1::MAP& map,
	const FaceAttribute<PFP1::VEC3, PFP1::MAP>& face_gradient,
	const FaceAttribute<PFP1::REAL, PFP1::MAP>& face_area,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& vertex_gradient) ;

template PFP1::VEC3 Algo::Surface::Geometry::vertexGradient<PFP1>(
	PFP1::MAP& map,
	Dart d,
	const FaceAttribute<PFP1::VEC3, PFP1::MAP>& face_gradient,
	const FaceAttribute<PFP1::REAL, PFP1::MAP>& area) ;


template void Algo::Surface::Geometry::computeTriangleType<PFP1>(
	PFP1::MAP& map,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& K,
	CellMarker<PFP1::MAP, FACE>& regularMarker) ;

template bool Algo::Surface::Geometry::isTriangleRegular<PFP1>(
	PFP1::MAP& map,
	Dart d,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& K) ;

template void Algo::Surface::Geometry::initRidgeSegments<PFP1>(
	PFP1::MAP& map,
	FaceAttribute<RidgeSegment<PFP1::REAL>, PFP1::MAP>& ridge_segments) ;

template void Algo::Surface::Geometry::computeRidgeLines<PFP1>(
	PFP1::MAP& map,
	CellMarker<PFP1::MAP, FACE>& regularMarker,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& K,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& vertex_gradient,
	const VertexAttribute<PFP1::REAL, PFP1::MAP>& k,
	const VertexAttribute<PFP1::REAL, PFP1::MAP>& k2,
	FaceAttribute<RidgeSegment<PFP1::REAL>, PFP1::MAP>& ridge_segments) ;


template void Algo::Surface::Geometry::ridgeLines<PFP1>(
	PFP1::MAP& map,
	Dart d,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& K,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& vertex_gradient,
	const VertexAttribute<PFP1::REAL, PFP1::MAP>& k,
	const VertexAttribute<PFP1::REAL, PFP1::MAP>& k2,
	FaceAttribute<RidgeSegment<PFP1::REAL>, PFP1::MAP>& ridge_segments) ;

template void Algo::Surface::Geometry::computeSingularTriangle<PFP1>(
	PFP1::MAP& map,
	CellMarker<PFP1::MAP, FACE>& regularMarker,
	FaceAttribute<RidgeSegment<PFP1::REAL>, PFP1::MAP>& ridge_segments) ;

template void Algo::Surface::Geometry::singularTriangle<PFP1>(
	PFP1::MAP& map,
	Dart d,
	CellMarker<PFP1::MAP, FACE>& regularMarker,
	FaceAttribute<RidgeSegment<PFP1::REAL>, PFP1::MAP>& ridge_segments) ;

template bool Algo::Surface::Geometry::isEdgeInTriangle<PFP1>(
	PFP1::MAP& map,
	Dart edge,
	Dart triangle) ;

template std::vector<PFP1::VEC3> Algo::Surface::Geometry::occludingContoursDetection<PFP1>(
	PFP1::MAP& map,
	const PFP1::VEC3& cameraPosition,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& normal) ;


// WITH DOUBLE

template void Algo::Surface::Geometry::featureEdgeDetection<PFP2>(
	PFP2::MAP& map,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	CellMarker<PFP2::MAP, EDGE>& featureEdge);

template void Algo::Surface::Geometry::computeFaceGradient<PFP2>(
	PFP2::MAP& map,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const FaceAttribute<PFP2::VEC3, PFP2::MAP>& face_normal,
	const VertexAttribute<PFP2::REAL, PFP2::MAP>& scalar,
	const FaceAttribute<PFP2::REAL, PFP2::MAP>& face_area,
	FaceAttribute<PFP2::VEC3, PFP2::MAP>& face_gradient);

template PFP2::VEC3 Algo::Surface::Geometry::faceGradient<PFP2>(
	PFP2::MAP& map,
	Dart d,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const FaceAttribute<PFP2::VEC3, PFP2::MAP>& face_normal,
	const VertexAttribute<PFP2::REAL, PFP2::MAP>& scalar,
	const FaceAttribute<PFP2::REAL, PFP2::MAP>& face_area);

template void Algo::Surface::Geometry::computeVertexGradient<PFP2>(
	PFP2::MAP& map,
	const FaceAttribute<PFP2::VEC3, PFP2::MAP>& face_gradient,
	const FaceAttribute<PFP2::REAL, PFP2::MAP>& face_area,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& vertex_gradient);

template PFP2::VEC3 Algo::Surface::Geometry::vertexGradient<PFP2>(
	PFP2::MAP& map,
	Dart d,
	const FaceAttribute<PFP2::VEC3, PFP2::MAP>& face_gradient,
	const FaceAttribute<PFP2::REAL, PFP2::MAP>& area);


template void Algo::Surface::Geometry::computeTriangleType<PFP2>(
	PFP2::MAP& map,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& K,
	CellMarker<PFP2::MAP, FACE>& regularMarker);

template bool Algo::Surface::Geometry::isTriangleRegular<PFP2>(
	PFP2::MAP& map,
	Dart d,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& K);

template void Algo::Surface::Geometry::initRidgeSegments<PFP2>(
	PFP2::MAP& map,
	FaceAttribute<RidgeSegment<PFP2::REAL>, PFP2::MAP>& ridge_segments);

template void Algo::Surface::Geometry::computeRidgeLines<PFP2>(
	PFP2::MAP& map,
	CellMarker<PFP2::MAP, FACE>& regularMarker,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& K,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& vertex_gradient,
	const VertexAttribute<PFP2::REAL, PFP2::MAP>& k,
	const VertexAttribute<PFP2::REAL, PFP2::MAP>& k2,
	FaceAttribute<RidgeSegment<PFP2::REAL>, PFP2::MAP>& ridge_segments);

template void Algo::Surface::Geometry::ridgeLines<PFP2>(
	PFP2::MAP& map,
	Dart d,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& K,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& vertex_gradient,
	const VertexAttribute<PFP2::REAL, PFP2::MAP>& k,
	const VertexAttribute<PFP2::REAL, PFP2::MAP>& k2,
	FaceAttribute<RidgeSegment<PFP2::REAL>, PFP2::MAP>& ridge_segments);

template void Algo::Surface::Geometry::computeSingularTriangle<PFP2>(
	PFP2::MAP& map,
	CellMarker<PFP2::MAP, FACE>& regularMarker,
	FaceAttribute<RidgeSegment<PFP2::REAL>, PFP2::MAP>& ridge_segments);

template void Algo::Surface::Geometry::singularTriangle<PFP2>(
	PFP2::MAP& map,
	Dart d,
	CellMarker<PFP2::MAP, FACE>& regularMarker,
	FaceAttribute<RidgeSegment<PFP2::REAL>, PFP2::MAP>& ridge_segments);

template bool Algo::Surface::Geometry::isEdgeInTriangle<PFP2>(
	PFP2::MAP& map,
	Dart edge,
	Dart triangle);

template std::vector<PFP2::VEC3> Algo::Surface::Geometry::occludingContoursDetection<PFP2>(
	PFP2::MAP& map,
	const PFP2::VEC3& cameraPosition,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& normal);


int test_feature()
{
	return 0;
}
