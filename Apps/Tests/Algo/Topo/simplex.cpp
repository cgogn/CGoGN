#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"
#include "Topology/map/embeddedMap3.h"

#include "Algo/Topo/simplex.h"

using namespace CGoGN;

template bool Algo::Topo::isSimplex<VERTEX, EmbeddedMap2>(const EmbeddedMap2& map, Dart d);
template bool Algo::Topo::isSimplex<EDGE, EmbeddedMap2>(const EmbeddedMap2& map, Dart d);
template bool Algo::Topo::isSimplex<FACE, EmbeddedMap2>(const EmbeddedMap2& map, Dart d);
template bool Algo::Topo::isSimplex<VOLUME, EmbeddedMap2>(const EmbeddedMap2& map, Dart d);


template bool Algo::Topo::isSimplex<VERTEX, EmbeddedMap3>(const EmbeddedMap3& map, Dart d);
template bool Algo::Topo::isSimplex<EDGE, EmbeddedMap3>(const EmbeddedMap3& map, Dart d);
template bool Algo::Topo::isSimplex<FACE, EmbeddedMap3>(const EmbeddedMap3& map, Dart d);
template bool Algo::Topo::isSimplex<VOLUME, EmbeddedMap3>(const EmbeddedMap3& map, Dart d);


int test_simplex()
{
	return 0;
}

