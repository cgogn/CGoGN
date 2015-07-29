#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/BooleanOperator/mergeVertices.h"

using namespace CGoGN;


struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

template bool Algo::Surface::BooleanOperator::isBetween<PFP1>(PFP1::MAP& map, const VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, Dart d, Dart e, Dart f);
template void Algo::Surface::BooleanOperator::mergeVertex<PFP1>(PFP1::MAP& map, VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, Dart d, Dart e, int precision);
template void Algo::Surface::BooleanOperator::mergeVertices<PFP1>(PFP1::MAP& map, VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, int precision);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

template bool Algo::Surface::BooleanOperator::isBetween<PFP2>(PFP2::MAP& map, const VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, Dart d, Dart e, Dart f);
template void Algo::Surface::BooleanOperator::mergeVertex<PFP2>(PFP2::MAP& map, VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, Dart d, Dart e, int precision);
template void Algo::Surface::BooleanOperator::mergeVertices<PFP2>(PFP2::MAP& map, VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, int precision);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//struct PFP3 : public PFP_STANDARD
//{
//	typedef EmbeddedGMap2 MAP;
//};

//template bool Algo::Surface::BooleanOperator::isBetween<PFP3>(PFP3::MAP& map, const VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, Dart d, Dart e, Dart f);
//template void Algo::Surface::BooleanOperator::mergeVertex<PFP3>(PFP3::MAP& map, VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, Dart d, Dart e, int precision);
//template void Algo::Surface::BooleanOperator::mergeVertices<PFP3>(PFP3::MAP& map, VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, int precision);

// TODO add removeEdgeFromVertex & insertEdgeInVertex in GMap2

int test_mergeVertices()
{

	return 0;
}
