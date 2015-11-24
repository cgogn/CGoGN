#include <iostream>
#include "Topology/generic/parameters.h"
#include "Topology/gmap/embeddedGMap2.h"
#include "Topology/map/embeddedMap2.h"

#include "Algo/Geometry/area.h"


using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

struct PFP3 : public PFP_STANDARD
{
	typedef EmbeddedGMap2 MAP;
};


using namespace CGoGN;


/*****************************************
*		AREA INSTANTIATION
*****************************************/

template PFP1::REAL Algo::Surface::Geometry::triangleArea<PFP1>(PFP1::MAP& map, Face f, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template PFP1::REAL Algo::Surface::Geometry::convexFaceArea<PFP1>(PFP1::MAP& map, Face f, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template PFP1::REAL Algo::Surface::Geometry::totalArea<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template PFP1::REAL Algo::Surface::Geometry::vertexOneRingArea<PFP1>(PFP1::MAP& map, Vertex v, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template PFP1::REAL Algo::Surface::Geometry::vertexBarycentricArea<PFP1>(PFP1::MAP& map, Vertex v, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template PFP1::REAL Algo::Surface::Geometry::vertexVoronoiArea<PFP1>(PFP1::MAP& map, Vertex v, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template PFP1::REAL  Algo::Surface::Geometry::edgeArea<PFP1>(PFP1::MAP& map, Edge e, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template void Algo::Surface::Geometry::computeAreaFaces<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, FaceAttribute<PFP1::REAL, PFP1::MAP>& face_area);
template void Algo::Surface::Geometry::computeAreaEdges<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, EdgeAttribute<PFP1::REAL, PFP1::MAP>& edge_area);
template void Algo::Surface::Geometry::computeOneRingAreaVertices<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, VertexAttribute<PFP1::REAL, PFP1::MAP>& vertex_area);
template void Algo::Surface::Geometry::computeBarycentricAreaVertices<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, VertexAttribute<PFP1::REAL, PFP1::MAP>& vertex_area);
template void Algo::Surface::Geometry::computeVoronoiAreaVertices<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, VertexAttribute<PFP1::REAL, PFP1::MAP>& vertex_area);

template PFP1::REAL Algo::Surface::Geometry::Parallel::totalArea<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template void Algo::Surface::Geometry::Parallel::computeAreaFaces<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, FaceAttribute<PFP1::REAL, PFP1::MAP>& area);
template void Algo::Surface::Geometry::Parallel::computeAreaEdges<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, EdgeAttribute<PFP1::REAL, PFP1::MAP>& area);
template void Algo::Surface::Geometry::Parallel::computeOneRingAreaVertices<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, VertexAttribute<PFP1::REAL, PFP1::MAP>& area);
template void Algo::Surface::Geometry::Parallel::computeBarycentricAreaVertices<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, VertexAttribute<PFP1::REAL, PFP1::MAP>& vertex_area);
template void Algo::Surface::Geometry::Parallel::computeVoronoiAreaVertices<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, VertexAttribute<PFP1::REAL, PFP1::MAP>& area);


template PFP2::REAL Algo::Surface::Geometry::triangleArea<PFP2>(PFP2::MAP& map, Face f, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template PFP2::REAL Algo::Surface::Geometry::convexFaceArea<PFP2>(PFP2::MAP& map, Face f, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template PFP2::REAL Algo::Surface::Geometry::totalArea<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template PFP2::REAL Algo::Surface::Geometry::vertexOneRingArea<PFP2>(PFP2::MAP& map, Vertex v, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template PFP2::REAL Algo::Surface::Geometry::vertexBarycentricArea<PFP2>(PFP2::MAP& map, Vertex v, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template PFP2::REAL Algo::Surface::Geometry::vertexVoronoiArea<PFP2>(PFP2::MAP& map, Vertex v, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template PFP2::REAL  Algo::Surface::Geometry::edgeArea<PFP2>(PFP2::MAP& map, Edge e, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template void Algo::Surface::Geometry::computeAreaFaces<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, FaceAttribute<PFP2::REAL, PFP2::MAP>& face_area);
template void Algo::Surface::Geometry::computeAreaEdges<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, EdgeAttribute<PFP2::REAL, PFP2::MAP>& edge_area);
template void Algo::Surface::Geometry::computeOneRingAreaVertices<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, VertexAttribute<PFP2::REAL, PFP2::MAP>& vertex_area);
template void Algo::Surface::Geometry::computeBarycentricAreaVertices<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, VertexAttribute<PFP2::REAL, PFP2::MAP>& vertex_area);
template void Algo::Surface::Geometry::computeVoronoiAreaVertices<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, VertexAttribute<PFP2::REAL, PFP2::MAP>& vertex_area);

template PFP2::REAL Algo::Surface::Geometry::Parallel::totalArea<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template void Algo::Surface::Geometry::Parallel::computeAreaFaces<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, FaceAttribute<PFP2::REAL, PFP2::MAP>& area);
template void Algo::Surface::Geometry::Parallel::computeAreaEdges<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, EdgeAttribute<PFP2::REAL, PFP2::MAP>& area);
template void Algo::Surface::Geometry::Parallel::computeOneRingAreaVertices<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, VertexAttribute<PFP2::REAL, PFP2::MAP>& area);
template void Algo::Surface::Geometry::Parallel::computeBarycentricAreaVertices<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, VertexAttribute<PFP2::REAL, PFP2::MAP>& vertex_area);
template void Algo::Surface::Geometry::Parallel::computeVoronoiAreaVertices<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, VertexAttribute<PFP2::REAL, PFP2::MAP>& area);

template PFP3::REAL Algo::Surface::Geometry::triangleArea<PFP3>(PFP3::MAP& map, Face f, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
template PFP3::REAL Algo::Surface::Geometry::convexFaceArea<PFP3>(PFP3::MAP& map, Face f, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
template PFP3::REAL Algo::Surface::Geometry::totalArea<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
template PFP3::REAL Algo::Surface::Geometry::vertexOneRingArea<PFP3>(PFP3::MAP& map, Vertex v, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
template PFP3::REAL Algo::Surface::Geometry::vertexBarycentricArea<PFP3>(PFP3::MAP& map, Vertex v, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
template PFP3::REAL Algo::Surface::Geometry::vertexVoronoiArea<PFP3>(PFP3::MAP& map, Vertex v, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
template PFP3::REAL  Algo::Surface::Geometry::edgeArea<PFP3>(PFP3::MAP& map, Edge e, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
template void Algo::Surface::Geometry::computeAreaFaces<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, FaceAttribute<PFP3::REAL, PFP3::MAP>& face_area);
template void Algo::Surface::Geometry::computeAreaEdges<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, EdgeAttribute<PFP3::REAL, PFP3::MAP>& edge_area);
template void Algo::Surface::Geometry::computeOneRingAreaVertices<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, VertexAttribute<PFP3::REAL, PFP3::MAP>& vertex_area);
template void Algo::Surface::Geometry::computeBarycentricAreaVertices<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, VertexAttribute<PFP3::REAL, PFP3::MAP>& vertex_area);
template void Algo::Surface::Geometry::computeVoronoiAreaVertices<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, VertexAttribute<PFP3::REAL, PFP3::MAP>& vertex_area);

template PFP3::REAL Algo::Surface::Geometry::Parallel::totalArea<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
template void Algo::Surface::Geometry::Parallel::computeAreaFaces<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, FaceAttribute<PFP3::REAL, PFP3::MAP>& area);
template void Algo::Surface::Geometry::Parallel::computeAreaEdges<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, EdgeAttribute<PFP3::REAL, PFP3::MAP>& area);
template void Algo::Surface::Geometry::Parallel::computeOneRingAreaVertices<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, VertexAttribute<PFP3::REAL, PFP3::MAP>& area);
template void Algo::Surface::Geometry::Parallel::computeBarycentricAreaVertices<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, VertexAttribute<PFP3::REAL, PFP3::MAP>& vertex_area);
template void Algo::Surface::Geometry::Parallel::computeVoronoiAreaVertices<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, VertexAttribute<PFP3::REAL, PFP3::MAP>& area);


int test_area()
{
	return 0;
}
