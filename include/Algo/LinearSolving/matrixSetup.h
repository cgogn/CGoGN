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

#ifndef __LINEAR_SOLVING_MATRIX_SETUP__
#define __LINEAR_SOLVING_MATRIX_SETUP__

namespace CGoGN
{

namespace LinearSolving
{

/*******************************************************************************
 * EQUALITY MATRIX : right-hand-side SCALAR
 *******************************************************************************/

template<typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
class FunctorEquality_PerVertexWeight_Scalar : public FunctorType
{
protected:
	LinearSolver<SOLVER_TRAITS>* solver ;
	const VertexAttribute<unsigned int>& indexTable ;
	const VertexAttribute<ATTR_TYPE>& attrTable ;
	const VertexAttribute<typename PFP::REAL>& weightTable ;

public:
	FunctorEquality_PerVertexWeight_Scalar(
		LinearSolver<SOLVER_TRAITS>* s,
		const VertexAttribute<unsigned int>& index,
		const VertexAttribute<ATTR_TYPE>& attr,
		const VertexAttribute<typename PFP::REAL>& weight
	) :	solver(s), indexTable(index), attrTable(attr), weightTable(weight)
	{}

	bool operator()(Dart d)
	{
		solver->begin_row() ;
		solver->add_coefficient(indexTable[d], 1) ;
		solver->set_right_hand_side(attrTable[d]) ;
		solver->normalize_row(weightTable[d]) ;
		solver->end_row() ;
		return false ;
	}
} ;

template<typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
class FunctorEquality_UniformWeight_Scalar : public FunctorType
{
protected:
	LinearSolver<SOLVER_TRAITS>* solver ;
	const VertexAttribute<unsigned int>& indexTable ;
	const VertexAttribute<ATTR_TYPE>& attrTable ;
	float weight ;

public:
	FunctorEquality_UniformWeight_Scalar(
		LinearSolver<SOLVER_TRAITS>* s,
		const VertexAttribute<unsigned int>& index,
		const VertexAttribute<ATTR_TYPE>& attr,
		float w
	) :	solver(s), indexTable(index), attrTable(attr), weight(w)
	{}

	bool operator()(Dart d)
	{
		solver->begin_row() ;
		solver->add_coefficient(indexTable[d], 1) ;
		solver->set_right_hand_side(attrTable[d]) ;
		solver->normalize_row(weight) ;
		solver->end_row() ;
		return false ;
	}
} ;

/*******************************************************************************
 * EQUALITY MATRIX : right-hand-side VECTOR + coordinate
 *******************************************************************************/

template<typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
class FunctorEquality_PerVertexWeight_Vector : public FunctorType
{
protected:
	LinearSolver<SOLVER_TRAITS>* solver ;
	const VertexAttribute<unsigned int>& indexTable ;
	const VertexAttribute<ATTR_TYPE>& attrTable ;
	const VertexAttribute<typename PFP::REAL>& weightTable ;
	unsigned int coord ;

public:
	FunctorEquality_PerVertexWeight_Vector(
		LinearSolver<SOLVER_TRAITS>* s,
		const VertexAttribute<unsigned int>& index,
		const VertexAttribute<ATTR_TYPE>& attr,
		const VertexAttribute<typename PFP::REAL>& weight,
		unsigned int c
	) :	solver(s), indexTable(index), attrTable(attr), weightTable(weight), coord(c)
	{}

	bool operator()(Dart d)
	{
		solver->begin_row() ;
		solver->add_coefficient(indexTable[d], 1) ;
		solver->set_right_hand_side((attrTable[d])[coord]) ;
		solver->normalize_row(weightTable[d]) ;
		solver->end_row() ;
		return false ;
	}
} ;

template<typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
class FunctorEquality_UniformWeight_Vector : public FunctorType
{
protected:
	LinearSolver<SOLVER_TRAITS>* solver ;
	const VertexAttribute<unsigned int>& indexTable ;
	const VertexAttribute<ATTR_TYPE>& attrTable ;
	float weight ;
	unsigned int coord ;

public:
	FunctorEquality_UniformWeight_Vector(
		LinearSolver<SOLVER_TRAITS>* s,
		const VertexAttribute<unsigned int>& index,
		const VertexAttribute<ATTR_TYPE>& attr,
		float w,
		unsigned int c
	) :	solver(s), indexTable(index), attrTable(attr), weight(w), coord(c)
	{}

	bool operator()(Dart d)
	{
		solver->begin_row() ;
		solver->add_coefficient(indexTable[d], 1) ;
		solver->set_right_hand_side((attrTable[d])[coord]) ;
		solver->normalize_row(weight) ;
		solver->end_row() ;
		return false ;
	}
} ;

/*******************************************************************************
 * LAPLACIAN TOPO MATRIX : right-hand-side 0
 *******************************************************************************/

template<typename PFP, class SOLVER_TRAITS>
class FunctorLaplacianTopo : public FunctorMap<typename PFP::MAP>
{
protected:
	LinearSolver<SOLVER_TRAITS>* solver ;
	const VertexAttribute<unsigned int>& indexTable ;

public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;

	FunctorLaplacianTopo(
		MAP& m,
		LinearSolver<SOLVER_TRAITS>* s,
		const VertexAttribute<unsigned int>& index
	) :	FunctorMap<MAP>(m), solver(s), indexTable(index)
	{}

	bool operator()(Dart d)
	{
		solver->begin_row() ;
		REAL aii = 0 ;
		Traversor2VE<typename PFP::MAP> t(this->m_map, d) ;
		for(Dart it = t.begin(); it != t.end(); it = t.next())
		{
			REAL aij = 1 ;
			aii += aij ;
			solver->add_coefficient(indexTable[this->m_map.phi1(it)], aij) ;
		}
		solver->add_coefficient(indexTable[d], -aii) ;
		solver->normalize_row() ;
		solver->set_right_hand_side(0) ;
		solver->end_row() ;
		return false ;
	}
} ;

/*******************************************************************************
 * LAPLACIAN TOPO MATRIX : right-hand-side SCALAR
 *******************************************************************************/

template<typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
class FunctorLaplacianTopoRHS_Scalar : public FunctorMap<typename PFP::MAP>
{
protected:
	LinearSolver<SOLVER_TRAITS>* solver ;
	const VertexAttribute<unsigned int>& indexTable ;
	const VertexAttribute<ATTR_TYPE>& attrTable ;

public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;

	FunctorLaplacianTopoRHS_Scalar(
		MAP& m,
		LinearSolver<SOLVER_TRAITS>* s,
		const VertexAttribute<unsigned int>& index,
		const VertexAttribute<ATTR_TYPE>& attr
	) :	FunctorMap<MAP>(m), solver(s), indexTable(index), attrTable(attr)
	{}

	bool operator()(Dart d)
	{
		solver->begin_row() ;
		REAL aii = 0 ;
		Traversor2VE<typename PFP::MAP> t(this->m_map, d) ;
		for(Dart it = t.begin(); it != t.end(); it = t.next())
		{
			REAL aij = 1 ;
			aii += aij ;
			solver->add_coefficient(indexTable[this->m_map.phi1(it)], aij) ;
		}
		solver->add_coefficient(indexTable[d], -aii) ;
		solver->normalize_row() ;
		solver->set_right_hand_side(attrTable[d]) ;
		solver->end_row() ;
		return false ;
	}
} ;

/*******************************************************************************
 * LAPLACIAN TOPO MATRIX : right-hand-side VECTOR + coordinate
 *******************************************************************************/

template<typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
class FunctorLaplacianTopoRHS_Vector : public FunctorMap<typename PFP::MAP>
{
protected:
	LinearSolver<SOLVER_TRAITS>* solver ;
	const VertexAttribute<unsigned int>& indexTable ;
	const VertexAttribute<ATTR_TYPE>& attrTable ;
	unsigned int coord ;

public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;

	FunctorLaplacianTopoRHS_Vector(
		MAP& m,
		LinearSolver<SOLVER_TRAITS>* s,
		const VertexAttribute<unsigned int>& index,
		const VertexAttribute<ATTR_TYPE>& attr,
		unsigned int c
	) :	FunctorMap<MAP>(m), solver(s), indexTable(index), attrTable(attr), coord(c)
	{}

	bool operator()(Dart d)
	{
		solver->begin_row() ;
		REAL aii = 0 ;
		Traversor2VE<typename PFP::MAP> t(this->m_map, d) ;
		for(Dart it = t.begin(); it != t.end(); it = t.next())
		{
			REAL aij = 1 ;
			aii += aij ;
			solver->add_coefficient(indexTable[this->m_map.phi1(it)], aij) ;
		}
		solver->add_coefficient(indexTable[d], -aii) ;
		solver->normalize_row() ;
		solver->set_right_hand_side((attrTable[d])[coord]) ;
		solver->end_row() ;
		return false ;
	}
} ;

/*******************************************************************************
 * LAPLACIAN COTAN MATRIX : right-hand-side 0
 *******************************************************************************/

template<typename PFP, class SOLVER_TRAITS>
class FunctorLaplacianCotan : public FunctorMap<typename PFP::MAP>
{
protected:
	LinearSolver<SOLVER_TRAITS>* solver ;
	const VertexAttribute<unsigned int>& indexTable ;
	const EdgeAttribute<typename PFP::REAL>& edgeWeight ;
	const VertexAttribute<typename PFP::REAL>& vertexArea ;

public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;

	FunctorLaplacianCotan(
		MAP& m,
		LinearSolver<SOLVER_TRAITS>* s,
		const VertexAttribute<unsigned int>& index,
		const EdgeAttribute<typename PFP::REAL>& eWeight,
		const VertexAttribute<typename PFP::REAL>& vArea
	) :	FunctorMap<MAP>(m), solver(s), indexTable(index), edgeWeight(eWeight), vertexArea(vArea)
	{}

	bool operator()(Dart d)
	{
		solver->begin_row() ;
		REAL vArea = vertexArea[d] ;
		REAL aii = 0 ;
		Dart it = d ;
//		Traversor2VE<typename PFP::MAP> t(this->m_map, d) ;
//		for(Dart it = t.begin(); it != t.end(); it = t.next())
		do
		{
			REAL aij = edgeWeight[it] / vArea ;
			aii += aij ;
			solver->add_coefficient(indexTable[this->m_map.phi1(it)], aij) ;
			it = this->m_map.phi2_1(it) ;
		} while(it != d) ;
		solver->add_coefficient(indexTable[d], -aii) ;
		solver->normalize_row() ;
		solver->set_right_hand_side(0) ;
		solver->end_row() ;
		return false ;
	}
} ;

/*******************************************************************************
 * LAPLACIAN COTAN MATRIX : right-hand-side SCALAR
 *******************************************************************************/

template<typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
class FunctorLaplacianCotanRHS_Scalar : public FunctorMap<typename PFP::MAP>
{
protected:
	LinearSolver<SOLVER_TRAITS>* solver ;
	const VertexAttribute<unsigned int>& indexTable ;
	const EdgeAttribute<typename PFP::REAL>& edgeWeight ;
	const VertexAttribute<typename PFP::REAL>& vertexArea ;
	const VertexAttribute<ATTR_TYPE>& attrTable ;

public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;

	FunctorLaplacianCotanRHS_Scalar(
		MAP& m,
		LinearSolver<SOLVER_TRAITS>* s,
		const VertexAttribute<unsigned int>& index,
		const EdgeAttribute<typename PFP::REAL>& eWeight,
		const VertexAttribute<typename PFP::REAL>& vArea,
		const VertexAttribute<ATTR_TYPE>& attr
	) :	FunctorMap<MAP>(m), solver(s), indexTable(index), edgeWeight(eWeight), vertexArea(vArea), attrTable(attr)
	{}

	bool operator()(Dart d)
	{
		solver->begin_row() ;
		REAL vArea = vertexArea[d] ;
		REAL aii = 0 ;
		Traversor2VE<typename PFP::MAP> t(this->m_map, d) ;
		for(Dart it = t.begin(); it != t.end(); it = t.next())
		{
			REAL aij = edgeWeight[it] / vArea ;
			aii += aij ;
			solver->add_coefficient(indexTable[this->m_map.phi1(it)], aij) ;
		}
		solver->add_coefficient(indexTable[d], -aii) ;
		solver->normalize_row() ;
		solver->set_right_hand_side(attrTable[d]) ;
		solver->end_row() ;
		return false ;
	}
} ;

/*******************************************************************************
 * LAPLACIAN COTAN MATRIX : right-hand-side VECTOR + coordinate
 *******************************************************************************/

template<typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
class FunctorLaplacianCotanRHS_Vector : public FunctorMap<typename PFP::MAP>
{
protected:
	LinearSolver<SOLVER_TRAITS>* solver ;
	const VertexAttribute<unsigned int>& indexTable ;
	const EdgeAttribute<typename PFP::REAL>& edgeWeight ;
	const VertexAttribute<typename PFP::REAL>& vertexArea ;
	const VertexAttribute<ATTR_TYPE>& attrTable ;
	unsigned int coord ;

public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;

	FunctorLaplacianCotanRHS_Vector(
		MAP& m,
		LinearSolver<SOLVER_TRAITS>* s,
		const VertexAttribute<unsigned int>& index,
		const EdgeAttribute<typename PFP::REAL>& eWeight,
		const VertexAttribute<typename PFP::REAL>& vArea,
		const VertexAttribute<ATTR_TYPE>& attr,
		unsigned int c
	) :	FunctorMap<MAP>(m), solver(s), indexTable(index), edgeWeight(eWeight), vertexArea(vArea), attrTable(attr), coord(c)
	{}

	bool operator()(Dart d)
	{
		solver->begin_row() ;
		REAL vArea = vertexArea[d] ;
		REAL aii = 0 ;
		Traversor2VE<typename PFP::MAP> t(this->m_map, d) ;
		for(Dart it = t.begin(); it != t.end(); it = t.next())
		{
			REAL aij = edgeWeight[it] / vArea ;
			aii += aij ;
			solver->add_coefficient(indexTable[this->m_map.phi1(it)], aij) ;
		}
		solver->add_coefficient(indexTable[d], -aii) ;
		solver->normalize_row() ;
		solver->set_right_hand_side((attrTable[d])[coord]) ;
		solver->end_row() ;
		return false ;
	}
} ;

} // namespace LinearSolving

} // namespace CGoGN

#endif
