#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Selection/raySelector.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

struct PFP3 : public PFP_DOUBLE
{
	typedef EmbeddedGMap2 MAP;
};

struct PFP4 : public PFP_DOUBLE
{
	typedef EmbeddedMap3 MAP;
};


template void Algo::Selection::facesRaySelection<PFP1>(	PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
		const PFP1::VEC3& rayA, const PFP1::VEC3& rayAB, std::vector<Face>& vecFaces, std::vector<PFP1::VEC3>& iPoints);

template void Algo::Selection::facesRaySelection<PFP1>(	PFP1::MAP& map,	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
		const PFP1::VEC3& rayA,	const PFP1::VEC3& rayAB, std::vector<Face>& vecFaces);

template void Algo::Selection::faceRaySelection<PFP1>( PFP1::MAP& map,	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
		const PFP1::VEC3& rayA,	const PFP1::VEC3& rayAB, Face& face);

template void Algo::Selection::edgesRaySelection<PFP1>( PFP1::MAP& map,	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
		const PFP1::VEC3& rayA,	const PFP1::VEC3& rayAB, std::vector<Edge>& vecEdges, float distMax);

template void Algo::Selection::edgeRaySelection<PFP1>( PFP1::MAP& map,	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
		const PFP1::VEC3& rayA, const PFP1::VEC3& rayAB, Edge& edge);

template void Algo::Selection::verticesRaySelection<PFP1>( PFP1::MAP& map,	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
		const PFP1::VEC3& rayA, const PFP1::VEC3& rayAB, std::vector<Vertex>& vecVertices, float dist);

template void Algo::Selection::vertexRaySelection<PFP1>( PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
		const PFP1::VEC3& rayA,	const PFP1::VEC3& rayAB, Vertex& vertex);

template void Algo::Selection::volumesRaySelection<PFP1>( PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
		const PFP1::VEC3& rayA,	const PFP1::VEC3& rayAB, std::vector<Vol>& vecVolumes);

template void Algo::Selection::facesPlanSelection<PFP1>( PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
		const Geom::Plane3D<PFP1::VEC3::DATA_TYPE>& plan,	std::vector<Face>& vecFaces);

template void Algo::Selection::verticesConeSelection<PFP1>(	PFP1::MAP& map,	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
		const PFP1::VEC3& rayA,	const PFP1::VEC3& rayAB, float angle, std::vector<Vertex>& vecVertices);

template void Algo::Selection::edgesConeSelection<PFP1>(PFP1::MAP& map,	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
		const PFP1::VEC3& rayA,	const PFP1::VEC3& rayAB, float angle, std::vector<Edge>& vecEdges);

template Vertex Algo::Selection::verticesBubbleSelection<PFP1>(	PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
		const PFP1::VEC3& cursor, 	PFP1::REAL radiusMax); 

template Edge Algo::Selection::edgesBubbleSelection<PFP1>( PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
		const PFP1::VEC3& cursor, PFP1::REAL radiusMax);


// MAP2 DOUBLE
template void Algo::Selection::facesRaySelection<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const PFP2::VEC3& rayA, const PFP2::VEC3& rayAB, std::vector<Face>& vecFaces, std::vector<PFP2::VEC3>& iPoints);

template void Algo::Selection::facesRaySelection<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const PFP2::VEC3& rayA, const PFP2::VEC3& rayAB, std::vector<Face>& vecFaces);

template void Algo::Selection::faceRaySelection<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const PFP2::VEC3& rayA, const PFP2::VEC3& rayAB, Face& face);

template void Algo::Selection::edgesRaySelection<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const PFP2::VEC3& rayA, const PFP2::VEC3& rayAB, std::vector<Edge>& vecEdges, float distMax);

template void Algo::Selection::edgeRaySelection<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const PFP2::VEC3& rayA, const PFP2::VEC3& rayAB, Edge& edge);

template void Algo::Selection::verticesRaySelection<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const PFP2::VEC3& rayA, const PFP2::VEC3& rayAB, std::vector<Vertex>& vecVertices, float dist);

template void Algo::Selection::vertexRaySelection<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const PFP2::VEC3& rayA, const PFP2::VEC3& rayAB, Vertex& vertex);

template void Algo::Selection::volumesRaySelection<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const PFP2::VEC3& rayA, const PFP2::VEC3& rayAB, std::vector<Vol>& vecVolumes);

template void Algo::Selection::facesPlanSelection<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const Geom::Plane3D<PFP2::VEC3::DATA_TYPE>& plan, std::vector<Face>& vecFaces);

template void Algo::Selection::verticesConeSelection<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const PFP2::VEC3& rayA, const PFP2::VEC3& rayAB, float angle, std::vector<Vertex>& vecVertices);

template void Algo::Selection::edgesConeSelection<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const PFP2::VEC3& rayA, const PFP2::VEC3& rayAB, float angle, std::vector<Edge>& vecEdges);

template Vertex Algo::Selection::verticesBubbleSelection<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const PFP2::VEC3& cursor, PFP2::REAL radiusMax);

template Edge Algo::Selection::edgesBubbleSelection<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const PFP2::VEC3& cursor, PFP2::REAL radiusMax);

// GMAP2

template void Algo::Selection::facesRaySelection<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const PFP3::VEC3& rayA, const PFP3::VEC3& rayAB, std::vector<Face>& vecFaces, std::vector<PFP3::VEC3>& iPoints);

template void Algo::Selection::facesRaySelection<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const PFP3::VEC3& rayA, const PFP3::VEC3& rayAB, std::vector<Face>& vecFaces);

template void Algo::Selection::faceRaySelection<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const PFP3::VEC3& rayA, const PFP3::VEC3& rayAB, Face& face);

template void Algo::Selection::edgesRaySelection<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const PFP3::VEC3& rayA, const PFP3::VEC3& rayAB, std::vector<Edge>& vecEdges, float distMax);

template void Algo::Selection::edgeRaySelection<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const PFP3::VEC3& rayA, const PFP3::VEC3& rayAB, Edge& edge);

template void Algo::Selection::verticesRaySelection<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const PFP3::VEC3& rayA, const PFP3::VEC3& rayAB, std::vector<Vertex>& vecVertices, float dist);

template void Algo::Selection::vertexRaySelection<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const PFP3::VEC3& rayA, const PFP3::VEC3& rayAB, Vertex& vertex);

template void Algo::Selection::volumesRaySelection<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const PFP3::VEC3& rayA, const PFP3::VEC3& rayAB, std::vector<Vol>& vecVolumes);

template void Algo::Selection::facesPlanSelection<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const Geom::Plane3D<PFP3::VEC3::DATA_TYPE>& plan, std::vector<Face>& vecFaces);

template void Algo::Selection::verticesConeSelection<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const PFP3::VEC3& rayA, const PFP3::VEC3& rayAB, float angle, std::vector<Vertex>& vecVertices);

template void Algo::Selection::edgesConeSelection<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const PFP3::VEC3& rayA, const PFP3::VEC3& rayAB, float angle, std::vector<Edge>& vecEdges);

template Vertex Algo::Selection::verticesBubbleSelection<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const PFP3::VEC3& cursor, PFP3::REAL radiusMax);

template Edge Algo::Selection::edgesBubbleSelection<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const PFP3::VEC3& cursor, PFP3::REAL radiusMax);


// MAP3 double

template void Algo::Selection::facesRaySelection<PFP4>(PFP4::MAP& map, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& position,
	const PFP4::VEC3& rayA, const PFP4::VEC3& rayAB, std::vector<Face>& vecFaces, std::vector<PFP4::VEC3>& iPoints);

template void Algo::Selection::facesRaySelection<PFP4>(PFP4::MAP& map, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& position,
	const PFP4::VEC3& rayA, const PFP4::VEC3& rayAB, std::vector<Face>& vecFaces);

template void Algo::Selection::faceRaySelection<PFP4>(PFP4::MAP& map, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& position,
	const PFP4::VEC3& rayA, const PFP4::VEC3& rayAB, Face& face);

template void Algo::Selection::edgesRaySelection<PFP4>(PFP4::MAP& map, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& position,
	const PFP4::VEC3& rayA, const PFP4::VEC3& rayAB, std::vector<Edge>& vecEdges, float distMax);

template void Algo::Selection::edgeRaySelection<PFP4>(PFP4::MAP& map, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& position,
	const PFP4::VEC3& rayA, const PFP4::VEC3& rayAB, Edge& edge);

template void Algo::Selection::verticesRaySelection<PFP4>(PFP4::MAP& map, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& position,
	const PFP4::VEC3& rayA, const PFP4::VEC3& rayAB, std::vector<Vertex>& vecVertices, float dist);

template void Algo::Selection::vertexRaySelection<PFP4>(PFP4::MAP& map, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& position,
	const PFP4::VEC3& rayA, const PFP4::VEC3& rayAB, Vertex& vertex);

template void Algo::Selection::volumesRaySelection<PFP4>(PFP4::MAP& map, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& position,
	const PFP4::VEC3& rayA, const PFP4::VEC3& rayAB, std::vector<Vol>& vecVolumes);

template void Algo::Selection::facesPlanSelection<PFP4>(PFP4::MAP& map, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& position,
	const Geom::Plane3D<PFP4::VEC3::DATA_TYPE>& plan, std::vector<Face>& vecFaces);

template void Algo::Selection::verticesConeSelection<PFP4>(PFP4::MAP& map, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& position,
	const PFP4::VEC3& rayA, const PFP4::VEC3& rayAB, float angle, std::vector<Vertex>& vecVertices);

template void Algo::Selection::edgesConeSelection<PFP4>(PFP4::MAP& map, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& position,
	const PFP4::VEC3& rayA, const PFP4::VEC3& rayAB, float angle, std::vector<Edge>& vecEdges);

template Vertex Algo::Selection::verticesBubbleSelection<PFP4>(PFP4::MAP& map, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& position,
	const PFP4::VEC3& cursor, PFP4::REAL radiusMax);

template Edge Algo::Selection::edgesBubbleSelection<PFP4>(PFP4::MAP& map, const VertexAttribute<PFP4::VEC3, PFP4::MAP>& position,
	const PFP4::VEC3& cursor, PFP4::REAL radiusMax);





int test_raySelector()
{
	return 0;
}

