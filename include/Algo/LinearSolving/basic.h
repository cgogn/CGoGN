/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2011, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.u-strasbg.fr/                                         *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#ifndef __LINEAR_SOLVING_BASIC__
#define __LINEAR_SOLVING_BASIC__

#include "OpenNL/linear_solver.h"
#include "Algo/LinearSolving/variablesSetup.h"
#include "Algo/LinearSolving/matrixSetup.h"

namespace CGoGN
{

namespace LinearSolving
{

/*******************************************************************************
 * SOLVER INIT
 *******************************************************************************/

template <class SOLVER_TRAITS>
void initSolver(LinearSolver<SOLVER_TRAITS>* s, unsigned int nb_variables, bool least_squares, bool direct)
{
	// create the solver if it does not exist already
	if(s == NULL)
		s = new LinearSolver<SOLVER_TRAITS>(nb_variables) ;

	// if the number of variables has changed, re-create the solver
	else if(s->nb_variables() != nb_variables)
	{
		delete s ;
		s = new LinearSolver<SOLVER_TRAITS>(nb_variables) ;
	}

	s->set_least_squares(least_squares) ;
	s->set_direct(direct) ;
}

/*******************************************************************************
 * VARIABLES SETUP
 *******************************************************************************/

template <typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
void setupVariables(
	typename PFP::MAP& m,
	LinearSolver<SOLVER_TRAITS>* s,
	const AttributeHandler<unsigned int> index,
	CellMarker& lm,
	const AttributeHandler<ATTR_TYPE>& attr)
{
	FunctorMeshToSolver_Scalar<PFP, ATTR_TYPE, SOLVER_TRAITS> fmts(s, index, lm, attr) ;
	m.foreach_orbit(VERTEX, fmts) ;
}

template <typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
void setupVariables(
	typename PFP::MAP& m,
	LinearSolver<SOLVER_TRAITS>* s,
	const AttributeHandler<unsigned int> index,
	CellMarker& lm,
	const AttributeHandler<ATTR_TYPE>& attr,
	unsigned int coord)
{
	FunctorMeshToSolver_Vector<PFP, ATTR_TYPE, SOLVER_TRAITS> fmts(s, index, lm, attr, coord) ;
	m.foreach_orbit(VERTEX, fmts) ;
}

/*******************************************************************************
 * START MATRIX DEFINITION
 *******************************************************************************/

template <class SOLVER_TRAITS>
void startMatrix(LinearSolver<SOLVER_TRAITS>* s)
{
	s->begin_system() ;
}

/*******************************************************************************
 * MATRIX SETUP : EQUALITY
 *******************************************************************************/

template <typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
void addRowsRHS_Equality(
	typename PFP::MAP& m,
	LinearSolver<SOLVER_TRAITS>* s,
	const AttributeHandler<unsigned int> index,
	const AttributeHandler<ATTR_TYPE>& attr,
	float amount)
{
	FunctorEquality_Scalar<PFP, ATTR_TYPE, SOLVER_TRAITS> ec(s, index, attr, amount) ;
	m.foreach_orbit(VERTEX, ec) ;
}

template <typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
void addRowsRHS_Equality(
	typename PFP::MAP& m,
	LinearSolver<SOLVER_TRAITS>* s,
	const AttributeHandler<unsigned int> index,
	const AttributeHandler<ATTR_TYPE>& attr,
	float amount,
	unsigned int coord)
{
	FunctorEquality_Vector<PFP, ATTR_TYPE, SOLVER_TRAITS> ec(s, index, attr, amount, coord) ;
	m.foreach_orbit(VERTEX, ec) ;
}

/*******************************************************************************
 * MATRIX SETUP : LAPLACIAN TOPO
 *******************************************************************************/

template <typename PFP, class SOLVER_TRAITS>
void addRows_Laplacian_Topo(
	typename PFP::MAP& m,
	LinearSolver<SOLVER_TRAITS>* s,
	const AttributeHandler<unsigned int> index)
{
	FunctorLaplacianTopo<PFP, SOLVER_TRAITS> flt(m, s, index) ;
	m.foreach_orbit(VERTEX, flt) ;
}

template <typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
void addRowsRHS_Laplacian_Topo(
	typename PFP::MAP& m,
	LinearSolver<SOLVER_TRAITS>* s,
	const AttributeHandler<unsigned int> index,
	const AttributeHandler<ATTR_TYPE>& attr)
{
	FunctorLaplacianTopoRHS_Scalar<PFP, ATTR_TYPE, SOLVER_TRAITS> flt(m, s, index, attr) ;
	m.foreach_orbit(VERTEX, flt) ;
}

template <typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
void addRowsRHS_Laplacian_Topo(
	typename PFP::MAP& m,
	LinearSolver<SOLVER_TRAITS>* s,
	const AttributeHandler<unsigned int> index,
	const AttributeHandler<ATTR_TYPE>& attr,
	unsigned int coord)
{
	FunctorLaplacianTopoRHS_Vector<PFP, ATTR_TYPE, SOLVER_TRAITS> flt(m, s, index, attr, coord) ;
	m.foreach_orbit(VERTEX, flt) ;
}

/*******************************************************************************
 * MATRIX SETUP : LAPLACIAN COTAN
 *******************************************************************************/

template <typename PFP, class SOLVER_TRAITS>
void addRows_Laplacian_Cotan(
	typename PFP::MAP& m,
	LinearSolver<SOLVER_TRAITS>* s,
	const AttributeHandler<unsigned int> index,
	const typename PFP::TREAL& edgeWeight,
	const typename PFP::TREAL& vertexArea)
{
	FunctorLaplacianCotan<PFP, SOLVER_TRAITS> flc(m, s, index, edgeWeight, vertexArea) ;
	m.foreach_orbit(VERTEX, flc) ;
}

template <typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
void addRowsRHS_Laplacian_Cotan(
	typename PFP::MAP& m,
	LinearSolver<SOLVER_TRAITS>* s,
	const AttributeHandler<unsigned int> index,
	const typename PFP::TREAL& edgeWeight,
	const typename PFP::TREAL& vertexArea,
	const AttributeHandler<ATTR_TYPE>& attr)
{
	FunctorLaplacianCotanRHS_Scalar<PFP, ATTR_TYPE, SOLVER_TRAITS> flc(m, s, index, edgeWeight, vertexArea, attr) ;
	m.foreach_orbit(VERTEX, flc) ;
}

template <typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
void addRowsRHS_Laplacian_Cotan(
	typename PFP::MAP& m,
	LinearSolver<SOLVER_TRAITS>* s,
	const AttributeHandler<unsigned int> index,
	const typename PFP::TREAL& edgeWeight,
	const typename PFP::TREAL& vertexArea,
	const AttributeHandler<ATTR_TYPE>& attr,
	unsigned int coord)
{
	FunctorLaplacianCotanRHS_Vector<PFP, ATTR_TYPE, SOLVER_TRAITS> flc(m, s, index, edgeWeight, vertexArea, attr, coord) ;
	m.foreach_orbit(VERTEX, flc) ;
}

/*******************************************************************************
 * END MATRIX DEFINITION
 *******************************************************************************/

template <class SOLVER_TRAITS>
void endMatrix(LinearSolver<SOLVER_TRAITS>* s)
{
	s->end_system() ;
}

/*******************************************************************************
 * SOLVE SYSTEM
 *******************************************************************************/

template <class SOLVER_TRAITS>
void solve(LinearSolver<SOLVER_TRAITS>* s)
{
	s->solve() ;
}

/*******************************************************************************
 * RESET SOLVER
 *******************************************************************************/

template <class SOLVER_TRAITS>
void resetSolver(LinearSolver<SOLVER_TRAITS>* s, bool keepMatrix)
{
	s->reset(keepMatrix) ;
}

/*******************************************************************************
 * GET RESULTS
 *******************************************************************************/

template <typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
void getResult(
	typename PFP::MAP& m,
	LinearSolver<SOLVER_TRAITS>* s,
	const AttributeHandler<unsigned int> index,
	AttributeHandler<ATTR_TYPE>& attr)
{
	FunctorSolverToMesh_Scalar<PFP, ATTR_TYPE, SOLVER_TRAITS> fstm(s, index, attr) ;
	m.foreach_orbit(VERTEX, fstm) ;
}

template <typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
void getResult(
	typename PFP::MAP& m,
	LinearSolver<SOLVER_TRAITS>* s,
	const AttributeHandler<unsigned int> index,
	AttributeHandler<ATTR_TYPE>& attr,
	unsigned int coord)
{
	FunctorSolverToMesh_Vector<PFP, ATTR_TYPE, SOLVER_TRAITS> fstm(s, index, attr, coord) ;
	m.foreach_orbit(VERTEX, fstm) ;
}

} // namespace LinearSolving

} // namespace CGoGN

#endif
