#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap3.h"

#include "Algo/Modelisation/tetrahedralization.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap3 MAP;
};


template class Algo::Volume::Modelisation::Tetrahedralization::EarTriangulation<PFP1>;
template Dart Algo::Volume::Modelisation::Tetrahedralization::splitVertex<PFP1>(PFP1::MAP& map, std::vector<Dart>& vd);
template bool Algo::Volume::Modelisation::Tetrahedralization::isTetrahedron<PFP1>(PFP1::MAP& map, Vol d);
template bool Algo::Volume::Modelisation::Tetrahedralization::isTetrahedralization<PFP1>(PFP1::MAP& map);
template Dart Algo::Volume::Modelisation::Tetrahedralization::swap2To2<PFP1>(PFP1::MAP& map, Dart d);
template Dart Algo::Volume::Modelisation::Tetrahedralization::swap4To4<PFP1>(PFP1::MAP& map, Dart d);
template Dart Algo::Volume::Modelisation::Tetrahedralization::swap3To2<PFP1>(PFP1::MAP& map, Dart d);
template Dart Algo::Volume::Modelisation::Tetrahedralization::swap2To3<PFP1>(PFP1::MAP& map, Dart d);
template Dart Algo::Volume::Modelisation::Tetrahedralization::swap5To4<PFP1>(PFP1::MAP& map, Dart d);
template Dart Algo::Volume::Modelisation::Tetrahedralization::swapGen3To2<PFP1>(PFP1::MAP& map, Dart d);
template std::vector<Dart> Algo::Volume::Modelisation::Tetrahedralization::swapGen3To2Optimized<PFP1>(PFP1::MAP& map, Dart d);
template void Algo::Volume::Modelisation::Tetrahedralization::swapGen2To3<PFP1>(PFP1::MAP& map, Dart d);
template Dart Algo::Volume::Modelisation::Tetrahedralization::flip1To4<PFP1>(PFP1::MAP& map, Dart d);
template Dart Algo::Volume::Modelisation::Tetrahedralization::flip1To3<PFP1>(PFP1::MAP& map, Dart d);
template Dart Algo::Volume::Modelisation::Tetrahedralization::edgeBisection<PFP1>(PFP1::MAP& map, Dart d);



int test_tetrahedralization()
{

	return 0;
}