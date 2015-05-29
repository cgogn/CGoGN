#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Import/AHEMImporterDefAttr.h"

using namespace CGoGN;

template class Algo::Import::UniversalLoader<EmbeddedMap2, Algo::Import::Vec3FloatLoader>;

int test_AHEMImporterDefAttr()
{

	return 0;
}