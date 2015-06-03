#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Import/import.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

template bool Algo::Surface::Import::importMesh<PFP1>(PFP1::MAP& map, const std::string& filename, std::vector<std::string>& attrNames, bool mergeCloseVertices);
template bool Algo::Surface::Import::importVoxellisation<PFP1>(PFP1::MAP& map, Algo::Surface::Modelisation::Voxellisation& voxellisation, std::vector<std::string>& attrNames, bool mergeCloseVertices);
//template bool Algo::Surface::Import::importChoupi<PFP1>(const std::string& filename, const std::vector<PFP1::VEC3>& tabV, const std::vector<unsigned int>& tabE);

struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

template bool Algo::Surface::Import::importMesh<PFP2>(PFP2::MAP& map, const std::string& filename, std::vector<std::string>& attrNames, bool mergeCloseVertices);
template bool Algo::Surface::Import::importVoxellisation<PFP2>(PFP2::MAP& map, Algo::Surface::Modelisation::Voxellisation& voxellisation, std::vector<std::string>& attrNames, bool mergeCloseVertices);
//template bool Algo::Surface::Import::importChoupi<PFP2>(const std::string& filename, const std::vector<PFP2::VEC3>& tabV, const std::vector<unsigned int>& tabE);

struct PFP3 : public PFP_DOUBLE
{
	typedef EmbeddedGMap2 MAP;
};

template bool Algo::Surface::Import::importMesh<PFP3>(PFP3::MAP& map, const std::string& filename, std::vector<std::string>& attrNames, bool mergeCloseVertices);
template bool Algo::Surface::Import::importVoxellisation<PFP3>(PFP3::MAP& map, Algo::Surface::Modelisation::Voxellisation& voxellisation, std::vector<std::string>& attrNames, bool mergeCloseVertices);
//template bool Algo::Surface::Import::importChoupi<PFP3>(const std::string& filename, const std::vector<PFP3::VEC3>& tabV, const std::vector<unsigned int>& tabE);




struct PFP4 : public PFP_STANDARD
{
	typedef EmbeddedMap3 MAP;
};

template bool Algo::Volume::Import::importMesh<PFP4>(PFP4::MAP& map, const std::string& filename, std::vector<std::string>& attrNames);
template bool Algo::Volume::Import::importMeshToExtrude<PFP4>(PFP4::MAP& map, const std::string& filename, std::vector<std::string>& attrNames, float scale, unsigned int nbStage);
//template bool Algo::Volume::Import::importMeshSAsV<PFP4>(PFP4::MAP& map, const std::string& filename, std::vector<std::string>& attrNames);

struct PFP5 : public PFP_DOUBLE
{
	typedef EmbeddedMap3 MAP;
};

template bool Algo::Volume::Import::importMesh<PFP5>(PFP5::MAP& map, const std::string& filename, std::vector<std::string>& attrNames);
template bool Algo::Volume::Import::importMeshToExtrude<PFP5>(PFP5::MAP& map, const std::string& filename, std::vector<std::string>& attrNames, float scale, unsigned int nbStage);
//template bool Algo::Volume::Import::importMeshSAsV<PFP5>(PFP5::MAP& map, const std::string& filename, std::vector<std::string>& attrNames);



int test_import()
{

	return 0;
}
