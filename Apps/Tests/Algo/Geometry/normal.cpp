#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"
#include "Topology/map/embeddedMap3.h"

#include "Algo/Geometry/normal.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

typedef VertexAttribute<PFP1::VEC3, PFP1::MAP> VATT1;
typedef FaceAttribute<PFP1::VEC3, PFP1::MAP> FATT1;
typedef EdgeAttribute<PFP1::REAL, PFP1::MAP> EATT1;

template VATT1::DATA_TYPE Algo::Surface::Geometry::triangleNormal<PFP1, VATT1>(PFP1::MAP& map, Face f, const VATT1& position);
template VATT1::DATA_TYPE Algo::Surface::Geometry::newellNormal<PFP1, VATT1>(PFP1::MAP& map, Face f, const VATT1& position);
template VATT1::DATA_TYPE Algo::Surface::Geometry::faceNormal<PFP1, VATT1>(PFP1::MAP& map, Face f, const VATT1& position);
template VATT1::DATA_TYPE Algo::Surface::Geometry::vertexNormal<PFP1, VATT1>(PFP1::MAP& map, Vertex v, const VATT1& position);
template void Algo::Surface::Geometry::computeNormalFaces<PFP1, VATT1, FATT1>(PFP1::MAP& map, const VATT1& position, FATT1& face_normal);
template void Algo::Surface::Geometry::computeNormalVertices<PFP1, VATT1>(PFP1::MAP& map, const VATT1& position, VATT1& normal);
template PFP1::REAL Algo::Surface::Geometry::computeAngleBetweenNormalsOnEdge<PFP1, VATT1>(PFP1::MAP& map, Edge d, const VATT1& position);
template void Algo::Surface::Geometry::computeAnglesBetweenNormalsOnEdges<PFP1,VATT1,EATT1>(PFP1::MAP& map, const VATT1& position, EATT1& angles);


struct PFP11 : public PFP_STANDARD
{
	typedef EmbeddedMap3 MAP;
};

typedef VertexAttribute<PFP11::VEC3, PFP11::MAP> VATT11;
typedef FaceAttribute<PFP11::VEC3, PFP11::MAP> FATT11;
typedef EdgeAttribute<PFP11::REAL, PFP11::MAP> EATT11;


template VATT11::DATA_TYPE Algo::Surface::Geometry::vertexBorderNormal<PFP11, VATT11>(PFP11::MAP& map, Vertex v, const VATT11& position);



struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

typedef VertexAttribute<PFP2::VEC3, PFP2::MAP> VATT2;
typedef FaceAttribute<PFP2::VEC3, PFP2::MAP> FATT2;
typedef EdgeAttribute<PFP2::REAL, PFP2::MAP> EATT2;

template VATT2::DATA_TYPE Algo::Surface::Geometry::triangleNormal<PFP2, VATT2>(PFP2::MAP& map, Face f, const VATT2& position);
template VATT2::DATA_TYPE Algo::Surface::Geometry::newellNormal<PFP2, VATT2>(PFP2::MAP& map, Face f, const VATT2& position);
template VATT2::DATA_TYPE Algo::Surface::Geometry::faceNormal<PFP2, VATT2>(PFP2::MAP& map, Face f, const VATT2& position);
template VATT2::DATA_TYPE Algo::Surface::Geometry::vertexNormal<PFP2, VATT2>(PFP2::MAP& map, Vertex v, const VATT2& position);
//TODO VOLUME INSTANTIATION
//	template VATT2::DATA_TYPE Algo::Surface::Geometry::vertexBorderNormal<PFP2, VATT2>(PFP2::MAP& map, Vertex v, const VATT2& position);
template void Algo::Surface::Geometry::computeNormalFaces<PFP2, VATT2, FATT2>(PFP2::MAP& map, const VATT2& position, FATT2& face_normal);
template void Algo::Surface::Geometry::computeNormalVertices<PFP2, VATT2>(PFP2::MAP& map, const VATT2& position, VATT2& normal);
template PFP2::REAL Algo::Surface::Geometry::computeAngleBetweenNormalsOnEdge<PFP2, VATT2>(PFP2::MAP& map, Edge d, const VATT2& position);
template void Algo::Surface::Geometry::computeAnglesBetweenNormalsOnEdges<PFP2, VATT2, EATT2>(PFP2::MAP& map, const VATT2& position, EATT2& angles);



struct PFP3 : public PFP_STANDARD
{
	typedef EmbeddedGMap2 MAP;
};
typedef VertexAttribute<PFP3::VEC3, PFP3::MAP> VATT3;
typedef FaceAttribute<PFP3::VEC3, PFP3::MAP> FATT3;
typedef EdgeAttribute<PFP3::REAL, PFP3::MAP> EATT3;

template VATT3::DATA_TYPE Algo::Surface::Geometry::triangleNormal<PFP3, VATT3>(PFP3::MAP& map, Face f, const VATT3& position);
template VATT3::DATA_TYPE Algo::Surface::Geometry::newellNormal<PFP3, VATT3>(PFP3::MAP& map, Face f, const VATT3& position);
template VATT3::DATA_TYPE Algo::Surface::Geometry::faceNormal<PFP3, VATT3>(PFP3::MAP& map, Face f, const VATT3& position);
template VATT3::DATA_TYPE Algo::Surface::Geometry::vertexNormal<PFP3, VATT3>(PFP3::MAP& map, Vertex v, const VATT3& position);
//TODO VOLUME INSTANTIATION
//	template VATT3::DATA_TYPE Algo::Surface::Geometry::vertexBorderNormal<PFP3, VATT3>(PFP3::MAP& map, Vertex v, const VATT3& position);
template void Algo::Surface::Geometry::computeNormalFaces<PFP3, VATT3, FATT3>(PFP3::MAP& map, const VATT3& position, FATT3& face_normal);
template void Algo::Surface::Geometry::computeNormalVertices<PFP3, VATT3>(PFP3::MAP& map, const VATT3& position, VATT3& normal);
template PFP3::REAL Algo::Surface::Geometry::computeAngleBetweenNormalsOnEdge<PFP3, VATT3>(PFP3::MAP& map, Edge d, const VATT3& position);
template void Algo::Surface::Geometry::computeAnglesBetweenNormalsOnEdges<PFP3, VATT3, EATT3>(PFP3::MAP& map, const VATT3& position, EATT3& angles);



int test_normal()
{
	return 0;
}