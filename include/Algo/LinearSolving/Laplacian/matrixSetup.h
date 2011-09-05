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

#ifndef __LINEAR_SOLVING_LAPLACIAN_SETUP__
#define __LINEAR_SOLVING_LAPLACIAN_SETUP__

namespace CGoGN
{

namespace LinearSolving
{

template<typename PFP, class SOLVER_TRAITS>
class FunctorLaplacianTopo : public FunctorMap<typename PFP::MAP>
{
protected:
	LinearSolver<SOLVER_TRAITS>* solver ;
	const AttributeHandler<unsigned int>& indexTable ;

public:
	typedef typename PFP::MAP MAP ;

	FunctorLaplacianTopo(MAP& m, LinearSolver<SOLVER_TRAITS>* s, const AttributeHandler<unsigned int> index) :
		FunctorMap<MAP>(m), solver(s), indexTable(index)
	{}

	bool operator()(Dart d)
	{
		solver->begin_row() ;
		Dart dd = d ;
		float aii = 0.0f ;
		do
		{
			float aij = 1.0f ;
			aii += aij ;
			solver->add_coefficient(indexTable[this->m_map.phi2(dd)], aij) ;
			dd = this->m_map.phi1(this->m_map.phi2(dd)) ;
		} while(dd != d) ;
		solver->add_coefficient(indexTable[d], -aii) ;
		solver->set_right_hand_side(0.0f) ;
		solver->normalize_row() ;
		solver->end_row() ;
		return false ;
	}
} ;

template<typename PFP, class SOLVER_TRAITS>
class FunctorLaplacianCotan : public FunctorMap<typename PFP::MAP>
{
protected:
	LinearSolver<SOLVER_TRAITS>* solver ;
	const AttributeHandler<unsigned int>& indexTable ;
	const typename PFP::TREAL& edgeWeight ;
	const typename PFP::TREAL& vertexArea ;

public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;

	FunctorLaplacianCotan(MAP& m, LinearSolver<SOLVER_TRAITS>* s, const AttributeHandler<unsigned int> index, const typename PFP::TREAL& eWeight, const typename PFP::TREAL& vArea) :
		FunctorMap<MAP>(m), solver(s), indexTable(index), edgeWeight(eWeight), vertexArea(vArea)
	{}

	bool operator()(Dart d)
	{
		solver->begin_row() ;
		Dart it = d ;
		REAL vArea = vertexArea[d] ;
		REAL aii = 0 ;
		do
		{
			typename PFP::REAL aij = edgeWeight[it] / vArea ;
			aii += aij ;
			solver->add_coefficient(indexTable[this->m_map.phi2(it)], aij) ;
			it = this->m_map.alpha1(it) ;
		} while(it != d) ;
		solver->add_coefficient(indexTable[d], -aii) ;
		solver->set_right_hand_side(0.0f) ;
		solver->normalize_row() ;
		solver->end_row() ;
		return false ;
	}
} ;

}

}

#endif
