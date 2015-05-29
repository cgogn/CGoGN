#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Modelisation/planeCutting.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

template void Algo::Surface::Modelisation::planeCut<PFP1>(PFP1::MAP& map, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
	const Geom::Plane3D<PFP1::REAL>& plane,	CellMarker<PFP1::MAP, FACE>& cmf_over, bool keepTriangles, bool with_unsew);

template void Algo::Surface::Modelisation::planeCut2<PFP1>(PFP1::MAP& map, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
	const Geom::Plane3D<PFP1::REAL>& plane, CellMarker<PFP1::MAP, FACE>& cmf_over, bool with_unsew);



struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

template void Algo::Surface::Modelisation::planeCut<PFP2>(PFP2::MAP& map, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const Geom::Plane3D<PFP2::REAL>& plane, CellMarker<PFP2::MAP, FACE>& cmf_over, bool keepTriangles, bool with_unsew);

template void Algo::Surface::Modelisation::planeCut2<PFP2>(PFP2::MAP& map, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const Geom::Plane3D<PFP2::REAL>& plane, CellMarker<PFP2::MAP, FACE>& cmf_over, bool with_unsew);



struct PFP3 : public PFP_DOUBLE
{
	typedef EmbeddedGMap2 MAP;
};

template void Algo::Surface::Modelisation::planeCut<PFP3>(PFP3::MAP& map, VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const Geom::Plane3D<PFP3::REAL>& plane, CellMarker<PFP3::MAP, FACE>& cmf_over, bool keepTriangles, bool with_unsew);

template void Algo::Surface::Modelisation::planeCut2<PFP3>(PFP3::MAP& map, VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const Geom::Plane3D<PFP3::REAL>& plane, CellMarker<PFP3::MAP, FACE>& cmf_over, bool with_unsew);




struct PFP4 : public PFP_DOUBLE
{
	typedef EmbeddedMap3 MAP;
};

template void Algo::Volume::Modelisation::planeCut<PFP4>(PFP4::MAP& map, VertexAttribute<PFP4::VEC3, PFP4::MAP>& position,
	const Geom::Plane3D<PFP4::REAL>& plane, CellMarker<PFP4::MAP, FACE>& cmv_over, bool keepTetrahedra, bool with_unsew);


int test_planeCutting()
{

	return 0;
}