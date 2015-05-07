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
typedef PFP::VEC3 VEC3;


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

	FaceAttribute<VEC3, MAP> center = myMap.addAttribute<VEC3, FACE, MAP>("centerFace");
	foreach_cell<FACE>(myMap, [&](Face f)
	{
		center[f] =Algo::Surface::Geometry::faceCentroid<PFP>(myMap,f,position);
	});

	EdgeAttribute<float, MAP> edge_att = myMap.addAttribute<float, EDGE, MAP>("pipoEdge");
	foreach_cell<EDGE>(myMap, [&](Edge e)
	{
		edge_att[e] = position[e.dart][0];
	});


	Utils::Chrono chrono;
	chrono.start();

	Algo::Surface::Modelisation::LoopSubdivision<PFP>(myMap, position) ;
	Algo::Surface::Modelisation::LoopSubdivision<PFP>(myMap, position) ;
	Algo::Surface::Modelisation::LoopSubdivision<PFP>(myMap, position) ;
	Algo::Surface::Modelisation::LoopSubdivision<PFP>(myMap, position) ;
	unsigned int nbVertices = Algo::Topo::getNbOrbits<VERTEX>(myMap) ;

	std::cout << "4 passes loop -> "<< std::endl;
	std::cout << "  NB Faces "<< Algo::Topo::getNbOrbits<FACE>(myMap) << std::endl;
	std::cout << "  NB Vertices "<< nbVertices << std::endl;

	std::vector<VertexAttribute<typename PFP::VEC3, MAP> > attr;
	attr.push_back(position);
	Algo::Surface::Decimation::decimate<PFP>(myMap, Algo::Surface::Decimation::S_QEM, Algo::Surface::Decimation::A_QEM, attr, nbVertices /20) ;

	VertexAttribute<PFP::VEC3, MAP> normal = myMap.addAttribute<PFP::VEC3,VERTEX,MAP>( "normal") ;
	VertexAttribute<PFP::VEC3, MAP> position2 = myMap.addAttribute<PFP::VEC3,VERTEX,MAP>( "pos2") ;

	CGoGNout << "creating "<< chrono.elapsed() << " ms"<< CGoGNendl;

	std::cout << "decimation 5% -> "<< std::endl;
	std::cout << "  NB Faces "<< Algo::Topo::getNbOrbits<FACE>(myMap) << std::endl;
	std::cout << "  NB Vertices "<<  Algo::Topo::getNbOrbits<VERTEX>(myMap) << std::endl;


	chrono.start();
	for (int i=0; i < 100; ++i)
	{
		Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position) ;
		Algo::Surface::Geometry::computeNormalVertices<PFP>(myMap, position, normal) ;
		Algo::Surface::Filtering::filterAverageAttribute_OneRing<PFP,PFP::VEC3>(myMap,position,position2, Algo::Surface::Filtering::INSIDE);
		myMap.swapAttributes(position,position2);
	}


	CGoGNout << "BenchTime process "<< chrono.elapsed() << " ms"<< CGoGNendl;

	std::cout << "Topologie fragmentation: "<<myMap.fragmentation(DART)<< std::endl;
	std::cout << "Vertex fragmentation: "<<myMap.fragmentation(VERTEX)<< std::endl;
	std::cout << "Face fragmentation: "<<myMap.fragmentation(FACE)<< std::endl;

	chrono.start();
	myMap.compact();
	std::cout << std::endl << "compacting in "<< chrono.elapsed() << " ms"<< std::endl << std::endl;


	std::cout << "Topologie fragmentation: "<<myMap.fragmentation(DART)<< std::endl;
	std::cout << "Vertex fragmentation: "<<myMap.fragmentation(VERTEX)<< std::endl;
	std::cout << "Face fragmentation: "<<myMap.fragmentation(FACE)<< std::endl;


	chrono.start();

	for (int i=0; i < 100; ++i)
	{
		Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position) ;
		Algo::Surface::Geometry::computeNormalVertices<PFP>(myMap, position, normal) ;
		Algo::Surface::Filtering::filterAverageAttribute_OneRing<PFP,PFP::VEC3>(myMap,position,position2, Algo::Surface::Filtering::INSIDE);
		myMap.swapAttributes(position,position2);
	}


	CGoGNout << "BenchTime after compact "<< chrono.elapsed() << " ms"<< CGoGNendl;


	Algo::Surface::Export::exportOFF<PFP>(myMap,position,"bench_res.off");

	return 0;
}
