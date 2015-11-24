#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"
#include "Topology/map/embeddedMap3.h"

#include "Algo/Topo/embedding.h"

using namespace CGoGN;

template void Algo::Topo::setOrbitEmbedding<VERTEX, EmbeddedMap2>(EmbeddedMap2& m, Cell<VERTEX> c, unsigned int em);
template void Algo::Topo::setOrbitEmbedding<EDGE, EmbeddedMap2>(EmbeddedMap2& m, Cell<EDGE> c, unsigned int em);
template void Algo::Topo::setOrbitEmbedding<FACE, EmbeddedMap2>(EmbeddedMap2& m, Cell<FACE> c, unsigned int em);
template void Algo::Topo::setOrbitEmbedding<VOLUME, EmbeddedMap2>(EmbeddedMap2& m, Cell<VOLUME> c, unsigned int em);

template void Algo::Topo::setOrbitEmbedding<VERTEX, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<VERTEX> c, unsigned int em);
template void Algo::Topo::setOrbitEmbedding<EDGE, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<EDGE> c, unsigned int em);
template void Algo::Topo::setOrbitEmbedding<FACE, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<FACE> c, unsigned int em);
template void Algo::Topo::setOrbitEmbedding<VOLUME, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<VOLUME> c, unsigned int em);

template void Algo::Topo::setOrbitEmbedding<VERTEX, EmbeddedMap3>(EmbeddedMap3& m, Cell<VERTEX> c, unsigned int em);
template void Algo::Topo::setOrbitEmbedding<EDGE, EmbeddedMap3>(EmbeddedMap3& m, Cell<EDGE> c, unsigned int em);
template void Algo::Topo::setOrbitEmbedding<FACE, EmbeddedMap3>(EmbeddedMap3& m, Cell<FACE> c, unsigned int em);
template void Algo::Topo::setOrbitEmbedding<VOLUME, EmbeddedMap3>(EmbeddedMap3& m, Cell<VOLUME> c, unsigned int em);



template void Algo::Topo::initOrbitEmbedding<VERTEX, EmbeddedMap2>(EmbeddedMap2& m, Cell<VERTEX> c, unsigned int em);
template void Algo::Topo::initOrbitEmbedding<EDGE, EmbeddedMap2>(EmbeddedMap2& m, Cell<EDGE> c, unsigned int em);
template void Algo::Topo::initOrbitEmbedding<FACE, EmbeddedMap2>(EmbeddedMap2& m, Cell<FACE> c, unsigned int em);
template void Algo::Topo::initOrbitEmbedding<VOLUME, EmbeddedMap2>(EmbeddedMap2& m, Cell<VOLUME> c, unsigned int em);

template void Algo::Topo::initOrbitEmbedding<VERTEX, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<VERTEX> c, unsigned int em);
template void Algo::Topo::initOrbitEmbedding<EDGE, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<EDGE> c, unsigned int em);
template void Algo::Topo::initOrbitEmbedding<FACE, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<FACE> c, unsigned int em);
template void Algo::Topo::initOrbitEmbedding<VOLUME, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<VOLUME> c, unsigned int em);

template void Algo::Topo::initOrbitEmbedding<VERTEX, EmbeddedMap3>(EmbeddedMap3& m, Cell<VERTEX> c, unsigned int em);
template void Algo::Topo::initOrbitEmbedding<EDGE, EmbeddedMap3>(EmbeddedMap3& m, Cell<EDGE> c, unsigned int em);
template void Algo::Topo::initOrbitEmbedding<FACE, EmbeddedMap3>(EmbeddedMap3& m, Cell<FACE> c, unsigned int em);
template void Algo::Topo::initOrbitEmbedding<VOLUME, EmbeddedMap3>(EmbeddedMap3& m, Cell<VOLUME> c, unsigned int em);


template unsigned int Algo::Topo::setOrbitEmbeddingOnNewCell<VERTEX, EmbeddedMap2>(EmbeddedMap2& m, Cell<VERTEX> c);
template unsigned int Algo::Topo::setOrbitEmbeddingOnNewCell<EDGE, EmbeddedMap2>(EmbeddedMap2& m, Cell<EDGE> c);
template unsigned int Algo::Topo::setOrbitEmbeddingOnNewCell<FACE, EmbeddedMap2>(EmbeddedMap2& m, Cell<FACE> c);
template unsigned int Algo::Topo::setOrbitEmbeddingOnNewCell<VOLUME, EmbeddedMap2>(EmbeddedMap2& m, Cell<VOLUME> c);

template unsigned int Algo::Topo::setOrbitEmbeddingOnNewCell<VERTEX, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<VERTEX> c);
template unsigned int Algo::Topo::setOrbitEmbeddingOnNewCell<EDGE, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<EDGE> c);
template unsigned int Algo::Topo::setOrbitEmbeddingOnNewCell<FACE, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<FACE> c);
template unsigned int Algo::Topo::setOrbitEmbeddingOnNewCell<VOLUME, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<VOLUME> c);

template unsigned int Algo::Topo::setOrbitEmbeddingOnNewCell<VERTEX, EmbeddedMap3>(EmbeddedMap3& m, Cell<VERTEX> c);
template unsigned int Algo::Topo::setOrbitEmbeddingOnNewCell<EDGE, EmbeddedMap3>(EmbeddedMap3& m, Cell<EDGE> c);
template unsigned int Algo::Topo::setOrbitEmbeddingOnNewCell<FACE, EmbeddedMap3>(EmbeddedMap3& m, Cell<FACE> c);
template unsigned int Algo::Topo::setOrbitEmbeddingOnNewCell<VOLUME, EmbeddedMap3>(EmbeddedMap3& m, Cell<VOLUME> c);



template unsigned int Algo::Topo::initOrbitEmbeddingOnNewCell<VERTEX, EmbeddedMap2>(EmbeddedMap2& m, Cell<VERTEX> c);
template unsigned int Algo::Topo::initOrbitEmbeddingOnNewCell<EDGE, EmbeddedMap2>(EmbeddedMap2& m, Cell<EDGE> c);
template unsigned int Algo::Topo::initOrbitEmbeddingOnNewCell<FACE, EmbeddedMap2>(EmbeddedMap2& m, Cell<FACE> c);
template unsigned int Algo::Topo::initOrbitEmbeddingOnNewCell<VOLUME, EmbeddedMap2>(EmbeddedMap2& m, Cell<VOLUME> c);

template unsigned int Algo::Topo::initOrbitEmbeddingOnNewCell<VERTEX, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<VERTEX> c);
template unsigned int Algo::Topo::initOrbitEmbeddingOnNewCell<EDGE, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<EDGE> c);
template unsigned int Algo::Topo::initOrbitEmbeddingOnNewCell<FACE, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<FACE> c);
template unsigned int Algo::Topo::initOrbitEmbeddingOnNewCell<VOLUME, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<VOLUME> c);

template unsigned int Algo::Topo::initOrbitEmbeddingOnNewCell<VERTEX, EmbeddedMap3>(EmbeddedMap3& m, Cell<VERTEX> c);
template unsigned int Algo::Topo::initOrbitEmbeddingOnNewCell<EDGE, EmbeddedMap3>(EmbeddedMap3& m, Cell<EDGE> c);
template unsigned int Algo::Topo::initOrbitEmbeddingOnNewCell<FACE, EmbeddedMap3>(EmbeddedMap3& m, Cell<FACE> c);
template unsigned int Algo::Topo::initOrbitEmbeddingOnNewCell<VOLUME, EmbeddedMap3>(EmbeddedMap3& m, Cell<VOLUME> c);



template void Algo::Topo::copyCellAttributes<VERTEX, EmbeddedMap2>(EmbeddedMap2& m, Cell<VERTEX> c, Cell<VERTEX> e);
template void Algo::Topo::copyCellAttributes<EDGE, EmbeddedMap2>(EmbeddedMap2& m, Cell<EDGE> c, Cell<EDGE> e);
template void Algo::Topo::copyCellAttributes<FACE, EmbeddedMap2>(EmbeddedMap2& m, Cell<FACE> c, Cell<FACE> e);
template void Algo::Topo::copyCellAttributes<VOLUME, EmbeddedMap2>(EmbeddedMap2& m, Cell<VOLUME> c, Cell<VOLUME> e);

template void Algo::Topo::copyCellAttributes<VERTEX, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<VERTEX> c, Cell<VERTEX> e);
template void Algo::Topo::copyCellAttributes<EDGE, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<EDGE> c, Cell<EDGE> e);
template void Algo::Topo::copyCellAttributes<FACE, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<FACE> c, Cell<FACE> e);
template void Algo::Topo::copyCellAttributes<VOLUME, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<VOLUME> c, Cell<VOLUME> e);

template void Algo::Topo::copyCellAttributes<VERTEX, EmbeddedMap3>(EmbeddedMap3& m, Cell<VERTEX> c, Cell<VERTEX> e);
template void Algo::Topo::copyCellAttributes<EDGE, EmbeddedMap3>(EmbeddedMap3& m, Cell<EDGE> c, Cell<EDGE> e);
template void Algo::Topo::copyCellAttributes<FACE, EmbeddedMap3>(EmbeddedMap3& m, Cell<FACE> c, Cell<FACE> e);
template void Algo::Topo::copyCellAttributes<VOLUME, EmbeddedMap3>(EmbeddedMap3& m, Cell<VOLUME> c, Cell<VOLUME> e);



template void Algo::Topo::boundaryMarkOrbit<2, VERTEX, EmbeddedMap2>(EmbeddedMap2& m, Cell<VERTEX> c);
template void Algo::Topo::boundaryMarkOrbit<2, EDGE, EmbeddedMap2>(EmbeddedMap2& m, Cell<EDGE> c);
template void Algo::Topo::boundaryMarkOrbit<2, FACE, EmbeddedMap2>(EmbeddedMap2& m, Cell<FACE> c);
template void Algo::Topo::boundaryMarkOrbit<2, VOLUME, EmbeddedMap2>(EmbeddedMap2& m, Cell<VOLUME> c);

template void Algo::Topo::boundaryMarkOrbit<2, VERTEX, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<VERTEX> c);
template void Algo::Topo::boundaryMarkOrbit<2, EDGE, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<EDGE> c);
template void Algo::Topo::boundaryMarkOrbit<2, FACE, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<FACE> c);
template void Algo::Topo::boundaryMarkOrbit<2, VOLUME, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<VOLUME> c);

template void Algo::Topo::boundaryMarkOrbit<3, VERTEX, EmbeddedMap3>(EmbeddedMap3& m, Cell<VERTEX> c);
template void Algo::Topo::boundaryMarkOrbit<3, EDGE, EmbeddedMap3>(EmbeddedMap3& m, Cell<EDGE> c);
template void Algo::Topo::boundaryMarkOrbit<3, FACE, EmbeddedMap3>(EmbeddedMap3& m, Cell<FACE> c);
template void Algo::Topo::boundaryMarkOrbit<3, VOLUME, EmbeddedMap3>(EmbeddedMap3& m, Cell<VOLUME> c);



template void Algo::Topo::boundaryUnmarkOrbit<2, VERTEX, EmbeddedMap2>(EmbeddedMap2& m, Cell<VERTEX> c);
template void Algo::Topo::boundaryUnmarkOrbit<2, EDGE, EmbeddedMap2>(EmbeddedMap2& m, Cell<EDGE> c);
template void Algo::Topo::boundaryUnmarkOrbit<2, FACE, EmbeddedMap2>(EmbeddedMap2& m, Cell<FACE> c);
template void Algo::Topo::boundaryUnmarkOrbit<2, VOLUME, EmbeddedMap2>(EmbeddedMap2& m, Cell<VOLUME> c);

template void Algo::Topo::boundaryUnmarkOrbit<2, VERTEX, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<VERTEX> c);
template void Algo::Topo::boundaryUnmarkOrbit<2, EDGE, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<EDGE> c);
template void Algo::Topo::boundaryUnmarkOrbit<2, FACE, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<FACE> c);
template void Algo::Topo::boundaryUnmarkOrbit<2, VOLUME, EmbeddedGMap2>(EmbeddedGMap2& m, Cell<VOLUME> c);

template void Algo::Topo::boundaryUnmarkOrbit<3, VERTEX, EmbeddedMap3>(EmbeddedMap3& m, Cell<VERTEX> c);
template void Algo::Topo::boundaryUnmarkOrbit<3, EDGE, EmbeddedMap3>(EmbeddedMap3& m, Cell<EDGE> c);
template void Algo::Topo::boundaryUnmarkOrbit<3, FACE, EmbeddedMap3>(EmbeddedMap3& m, Cell<FACE> c);
template void Algo::Topo::boundaryUnmarkOrbit<3, VOLUME, EmbeddedMap3>(EmbeddedMap3& m, Cell<VOLUME> c);


int test_embedding()
{
	return 0;
}

