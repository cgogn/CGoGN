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
#include "Algo/LinearSolving/matrixSetup.h"

namespace CGoGN
{

namespace LinearSolving
{

template <typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
class FunctorMeshToSolver_Scalar : public FunctorMap<typename PFP::MAP>
{
protected:
	LinearSolver<SOLVER_TRAITS>* solver ;
	CellMarker& lockingMarker ;
	const AttributeHandler<ATTR_TYPE>& attrTable ;
	const AttributeHandler<unsigned int>& indexTable ;
	bool lockedVertices ;

public:
	typedef typename PFP::MAP MAP ;
	
	FunctorMeshToSolver_Scalar(MAP& m, LinearSolver<SOLVER_TRAITS>* s, CellMarker& lm, const AttributeHandler<ATTR_TYPE>& attr, const AttributeHandler<unsigned int> index) :
		FunctorMap<MAP>(m), solver(s), lockingMarker(lm), attrTable(attr), indexTable(index), lockedVertices(false)
	{}

	bool operator()(Dart d)
	{
		solver->variable(indexTable[d]).set_value(attrTable[d]) ;
		if(lockingMarker.isMarked(d))
		{
			solver->variable(indexTable[d]).lock() ;
			lockedVertices = true ;
		}
		return false ;
	}
	bool hasLockedVertices() { return lockedVertices ; }
} ;

template <typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
class FunctorMeshToSolver_Vector : public FunctorMap<typename PFP::MAP>
{
protected:
	LinearSolver<SOLVER_TRAITS>* solver ;
	CellMarker& lockingMarker ;
	const AttributeHandler<ATTR_TYPE>& attrTable ;
	const AttributeHandler<unsigned int>& indexTable ;
	unsigned int coord ;
	bool lockedVertices ;

public:
	typedef typename PFP::MAP MAP ;
	
	FunctorMeshToSolver_Vector(MAP& m, LinearSolver<SOLVER_TRAITS>* s, CellMarker& lm, const AttributeHandler<ATTR_TYPE>& attr, unsigned int c, const AttributeHandler<unsigned int> index) :
		FunctorMap<MAP>(m), solver(s), lockingMarker(lm), attrTable(attr), indexTable(index), coord(c), lockedVertices(false)
	{}

	bool operator()(Dart d)
	{
		solver->variable(indexTable[d]).set_value((attrTable[d])[coord]) ;
		if(lockingMarker.isMarked(d))
		{
			solver->variable(indexTable[d]).lock() ;
			lockedVertices = true ;
		}
		return false ;
	}
	bool hasLockedVertices() { return lockedVertices ; }
} ;

template <typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
class FunctorSolverToMesh_Scalar : public FunctorMap<typename PFP::MAP>
{
protected:
	LinearSolver<SOLVER_TRAITS>* solver ;
	AttributeHandler<ATTR_TYPE>& attrTable ;
	const AttributeHandler<unsigned int>& indexTable ;

public:
	typedef typename PFP::MAP MAP ;
	
	FunctorSolverToMesh_Scalar(MAP& m, LinearSolver<SOLVER_TRAITS>* s, AttributeHandler<ATTR_TYPE>& attr, const AttributeHandler<unsigned int> index) :
		FunctorMap<MAP>(m), solver(s), attrTable(attr), indexTable(index)
	{}

	bool operator()(Dart d)
	{
		attrTable[d] = solver->variable(indexTable[d]).value() ;
		return false ;
	}
} ;

template <typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
class FunctorSolverToMesh_Vector : public FunctorMap<typename PFP::MAP>
{
protected:
	LinearSolver<SOLVER_TRAITS>* solver ;
	AttributeHandler<ATTR_TYPE>& attrTable ;
	const AttributeHandler<unsigned int>& indexTable ;
	unsigned int coord ;

public:
	typedef typename PFP::MAP MAP ;

	FunctorSolverToMesh_Vector(MAP& m, LinearSolver<SOLVER_TRAITS>* s, AttributeHandler<ATTR_TYPE>& attr, unsigned int c, const AttributeHandler<unsigned int> index) :
		FunctorMap<MAP>(m), solver(s), attrTable(attr), indexTable(index), coord(c)
	{}

	bool operator()(Dart d)
	{
		(attrTable[d])[coord] = solver->variable(indexTable[d]).value() ;
		return false ;
	}
} ;


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

template <typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
void setupVariables(typename PFP::MAP& m, LinearSolver<SOLVER_TRAITS>* s, CellMarker& lm, const AttributeHandler<ATTR_TYPE>& attr, const AttributeHandler<unsigned int> index)
{
	FunctorMeshToSolver_Scalar<PFP, ATTR_TYPE, SOLVER_TRAITS> fmts(m, s, lm, attr, index) ;
	m.foreach_orbit(VERTEX, fmts) ;
}

template <typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
void setupVariables(typename PFP::MAP& m, LinearSolver<SOLVER_TRAITS>* s, CellMarker& lm, const AttributeHandler<ATTR_TYPE>& attr, unsigned int coord, const AttributeHandler<unsigned int> index)
{
	FunctorMeshToSolver_Vector<PFP, ATTR_TYPE, SOLVER_TRAITS> fmts(m, s, lm, attr, coord, index) ;
	m.foreach_orbit(VERTEX, fmts) ;
}

template <class SOLVER_TRAITS>
void startMatrix(LinearSolver<SOLVER_TRAITS>* s)
{
	s->begin_system() ;
}

template <typename PFP, class SOLVER_TRAITS>
void addRows_Laplacian_Topo(typename PFP::MAP& m, LinearSolver<SOLVER_TRAITS>* s, const AttributeHandler<unsigned int> index)
{
	FunctorLaplacianTopo<PFP, SOLVER_TRAITS> lt(m, s, index) ;
	m.foreach_orbit(VERTEX, lt) ;
}

template <typename PFP, class SOLVER_TRAITS>
void addRows_Laplacian_Cotan(typename PFP::MAP& m, LinearSolver<SOLVER_TRAITS>* s, const AttributeHandler<unsigned int> index, const typename PFP::TREAL& edgeWeight, const typename PFP::TREAL& vertexArea)
{
	FunctorLaplacianCotan<PFP, SOLVER_TRAITS> lt(m, s, index, edgeWeight, vertexArea) ;
	m.foreach_orbit(VERTEX, lt) ;
}

template <typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
void addRows_Equality(typename PFP::MAP& m, LinearSolver<SOLVER_TRAITS>* s, const AttributeHandler<ATTR_TYPE>& attr, const AttributeHandler<unsigned int> index, float amount)
{
	setupEqualityMatrix<PFP, ATTR_TYPE, SOLVER_TRAITS>(m, s, attr, index, amount) ;
}

template <typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
void addRows_Equality(typename PFP::MAP& m, LinearSolver<SOLVER_TRAITS>* s, const AttributeHandler<ATTR_TYPE>& attr, unsigned int coord, const AttributeHandler<unsigned int> index, float amount)
{
	setupEqualityMatrix<PFP, ATTR_TYPE, SOLVER_TRAITS>(m, s, attr, coord, index, amount) ;
}

template <class SOLVER_TRAITS>
void endMatrix(LinearSolver<SOLVER_TRAITS>* s)
{
	s->end_system() ;
}

template <class SOLVER_TRAITS>
void solve(LinearSolver<SOLVER_TRAITS>* s)
{
	s->solve() ;
}

template <class SOLVER_TRAITS>
void resetSolver(LinearSolver<SOLVER_TRAITS>* s, bool keepMatrix)
{
	s->reset(keepMatrix) ;
}

template <typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
void getResult(typename PFP::MAP& m, LinearSolver<SOLVER_TRAITS>* s, AttributeHandler<ATTR_TYPE>& attr, const AttributeHandler<unsigned int> index)
{
	FunctorSolverToMesh_Scalar<PFP, ATTR_TYPE, SOLVER_TRAITS> fstm(m, s, attr, index) ;
	m.foreach_orbit(VERTEX, fstm) ;
}

template <typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
void getResult(typename PFP::MAP& m, LinearSolver<SOLVER_TRAITS>* s, AttributeHandler<ATTR_TYPE>& attr, unsigned int coord, const AttributeHandler<unsigned int> index)
{
	FunctorSolverToMesh_Vector<PFP, ATTR_TYPE, SOLVER_TRAITS> fstm(m, s, attr, coord, index) ;
	m.foreach_orbit(VERTEX, fstm) ;
}

} // namespace LinearSolving

} // namespace CGoGN

#endif
