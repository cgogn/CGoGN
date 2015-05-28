#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Filtering/average_normals.h"

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



template void Algo::Surface::Filtering::computeNewPositionsFromFaceNormals<PFP1>(PFP1::MAP& map,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,	VertexAttribute<PFP1::VEC3, PFP1::MAP>& position2,
	const FaceAttribute<PFP1::REAL, PFP1::MAP>& faceArea,
	const FaceAttribute<PFP1::VEC3, PFP1::MAP>& faceCentroid,
	const FaceAttribute<PFP1::VEC3, PFP1::MAP>& faceNormal,
	const FaceAttribute<PFP1::VEC3, PFP1::MAP>& faceNewNormal);

template void Algo::Surface::Filtering::filterAverageNormals<PFP1>(PFP1::MAP& map,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position2);

template void Algo::Surface::Filtering::filterMMSE<PFP1>(PFP1::MAP& map, float sigmaN2,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position2);

template void Algo::Surface::Filtering::filterTNBA<PFP1>(PFP1::MAP& map, float sigmaN2, float SUSANthreshold,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position2);

template void Algo::Surface::Filtering::filterVNBA<PFP1>(PFP1::MAP& map, float sigmaN2, float SUSANthreshold,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position2, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& normal);




template void Algo::Surface::Filtering::computeNewPositionsFromFaceNormals<PFP2>(PFP2::MAP& map,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position2,
	const FaceAttribute<PFP2::REAL, PFP2::MAP>& faceArea,
	const FaceAttribute<PFP2::VEC3, PFP2::MAP>& faceCentroid,
	const FaceAttribute<PFP2::VEC3, PFP2::MAP>& faceNormal,
	const FaceAttribute<PFP2::VEC3, PFP2::MAP>& faceNewNormal);

template void Algo::Surface::Filtering::filterAverageNormals<PFP2>(PFP2::MAP& map,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position2);

template void Algo::Surface::Filtering::filterMMSE<PFP2>(PFP2::MAP& map, float sigmaN2,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position2);

template void Algo::Surface::Filtering::filterTNBA<PFP2>(PFP2::MAP& map, float sigmaN2, float SUSANthreshold,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position2);

template void Algo::Surface::Filtering::filterVNBA<PFP2>(PFP2::MAP& map, float sigmaN2, float SUSANthreshold,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position2, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& normal);



template void Algo::Surface::Filtering::computeNewPositionsFromFaceNormals<PFP3>(PFP3::MAP& map,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, VertexAttribute<PFP3::VEC3, PFP3::MAP>& position2,
	const FaceAttribute<PFP3::REAL, PFP3::MAP>& faceArea,
	const FaceAttribute<PFP3::VEC3, PFP3::MAP>& faceCentroid,
	const FaceAttribute<PFP3::VEC3, PFP3::MAP>& faceNormal,
	const FaceAttribute<PFP3::VEC3, PFP3::MAP>& faceNewNormal);

template void Algo::Surface::Filtering::filterAverageNormals<PFP3>(PFP3::MAP& map,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, VertexAttribute<PFP3::VEC3, PFP3::MAP>& position2);

template void Algo::Surface::Filtering::filterMMSE<PFP3>(PFP3::MAP& map, float sigmaN2,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, VertexAttribute<PFP3::VEC3, PFP3::MAP>& position2);

template void Algo::Surface::Filtering::filterTNBA<PFP3>(PFP3::MAP& map, float sigmaN2, float SUSANthreshold,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, VertexAttribute<PFP3::VEC3, PFP3::MAP>& position2);

template void Algo::Surface::Filtering::filterVNBA<PFP3>(PFP3::MAP& map, float sigmaN2, float SUSANthreshold,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, VertexAttribute<PFP3::VEC3, PFP3::MAP>& position2, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& normal);


int test_average_normals()
{

	return 0;
}