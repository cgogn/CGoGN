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

#ifndef __LINEAR_SOLVING_CONSTRAINTS__
#define __LINEAR_SOLVING_CONSTRAINTS__

#include "Algo/LinearSolving/Laplacian/matrixSetup.h"

namespace CGoGN
{

namespace LinearSolving
{

template<typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
class EqualityConstraint_Scalar : public FunctorMap<typename PFP::MAP>
{
protected:
	LinearSolver<SOLVER_TRAITS>* solver ;
	const AttributeHandler<ATTR_TYPE>& attrTable ;
	const AttributeHandler<unsigned int>& indexTable ;
	float weight ;

public:
	typedef typename PFP::MAP MAP ;
	

	EqualityConstraint_Scalar(MAP& m, LinearSolver<SOLVER_TRAITS>* s, const AttributeHandler<ATTR_TYPE>& attr, const AttributeHandler<unsigned int> index, float w) :
		FunctorMap<MAP>(m), solver(s), attrTable(attr), indexTable(index), weight(w)
	{}
	bool operator()(Dart d)
	{
		this->solver->begin_row() ;
		solver->add_coefficient(indexTable[d], 1.0f) ;
		solver->set_right_hand_side(attrTable[d]) ;
		solver->normalize_row(weight) ;
		solver->end_row() ;
		return false ;
	}
} ;

template<typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
class EqualityConstraint_Vector : public FunctorMap<typename PFP::MAP>
{
protected:
	LinearSolver<SOLVER_TRAITS>* solver ;
	const AttributeHandler<ATTR_TYPE>& attrTable ;
	const AttributeHandler<unsigned int>& indexTable ;
	unsigned int coord ;
	float weight ;

public:
	typedef typename PFP::MAP MAP ;
	

	EqualityConstraint_Vector(MAP& m, LinearSolver<SOLVER_TRAITS>* s, const AttributeHandler<ATTR_TYPE>& attr, int c, const AttributeHandler<unsigned int> index, float w) :
		FunctorMap<MAP>(m), solver(s), attrTable(attr), indexTable(index), coord(c), weight(w)
	{}
	bool operator()(Dart d)
	{
		this->solver->begin_row() ;
		solver->add_coefficient(indexTable[d], 1.0f) ;
		solver->set_right_hand_side((attrTable[d])[coord]) ;
		solver->normalize_row(weight) ;
		solver->end_row() ;
		return false ;
	}
} ;

template <typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
void setupEqualityMatrix(typename PFP::MAP& m, LinearSolver<SOLVER_TRAITS>* s, const AttributeHandler<ATTR_TYPE>& attr, const AttributeHandler<unsigned int> index, float amount)
{
	EqualityConstraint_Scalar<PFP, ATTR_TYPE, SOLVER_TRAITS> ec(m, s, attr, index, amount) ;
	m.foreach_orbit(VERTEX, ec) ;
}

template <typename PFP, typename ATTR_TYPE, class SOLVER_TRAITS>
void setupEqualityMatrix(typename PFP::MAP& m, LinearSolver<SOLVER_TRAITS>* s, const AttributeHandler<ATTR_TYPE>& attr, unsigned int coord, const AttributeHandler<unsigned int> index, float amount)
{
	EqualityConstraint_Vector<PFP, ATTR_TYPE, SOLVER_TRAITS> ec(m, s, attr, coord, index, amount) ;
	m.foreach_orbit(VERTEX, ec) ;
}

}

}

#endif
