#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Render/GL2/explodeVolumeRender.h"

using namespace CGoGN;


struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap3 MAP;
};
typedef VertexAttribute<Geom::Vec3f, PFP1::MAP> VATT1;
typedef VolumeAttribute<Geom::Vec3f, PFP1::MAP> WATT1;

template void Algo::Render::GL2::ExplodeVolumeRender::updateData<PFP1,VATT1>(PFP1::MAP& map, const VATT1& positions);
template void Algo::Render::GL2::ExplodeVolumeRender::updateData<PFP1,VATT1,WATT1>(PFP1::MAP& map, const VATT1& positions, const WATT1& colorPerFace);



struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap3 MAP;
};
typedef VertexAttribute<Geom::Vec3d, PFP2::MAP> VATT2;
typedef VolumeAttribute<Geom::Vec3d, PFP2::MAP> WATT2;

template void Algo::Render::GL2::ExplodeVolumeRender::updateData<PFP2, VATT2>(PFP2::MAP& map, const VATT2& positions);
template void Algo::Render::GL2::ExplodeVolumeRender::updateData<PFP2, VATT2, WATT2>(PFP2::MAP& map, const VATT2& positions, const WATT2& colorPerFace);




int test_explodeVolumeRender()
{

	return 0;
}


