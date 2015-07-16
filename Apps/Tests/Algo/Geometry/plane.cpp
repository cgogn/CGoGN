#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"

#include "Algo/Geometry/plane.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};


template Geom::Plane3D<PFP1::REAL> Algo::Surface::Geometry::trianglePlane<PFP1>(PFP1::MAP& map, Face f, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template Geom::Plane3D<PFP1::REAL> Algo::Surface::Geometry::facePlane<PFP1>(PFP1::MAP& map, Face f, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template Geom::Plane3D<PFP1::REAL> Algo::Surface::Geometry::vertexTangentPlane<PFP1>(PFP1::MAP& map, Vertex v, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);

struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

template Geom::Plane3D<PFP2::REAL> Algo::Surface::Geometry::trianglePlane<PFP2>(PFP2::MAP& map, Face f, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template Geom::Plane3D<PFP2::REAL> Algo::Surface::Geometry::facePlane<PFP2>(PFP2::MAP& map, Face f, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template Geom::Plane3D<PFP2::REAL> Algo::Surface::Geometry::vertexTangentPlane<PFP2>(PFP2::MAP& map, Vertex v, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);


struct PFP3 : public PFP_DOUBLE
{
	typedef EmbeddedGMap2 MAP;
};

template Geom::Plane3D<PFP3::REAL> Algo::Surface::Geometry::trianglePlane<PFP3>(PFP3::MAP& map, Face f, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
template Geom::Plane3D<PFP3::REAL> Algo::Surface::Geometry::facePlane<PFP3>(PFP3::MAP& map, Face f, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
template Geom::Plane3D<PFP3::REAL> Algo::Surface::Geometry::vertexTangentPlane<PFP3>(PFP3::MAP& map, Vertex v, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);



int test_plane()
{
	return 0;
}
