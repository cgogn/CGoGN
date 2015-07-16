#include <iostream>
#include "Topology/generic/parameters.h"
#include "Topology/gmap/embeddedGMap2.h"
#include "Topology/map/embeddedMap2.h"

#include "Algo/Geometry/centroid.h"

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
*		CENTROID INSTANTIATION
*****************************************/

typedef VertexAttribute<PFP1::VEC3, PFP1::MAP> VATT1;
typedef FaceAttribute<PFP1::VEC3, PFP1::MAP> FATT1;
typedef VolumeAttribute<PFP1::VEC3, PFP1::MAP> WATT1;
template VATT1::DATA_TYPE Algo::Surface::Geometry::volumeCentroid<PFP1,VATT1>(PFP1::MAP& map, Vol d, const VATT1& attributs);
template VATT1::DATA_TYPE Algo::Surface::Geometry::volumeCentroidELW<PFP1, VATT1>(PFP1::MAP& map, Vol d, const VATT1& attributs);
template VATT1::DATA_TYPE Algo::Surface::Geometry::faceCentroid<PFP1, VATT1>(PFP1::MAP& map, Face d, const VATT1& attributs);
template VATT1::DATA_TYPE Algo::Surface::Geometry::faceCentroidELW<PFP1, VATT1>(PFP1::MAP& map, Face d, const VATT1& attributs);
template VATT1::DATA_TYPE Algo::Surface::Geometry::vertexNeighborhoodCentroid<PFP1, VATT1>(PFP1::MAP& map, Vertex d, const VATT1& attributs);
template void Algo::Surface::Geometry::computeCentroidFaces<PFP1, VATT1, FATT1>(PFP1::MAP& map, const VATT1& position, FATT1& face_centroid);
template void Algo::Surface::Geometry::computeCentroidELWFaces<PFP1, VATT1, FATT1>(PFP1::MAP& map, const VATT1& position, FATT1& face_centroid);
template void Algo::Surface::Geometry::computeNeighborhoodCentroidVertices<PFP1, VATT1>(PFP1::MAP& map, const VATT1& position, VATT1& vertex_centroid);

template void Algo::Surface::Geometry::Parallel::computeCentroidFaces<PFP1, VATT1, FATT1>(PFP1::MAP& map, const VATT1& position, FATT1& face_centroid);
template void Algo::Surface::Geometry::Parallel::computeCentroidELWFaces<PFP1, VATT1, FATT1>(PFP1::MAP& map, const VATT1& position, FATT1& face_centroid);
template void Algo::Surface::Geometry::Parallel::computeNeighborhoodCentroidVertices<PFP1, VATT1>(PFP1::MAP& map, const VATT1& position, VATT1& vertex_centroid);

template VATT1::DATA_TYPE Algo::Volume::Geometry::vertexNeighborhoodCentroid<PFP1, VATT1>(PFP1::MAP& map, Vertex d, const VATT1& attributs);
template void Algo::Volume::Geometry::computeCentroidVolumes<PFP1, VATT1, WATT1>(PFP1::MAP& map, const VATT1& position, WATT1& vol_centroid);
template void Algo::Volume::Geometry::computeCentroidELWVolumes<PFP1, VATT1, WATT1>(PFP1::MAP& map, const VATT1& position, WATT1& vol_centroid);
template void Algo::Volume::Geometry::computeNeighborhoodCentroidVertices<PFP1, VATT1>(PFP1::MAP& map, const VATT1& position, VATT1& vertex_centroid);


typedef VertexAttribute<PFP2::VEC3, PFP2::MAP> VATT2;
typedef FaceAttribute<PFP2::VEC3, PFP2::MAP> FATT2;
typedef VolumeAttribute<PFP2::VEC3, PFP2::MAP> WATT2;
template VATT2::DATA_TYPE Algo::Surface::Geometry::volumeCentroid<PFP2, VATT2>(PFP2::MAP& map, Vol d, const VATT2& attributs);
template VATT2::DATA_TYPE Algo::Surface::Geometry::volumeCentroidELW<PFP2, VATT2>(PFP2::MAP& map, Vol d, const VATT2& attributs);
template VATT2::DATA_TYPE Algo::Surface::Geometry::faceCentroid<PFP2, VATT2>(PFP2::MAP& map, Face d, const VATT2& attributs);
template VATT2::DATA_TYPE Algo::Surface::Geometry::faceCentroidELW<PFP2, VATT2>(PFP2::MAP& map, Face d, const VATT2& attributs);
template VATT2::DATA_TYPE Algo::Surface::Geometry::vertexNeighborhoodCentroid<PFP2, VATT2>(PFP2::MAP& map, Vertex d, const VATT2& attributs);
template void Algo::Surface::Geometry::computeCentroidFaces<PFP2, VATT2, FATT2>(PFP2::MAP& map, const VATT2& position, FATT2& face_centroid);
template void Algo::Surface::Geometry::computeCentroidELWFaces<PFP2, VATT2, FATT2>(PFP2::MAP& map, const VATT2& position, FATT2& face_centroid);
template void Algo::Surface::Geometry::computeNeighborhoodCentroidVertices<PFP2, VATT2>(PFP2::MAP& map, const VATT2& position, VATT2& vertex_centroid);

template void Algo::Surface::Geometry::Parallel::computeCentroidFaces<PFP2, VATT2, FATT2>(PFP2::MAP& map, const VATT2& position, FATT2& face_centroid);
template void Algo::Surface::Geometry::Parallel::computeCentroidELWFaces<PFP2, VATT2, FATT2>(PFP2::MAP& map, const VATT2& position, FATT2& face_centroid);
template void Algo::Surface::Geometry::Parallel::computeNeighborhoodCentroidVertices<PFP2, VATT2>(PFP2::MAP& map, const VATT2& position, VATT2& vertex_centroid);

template VATT2::DATA_TYPE Algo::Volume::Geometry::vertexNeighborhoodCentroid<PFP2, VATT2>(PFP2::MAP& map, Vertex d, const VATT2& attributs);
template void Algo::Volume::Geometry::computeCentroidVolumes<PFP2, VATT2, WATT2>(PFP2::MAP& map, const VATT2& position, WATT2& vol_centroid);
template void Algo::Volume::Geometry::computeCentroidELWVolumes<PFP2, VATT2, WATT2>(PFP2::MAP& map, const VATT2& position, WATT2& vol_centroid);
template void Algo::Volume::Geometry::computeNeighborhoodCentroidVertices<PFP2, VATT2>(PFP2::MAP& map, const VATT2& position, VATT2& vertex_centroid);


typedef VertexAttribute<PFP3::VEC3, PFP3::MAP> VATT3;
typedef FaceAttribute<PFP3::VEC3, PFP3::MAP> FATT3;
typedef VolumeAttribute<PFP3::VEC3, PFP3::MAP> WATT3;
template VATT3::DATA_TYPE Algo::Surface::Geometry::volumeCentroid<PFP3, VATT3>(PFP3::MAP& map, Vol d, const VATT3& attributs);
template VATT3::DATA_TYPE Algo::Surface::Geometry::volumeCentroidELW<PFP3, VATT3>(PFP3::MAP& map, Vol d, const VATT3& attributs);
template VATT3::DATA_TYPE Algo::Surface::Geometry::faceCentroid<PFP3, VATT3>(PFP3::MAP& map, Face d, const VATT3& attributs);
template VATT3::DATA_TYPE Algo::Surface::Geometry::faceCentroidELW<PFP3, VATT3>(PFP3::MAP& map, Face d, const VATT3& attributs);
template VATT3::DATA_TYPE Algo::Surface::Geometry::vertexNeighborhoodCentroid<PFP3, VATT3>(PFP3::MAP& map, Vertex d, const VATT3& attributs);
template void Algo::Surface::Geometry::computeCentroidFaces<PFP3, VATT3, FATT3>(PFP3::MAP& map, const VATT3& position, FATT3& face_centroid);
template void Algo::Surface::Geometry::computeCentroidELWFaces<PFP3, VATT3, FATT3>(PFP3::MAP& map, const VATT3& position, FATT3& face_centroid);
template void Algo::Surface::Geometry::computeNeighborhoodCentroidVertices<PFP3, VATT3>(PFP3::MAP& map, const VATT3& position, VATT3& vertex_centroid);

template void Algo::Surface::Geometry::Parallel::computeCentroidFaces<PFP3, VATT3, FATT3>(PFP3::MAP& map, const VATT3& position, FATT3& face_centroid);
template void Algo::Surface::Geometry::Parallel::computeCentroidELWFaces<PFP3, VATT3, FATT3>(PFP3::MAP& map, const VATT3& position, FATT3& face_centroid);
template void Algo::Surface::Geometry::Parallel::computeNeighborhoodCentroidVertices<PFP3, VATT3>(PFP3::MAP& map, const VATT3& position, VATT3& vertex_centroid);

template VATT3::DATA_TYPE Algo::Volume::Geometry::vertexNeighborhoodCentroid<PFP3, VATT3>(PFP3::MAP& map, Vertex d, const VATT3& attributs);
template void Algo::Volume::Geometry::computeCentroidVolumes<PFP3, VATT3, WATT3>(PFP3::MAP& map, const VATT3& position, WATT3& vol_centroid);
template void Algo::Volume::Geometry::computeCentroidELWVolumes<PFP3, VATT3, WATT3>(PFP3::MAP& map, const VATT3& position, WATT3& vol_centroid);
template void Algo::Volume::Geometry::computeNeighborhoodCentroidVertices<PFP3, VATT3>(PFP3::MAP& map, const VATT3& position, VATT3& vertex_centroid);



int test_centroid()
{
	return 0;
}
