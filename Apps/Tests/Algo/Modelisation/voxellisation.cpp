#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Modelisation/voxellisation.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};




int test_voxellisation()
{

	Algo::Surface::Modelisation::Voxellisation vox;

	return 0;
}