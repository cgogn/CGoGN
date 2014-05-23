#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"

#include "Geometry/vector_gen.h"

#include "Algo/Import/import.h"
#include "Algo/Export/export.h"

#include "Algo/Geometry/boundingbox.h"
#include "Algo/Geometry/normal.h"

#include "Algo/Modelisation/subdivision.h"
#include "Algo/Decimation/decimation.h"

#include "Utils/chrono.h"
#include "Algo/Filtering/average.h"

using namespace CGoGN ;

struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef EmbeddedMap2 MAP ;
};

typedef PFP::MAP MAP ;


int main(int argc, char **argv)
{

	if(argc != 2)
		return 1;

	MAP myMap;

	std::vector<std::string> attrNames ;
	if(!Algo::Surface::Import::importMesh<PFP>(myMap, argv[1], attrNames))
	{
		CGoGNerr << "could not import " << argv[1] << CGoGNendl ;
		return 2;
	}
	VertexAttribute<PFP::VEC3, MAP> position = myMap.getAttribute<PFP::VEC3,VERTEX,MAP>( attrNames[0]) ;


	Utils::Chrono chrono;

	chrono.start();

	Algo::Surface::Modelisation::LoopSubdivision<PFP>(myMap, position) ;
	Algo::Surface::Modelisation::LoopSubdivision<PFP>(myMap, position) ;
	Algo::Surface::Modelisation::LoopSubdivision<PFP>(myMap, position) ;
	unsigned int nbVertices = Algo::Topo::getNbOrbits<VERTEX>(myMap) ;

	std::vector<VertexAttribute<typename PFP::VEC3, MAP> *> attr;
	attr.push_back(&position);
	Algo::Surface::Decimation::decimate<PFP>(myMap, Algo::Surface::Decimation::S_QEM, Algo::Surface::Decimation::A_QEM, attr, nbVertices * 0.1) ;

	Algo::Surface::Modelisation::LoopSubdivision<PFP>(myMap, position) ;
	Algo::Surface::Modelisation::LoopSubdivision<PFP>(myMap, position) ;
	nbVertices = Algo::Topo::getNbOrbits<VERTEX>(myMap) ;

	Algo::Surface::Decimation::decimate<PFP>(myMap, Algo::Surface::Decimation::S_QEM, Algo::Surface::Decimation::A_QEM, attr, nbVertices * 0.1) ;

	Algo::Surface::Modelisation::LoopSubdivision<PFP>(myMap, position) ;
	Algo::Surface::Modelisation::LoopSubdivision<PFP>(myMap, position) ;
	nbVertices = Algo::Topo::getNbOrbits<VERTEX>(myMap) ;

	Algo::Surface::Decimation::decimate<PFP>(myMap, Algo::Surface::Decimation::S_QEM, Algo::Surface::Decimation::A_QEM, attr, nbVertices * 0.1) ;

	Algo::Surface::Modelisation::CatmullClarkSubdivision<PFP>(myMap, position) ;
	Algo::Surface::Modelisation::CatmullClarkSubdivision<PFP>(myMap, position) ;

	CGoGNout << "BenchTime dynamic "<< chrono.elapsed() << " ms"<< CGoGNendl;

	Algo::Surface::Export::exportOFF<PFP>(myMap,position,"bench_res.off");

	return 0;
}
