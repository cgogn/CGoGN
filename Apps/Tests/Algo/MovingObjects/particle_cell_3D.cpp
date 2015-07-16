#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap3.h"


#include "Algo/MovingObjects/particle_cell_3D.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap3 MAP;
};

template class Algo::Volume::MovingObjects::ParticleCell3D<PFP1>;


struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap3 MAP;
};

template class Algo::Volume::MovingObjects::ParticleCell3D<PFP2>;



struct PFP3 : public PFP_DOUBLE
{
	typedef EmbeddedGMap3 MAP;
};

template class Algo::Volume::MovingObjects::ParticleCell3D<PFP3>;


int test_particle_cell_3D()
{

	return 0;
}

