#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Filtering/average.h"

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


template void Algo::Surface::Filtering::filterAverageAttribute_OneRing<PFP1, Geom::Vec3d>(PFP1::MAP& map,
const VertexAttribute<Geom::Vec3d, PFP1::MAP>& attIn, VertexAttribute<Geom::Vec3d, PFP1::MAP>& attOut, int neigh);

template void Algo::Surface::Filtering::filterAverageVertexAttribute_WithinSphere<PFP1, Geom::Vec3d>(PFP1::MAP& map,
	const VertexAttribute<Geom::Vec3d, PFP1::MAP>& attIn, VertexAttribute<Geom::Vec3d, PFP1::MAP>& attOut, int neigh,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, PFP1::REAL radius);

template void Algo::Surface::Filtering::filterAverageEdgeAttribute_WithinSphere<PFP1, Geom::Vec3d>(PFP1::MAP& map,
	const EdgeAttribute<Geom::Vec3d, PFP1::MAP>& attIn, EdgeAttribute<Geom::Vec3d, PFP1::MAP>& attOut, int neigh,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, PFP1::REAL radius);

template void Algo::Surface::Filtering::filterAverageFaceAttribute_WithinSphere<PFP1, Geom::Vec3d>(PFP1::MAP& map,
	const FaceAttribute<Geom::Vec3d, PFP1::MAP>& attIn, FaceAttribute<Geom::Vec3d, PFP1::MAP>& attOut, int neigh,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, PFP1::REAL radius);



template void Algo::Surface::Filtering::filterAverageAttribute_OneRing<PFP2, Geom::Vec3f>(PFP2::MAP& map,
	const VertexAttribute<Geom::Vec3f, PFP2::MAP>& attIn, VertexAttribute<Geom::Vec3f, PFP2::MAP>& attOut, int neigh);

template void Algo::Surface::Filtering::filterAverageVertexAttribute_WithinSphere<PFP2, Geom::Vec3f>(PFP2::MAP& map,
	const VertexAttribute<Geom::Vec3f, PFP2::MAP>& attIn, VertexAttribute<Geom::Vec3f, PFP2::MAP>& attOut, int neigh,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, PFP2::REAL radius);

template void Algo::Surface::Filtering::filterAverageEdgeAttribute_WithinSphere<PFP2, Geom::Vec3f>(PFP2::MAP& map,
	const EdgeAttribute<Geom::Vec3f, PFP2::MAP>& attIn, EdgeAttribute<Geom::Vec3f, PFP2::MAP>& attOut, int neigh,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, PFP2::REAL radius);

template void Algo::Surface::Filtering::filterAverageFaceAttribute_WithinSphere<PFP2, Geom::Vec3f>(PFP2::MAP& map,
	const FaceAttribute<Geom::Vec3f, PFP2::MAP>& attIn, FaceAttribute<Geom::Vec3f, PFP2::MAP>& attOut, int neigh,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, PFP2::REAL radius);



template void Algo::Surface::Filtering::filterAverageAttribute_OneRing<PFP3, Geom::Vec3f>(PFP3::MAP& map,
	const VertexAttribute<Geom::Vec3f, PFP3::MAP>& attIn, VertexAttribute<Geom::Vec3f, PFP3::MAP>& attOut, int neigh);

template void Algo::Surface::Filtering::filterAverageVertexAttribute_WithinSphere<PFP3, Geom::Vec3f>(PFP3::MAP& map,
	const VertexAttribute<Geom::Vec3f, PFP3::MAP>& attIn, VertexAttribute<Geom::Vec3f, PFP3::MAP>& attOut, int neigh,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, PFP3::REAL radius);

template void Algo::Surface::Filtering::filterAverageEdgeAttribute_WithinSphere<PFP3, Geom::Vec3f>(PFP3::MAP& map,
	const EdgeAttribute<Geom::Vec3f, PFP3::MAP>& attIn, EdgeAttribute<Geom::Vec3f, PFP3::MAP>& attOut, int neigh,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, PFP3::REAL radius);

template void Algo::Surface::Filtering::filterAverageFaceAttribute_WithinSphere<PFP3, Geom::Vec3f>(PFP3::MAP& map,
	const FaceAttribute<Geom::Vec3f, PFP3::MAP>& attIn, FaceAttribute<Geom::Vec3f, PFP3::MAP>& attOut, int neigh,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, PFP3::REAL radius);

int test_average()
{

	return 0;
}