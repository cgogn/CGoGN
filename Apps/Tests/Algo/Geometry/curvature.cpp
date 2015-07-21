#include <iostream>
#include "Topology/generic/parameters.h"
#include "Topology/gmap/embeddedGMap2.h"
#include "Topology/map/embeddedMap2.h"

#include "Algo/Geometry/curvature.h"

using namespace CGoGN;

struct PFP1 : public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

struct PFP2 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};

struct PFP3 : public PFP_STANDARD
{
	typedef EmbeddedGMap2 MAP;
};


using namespace CGoGN;


/*****************************************
*		MAP FLOAT INSTANTIATION
*****************************************/


template void Algo::Surface::Geometry::computeCurvatureVertices_QuadraticFitting<PFP1>(
	PFP1::MAP& map,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& normal,
	VertexAttribute<PFP1::REAL, PFP1::MAP>& kmax,
	VertexAttribute<PFP1::REAL, PFP1::MAP>& kmin,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& Kmax,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& Kmin
	);

template void Algo::Surface::Geometry::computeCurvatureVertex_QuadraticFitting<PFP1>(
	PFP1::MAP& map,
	Vertex v,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& normal,
	VertexAttribute<PFP1::REAL, PFP1::MAP>& kmax,
	VertexAttribute<PFP1::REAL, PFP1::MAP>& kmin,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& Kmax,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& Kmin
	);

template void Algo::Surface::Geometry::vertexQuadraticFitting<PFP1>(
	PFP1::MAP& map,
	Vertex v,
	PFP1::MATRIX33& localFrame,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& normal,
	PFP1::REAL& a, PFP1::REAL& b, PFP1::REAL& c, PFP1::REAL& d, PFP1::REAL& e
	);

template void Algo::Surface::Geometry::quadraticFittingAddVertexPos<PFP1>(
	PFP1::VEC3& v,
	PFP1::VEC3& p,
	PFP1::MATRIX33& localFrame
	);

template void Algo::Surface::Geometry::quadraticFittingAddVertexNormal<PFP1>(
	PFP1::VEC3& v,
	PFP1::VEC3& n,
	PFP1::VEC3& p,
	PFP1::MATRIX33& localFrame
	);


template void Algo::Surface::Geometry::computeCurvatureVertices_NormalCycles<PFP1>(
	PFP1::MAP& map,
	PFP1::REAL radius,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& normal,
	const EdgeAttribute<PFP1::REAL, PFP1::MAP>& edgeangle,
	const EdgeAttribute<PFP1::REAL, PFP1::MAP>& edgearea,
	VertexAttribute<PFP1::REAL, PFP1::MAP>& kmax,
	VertexAttribute<PFP1::REAL, PFP1::MAP>& kmin,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& Kmax,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& Kmin,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& Knormal
	);

template void Algo::Surface::Geometry::computeCurvatureVertex_NormalCycles<PFP1>(
	PFP1::MAP& map,
	Vertex v,
	PFP1::REAL radius,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& normal,
	const EdgeAttribute<PFP1::REAL, PFP1::MAP>& edgeangle,
	const EdgeAttribute<PFP1::REAL, PFP1::MAP>& edgearea,
	VertexAttribute<PFP1::REAL, PFP1::MAP>& kmax,
	VertexAttribute<PFP1::REAL, PFP1::MAP>& kmin,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& Kmax,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& Kmin,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& Knormal
	);

template void Algo::Surface::Geometry::computeCurvatureVertices_NormalCycles_Projected<PFP1>(
	PFP1::MAP& map,
	PFP1::REAL radius,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& normal,
	const EdgeAttribute<PFP1::REAL, PFP1::MAP>& edgeangle,
	const EdgeAttribute<PFP1::REAL, PFP1::MAP>& edgearea,
	VertexAttribute<PFP1::REAL, PFP1::MAP>& kmax,
	VertexAttribute<PFP1::REAL, PFP1::MAP>& kmin,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& Kmax,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& Kmin,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& Knormal
	);

template void Algo::Surface::Geometry::computeCurvatureVertex_NormalCycles_Projected<PFP1>(
	PFP1::MAP& map,
	Vertex v,
	PFP1::REAL radius,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& normal,
	const EdgeAttribute<PFP1::REAL, PFP1::MAP>& edgeangle,
	const EdgeAttribute<PFP1::REAL, PFP1::MAP>& edgearea,
	VertexAttribute<PFP1::REAL, PFP1::MAP>& kmax,
	VertexAttribute<PFP1::REAL, PFP1::MAP>& kmin,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& Kmax,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& Kmin,
	VertexAttribute<PFP1::VEC3, PFP1::MAP>& Knormal
	);



template void Algo::Surface::Geometry::normalCycles_computeTensor<PFP1>(
	Algo::Surface::Selection::Collector<PFP1>& col,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
	Geom::Matrix<3, 3, PFP1::REAL>& tensor
	);

template void Algo::Surface::Geometry::normalCycles_computeTensor<PFP1>(
	Algo::Surface::Selection::Collector<PFP1>& col,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
	const EdgeAttribute<PFP1::REAL, PFP1::MAP>& edgeangle,
	Geom::Matrix<3, 3, PFP1::REAL>& tensor
	);

template void Algo::Surface::Geometry::normalCycles_computeTensor<PFP1>(
	Algo::Surface::Selection::Collector<PFP1>& col,
	const VertexAttribute<PFP1::VEC3, PFP1::MAP>& position,
	const EdgeAttribute<PFP1::REAL, PFP1::MAP>& edgeangle,
	const EdgeAttribute<PFP1::REAL, PFP1::MAP>& edgearea,
	Geom::Matrix<3, 3, PFP1::REAL>& tensor
	);

template void Algo::Surface::Geometry::normalCycles_SortAndSetEigenComponents<PFP1>(
	const PFP1::VEC3& e_val,
	const Geom::Matrix<3, 3, PFP1::REAL> & e_vec,
	PFP1::REAL& kmax,
	PFP1::REAL& kmin,
	PFP1::VEC3& Kmax,
	PFP1::VEC3& Kmin,
	PFP1::VEC3& Knormal,
	const PFP1::VEC3& normal
	);

template void Algo::Surface::Geometry::normalCycles_SortTensor<PFP1>(Geom::Matrix<3, 3, PFP1::REAL>& tensor);

template void Algo::Surface::Geometry::normalCycles_ProjectTensor<PFP1>(Geom::Matrix<3, 3, PFP1::REAL>& tensor, const PFP1::VEC3& normal_vector);


/*****************************************
*		MAP DOUBLE INSTANTIATION
*****************************************/


template void Algo::Surface::Geometry::computeCurvatureVertices_QuadraticFitting<PFP2>(
	PFP2::MAP& map,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& normal,
	VertexAttribute<PFP2::REAL, PFP2::MAP>& kmax,
	VertexAttribute<PFP2::REAL, PFP2::MAP>& kmin,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& Kmax,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& Kmin
	);

template void Algo::Surface::Geometry::computeCurvatureVertex_QuadraticFitting<PFP2>(
	PFP2::MAP& map,
	Vertex v,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& normal,
	VertexAttribute<PFP2::REAL, PFP2::MAP>& kmax,
	VertexAttribute<PFP2::REAL, PFP2::MAP>& kmin,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& Kmax,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& Kmin
	);

template void Algo::Surface::Geometry::vertexQuadraticFitting<PFP2>(
	PFP2::MAP& map,
	Vertex v,
	PFP2::MATRIX33& localFrame,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& normal,
	PFP2::REAL& a, PFP2::REAL& b, PFP2::REAL& c, PFP2::REAL& d, PFP2::REAL& e
	);

template void Algo::Surface::Geometry::quadraticFittingAddVertexPos<PFP2>(
	PFP2::VEC3& v,
	PFP2::VEC3& p,
	PFP2::MATRIX33& localFrame
	);

template void Algo::Surface::Geometry::quadraticFittingAddVertexNormal<PFP2>(
	PFP2::VEC3& v,
	PFP2::VEC3& n,
	PFP2::VEC3& p,
	PFP2::MATRIX33& localFrame
	);


template void Algo::Surface::Geometry::computeCurvatureVertices_NormalCycles<PFP2>(
	PFP2::MAP& map,
	PFP2::REAL radius,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& normal,
	const EdgeAttribute<PFP2::REAL, PFP2::MAP>& edgeangle,
	const EdgeAttribute<PFP2::REAL, PFP2::MAP>& edgearea,
	VertexAttribute<PFP2::REAL, PFP2::MAP>& kmax,
	VertexAttribute<PFP2::REAL, PFP2::MAP>& kmin,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& Kmax,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& Kmin,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& Knormal
	);

template void Algo::Surface::Geometry::computeCurvatureVertex_NormalCycles<PFP2>(
	PFP2::MAP& map,
	Vertex v,
	PFP2::REAL radius,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& normal,
	const EdgeAttribute<PFP2::REAL, PFP2::MAP>& edgeangle,
	const EdgeAttribute<PFP2::REAL, PFP2::MAP>& edgearea,
	VertexAttribute<PFP2::REAL, PFP2::MAP>& kmax,
	VertexAttribute<PFP2::REAL, PFP2::MAP>& kmin,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& Kmax,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& Kmin,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& Knormal
	);

template void Algo::Surface::Geometry::computeCurvatureVertices_NormalCycles_Projected<PFP2>(
	PFP2::MAP& map,
	PFP2::REAL radius,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& normal,
	const EdgeAttribute<PFP2::REAL, PFP2::MAP>& edgeangle,
	const EdgeAttribute<PFP2::REAL, PFP2::MAP>& edgearea,
	VertexAttribute<PFP2::REAL, PFP2::MAP>& kmax,
	VertexAttribute<PFP2::REAL, PFP2::MAP>& kmin,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& Kmax,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& Kmin,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& Knormal
	);

template void Algo::Surface::Geometry::computeCurvatureVertex_NormalCycles_Projected<PFP2>(
	PFP2::MAP& map,
	Vertex v,
	PFP2::REAL radius,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& normal,
	const EdgeAttribute<PFP2::REAL, PFP2::MAP>& edgeangle,
	const EdgeAttribute<PFP2::REAL, PFP2::MAP>& edgearea,
	VertexAttribute<PFP2::REAL, PFP2::MAP>& kmax,
	VertexAttribute<PFP2::REAL, PFP2::MAP>& kmin,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& Kmax,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& Kmin,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& Knormal
	);



template void Algo::Surface::Geometry::normalCycles_computeTensor<PFP2>(
	Algo::Surface::Selection::Collector<PFP2>& col,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	Geom::Matrix<3, 3, PFP2::REAL>& tensor
	);

template void Algo::Surface::Geometry::normalCycles_computeTensor<PFP2>(
	Algo::Surface::Selection::Collector<PFP2>& col,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const EdgeAttribute<PFP2::REAL, PFP2::MAP>& edgeangle,
	Geom::Matrix<3, 3, PFP2::REAL>& tensor
	);

template void Algo::Surface::Geometry::normalCycles_computeTensor<PFP2>(
	Algo::Surface::Selection::Collector<PFP2>& col,
	const VertexAttribute<PFP2::VEC3, PFP2::MAP>& position,
	const EdgeAttribute<PFP2::REAL, PFP2::MAP>& edgeangle,
	const EdgeAttribute<PFP2::REAL, PFP2::MAP>& edgearea,
	Geom::Matrix<3, 3, PFP2::REAL>& tensor
	);

template void Algo::Surface::Geometry::normalCycles_SortAndSetEigenComponents<PFP2>(
	const PFP2::VEC3& e_val,
	const Geom::Matrix<3, 3, PFP2::REAL> & e_vec,
	PFP2::REAL& kmax,
	PFP2::REAL& kmin,
	PFP2::VEC3& Kmax,
	PFP2::VEC3& Kmin,
	PFP2::VEC3& Knormal,
	const PFP2::VEC3& normal
	);

template void Algo::Surface::Geometry::normalCycles_SortTensor<PFP2>(Geom::Matrix<3, 3, PFP2::REAL>& tensor);

template void Algo::Surface::Geometry::normalCycles_ProjectTensor<PFP2>(Geom::Matrix<3, 3, PFP2::REAL>& tensor, const PFP2::VEC3& normal_vector);


/*****************************************
*		GMAP float INSTANTIATION
*****************************************/


template void Algo::Surface::Geometry::computeCurvatureVertices_QuadraticFitting<PFP3>(
	PFP3::MAP& map,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& normal,
	VertexAttribute<PFP3::REAL, PFP3::MAP>& kmax,
	VertexAttribute<PFP3::REAL, PFP3::MAP>& kmin,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& Kmax,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& Kmin
	);

template void Algo::Surface::Geometry::computeCurvatureVertex_QuadraticFitting<PFP3>(
	PFP3::MAP& map,
	Vertex v,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& normal,
	VertexAttribute<PFP3::REAL, PFP3::MAP>& kmax,
	VertexAttribute<PFP3::REAL, PFP3::MAP>& kmin,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& Kmax,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& Kmin
	);

template void Algo::Surface::Geometry::vertexQuadraticFitting<PFP3>(
	PFP3::MAP& map,
	Vertex v,
	PFP3::MATRIX33& localFrame,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& normal,
	PFP3::REAL& a, PFP3::REAL& b, PFP3::REAL& c, PFP3::REAL& d, PFP3::REAL& e
	);

template void Algo::Surface::Geometry::quadraticFittingAddVertexPos<PFP3>(
	PFP3::VEC3& v,
	PFP3::VEC3& p,
	PFP3::MATRIX33& localFrame
	);

template void Algo::Surface::Geometry::quadraticFittingAddVertexNormal<PFP3>(
	PFP3::VEC3& v,
	PFP3::VEC3& n,
	PFP3::VEC3& p,
	PFP3::MATRIX33& localFrame
	);


template void Algo::Surface::Geometry::computeCurvatureVertices_NormalCycles<PFP3>(
	PFP3::MAP& map,
	PFP3::REAL radius,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& normal,
	const EdgeAttribute<PFP3::REAL, PFP3::MAP>& edgeangle,
	const EdgeAttribute<PFP3::REAL, PFP3::MAP>& edgearea,
	VertexAttribute<PFP3::REAL, PFP3::MAP>& kmax,
	VertexAttribute<PFP3::REAL, PFP3::MAP>& kmin,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& Kmax,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& Kmin,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& Knormal
	);

template void Algo::Surface::Geometry::computeCurvatureVertex_NormalCycles<PFP3>(
	PFP3::MAP& map,
	Vertex v,
	PFP3::REAL radius,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& normal,
	const EdgeAttribute<PFP3::REAL, PFP3::MAP>& edgeangle,
	const EdgeAttribute<PFP3::REAL, PFP3::MAP>& edgearea,
	VertexAttribute<PFP3::REAL, PFP3::MAP>& kmax,
	VertexAttribute<PFP3::REAL, PFP3::MAP>& kmin,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& Kmax,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& Kmin,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& Knormal
	);

template void Algo::Surface::Geometry::computeCurvatureVertices_NormalCycles_Projected<PFP3>(
	PFP3::MAP& map,
	PFP3::REAL radius,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& normal,
	const EdgeAttribute<PFP3::REAL, PFP3::MAP>& edgeangle,
	const EdgeAttribute<PFP3::REAL, PFP3::MAP>& edgearea,
	VertexAttribute<PFP3::REAL, PFP3::MAP>& kmax,
	VertexAttribute<PFP3::REAL, PFP3::MAP>& kmin,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& Kmax,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& Kmin,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& Knormal
	);

template void Algo::Surface::Geometry::computeCurvatureVertex_NormalCycles_Projected<PFP3>(
	PFP3::MAP& map,
	Vertex v,
	PFP3::REAL radius,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& normal,
	const EdgeAttribute<PFP3::REAL, PFP3::MAP>& edgeangle,
	const EdgeAttribute<PFP3::REAL, PFP3::MAP>& edgearea,
	VertexAttribute<PFP3::REAL, PFP3::MAP>& kmax,
	VertexAttribute<PFP3::REAL, PFP3::MAP>& kmin,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& Kmax,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& Kmin,
	VertexAttribute<PFP3::VEC3, PFP3::MAP>& Knormal
	);



template void Algo::Surface::Geometry::normalCycles_computeTensor<PFP3>(
	Algo::Surface::Selection::Collector<PFP3>& col,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	Geom::Matrix<3, 3, PFP3::REAL>& tensor
	);

template void Algo::Surface::Geometry::normalCycles_computeTensor<PFP3>(
	Algo::Surface::Selection::Collector<PFP3>& col,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const EdgeAttribute<PFP3::REAL, PFP3::MAP>& edgeangle,
	Geom::Matrix<3, 3, PFP3::REAL>& tensor
	);

template void Algo::Surface::Geometry::normalCycles_computeTensor<PFP3>(
	Algo::Surface::Selection::Collector<PFP3>& col,
	const VertexAttribute<PFP3::VEC3, PFP3::MAP>& position,
	const EdgeAttribute<PFP3::REAL, PFP3::MAP>& edgeangle,
	const EdgeAttribute<PFP3::REAL, PFP3::MAP>& edgearea,
	Geom::Matrix<3, 3, PFP3::REAL>& tensor
	);

template void Algo::Surface::Geometry::normalCycles_SortAndSetEigenComponents<PFP3>(
	const PFP3::VEC3& e_val,
	const Geom::Matrix<3, 3, PFP3::REAL> & e_vec,
	PFP3::REAL& kmax,
	PFP3::REAL& kmin,
	PFP3::VEC3& Kmax,
	PFP3::VEC3& Kmin,
	PFP3::VEC3& Knormal,
	const PFP3::VEC3& normal
	);

template void Algo::Surface::Geometry::normalCycles_SortTensor<PFP3>(Geom::Matrix<3, 3, PFP3::REAL>& tensor);

template void Algo::Surface::Geometry::normalCycles_ProjectTensor<PFP3>(Geom::Matrix<3, 3, PFP3::REAL>& tensor, const PFP3::VEC3& normal_vector);




int test_curvature()
{
	return 0;
}
