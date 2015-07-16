#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/Modelisation/subdivision.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

typedef VertexAttribute<PFP1::VEC3, PFP1::MAP> VPOS1;



template Dart Algo::Surface::Modelisation::trianguleFace<PFP1>(PFP1::MAP& map, Dart d);
template void Algo::Surface::Modelisation::trianguleFaces<PFP1,VPOS1>(PFP1::MAP& map, VPOS1& attributs);
template void Algo::Surface::Modelisation::trianguleFaces<PFP1>(PFP1::MAP& map,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& position, const FaceAttribute<PFP1::VEC3, PFP1::MAP>& positionF) ;
template Dart Algo::Surface::Modelisation::quadranguleFace<PFP1>(PFP1::MAP& map, Dart d);
template void Algo::Surface::Modelisation::quadranguleFaces<PFP1, VPOS1>(PFP1::MAP& map, VPOS1& attributs);
template void Algo::Surface::Modelisation::CatmullClarkSubdivision<PFP1, VPOS1>(PFP1::MAP& map, VPOS1& attributs);
template void Algo::Surface::Modelisation::CatmullClarkInterpolSubdivision<PFP1, VPOS1>(PFP1::MAP& map, VPOS1& attributs);
template void Algo::Surface::Modelisation::LoopSubdivision<PFP1, VPOS1>(PFP1::MAP& map, VPOS1& attributs);
template void Algo::Surface::Modelisation::LoopSubdivisionGen<PFP1>(PFP1::MAP& map, VertexAttributeGen& attrib);
template void Algo::Surface::Modelisation::LoopSubdivisionAttribNameTyped<PFP1, Geom::Vec3d>(PFP1::MAP& map, const std::string& nameAttrib);
template void Algo::Surface::Modelisation::LoopSubdivisionAttribName<PFP1>(PFP1::MAP& map, const std::string& nameAttrib);
template void Algo::Surface::Modelisation::TwoNPlusOneSubdivision<PFP1, VPOS1>(PFP1::MAP& map, VPOS1& attributs, float size);
template void Algo::Surface::Modelisation::DooSabin<PFP1, VPOS1>(PFP1::MAP& map, VPOS1& position);
template void Algo::Surface::Modelisation::computeDual<PFP1>(PFP1::MAP& map, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template void Algo::Surface::Modelisation::computeBoundaryConstraintDual<PFP1>(PFP1::MAP& map, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);
template void Algo::Surface::Modelisation::computeBoundaryConstraintKeepingOldVerticesDual<PFP1>(PFP1::MAP& map, VertexAttribute<PFP1::VEC3, PFP1::MAP>& position);



struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

typedef VertexAttribute<PFP2::VEC3, PFP2::MAP> VPOS2;


template Dart Algo::Surface::Modelisation::trianguleFace<PFP2>(PFP2::MAP& map, Dart d);
template void Algo::Surface::Modelisation::trianguleFaces<PFP2, VPOS2>(PFP2::MAP& map, VPOS2& attributs);
template void Algo::Surface::Modelisation::trianguleFaces<PFP2>(PFP2::MAP& map,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& position, const FaceAttribute<PFP2::VEC3, PFP2::MAP>& positionF);
template Dart Algo::Surface::Modelisation::quadranguleFace<PFP2>(PFP2::MAP& map, Dart d);
template void Algo::Surface::Modelisation::quadranguleFaces<PFP2, VPOS2>(PFP2::MAP& map, VPOS2& attributs);
template void Algo::Surface::Modelisation::CatmullClarkSubdivision<PFP2, VPOS2>(PFP2::MAP& map, VPOS2& attributs);
template void Algo::Surface::Modelisation::CatmullClarkInterpolSubdivision<PFP2, VPOS2>(PFP2::MAP& map, VPOS2& attributs);
template void Algo::Surface::Modelisation::LoopSubdivision<PFP2, VPOS2>(PFP2::MAP& map, VPOS2& attributs);
template void Algo::Surface::Modelisation::LoopSubdivisionGen<PFP2>(PFP2::MAP& map, VertexAttributeGen& attrib);
template void Algo::Surface::Modelisation::LoopSubdivisionAttribNameTyped<PFP2, Geom::Vec3d>(PFP2::MAP& map, const std::string& nameAttrib);
template void Algo::Surface::Modelisation::LoopSubdivisionAttribName<PFP2>(PFP2::MAP& map, const std::string& nameAttrib);
template void Algo::Surface::Modelisation::TwoNPlusOneSubdivision<PFP2, VPOS2>(PFP2::MAP& map, VPOS2& attributs, float size);
template void Algo::Surface::Modelisation::DooSabin<PFP2, VPOS2>(PFP2::MAP& map, VPOS2& position);
template void Algo::Surface::Modelisation::computeDual<PFP2>(PFP2::MAP& map, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template void Algo::Surface::Modelisation::computeBoundaryConstraintDual<PFP2>(PFP2::MAP& map, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);
template void Algo::Surface::Modelisation::computeBoundaryConstraintKeepingOldVerticesDual<PFP2>(PFP2::MAP& map, VertexAttribute<PFP2::VEC3, PFP2::MAP>& position);




struct PFP3 : public PFP_DOUBLE
{
	typedef EmbeddedGMap2 MAP;
};

typedef VertexAttribute<PFP3::VEC3, PFP3::MAP> VPOS3;


template Dart Algo::Surface::Modelisation::trianguleFace<PFP3>(PFP3::MAP& map, Dart d);
template void Algo::Surface::Modelisation::trianguleFaces<PFP3, VPOS3>(PFP3::MAP& map, VPOS3& attributs);
template void Algo::Surface::Modelisation::trianguleFaces<PFP3>(PFP3::MAP& map,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& position, const FaceAttribute<PFP3::VEC3, PFP3::MAP>& positionF);
template Dart Algo::Surface::Modelisation::quadranguleFace<PFP3>(PFP3::MAP& map, Dart d);
template void Algo::Surface::Modelisation::quadranguleFaces<PFP3, VPOS3>(PFP3::MAP& map, VPOS3& attributs);
template void Algo::Surface::Modelisation::CatmullClarkSubdivision<PFP3, VPOS3>(PFP3::MAP& map, VPOS3& attributs);
template void Algo::Surface::Modelisation::CatmullClarkInterpolSubdivision<PFP3, VPOS3>(PFP3::MAP& map, VPOS3& attributs);
template void Algo::Surface::Modelisation::LoopSubdivision<PFP3, VPOS3>(PFP3::MAP& map, VPOS3& attributs);
template void Algo::Surface::Modelisation::LoopSubdivisionGen<PFP3>(PFP3::MAP& map, VertexAttributeGen& attrib);
template void Algo::Surface::Modelisation::LoopSubdivisionAttribNameTyped<PFP3, Geom::Vec3d>(PFP3::MAP& map, const std::string& nameAttrib);
template void Algo::Surface::Modelisation::LoopSubdivisionAttribName<PFP3>(PFP3::MAP& map, const std::string& nameAttrib);
template void Algo::Surface::Modelisation::TwoNPlusOneSubdivision<PFP3, VPOS3>(PFP3::MAP& map, VPOS3& attributs, float size);
//template void Algo::Surface::Modelisation::DooSabin<PFP3, VPOS3>(PFP3::MAP& map, VPOS3& position);
template void Algo::Surface::Modelisation::computeDual<PFP3>(PFP3::MAP& map, VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
//template void Algo::Surface::Modelisation::computeBoundaryConstraintDual<PFP3>(PFP3::MAP& map, VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);
//template void Algo::Surface::Modelisation::computeBoundaryConstraintKeepingOldVerticesDual<PFP3>(PFP3::MAP& map, VertexAttribute<PFP3::VEC3, PFP3::MAP>& position);



int test_subdivision()
{

	return 0;
}