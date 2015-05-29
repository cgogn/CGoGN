#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Modelisation/extrusion.h"

using namespace CGoGN;


// MAP2 / FLOAT instantiation
struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

template Dart Algo::Surface::Modelisation::extrusion_scale<PFP1>(PFP1::MAP& the_map,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions,
	const std::vector<PFP1::VEC3>& profile,
	const PFP1::VEC3& centerProfile,
	const PFP1::VEC3& normalProfile,
	bool profile_closed,
	const std::vector<PFP1::VEC3>& path,
	bool path_closed,
	const std::vector<float>& scalePath);

template Algo::Surface::Tilings::Tiling<PFP1>* Algo::Surface::Modelisation::extrusion_scale_prim<PFP1>(PFP1::MAP& the_map,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions,
	const std::vector<PFP1::VEC3>& profile,
	const PFP1::VEC3& centerProfile,
	const PFP1::VEC3& normalProfile,
	bool profile_closed,
	const std::vector<PFP1::VEC3>& path,
	bool path_closed,
	const std::vector<float>& scalePath);

template Algo::Surface::Tilings::Tiling<PFP1>* Algo::Surface::Modelisation::extrusion_prim<PFP1>(PFP1::MAP& the_map,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions,
	const std::vector<PFP1::VEC3>& profile,
	const PFP1::VEC3& centerProfile,
	const PFP1::VEC3& normalProfile,
	bool profile_closed,
	const std::vector<PFP1::VEC3>& path,
	bool path_closed);

template Dart Algo::Surface::Modelisation::extrusion<PFP1>(PFP1::MAP& the_map,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions,
	const std::vector<PFP1::VEC3>& profile,
	const PFP1::VEC3& centerProfile,
	const PFP1::VEC3& normal,
	bool profile_closed,
	const std::vector<PFP1::VEC3>& path,
	bool path_closed);

template Algo::Surface::Tilings::Tiling<PFP1>* Algo::Surface::Modelisation::revolution_prim<PFP1>(PFP1::MAP& the_map,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions,
	const std::vector<PFP1::VEC3>& profile,
	const PFP1::VEC3& center,
	const PFP1::VEC3& axis,
	bool profile_closed,
	int nbSides);

template Dart Algo::Surface::Modelisation::revolution<PFP1>(PFP1::MAP& the_map,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions,
	const std::vector<PFP1::VEC3>& profile,
	const PFP1::VEC3& center,
	const PFP1::VEC3& axis,
	bool profile_closed,
	int nbSides);

template Dart Algo::Surface::Modelisation::extrudeFace<PFP1>(PFP1::MAP& the_map,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, Dart d, const PFP1::VEC3& N);

template Dart Algo::Surface::Modelisation::extrudeFace<PFP1>(PFP1::MAP& the_map,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& positions, Dart d, float dist);

template Dart Algo::Surface::Modelisation::extrudeRegion<PFP1>(PFP1::MAP& the_map,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, Dart d, const CellMarker<PFP1::MAP, FACE>& cm);


// MAP2 / DOUBLE instantiation
struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

template Dart Algo::Surface::Modelisation::extrusion_scale<PFP2>(PFP2::MAP& the_map,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions,
	const std::vector<PFP2::VEC3>& profile,
	const PFP2::VEC3& centerProfile,
	const PFP2::VEC3& normalProfile,
	bool profile_closed,
	const std::vector<PFP2::VEC3>& path,
	bool path_closed,
	const std::vector<float>& scalePath);

template Algo::Surface::Tilings::Tiling<PFP2>* Algo::Surface::Modelisation::extrusion_scale_prim<PFP2>( PFP2::MAP& the_map, 
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions,
	const std::vector<PFP2::VEC3>& profile,
	const PFP2::VEC3& centerProfile,
	const PFP2::VEC3& normalProfile,
	bool profile_closed,
	const std::vector<PFP2::VEC3>& path,
	bool path_closed,
	const std::vector<float>& scalePath);

template Algo::Surface::Tilings::Tiling<PFP2>* Algo::Surface::Modelisation::extrusion_prim<PFP2>(PFP2::MAP& the_map,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions,
	const std::vector<PFP2::VEC3>& profile,
	const PFP2::VEC3& centerProfile,
	const PFP2::VEC3& normalProfile,
	bool profile_closed,
	const std::vector<PFP2::VEC3>& path,
	bool path_closed);

template Dart Algo::Surface::Modelisation::extrusion<PFP2>(PFP2::MAP& the_map,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions,
	const std::vector<PFP2::VEC3>& profile,
	const PFP2::VEC3& centerProfile,
	const PFP2::VEC3& normal,
	bool profile_closed,
	const std::vector<PFP2::VEC3>& path,
	bool path_closed);

template Algo::Surface::Tilings::Tiling<PFP2>* Algo::Surface::Modelisation::revolution_prim<PFP2>(PFP2::MAP& the_map,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions,
	const std::vector<PFP2::VEC3>& profile,
	const PFP2::VEC3& center,
	const PFP2::VEC3& axis,
	bool profile_closed,
	int nbSides);

template Dart Algo::Surface::Modelisation::revolution<PFP2>(PFP2::MAP& the_map,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions,
	const std::vector<PFP2::VEC3>& profile,
	const PFP2::VEC3& center,
	const PFP2::VEC3& axis,
	bool profile_closed,
	int nbSides);


template Dart Algo::Surface::Modelisation::extrudeFace<PFP2>(PFP2::MAP& the_map,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, Dart d, const PFP2::VEC3& N);

template Dart Algo::Surface::Modelisation::extrudeFace<PFP2>(PFP2::MAP& the_map,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions, Dart d, float dist);

template Dart Algo::Surface::Modelisation::extrudeRegion<PFP2>(PFP2::MAP& the_map,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, Dart d, const CellMarker<PFP2::MAP, FACE>& cm);



// GMAP2 / DOUBLE instantiation
struct PFP3 : public PFP_DOUBLE
{
	typedef EmbeddedGMap2 MAP;
};

template Dart Algo::Surface::Modelisation::extrusion_scale<PFP3>(PFP3::MAP& the_map,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions,
	const std::vector<PFP3::VEC3>& profile,
	const PFP3::VEC3& centerProfile,
	const PFP3::VEC3& normalProfile,
	bool profile_closed,
	const std::vector<PFP3::VEC3>& path,
	bool path_closed,
	const std::vector<float>& scalePath);

template Algo::Surface::Tilings::Tiling<PFP3>* Algo::Surface::Modelisation::extrusion_scale_prim<PFP3>(PFP3::MAP& the_map,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions,
	const std::vector<PFP3::VEC3>& profile,
	const PFP3::VEC3& centerProfile,
	const PFP3::VEC3& normalProfile,
	bool profile_closed,
	const std::vector<PFP3::VEC3>& path,
	bool path_closed,
	const std::vector<float>& scalePath);

template Algo::Surface::Tilings::Tiling<PFP3>* Algo::Surface::Modelisation::extrusion_prim<PFP3>(PFP3::MAP& the_map,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions,
	const std::vector<PFP3::VEC3>& profile,
	const PFP3::VEC3& centerProfile,
	const PFP3::VEC3& normalProfile,
	bool profile_closed,
	const std::vector<PFP3::VEC3>& path,
	bool path_closed);

template Dart Algo::Surface::Modelisation::extrusion<PFP3>(PFP3::MAP& the_map,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions,
	const std::vector<PFP3::VEC3>& profile,
	const PFP3::VEC3& centerProfile,
	const PFP3::VEC3& normal,
	bool profile_closed,
	const std::vector<PFP3::VEC3>& path,
	bool path_closed);

template Algo::Surface::Tilings::Tiling<PFP3>* Algo::Surface::Modelisation::revolution_prim<PFP3>(PFP3::MAP& the_map,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions,
	const std::vector<PFP3::VEC3>& profile,
	const PFP3::VEC3& center,
	const PFP3::VEC3& axis,
	bool profile_closed,
	int nbSides);

template Dart Algo::Surface::Modelisation::revolution<PFP3>(PFP3::MAP& the_map,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions,
	const std::vector<PFP3::VEC3>& profile,
	const PFP3::VEC3& center,
	const PFP3::VEC3& axis,
	bool profile_closed,
	int nbSides);

template Dart Algo::Surface::Modelisation::extrudeFace<PFP3>(PFP3::MAP& the_map,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, Dart d, const PFP3::VEC3& N);

template Dart Algo::Surface::Modelisation::extrudeFace<PFP3>(PFP3::MAP& the_map,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& positions, Dart d, float dist);

template Dart Algo::Surface::Modelisation::extrudeRegion<PFP3>(PFP3::MAP& the_map,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, Dart d, const CellMarker<PFP3::MAP, FACE>& cm);

int test_extrusion()
{

	return 0;
}