#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Histogram/histogram.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};
typedef VertexAttribute<double, PFP1::MAP> VA1;
typedef VertexAttribute<Geom::Vec3f, PFP1::MAP> VAC1;
typedef CellMarker<PFP1::MAP, VERTEX> CM1;

template void Algo::Histogram::Histogram::initData<VA1>(const VA1& attr);
template void Algo::Histogram::Histogram::histoColorize<VAC1>(VAC1& colors);
template void Algo::Histogram::Histogram::quantilesColorize<VAC1>(VAC1& colors, const std::vector<Geom::Vec3f>& tc);
template unsigned int Algo::Histogram::Histogram::markCellsOfHistogramColumn<CM1>(unsigned int c, CM1& cm) const;
template unsigned int  Algo::Histogram::Histogram::markCellsOfQuantilesColumn<CM1>(unsigned int c, CM1& cm) const;




int test_histogram()
{

	return 0;
}
