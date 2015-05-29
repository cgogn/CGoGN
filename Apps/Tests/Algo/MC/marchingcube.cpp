#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/MC/marchingcube.h"
#include "Algo/MC/windowing.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

template class Algo::Surface::MC::MarchingCube< char, Algo::Surface::MC::WindowingEqual, PFP1 >;
template class Algo::Surface::MC::MarchingCube< unsigned char, Algo::Surface::MC::WindowingEqual, PFP1 >;
template class Algo::Surface::MC::MarchingCube< int, Algo::Surface::MC::WindowingEqual, PFP1 >;
template class Algo::Surface::MC::MarchingCube< double, Algo::Surface::MC::WindowingEqual, PFP1 >;


struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

template class Algo::Surface::MC::MarchingCube< char, Algo::Surface::MC::WindowingEqual, PFP2 >;
template class Algo::Surface::MC::MarchingCube< unsigned char, Algo::Surface::MC::WindowingEqual, PFP2 >;
template class Algo::Surface::MC::MarchingCube< int, Algo::Surface::MC::WindowingEqual, PFP2 >;
template class Algo::Surface::MC::MarchingCube< double, Algo::Surface::MC::WindowingEqual, PFP2 >;


struct PFP3 : public PFP_DOUBLE
{
	typedef EmbeddedGMap2 MAP;
};

template class Algo::Surface::MC::MarchingCube< char, Algo::Surface::MC::WindowingEqual, PFP3 >;
template class Algo::Surface::MC::MarchingCube< unsigned char, Algo::Surface::MC::WindowingEqual, PFP3 >;
template class Algo::Surface::MC::MarchingCube< int, Algo::Surface::MC::WindowingEqual, PFP3 >;
template class Algo::Surface::MC::MarchingCube< double, Algo::Surface::MC::WindowingEqual, PFP3 >;



int test_marchingcube()
{

	return 0;
}

