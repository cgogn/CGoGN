#include <iostream>
#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"

#include "Algo/Geometry/normalization.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

//PFP1::REAL Algo::Geometry::normalizeLength<PFP1, VERTEX>(PFP1::MAP& map, AttributeHandler<PFP1::VEC3, VERTEX, PFP1::MAP>& m_attr, PFP1::REAL scale);
//PFP1::REAL Algo::Geometry::normalizeLength<PFP1>(PFP1::MAP & map, AttributeHandler<PFP1::VEC3, EDGE, PFP1::MAP> & m_attr, PFP1::REAL scale);
//PFP1::REAL Algo::Geometry::normalizeLength<PFP1>(PFP1::MAP & map, AttributeHandler<PFP1::VEC3, FACE, PFP1::MAP> & m_attr, PFP1::REAL scale);

//struct PFP2 : public PFP_STANDARD
//{
//	typedef EmbeddedMap2 MAP;
//};
//
//PFP2::REAL Algo::Geometry::normalizeLength<PFP2, VERTEX>(PFP2::MAP & map, AttributeHandler<PFP2::VEC3, VERTEX, PFP2::MAP> & m_attr, const PFP2::REAL scale);
//PFP2::REAL Algo::Geometry::normalizeLength<PFP2, EDGE>(PFP2::MAP & map, AttributeHandler<PFP2::VEC3, EDGE, PFP2::MAP> & m_attr, const PFP2::REAL scale);
//PFP2::REAL Algo::Geometry::normalizeLength<PFP2, FACE>(PFP2::MAP & map, AttributeHandler<PFP2::VEC3, FACE, PFP2::MAP> & m_attr, const PFP2::REAL scale);

int test_normalization()
{ 
	return 0;
}
