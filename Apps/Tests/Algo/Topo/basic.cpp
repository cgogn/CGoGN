#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"
#include "Topology/map/embeddedMap3.h"

#include "Algo/Topo/basic.h"

using namespace CGoGN;

template unsigned int Algo::Topo::getNbOrbits<VERTEX, EmbeddedMap2>(const EmbeddedMap2& map);
template unsigned int Algo::Topo::getNbOrbits<EDGE, EmbeddedMap2>(const EmbeddedMap2& map);
template unsigned int Algo::Topo::getNbOrbits<FACE, EmbeddedMap2>(const EmbeddedMap2& map);
template unsigned int Algo::Topo::getNbOrbits<VOLUME, EmbeddedMap2>(const EmbeddedMap2& map);

template unsigned int Algo::Topo::getNbOrbits<VERTEX, EmbeddedGMap2>(const EmbeddedGMap2& map);
template unsigned int Algo::Topo::getNbOrbits<EDGE, EmbeddedGMap2>(const EmbeddedGMap2& map);
template unsigned int Algo::Topo::getNbOrbits<FACE, EmbeddedGMap2>(const EmbeddedGMap2& map);
template unsigned int Algo::Topo::getNbOrbits<VOLUME, EmbeddedGMap2>(const EmbeddedGMap2& map);

template unsigned int Algo::Topo::getNbOrbits<VERTEX, EmbeddedMap3>(const EmbeddedMap3& map);
template unsigned int Algo::Topo::getNbOrbits<EDGE, EmbeddedMap3>(const EmbeddedMap3& map);
template unsigned int Algo::Topo::getNbOrbits<FACE, EmbeddedMap3>(const EmbeddedMap3& map);
template unsigned int Algo::Topo::getNbOrbits<VOLUME, EmbeddedMap3>(const EmbeddedMap3& map);



template unsigned int Algo::Topo::getNbOrbits<EmbeddedMap2>(const EmbeddedMap2& map, unsigned int orbit);
template unsigned int Algo::Topo::getNbOrbits<EmbeddedGMap2>(const EmbeddedGMap2& map, unsigned int orbit);
template unsigned int Algo::Topo::getNbOrbits<EmbeddedMap3>(const EmbeddedMap3& map, unsigned int orbit);


template void Algo::Topo::initAllOrbitsEmbedding<VERTEX, EmbeddedMap2>(EmbeddedMap2& map, bool realloc);
template void Algo::Topo::initAllOrbitsEmbedding<EDGE, EmbeddedMap2>(EmbeddedMap2& map, bool realloc);
template void Algo::Topo::initAllOrbitsEmbedding<FACE, EmbeddedMap2>(EmbeddedMap2& map, bool realloc);
template void Algo::Topo::initAllOrbitsEmbedding<VOLUME, EmbeddedMap2>(EmbeddedMap2& map, bool realloc);

template void Algo::Topo::initAllOrbitsEmbedding<VERTEX, EmbeddedGMap2>(EmbeddedGMap2& map, bool realloc);
template void Algo::Topo::initAllOrbitsEmbedding<EDGE, EmbeddedGMap2>(EmbeddedGMap2& map, bool realloc);
template void Algo::Topo::initAllOrbitsEmbedding<FACE, EmbeddedGMap2>(EmbeddedGMap2& map, bool realloc);
template void Algo::Topo::initAllOrbitsEmbedding<VOLUME, EmbeddedGMap2>(EmbeddedGMap2& map, bool realloc);

template void Algo::Topo::initAllOrbitsEmbedding<VERTEX, EmbeddedMap3>(EmbeddedMap3& map, bool realloc);
template void Algo::Topo::initAllOrbitsEmbedding<EDGE, EmbeddedMap3>(EmbeddedMap3& map, bool realloc);
template void Algo::Topo::initAllOrbitsEmbedding<FACE, EmbeddedMap3>(EmbeddedMap3& map, bool realloc);
template void Algo::Topo::initAllOrbitsEmbedding<VOLUME, EmbeddedMap3>(EmbeddedMap3& map, bool realloc);


template unsigned int Algo::Topo::computeIndexCells<VERTEX, EmbeddedMap2>(EmbeddedMap2& map, AttributeHandler<unsigned int, VERTEX, EmbeddedMap2>& idx);
template unsigned int Algo::Topo::computeIndexCells<EDGE, EmbeddedMap2>(EmbeddedMap2& map, AttributeHandler<unsigned int, EDGE, EmbeddedMap2>& idx);
template unsigned int Algo::Topo::computeIndexCells<FACE, EmbeddedMap2>(EmbeddedMap2& map, AttributeHandler<unsigned int, FACE, EmbeddedMap2>& idx);
template unsigned int Algo::Topo::computeIndexCells<VOLUME, EmbeddedMap2>(EmbeddedMap2& map, AttributeHandler<unsigned int, VOLUME, EmbeddedMap2>& idx);

template unsigned int Algo::Topo::computeIndexCells<VERTEX, EmbeddedGMap2>(EmbeddedGMap2& map, AttributeHandler<unsigned int, VERTEX, EmbeddedGMap2>& idx);
template unsigned int Algo::Topo::computeIndexCells<EDGE, EmbeddedGMap2>(EmbeddedGMap2& map, AttributeHandler<unsigned int, EDGE, EmbeddedGMap2>& idx);
template unsigned int Algo::Topo::computeIndexCells<FACE, EmbeddedGMap2>(EmbeddedGMap2& map, AttributeHandler<unsigned int, FACE, EmbeddedGMap2>& idx);
template unsigned int Algo::Topo::computeIndexCells<VOLUME, EmbeddedGMap2>(EmbeddedGMap2& map, AttributeHandler<unsigned int, VOLUME, EmbeddedGMap2>& idx);

template unsigned int Algo::Topo::computeIndexCells<VERTEX, EmbeddedMap3>(EmbeddedMap3& map, AttributeHandler<unsigned int, VERTEX, EmbeddedMap3>& idx);
template unsigned int Algo::Topo::computeIndexCells<EDGE, EmbeddedMap3>(EmbeddedMap3& map, AttributeHandler<unsigned int, EDGE, EmbeddedMap3>& idx);
template unsigned int Algo::Topo::computeIndexCells<FACE, EmbeddedMap3>(EmbeddedMap3& map, AttributeHandler<unsigned int, FACE, EmbeddedMap3>& idx);
template unsigned int Algo::Topo::computeIndexCells<VOLUME, EmbeddedMap3>(EmbeddedMap3& map, AttributeHandler<unsigned int, VOLUME, EmbeddedMap3>& idx);


template void Algo::Topo::bijectiveOrbitEmbedding<VERTEX, EmbeddedMap2>(EmbeddedMap2& map);
template void Algo::Topo::bijectiveOrbitEmbedding<EDGE, EmbeddedMap2>(EmbeddedMap2& map);
template void Algo::Topo::bijectiveOrbitEmbedding<FACE, EmbeddedMap2>(EmbeddedMap2& map);
template void Algo::Topo::bijectiveOrbitEmbedding<VOLUME, EmbeddedMap2>(EmbeddedMap2& map);

template void Algo::Topo::bijectiveOrbitEmbedding<VERTEX, EmbeddedGMap2>(EmbeddedGMap2& map);
template void Algo::Topo::bijectiveOrbitEmbedding<EDGE, EmbeddedGMap2>(EmbeddedGMap2& map);
template void Algo::Topo::bijectiveOrbitEmbedding<FACE, EmbeddedGMap2>(EmbeddedGMap2& map);
template void Algo::Topo::bijectiveOrbitEmbedding<VOLUME, EmbeddedGMap2>(EmbeddedGMap2& map);

template void Algo::Topo::bijectiveOrbitEmbedding<VERTEX, EmbeddedMap3>(EmbeddedMap3& map);
template void Algo::Topo::bijectiveOrbitEmbedding<EDGE, EmbeddedMap3>(EmbeddedMap3& map);
template void Algo::Topo::bijectiveOrbitEmbedding<FACE, EmbeddedMap3>(EmbeddedMap3& map);
template void Algo::Topo::bijectiveOrbitEmbedding<VOLUME, EmbeddedMap3>(EmbeddedMap3& map);



int test_basic()
{
	return 0;
}