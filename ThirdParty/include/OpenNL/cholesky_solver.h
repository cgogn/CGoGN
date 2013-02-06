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

/*
 * This interface to CHOLMOD is largely inspired from the one found in
 * Graphite developped by Bruno Levy, ALICE Project, LORIA, INRIA Lorraine
 */


#ifndef __CHOLESKY_SOLVER__
#define __CHOLESKY_SOLVER__

#include <cassert>
#include "cholmod.h"

template <typename CoeffType>
class Solver_CHOLESKY
{
public:
    Solver_CHOLESKY() {
    	N = 0 ;
    	factorized_ = false ;
    	L = NULL ;
		// Initialize CHOLMOD library
		cholmod_start(&c) ;
    }

    ~Solver_CHOLESKY() {
		cholmod_free_factor(&L, &c) ;
		cholmod_finish(&c) ;
    }

    bool factorized() { return factorized_ ; }

	void factorize(const Eigen::SparseMatrix<CoeffType>& A_in) {
		assert(A_in.rows() == A_in.cols()) ;
//		assert(A_in.has_symmetric_storage()) ;

		N = A_in.rows() ;
//		int NNZ = A_in.nonZeros() ;

		// Translate sparse matrix into cholmod format
//		cholmod_sparse* A = cholmod_allocate_sparse(N, N, NNZ, false, true, -1, CHOLMOD_REAL, &c);

//		int* colptr = static_cast<int*>(A->p) ;
//		int* rowind = static_cast<int*>(A->i) ;
//		double* a = static_cast<double*>(A->x) ;

		// Convert local Matrix into CHOLMOD Matrix
//		int count = 0 ;
//		for(int j = 0; j < N; j++) {
//			const typename SparseMatrix<CoeffType>::Column& Cj = A_in.column(j) ;
//			colptr[j] = count ;
//			for(unsigned int ii = 0; ii < Cj.nb_coeffs(); ii++) {
//				a[count] = Cj.coeff(ii).a ;
//				rowind[count] = Cj.coeff(ii).index ;
//				count++ ;
//			}
//		}
//		colptr[N] = NNZ ;

		cholmod_sparse A = Eigen::viewAsCholmod(A_in);

		// Factorize
		L = cholmod_analyze(&A, &c) ;
		cholmod_factorize(&A, L, &c) ;
		factorized_ = true ;

		// Cleanup
//		cholmod_free_sparse(&A, &c) ;
    }

	void solve(const VECTOR& b_in, VECTOR& x_out) {
    	assert(factorized_) ;
        assert(L->n == b_in.dimension()) ;
        assert(L->n == x_out.dimension()) ;

		// Translate right-hand side into cholmod format
		cholmod_dense* b = cholmod_allocate_dense(N, 1, N, CHOLMOD_REAL, &c) ;
		double* cb = static_cast<double*>(b->x) ;
		for(int i = 0; i < N; ++i)
			cb[i] = b_in[i] ;

		// Solve
		cholmod_dense* x = cholmod_solve(CHOLMOD_A, L, b, &c) ;

		// Get result
		double* cx = static_cast<double*>(x->x) ;
		for(int i = 0; i < N; ++i)
			x_out[i] = cx[i] ;

		// Cleanup
		cholmod_free_dense(&x, &c) ;
		cholmod_free_dense(&b, &c) ;
    }

    void reset() {
    	if(factorized_)
    	{
    		cholmod_free_factor(&L, &c) ;
			factorized_ = false ;
			L = NULL ;
    	}
    }

private:
    int N ;
    bool factorized_ ;
	cholmod_common c ;
	cholmod_factor* L ;
} ;

#endif
