#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"

#include "Algo/Topo/Map2/uniformOrientation.h"

using namespace CGoGN;

template void Algo::Topo::uniformOrientationCC<EmbeddedMap2>(EmbeddedMap2& map, Dart faceSeed);

// do not work but do not need !
//template void Algo::Topo::uniformOrientationCC<EmbeddedGMap2>(EmbeddedGMap2& map, Dart faceSeed);


int test_uniformOrientation()
{
	return 0;
}
