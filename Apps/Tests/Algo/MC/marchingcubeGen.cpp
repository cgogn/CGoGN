#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/MC/marchingcubeGen.h"
#include "Algo/MC/windowing.h"

using namespace CGoGN;

template<typename T>
class ImageTest
{
public:
	float getVoxSizeX() const { return 1.0f; }
	float getVoxSizeY() const { return 1.0f; }
	float getVoxSizeZ() const { return 1.0f; }
	int getWidthX() const { return 256; }
	int getWidthY() const { return 256; }
	int getWidthZ() const { return 256; }
	T getVoxel(int x, int y, int z) const { return T(std::sqrt(double((x - 128)*(x - 128) + (y - 128)*(y - 128) + (z - 128)*(y - 128)))); }
};

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

template class Algo::Surface::MC::MarchingCubeGen< char, ImageTest<char>, Algo::Surface::MC::WindowingEqual, PFP1 >;
template class Algo::Surface::MC::MarchingCubeGen< unsigned char, ImageTest<unsigned char>, Algo::Surface::MC::WindowingEqual, PFP1 >;
template class Algo::Surface::MC::MarchingCubeGen< int, ImageTest<int>, Algo::Surface::MC::WindowingEqual, PFP1 >;
template class Algo::Surface::MC::MarchingCubeGen< double, ImageTest<double>, Algo::Surface::MC::WindowingEqual, PFP1 >;


struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

template class Algo::Surface::MC::MarchingCubeGen< char, ImageTest<char>, Algo::Surface::MC::WindowingEqual, PFP2 >;
template class Algo::Surface::MC::MarchingCubeGen< unsigned char, ImageTest<unsigned char>, Algo::Surface::MC::WindowingEqual, PFP2 >;
template class Algo::Surface::MC::MarchingCubeGen< int, ImageTest<int>, Algo::Surface::MC::WindowingEqual, PFP2 >;
template class Algo::Surface::MC::MarchingCubeGen< double, ImageTest<double>, Algo::Surface::MC::WindowingEqual, PFP2 >;



struct PFP3 : public PFP_DOUBLE
{
	typedef EmbeddedGMap2 MAP;
};

template class Algo::Surface::MC::MarchingCubeGen< char, ImageTest<char>, Algo::Surface::MC::WindowingEqual, PFP3 >;
template class Algo::Surface::MC::MarchingCubeGen< unsigned char, ImageTest<unsigned char>, Algo::Surface::MC::WindowingEqual, PFP3 >;
template class Algo::Surface::MC::MarchingCubeGen< int, ImageTest<int>, Algo::Surface::MC::WindowingEqual, PFP3 >;
template class Algo::Surface::MC::MarchingCubeGen< double, ImageTest<double>, Algo::Surface::MC::WindowingEqual, PFP3 >;


int test_marchingcubeGen()
{
	// TODO faire une passe de MAJ et tester 
	return 0;
}


