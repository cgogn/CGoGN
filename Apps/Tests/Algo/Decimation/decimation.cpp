#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Decimation/decimation.h"


using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

struct PFP3 : public PFP_DOUBLE
{
	typedef EmbeddedGMap2 MAP;
};



template int Algo::Surface::Decimation::decimate<PFP1, Geom::Vec3f>(
	PFP1::MAP& map,
	SelectorType s,
	ApproximatorType a,
	std::vector<VertexAttribute<Geom::Vec3f, PFP1::MAP> >& attribs,
	unsigned int nbWantedVertices,
	EdgeAttribute<PFP1::REAL, PFP1::MAP>* edgeErrors,
	void(*callback_wrapper)(void*, const void*) ,
	void* callback_object
	);


template int Algo::Surface::Decimation::decimate<PFP1>(
	PFP1::MAP& map,
	Selector<PFP1>* s,
	std::vector<ApproximatorGen<PFP1>*>& a,
	unsigned int nbWantedVertices,
	bool recomputePriorityList,
	EdgeAttribute<PFP1::REAL, PFP1::MAP>* edgeErrors,
	void(*callback_wrapper)(void*, const void*),
	void* callback_object 
	);


template int Algo::Surface::Decimation::decimate<PFP2, Geom::Vec3d>(
	PFP2::MAP& map,
	SelectorType s,
	ApproximatorType a,
	std::vector<VertexAttribute<Geom::Vec3d, PFP2::MAP> >& attribs,
	unsigned int nbWantedVertices,
	EdgeAttribute<PFP2::REAL, PFP2::MAP>* edgeErrors,
	void(*callback_wrapper)(void*, const void*),
	void* callback_object
	);
//
//
//template int Algo::Surface::Decimation::decimate<PFP2>(
//	PFP2::MAP& map,
//	Selector<PFP2>* s,
//	std::vector<ApproximatorGen<PFP2>*>& a,
//	unsigned int nbWantedVertices,
//	bool recomputePriorityList,
//	EdgeAttribute<PFP2::REAL, PFP2::MAP>* edgeErrors,
//	void(*callback_wrapper)(void*, const void*),
//	void* callback_object
//	);



int test_decimation()
{

	return 0;
}