/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2012, IGG Team, LSIIT, University of Strasbourg           *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#ifndef __LINEAR_SOLVING_BASIC__
#define __LINEAR_SOLVING_BASIC__

#include "NL/nl.h"
#include "Algo/LinearSolving/variablesSetup.h"
#include "Algo/LinearSolving/matrixSetup.h"
#include "Algo/Topo/basic.h"

namespace CGoGN
{

namespace LinearSolving
{

/*******************************************************************************
 * VARIABLES SETUP
 *******************************************************************************/

template <typename PFP, typename ATTR_TYPE>
void setupVariables(
	typename PFP::MAP& m,
	const VertexAttribute<unsigned int, typename PFP::MAP::IMPL>& index,
	const CellMarker<typename PFP::MAP, VERTEX>& fm,
	const VertexAttribute<ATTR_TYPE, typename PFP::MAP::IMPL>& attr)
{
//	TraversorV<MAP> t(m);
//	for (Dart d = t.begin(); d != t.end(); d = t.next())
//	{

//	}

	FunctorMeshToSolver_Scalar<PFP, ATTR_TYPE> fmts(index, fm, attr) ;
	Algo::Topo::foreach_orbit<VERTEX>(m, fmts) ;
}

template <typename PFP, typename ATTR_TYPE>
void setupVariables(
	typename PFP::MAP& m,
	const VertexAttribute<unsigned int, typename PFP::MAP::IMPL>& index,
	const CellMarker<typename PFP::MAP, VERTEX>& fm,
	const VertexAttribute<ATTR_TYPE, typename PFP::MAP::IMPL>& attr,
	unsigned int coord)
{
	FunctorMeshToSolver_Vector<PFP, ATTR_TYPE> fmts(index, fm, attr, coord) ;
	Algo::Topo::foreach_orbit<VERTEX>(m, fmts) ;
}

/*******************************************************************************
 * MATRIX SETUP : EQUALITY
 *******************************************************************************/

template <typename PFP, typename ATTR_TYPE>
void addRowsRHS_Equality(
	typename PFP::MAP& m,
	const VertexAttribute<unsigned int, typename PFP::MAP::IMPL>& index,
	const VertexAttribute<ATTR_TYPE, typename PFP::MAP::IMPL>& attr,
	const VertexAttribute<typename PFP::REAL, typename PFP::MAP::IMPL>& weight)
{
	nlEnable(NL_NORMALIZE_ROWS) ;
	FunctorEquality_PerVertexWeight_Scalar<PFP, ATTR_TYPE> feq(index, attr, weight) ;
	Algo::Topo::foreach_orbit<VERTEX>(m, feq) ;
	nlDisable(NL_NORMALIZE_ROWS) ;
}

template <typename PFP, typename ATTR_TYPE>
void addRowsRHS_Equality(
	typename PFP::MAP& m,
	const VertexAttribute<unsigned int, typename PFP::MAP::IMPL>& index,
	const VertexAttribute<ATTR_TYPE, typename PFP::MAP::IMPL>& attr,
	float weight)
{
	nlEnable(NL_NORMALIZE_ROWS) ;
	FunctorEquality_UniformWeight_Scalar<PFP, ATTR_TYPE> feq(index, attr, weight) ;
	Algo::Topo::foreach_orbit<VERTEX>(m, feq) ;
	nlDisable(NL_NORMALIZE_ROWS) ;
}

template <typename PFP, typename ATTR_TYPE>
void addRowsRHS_Equality(
	typename PFP::MAP& m,
	const VertexAttribute<unsigned int, typename PFP::MAP::IMPL>& index,
	const VertexAttribute<ATTR_TYPE, typename PFP::MAP::IMPL>& attr,
	const VertexAttribute<typename PFP::REAL, typename PFP::MAP::IMPL>& weight,
	unsigned int coord)
{
	nlEnable(NL_NORMALIZE_ROWS) ;
	FunctorEquality_PerVertexWeight_Vector<PFP, ATTR_TYPE> feq(index, attr, weight, coord) ;
	Algo::Topo::foreach_orbit<VERTEX>(m, feq) ;
	nlDisable(NL_NORMALIZE_ROWS) ;
}

template <typename PFP, typename ATTR_TYPE>
void addRowsRHS_Equality(
	typename PFP::MAP& m,
	const VertexAttribute<unsigned int, typename PFP::MAP::IMPL>& index,
	const VertexAttribute<ATTR_TYPE, typename PFP::MAP::IMPL>& attr,
	float weight,
	unsigned int coord)
{
	nlEnable(NL_NORMALIZE_ROWS) ;
	FunctorEquality_UniformWeight_Vector<PFP, ATTR_TYPE> feq(index, attr, weight, coord) ;
	Algo::Topo::foreach_orbit<VERTEX>(m, feq) ;
	nlDisable(NL_NORMALIZE_ROWS) ;
}

/*******************************************************************************
 * MATRIX SETUP : LAPLACIAN TOPO
 *******************************************************************************/

template <typename PFP>
void addRows_Laplacian_Topo(
	typename PFP::MAP& m,
	const VertexAttribute<unsigned int, typename PFP::MAP::IMPL> index)
{
	nlEnable(NL_NORMALIZE_ROWS) ;
	FunctorLaplacianTopo<PFP> flt(m, index) ;
	Algo::Topo::foreach_orbit<VERTEX>(m, flt) ;
	nlDisable(NL_NORMALIZE_ROWS) ;
}

template <typename PFP, typename ATTR_TYPE>
void addRowsRHS_Laplacian_Topo(
	typename PFP::MAP& m,
	const VertexAttribute<unsigned int, typename PFP::MAP::IMPL> index,
	const VertexAttribute<ATTR_TYPE, typename PFP::MAP::IMPL>& attr)
{
	nlEnable(NL_NORMALIZE_ROWS) ;
	FunctorLaplacianTopoRHS_Scalar<PFP, ATTR_TYPE> flt(m, index, attr) ;
	Algo::Topo::foreach_orbit<VERTEX>(m, flt) ;
	nlDisable(NL_NORMALIZE_ROWS) ;
}

template <typename PFP, typename ATTR_TYPE>
void addRowsRHS_Laplacian_Topo(
	typename PFP::MAP& m,
	const VertexAttribute<unsigned int, typename PFP::MAP::IMPL> index,
	const VertexAttribute<ATTR_TYPE, typename PFP::MAP::IMPL>& attr,
	unsigned int coord)
{
	nlEnable(NL_NORMALIZE_ROWS) ;
	FunctorLaplacianTopoRHS_Vector<PFP, ATTR_TYPE> flt(m, index, attr, coord) ;
	Algo::Topo::foreach_orbit<VERTEX>(m, flt) ;
	nlDisable(NL_NORMALIZE_ROWS) ;
}

/*******************************************************************************
 * MATRIX SETUP : LAPLACIAN COTAN
 *******************************************************************************/

template <typename PFP>
void addRows_Laplacian_Cotan(
	typename PFP::MAP& m,
	const VertexAttribute<unsigned int, typename PFP::MAP::IMPL> index,
	const EdgeAttribute<typename PFP::REAL, typename PFP::MAP::IMPL>& edgeWeight,
	const VertexAttribute<typename PFP::REAL, typename PFP::MAP::IMPL>& vertexArea)
{
	nlEnable(NL_NORMALIZE_ROWS) ;
	FunctorLaplacianCotan<PFP> flc(m, index, edgeWeight, vertexArea) ;
	Algo::Topo::foreach_orbit<VERTEX>(m, flc) ;
	nlDisable(NL_NORMALIZE_ROWS) ;
}

template <typename PFP, typename ATTR_TYPE>
void addRowsRHS_Laplacian_Cotan(
	typename PFP::MAP& m,
	const VertexAttribute<unsigned int, typename PFP::MAP::IMPL> index,
	const EdgeAttribute<typename PFP::REAL, typename PFP::MAP::IMPL>& edgeWeight,
	const VertexAttribute<typename PFP::REAL, typename PFP::MAP::IMPL>& vertexArea,
	const VertexAttribute<ATTR_TYPE, typename PFP::MAP::IMPL>& attr)
{
	nlEnable(NL_NORMALIZE_ROWS) ;
	FunctorLaplacianCotanRHS_Scalar<PFP, ATTR_TYPE> flc(m, index, edgeWeight, vertexArea, attr) ;
	Algo::Topo::foreach_orbit<VERTEX>(m, flc) ;
	nlDisable(NL_NORMALIZE_ROWS) ;
}

template <typename PFP, typename ATTR_TYPE>
void addRowsRHS_Laplacian_Cotan(
	typename PFP::MAP& m,
	const VertexAttribute<unsigned int, typename PFP::MAP::IMPL> index,
	const EdgeAttribute<typename PFP::REAL, typename PFP::MAP::IMPL>& edgeWeight,
	const VertexAttribute<typename PFP::REAL, typename PFP::MAP::IMPL>& vertexArea,
	const VertexAttribute<ATTR_TYPE, typename PFP::MAP::IMPL>& attr,
	unsigned int coord)
{
	nlEnable(NL_NORMALIZE_ROWS) ;
	FunctorLaplacianCotanRHS_Vector<PFP, ATTR_TYPE> flc(m, index, edgeWeight, vertexArea, attr, coord) ;
	Algo::Topo::foreach_orbit<VERTEX>(m, flc) ;
	nlDisable(NL_NORMALIZE_ROWS) ;
}

template <typename PFP, typename ATTR_TYPE>
void addRowsRHS_Laplacian_Cotan_NL(
	typename PFP::MAP& m,
	const VertexAttribute<unsigned int, typename PFP::MAP::IMPL> index,
	const EdgeAttribute<typename PFP::REAL, typename PFP::MAP::IMPL>& edgeWeight,
	const VertexAttribute<typename PFP::REA, typename PFP::MAP::IMPLL>& vertexArea,
	const VertexAttribute<ATTR_TYPE, typename PFP::MAP::IMPL>& attr,
	unsigned int coord)
{
	nlEnable(NL_NORMALIZE_ROWS) ;
	FunctorLaplacianCotanRHS_Vector<PFP, ATTR_TYPE> flc(m, index, edgeWeight, vertexArea, attr, coord) ;
	Algo::Topo::foreach_orbit<VERTEX>(m, flc) ;
	nlDisable(NL_NORMALIZE_ROWS) ;
}

/*******************************************************************************
 * GET RESULTS
 *******************************************************************************/

template <typename PFP, typename ATTR_TYPE>
void getResult(
	typename PFP::MAP& m,
	const VertexAttribute<unsigned int, typename PFP::MAP::IMPL> index,
	VertexAttribute<ATTR_TYPE, typename PFP::MAP::IMPL>& attr)
{
	FunctorSolverToMesh_Scalar<PFP, ATTR_TYPE> fstm(index, attr) ;
	Algo::Topo::foreach_orbit<VERTEX>(m, fstm) ;
}

template <typename PFP, typename ATTR_TYPE>
void getResult(
	typename PFP::MAP& m,
	const VertexAttribute<unsigned int, typename PFP::MAP::IMPL> index,
	VertexAttribute<ATTR_TYPE, typename PFP::MAP::IMPL>& attr,
	unsigned int coord)
{
	FunctorSolverToMesh_Vector<PFP, ATTR_TYPE> fstm(index, attr, coord) ;
	Algo::Topo::foreach_orbit<VERTEX>(m, fstm) ;
}

} // namespace LinearSolving

} // namespace CGoGN

#endif
