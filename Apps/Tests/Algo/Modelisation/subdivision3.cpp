#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap3.h"

#include "Algo/Modelisation/subdivision3.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap3 MAP;
};

typedef VertexAttribute<PFP1::VEC3, PFP1::MAP> VPOS1;


template bool Algo::Volume::Modelisation::isHexahedron<PFP1>(PFP1::MAP& the_map, Dart d);
template Dart Algo::Volume::Modelisation::cut3Ear<PFP1>(PFP1::MAP& map, Dart d);
template Dart Algo::Volume::Modelisation::sliceConvexVolume<PFP1>(PFP1::MAP& map, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, Dart d, Geom::Plane3D<PFP1::REAL> pl);
template Dart Algo::Volume::Modelisation::sliceConvexVolume<PFP1>(PFP1::MAP& map, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, Dart d, CellMarker<PFP1::MAP, EDGE>& edgesToCut, CellMarker<PFP1::MAP, VERTEX>& verticesToSplit);
template std::vector<Dart> Algo::Volume::Modelisation::sliceConvexVolumes<PFP1>(PFP1::MAP& map, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, CellMarker<PFP1::MAP, VOLUME>& volumesToCut, CellMarker<PFP1::MAP, EDGE>& edgesToCut, CellMarker<PFP1::MAP, VERTEX>& verticesToSplit);
template void Algo::Volume::Modelisation::catmullClarkVol<PFP1,VPOS1>(PFP1::MAP& map, VPOS1& attributs);
template void Algo::Volume::Modelisation::sqrt3Vol<PFP1>(PFP1::MAP& map, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template void Algo::Volume::Modelisation::relaxation<PFP1>(PFP1::MAP& map, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template void Algo::Volume::Modelisation::computeDual<PFP1>(PFP1::MAP& map, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);


struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap3 MAP;
};
typedef VertexAttribute<PFP2::VEC3, PFP2::MAP> VPOS2;

template bool Algo::Volume::Modelisation::isHexahedron<PFP2>(PFP2::MAP& the_map, Dart d);
template Dart Algo::Volume::Modelisation::cut3Ear<PFP2>(PFP2::MAP& map, Dart d);
template Dart Algo::Volume::Modelisation::sliceConvexVolume<PFP2>(PFP2::MAP& map, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, Dart d, Geom::Plane3D<PFP2::REAL> pl);
template Dart Algo::Volume::Modelisation::sliceConvexVolume<PFP2>(PFP2::MAP& map, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, Dart d, CellMarker<PFP2::MAP, EDGE>& edgesToCut, CellMarker<PFP2::MAP, VERTEX>& verticesToSplit);
template std::vector<Dart> Algo::Volume::Modelisation::sliceConvexVolumes<PFP2>(PFP2::MAP& map, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, CellMarker<PFP2::MAP, VOLUME>& volumesToCut, CellMarker<PFP2::MAP, EDGE>& edgesToCut, CellMarker<PFP2::MAP, VERTEX>& verticesToSplit);
template void Algo::Volume::Modelisation::catmullClarkVol<PFP2, VPOS2>(PFP2::MAP& map, VPOS2& attributs);
template void Algo::Volume::Modelisation::sqrt3Vol<PFP2>(PFP2::MAP& map, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template void Algo::Volume::Modelisation::relaxation<PFP2>(PFP2::MAP& map, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template void Algo::Volume::Modelisation::computeDual<PFP2>(PFP2::MAP& map, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);


//struct PFP3 : public PFP_DOUBLE
//{
//	typedef EmbeddedGMap3 MAP;
//};
//typedef VertexAttribute<PFP3::VEC3, PFP3::MAP> VPOS3;



int test_subdivision3()
{

	return 0;
}