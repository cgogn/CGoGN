/*
 *  CNC: Concurrent Number Cruncher
 *  Copyright (C) 2008 GOCAD/ASGA, INRIA/ALICE
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  If you modify this software, you should include a notice giving the
 *  name of the person performing the modification, the date of modification,
 *  and the reason for such modification.
 *
 *  Contact: Luc Buatois
 *
 *     buatois@gocad.org
 *
 *     ASGA-INPL Bt. G
 *     Rue du Doyen Marcel Roubault - BP 40
 *     54501 VANDOEUVRE LES NANCY
 *     FRANCE
 *
 *  Note that the GNU General Public License does not permit incorporating
 *  the Software into proprietary programs. 
 */

#ifndef CNC_SPARSE_MATRIX_CRS_H
#define CNC_SPARSE_MATRIX_CRS_H

#include "cnc_utils.h"
#include "cnc_cublas_utils.h"
#include "cnc_arrays.h"

#include <ostream>
#include <algorithm>


//---------------------------------------------------------------------------//
// External CUDA Kernel Functions, forward declarations						 //
//---------------------------------------------------------------------------//

extern "C" void cnc_cuda_mat1x1vecmult (
				float * matrix, unsigned int size_matrix,
				uint2 * rowptr, unsigned int size_rowptr,
				unsigned int * colind, unsigned int size_colind,
				float * x, float * b, unsigned int size_vec ) ;


// -------------------------------------------------------------------------- //
// CRS Sparse matrix data structure with CPU and GPU storage				  //
// -------------------------------------------------------------------------- //

    /**
     * SparseMatrixPattern in Compressed Row Storage mode.
     */
    class CNCSparseMatrixPatternCRS {
    public:
        typedef CNCArray1d<long> IndexArray ;
        enum { NO_INDEX = ~0 } ;

        CNCSparseMatrixPatternCRS() {
            N = 0 ;
            symmetric_storage = false ;
        }

        void clear() {
            N = 0 ;
            symmetric_storage = false ;
            colind.clear() ;
            rowptr.clear() ;
        }


        /** number of rows */
        long m() const { return rowptr.size() - 1 ; }

        /** number of columns */
        long n() const { return N ; }

        /** number of non-zero coefficients */
        long nnz() const { return colind.size() ; }

        /** number of non-zero coefficients in row i */
        long row_nnz(long i) const { 
            return rowptr[i+1] - rowptr[i] ;
        }

        long mem_usage() const {
            return colind.mem_usage() +
				   rowptr.mem_usage() +
				   sizeof(long) +
				   sizeof(bool) ;
        }

    public:
        IndexArray colind ;
        IndexArray rowptr ;

        long N ;
        bool symmetric_storage ;

    } ;

//---------------------------------------------------------------------------//

	template<class T> class CNCSparseMatrixCRS : public CNCSparseMatrixPatternCRS {
    public:

		typedef CNCArray1d<T> CoeffArray ;

        inline CNCSparseMatrixCRS() : separate_diag(true) { }
        inline void mult(const T* x, T* y) const ;
        inline void mult(const CNCArray1d<T>& x, CNCArray1d<T>& y) const {
            mult(x.data(), y.data()) ;
        }
        inline void print(std::ostream& out) ;

        long mem_usage() const {
            return CNCSparseMatrixPatternCRS::mem_usage() +
										   a.mem_usage() +
										   diag.mem_usage() +
										   sizeof(bool) ;
        }

		inline void gpu_allocate_and_upload () ;
		inline void gpu_deallocate () ;
		inline void gpu_mult ( const void * x, void * y, unsigned int vec_size ) ;


		inline void print () {
			for(long i=0; i<m(); i++) {
				for(long jj=rowptr[i]; jj<rowptr[i+1]; jj++) {
					if(a[jj] != 0.0) {
						long j = colind[jj] ;
						printf ( "%d %d %f\n", i, j, a[jj] ) ;
						if(symmetric_storage && j != i) {
							printf ( "%d %d %f\n", j, i, a[jj] ) ;
						}
					}
				}
			}
			if(separate_diag) {
				for(long i=0; i<diag.size(); i++) {
					if(diag[i] != 0.0) {
						printf ( "%d %d %f\n", i, i, diag[i] ) ;
					}
				}
			}
		}

        CoeffArray a ;
        CoeffArray diag ;
        bool separate_diag ;

		// GPU data
		void * gpu_redundant_rp_ ;
		void * gpu_ci_ ;
		void * gpu_mat_ ;
    } ;

typedef CNCSparseMatrixCRS<float> SmCRS ;

//---------------------------------------------------------------------------//

	template <class T> inline void 
    CNCSparseMatrixCRS<T>::mult(const T* x, T* y) const {
        if(separate_diag) {
            if(symmetric_storage) {
                // Symmetric storage and separate diagonal
                for(long i=0; i<diag.size(); i++) {
                    y[i] = x[i] * diag[i] ;
                }
                for(long i=diag.size(); i<m(); i++) {
                    y[i] = 0.0 ;
                }
                for(long i=0; i<m(); i++) {
                    for(long j=rowptr[i] ; j<rowptr[i+1]; j++) {
                        y[i] += a[j] * x[colind[j]] ;
                        y[colind[j]] += a[j] * x[i] ;                        
                    }
                }
            } else {
                // No symmetric storage and separate diagonal
                for(long i=0; i<m(); i++) {
                    T sum = 0.0 ;
                    for(long j=rowptr[i] ; j<rowptr[i+1]; j++) {
                        sum += a[j] * x[colind[j]] ;
                    }
                    y[i] = sum ;
                }
                for(long i=0; i<diag.size(); i++) {
                    y[i] += x[i] * diag[i] ;
                }
            }
        } else {
            if(symmetric_storage) {
                // Symmetric storage, no separate diagonal
				memset ( y, 0, sizeof(T) ) ;
                for(long i=0; i<m(); i++) {
                    for(long j=rowptr[i] ; j<rowptr[i+1]; j++) {
                        y[i] += a[j] * x[colind[j]] ;
                        if(colind[j] != i) {
                            y[colind[j]] += a[j] * x[i] ;                        
                        }
                    }
                }
            } else {
                // No symmetric storage, no separate diagonal
                for(long i=0; i<m(); i++) {
                    T sum = 0.0 ;
                    for(long j=rowptr[i] ; j<rowptr[i+1]; j++) {
                        sum += a[j] * x[colind[j]] ;
                    }
                    y[i] = sum ;
                }
            }
        }
    }


//---------------------------------------------------------------------------//
// Allocate and upload a CRS sparse matrix on GPU memory					 //
//---------------------------------------------------------------------------//

template <class T> inline void CNCSparseMatrixCRS<T>::gpu_allocate_and_upload () {

	// allocate and upload non-zero matrix coefficients
	cublasAlloc ( a.size()+16, 4, &(gpu_mat_) ) ;
	cublasSetVector ( a.size(), 4, a.data(), 1, gpu_mat_, 1 ) ;

	// allocate and upload rowptr and colind
	cublasAlloc ( colind.size()+16, 4, &(gpu_ci_) ) ;
	cublasAlloc ( rowptr.size()-1+16, sizeof(uint2), &(gpu_redundant_rp_) ) ;

	unsigned int * ci = CNCallocate_and_copy<long, unsigned int> ( colind.data(),
																colind.size() ) ;

	uint2 * cpu_redundant_rp =  CNCallocate<uint2> ( rowptr.size()-1 ) ;
	{for (unsigned int i=0; i<rowptr.size()-1; i++) {
		cpu_redundant_rp[i].x = rowptr.data()[i  ] ;
		cpu_redundant_rp[i].y = rowptr.data()[i+1] ;
	}}

	cublasSetVector ( rowptr.size()-1, sizeof(uint2),
					  cpu_redundant_rp, 1,
					  gpu_redundant_rp_, 1 ) ;

	cublasSetVector ( colind.size(), sizeof(unsigned int),
					  ci, 1, gpu_ci_, 1 ) ;

	CNCdeallocate<uint2> ( cpu_redundant_rp ) ;
	CNCdeallocate<unsigned int> ( ci ) ;
}


//---------------------------------------------------------------------------//
// Free graphics memory used to store a CRS sparse matrix					 //
//---------------------------------------------------------------------------//

template <class T> inline void CNCSparseMatrixCRS<T>::gpu_deallocate () {

	cublasFree ( gpu_mat_ ) ;
	cublasFree ( gpu_ci_ ) ;
	cublasFree ( gpu_redundant_rp_ ) ;
}


//---------------------------------------------------------------------------//
// GPU CRS Matrix / Vector Multiply											 //
//---------------------------------------------------------------------------//

template <class T> inline void CNCSparseMatrixCRS<T>::gpu_mult (
		const void * x, void * y,
		unsigned int vec_size ) {

	cnc_cuda_mat1x1vecmult ( (float*)gpu_mat_, a.size(),
							 (uint2*)gpu_redundant_rp_, rowptr.size(),
							 (unsigned int*)gpu_ci_, colind.size(),
							 (float*)x, (float*)y, vec_size ) ;

}

//---------------------------------------------------------------------------//


#endif
