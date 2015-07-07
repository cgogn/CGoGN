#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/map/embeddedMap3.h"
#include "Topology/gmap/embeddedGMap2.h"


#include "Algo/LinearSolving/basic.h"

using namespace CGoGN;


template struct Algo::LinearSolving::Coeff<float>;
template struct Algo::LinearSolving::Coeff<double>;


struct PFP1 : public PFP_DOUBLE
{
	typedef EmbeddedMap2 MAP;
};
typedef float SCAL_1;
typedef Geom::Vec3f VEC_1;

template void Algo::LinearSolving::setupVariables<PFP1, SCAL_1>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	const CellMarker<PFP1::MAP, VERTEX>& freeMarker,
	const VertexAttribute<SCAL_1, PFP1::MAP>& attr);

template void Algo::LinearSolving::setupVariables<PFP1, VEC_1>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	const CellMarker<PFP1::MAP, VERTEX>& freeMarker,
	const VertexAttribute<VEC_1, PFP1::MAP>& attr,
	unsigned int coord);

template void Algo::LinearSolving::addRowsRHS_Equality<PFP1, SCAL_1>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	const VertexAttribute<SCAL_1, PFP1::MAP>& attr,
	const VertexAttribute<PFP1::REAL, PFP1::MAP>& weight);

template void Algo::LinearSolving::addRowsRHS_Equality<PFP1, SCAL_1>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	const VertexAttribute<SCAL_1, PFP1::MAP>& attr,
	float weight);

template void Algo::LinearSolving::addRowsRHS_Equality<PFP1, VEC_1>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	const VertexAttribute<VEC_1, PFP1::MAP>& attr,
	const VertexAttribute<PFP1::REAL, PFP1::MAP>& weight,
	unsigned int coord);

template void Algo::LinearSolving::addRowsRHS_Equality<PFP1, VEC_1>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	const VertexAttribute<VEC_1, PFP1::MAP>& attr,
	float weight,
	unsigned int coord);

template void Algo::LinearSolving::addRows_Laplacian_Topo<PFP1>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index);

template void Algo::LinearSolving::addRowsRHS_Laplacian_Topo<PFP1, SCAL_1>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	const VertexAttribute<SCAL_1, PFP1::MAP>& attr);

template void Algo::LinearSolving::addRowsRHS_Laplacian_Topo<PFP1, VEC_1>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	const VertexAttribute<VEC_1, PFP1::MAP>& attr,
	unsigned int coord);

template void Algo::LinearSolving::addRows_Laplacian_Cotan<PFP1>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	const EdgeAttribute<PFP1::REAL, PFP1::MAP>& edgeWeight,
	const VertexAttribute<PFP1::REAL, PFP1::MAP>& vertexArea);

template void Algo::LinearSolving::addRowsRHS_Laplacian_Cotan<PFP1, SCAL_1>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	const EdgeAttribute<PFP1::REAL, PFP1::MAP>& edgeWeight,
	const VertexAttribute<PFP1::REAL, PFP1::MAP>& vertexArea,
	const VertexAttribute<SCAL_1, PFP1::MAP>& attr);

template void Algo::LinearSolving::addRowsRHS_Laplacian_Cotan<PFP1, VEC_1>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	const EdgeAttribute<PFP1::REAL, PFP1::MAP>& edgeWeight,
	const VertexAttribute<PFP1::REAL, PFP1::MAP>& vertexArea,
	const VertexAttribute<VEC_1, PFP1::MAP>& attr,
	unsigned int coord);

template void Algo::LinearSolving::getResult<PFP1, SCAL_1>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	VertexAttribute<SCAL_1, PFP1::MAP>& attr);

template void Algo::LinearSolving::getResult<PFP1, VEC_1>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	VertexAttribute<VEC_1, PFP1::MAP>& attr,
	unsigned int coord);



//struct PFP1 : public PFP_DOUBLE
//{
//	typedef EmbeddedMap2 MAP;
//};
typedef double SCAL_2;
typedef Geom::Vec3d VEC_2;

template void Algo::LinearSolving::setupVariables<PFP1, SCAL_2>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	const CellMarker<PFP1::MAP, VERTEX>& freeMarker,
	const VertexAttribute<SCAL_2, PFP1::MAP>& attr);

template void Algo::LinearSolving::setupVariables<PFP1, VEC_2>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	const CellMarker<PFP1::MAP, VERTEX>& freeMarker,
	const VertexAttribute<VEC_2, PFP1::MAP>& attr,
	unsigned int coord);

template void Algo::LinearSolving::addRowsRHS_Equality<PFP1, SCAL_2>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	const VertexAttribute<SCAL_2, PFP1::MAP>& attr,
	const VertexAttribute<PFP1::REAL, PFP1::MAP>& weight);

template void Algo::LinearSolving::addRowsRHS_Equality<PFP1, SCAL_2>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	const VertexAttribute<SCAL_2, PFP1::MAP>& attr,
	float weight);

template void Algo::LinearSolving::addRowsRHS_Equality<PFP1, VEC_2>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	const VertexAttribute<VEC_2, PFP1::MAP>& attr,
	const VertexAttribute<PFP1::REAL, PFP1::MAP>& weight,
	unsigned int coord);

template void Algo::LinearSolving::addRowsRHS_Equality<PFP1, VEC_2>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	const VertexAttribute<VEC_2, PFP1::MAP>& attr,
	float weight,
	unsigned int coord);


template void Algo::LinearSolving::addRowsRHS_Laplacian_Topo<PFP1, SCAL_2>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	const VertexAttribute<SCAL_2, PFP1::MAP>& attr);

template void Algo::LinearSolving::addRowsRHS_Laplacian_Topo<PFP1, VEC_2>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	const VertexAttribute<VEC_2, PFP1::MAP>& attr,
	unsigned int coord);


template void Algo::LinearSolving::addRowsRHS_Laplacian_Cotan<PFP1, SCAL_2>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	const EdgeAttribute<PFP1::REAL, PFP1::MAP>& edgeWeight,
	const VertexAttribute<PFP1::REAL, PFP1::MAP>& vertexArea,
	const VertexAttribute<SCAL_2, PFP1::MAP>& attr);

template void Algo::LinearSolving::addRowsRHS_Laplacian_Cotan<PFP1, VEC_2>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	const EdgeAttribute<PFP1::REAL, PFP1::MAP>& edgeWeight,
	const VertexAttribute<PFP1::REAL, PFP1::MAP>& vertexArea,
	const VertexAttribute<VEC_2, PFP1::MAP>& attr,
	unsigned int coord);

template void Algo::LinearSolving::getResult<PFP1, SCAL_2>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	VertexAttribute<SCAL_2, PFP1::MAP>& attr);

template void Algo::LinearSolving::getResult<PFP1, VEC_2>(PFP1::MAP& m,
	const VertexAttribute<unsigned int, PFP1::MAP>& index,
	VertexAttribute<VEC_2, PFP1::MAP>& attr,
	unsigned int coord);



int test_basic()
{

	return 0;
}
