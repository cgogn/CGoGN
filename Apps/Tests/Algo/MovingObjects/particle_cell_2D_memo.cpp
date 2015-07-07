#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/MovingObjects/particle_cell_2D_memo.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

template class Algo::Surface::MovingObjects::ParticleCell2DMemo<PFP1>;


struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

template class Algo::Surface::MovingObjects::ParticleCell2DMemo<PFP2>;



//struct PFP3 : public PFP_DOUBLE
//{
//	typedef EmbeddedGMap2 MAP;
//};
//
//template class Algo::Surface::MovingObjects::ParticleCell2DMemo<PFP3>;
//

int test_particle_cell_2D_memo()
{

	return 0;
}

