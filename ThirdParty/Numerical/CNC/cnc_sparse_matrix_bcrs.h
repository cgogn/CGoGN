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

#ifndef CNC_SPARSE_MATRIX_BCRS_H
#define CNC_SPARSE_MATRIX_BCRS_H

#include "cnc_utils.h"
#include "cnc_cublas_utils.h"
#include "cnc_arrays.h"

#include <algorithm>


//---------------------------------------------------------------------------//
// External CUDA Kernel Functions, forward declarations						 //
//---------------------------------------------------------------------------//

extern "C" void cnc_cuda_mat2x2vecmult4 (
				float4 *matrix, unsigned int size_matrix,
				uint2 * rowptr, unsigned int size_rowptr,
				unsigned int * colind, unsigned int size_colind,
				float2 * x, float2 * b, unsigned int size_vec ) ;

extern "C" void cnc_cuda_mat4x4vecmult4 (
				float4 * mat0, float4 * mat1,
				float4 * mat2, float4 * mat3,
				unsigned int size_matrix,
				uint2 * rowptr, unsigned int size_rowptr,
				unsigned int * colind, unsigned int size_colind,
				float4 * x, float4 * b, unsigned int size_vec ) ;


// -------------------------------------------------------------------------- //
// BCRS Sparse matrix data structure with CPU and GPU storage				  //
// -------------------------------------------------------------------------- //

    /**
     * The base class for all bloc compressed row storage matrices.
     */
    template <class T, int BLOCM, int BLOCN> class CNCGenericSparseMatrixBCRS {
    public:
        typedef CNCArray1d<long> IndexArray ;
        typedef CNCArray1d<T> CoeffArray ;
        enum { BM=BLOCM } ;
        enum { BN=BLOCN } ;
        enum { BLOC_SIZE=BM*BN } ;

        inline CNCGenericSparseMatrixBCRS() : N_(0) { } 

        inline float filling_ratio() const ;

        /** number of bloc rows */
        long M() const { return rowptr.size() - 1 ; }
        /** number of bloc columns */
        long N() const { return N_ ; }
        /** number of rows */
        long m() const { return M() * BM ; } 
        /** number of columns */
        long n() const { return N() * BN ; } 

        long number_of_blocs_in_row(long i) {
            return rowptr[i+1] - rowptr[i] ;
        }

        long max_number_of_blocs_in_row() {
            long result = 0 ;
            for(long i=0; i<M(); i++) {
				result = (result>number_of_blocs_in_row(i))?(result):(number_of_blocs_in_row(i)) ;
            }
            return result ;
        }

        inline void mult(const T* x, T* y) const ;
        inline void mult(const CNCArray1d<T>& x, CNCArray1d<T>& y) const {
            mult(x.data(), y.data()) ;
        }

        long mem_usage() const {
            return 
                rowptr.mem_usage() + 
                colind.mem_usage() + 
                a.mem_usage() + 
                diag.mem_usage() + 
                sizeof(long) ;
        }

		inline void print() const {
			for(long I=0; I<M(); I++) {
				for(long JJ=rowptr[I]; JJ<rowptr[I+1]; JJ++) {
					long ofs = 0 ;
					long J = colind[JJ] ;
					for(long di=0; di<BM; di++) {
						for(long dj=0; dj<BN; dj++) {
							if(a[JJ * BLOC_SIZE + ofs] != 0.0) {
								printf ( "%d %d %f\n", (I * BM + di), (J * BN + dj), a[JJ * BLOC_SIZE + ofs] ) ;
							}
							ofs++ ;
						}
					}
				}
			}
		}

        IndexArray rowptr ;
        IndexArray colind ;
        CoeffArray a ;
        CoeffArray diag ;
        long N_ ;
    } ;

//---------------------------------------------------------------------------//

    /**
     * The general implementation of block compressed row storage
     * matrices.
     */
    template <class T, int BLOCM, int BLOCN> class CNCSparseMatrixBCRS : 
        public CNCGenericSparseMatrixBCRS<T, BLOCM, BLOCN> {
    public:
        typedef CNCGenericSparseMatrixBCRS<T, BLOCM, BLOCN> superclass ;
        typedef CNCArray1d<long> IndexArray ;
		CNCSparseMatrixBCRS() : 
			gpu_redundant_rp_(NULL),
			gpu_ci_(NULL),
			gpu_mat_(NULL),
			gpu_mat0_(NULL),
			gpu_mat1_(NULL),
			gpu_mat2_(NULL),
			gpu_mat3_(NULL) { }

		inline void gpu_allocate_and_upload () ;
		inline void gpu_deallocate () ;
		inline void gpu_mult ( const void * x, void * y, unsigned int vec_size ) ;

		void * gpu_redundant_rp_ ;
		void * gpu_ci_ ;
		void * gpu_mat_ ;
		void * gpu_mat0_;
		void * gpu_mat1_;
		void * gpu_mat2_;
		void * gpu_mat3_;
    } ;

typedef CNCSparseMatrixBCRS<float, 2, 2> SmBCRS2x2 ;
typedef CNCSparseMatrixBCRS<float, 4, 4> SmBCRS4x4 ;

//---------------------------------------------------------------------------//

    template <class T, int BM, int BN> inline
    void CNCGenericSparseMatrixBCRS<T,BM,BN>::mult(const T* x, T* y) const {
        long y_base = 0 ;
        for(long I=0; I<M(); I++) {
            T sum[BM] ;
            for(long i=0; i<BM; i++) { sum[i] = 0 ; }
            for(long JJ=rowptr[I]; JJ<rowptr[I+1]; JJ++) {
                long ofs = 0 ;
                long J = colind[JJ] ;
                long a_base = JJ*BLOC_SIZE ;
                long x_base = J * BN ;
                for(long di=0; di<BM; di++) {
                    for(long dj=0; dj<BN; dj++) {
                        sum[di] += a[a_base+ofs] * x[x_base+dj] ;
                        ofs++ ;
                    }
                }
            }
            for(long i=0; i<BM; i++) {
                y[y_base + i] = sum[i] ;
            }
            y_base += BM ;
        }
    }


//---------------------------------------------------------------------------//
// Allocation of graphics memory and upload of a 2x2 BCRS sparse matrix		 //
//---------------------------------------------------------------------------//

template <> inline void CNCSparseMatrixBCRS<float,2,2>::gpu_allocate_and_upload () {

	unsigned int size_matrix = a.size () ;
	
	// allocate and upload non-zero matrix coefficients	
	cublasAlloc ( size_matrix  +16, 4, &(gpu_mat_ ) ) ;
	cublasSetVector ( size_matrix, 4, a.data(), 1, gpu_mat_, 1 ) ;

	// allocate and upload rowptr and colind
	cublasAlloc ( colind.size()+16, 4, &(gpu_ci_) ) ;
	cublasAlloc ( 2*(rowptr.size()-1)+16, sizeof(unsigned int), &(gpu_redundant_rp_) ) ;
	unsigned int * ci = CNCallocate_and_copy<long, unsigned int> ( colind.data(),
																   colind.size() ) ;

	uint2 * cpu_redundant_rp =  CNCallocate<uint2>(rowptr.size()-1) ;
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
// Allocation of graphics memory and upload of a 4x4 BCRS sparse matrix		 //
//---------------------------------------------------------------------------//

template <> inline void CNCSparseMatrixBCRS<float,4,4>::gpu_allocate_and_upload () {

	unsigned int size_matrix = a.size () ;
	
	// allocate non-zero matrix coefficients
	cublasAlloc ( size_matrix/4+16, 4, &(gpu_mat0_) ) ;
	cublasAlloc ( size_matrix/4+16, 4, &(gpu_mat1_) ) ;
	cublasAlloc ( size_matrix/4+16, 4, &(gpu_mat2_) ) ;
	cublasAlloc ( size_matrix/4+16, 4, &(gpu_mat3_) ) ;

	float * m0 = (float*)malloc(sizeof(float)*(size_matrix/4)) ;
	float * m1 = (float*)malloc(sizeof(float)*(size_matrix/4)) ;
	float * m2 = (float*)malloc(sizeof(float)*(size_matrix/4)) ;
	float * m3 = (float*)malloc(sizeof(float)*(size_matrix/4)) ;

	// reorder sparse matrix blocs to fit into four arrays
	{for(long i=0; i<(long)(size_matrix/16); i++) {
		m0[i*4  ] = a.data()[i*16   ] ;
		m0[i*4+1] = a.data()[i*16+ 1] ;
		m0[i*4+2] = a.data()[i*16+ 2] ;
		m0[i*4+3] = a.data()[i*16+ 3] ;
		m1[i*4  ] = a.data()[i*16+ 4] ;
		m1[i*4+1] = a.data()[i*16+ 5] ;
		m1[i*4+2] = a.data()[i*16+ 6] ;
		m1[i*4+3] = a.data()[i*16+ 7] ;
		m2[i*4  ] = a.data()[i*16+ 8] ;
		m2[i*4+1] = a.data()[i*16+ 9] ;
		m2[i*4+2] = a.data()[i*16+10] ;
		m2[i*4+3] = a.data()[i*16+11] ;
		m3[i*4  ] = a.data()[i*16+12] ;
		m3[i*4+1] = a.data()[i*16+13] ;
		m3[i*4+2] = a.data()[i*16+14] ;
		m3[i*4+3] = a.data()[i*16+15] ;
	}}

	cublasSetVector ( size_matrix/4, 4, m0, 1, gpu_mat0_, 1 ) ;
	cublasSetVector ( size_matrix/4, 4, m1, 1, gpu_mat1_, 1 ) ;
	cublasSetVector ( size_matrix/4, 4, m2, 1, gpu_mat2_, 1 ) ;
	cublasSetVector ( size_matrix/4, 4, m3, 1, gpu_mat3_, 1 ) ;

	free (m0) ;
	free (m1) ;
	free (m2) ;
	free (m3) ;

	// allocate and upload rowptr and colind
	cublasAlloc ( colind.size()+16, 4, &(gpu_ci_) ) ;
	cublasAlloc ( 2*(rowptr.size()-1)+16, sizeof(unsigned int), &(gpu_redundant_rp_) ) ;

	unsigned int * ci = CNCallocate_and_copy<long, unsigned int> ( colind.data(),
																colind.size() ) ;
	uint2 * cpu_redundant_rp =  CNCallocate<uint2>(rowptr.size()-1) ;
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
// Free the gpu memory used to store a BCRS sparse matrix					 //
//---------------------------------------------------------------------------//

template <> inline void CNCSparseMatrixBCRS<float,2,2>::gpu_deallocate () {

	cublasFree ( gpu_ci_ ) ;
	cublasFree ( gpu_redundant_rp_ ) ;
	cublasFree ( gpu_mat_ ) ;
}


//---------------------------------------------------------------------------//
// Free the gpu memory used to store a BCRS sparse matrix					 //
//---------------------------------------------------------------------------//

template <> inline void CNCSparseMatrixBCRS<float,4,4>::gpu_deallocate () {

	cublasFree ( gpu_ci_ ) ;
	cublasFree ( gpu_redundant_rp_ ) ;
	cublasFree ( gpu_mat0_ ) ;
	cublasFree ( gpu_mat1_ ) ;
	cublasFree ( gpu_mat2_ ) ;
	cublasFree ( gpu_mat3_ ) ;
}

//---------------------------------------------------------------------------//
// GPU BCRS 2x2 Matrix / Vector Multiply									 //
//---------------------------------------------------------------------------//

template <> inline void CNCSparseMatrixBCRS<float,2,2>::gpu_mult ( const void * x, void * y,
												    unsigned int vec_size ) {

	cnc_cuda_mat2x2vecmult4(
		 (float4*)gpu_mat_, a.size(),
		 (uint2*)gpu_redundant_rp_, rowptr.size(),
		 (unsigned int*)gpu_ci_, colind.size(),
		 (float2*)x, (float2*)y, vec_size ) ;

}

//---------------------------------------------------------------------------//
// GPU BCRS 4x4 Matrix / Vector Multiply									 //
//---------------------------------------------------------------------------//

template <> inline void CNCSparseMatrixBCRS<float,4,4>::gpu_mult ( const void * x, void * y,
												    const unsigned int vec_size ) {

	cnc_cuda_mat4x4vecmult4( (float4*)gpu_mat0_, (float4*)gpu_mat1_,
						     (float4*)gpu_mat2_, (float4*)gpu_mat3_, 
							 a.size(),
							 (uint2*)gpu_redundant_rp_, rowptr.size(),
							 (unsigned int*)gpu_ci_, colind.size(),
							 (float4*)x, (float4*)y, vec_size ) ;

}

//---------------------------------------------------------------------------//


#endif
