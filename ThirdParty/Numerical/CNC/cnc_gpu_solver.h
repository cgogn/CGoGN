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

#ifndef CNC_GPU_SOLVER_H
#define CNC_GPU_SOLVER_H

#include "cnc_utils.h"
#include "cnc_cublas_utils.h"
#include "cnc_sparse_matrix.h"
#include "cnc_sparse_matrix_crs.h"
#include "cnc_sparse_matrix_bcrs.h"
#include "cnc_timer.h"
#include <vector>
#include <algorithm>



//---------------------------------------------------------------------------//
// GPU Solver class															 //
//---------------------------------------------------------------------------//

class CNCGPUSolver {

public:


//---------------------------------------------------------------------------//
//						---- MAIN ENTRY POINT ----							 //
// ---------------- GPU Solver Function, calls GPU Kernels ----------------- //
// Jacobi-Preconditioned Conjugate Gradient Solver implementation on the GPU //
//---------------------------------------------------------------------------//
// Solves the symetric definite positive system Ax = b						 //
// block_size:  For (B)CRS matrix: corresponds to the storage block size	 //
//				    (1=CRS1x1, 2=BCRS2x2 or 4=BCRS4x4)						 //
//				    with the implicit use of the largest possible size		 //
//					for the vectorization/register blocking					 //
// Function return false if nb_iter_max is reached,							 //
//	and thus the convergence is not reached.								 //
//---------------------------------------------------------------------------//

	inline static bool solve_cg (
				const CNCSparseMatrix &A,
				const floatArray &b,
				floatArray &x,
				const unsigned int nb_iter_max,
				const double epsilon,
				const unsigned int block_size ) ;



//---------------------------------------------------------------------------//
// Function comparing two float vectors vec0 and vec1 that can be stored on	 //
// CPU memory (Type=0) or GPU memory (Type=1)								 //
// vec_size gives the number of elements in each vectors					 //
// tolerance is the minimum difference between two elements to consider them //
// to be different.															 //
//---------------------------------------------------------------------------//

	inline static void compare_vec ( float * vec0, int Type0,
					   float * vec1, int Type1,
					   unsigned int vec_size,
					   float tolerance ) ;



private:

//---------------------------------------------------------------------------//
// Internal implementation of the Conjugate Gradient Solver on the GPU       //
//---------------------------------------------------------------------------//

	template<class T> inline static bool solve_cg_internal (
				T &A,
				const floatArray &b,
				floatArray &x,
				const unsigned int nb_iter_max,
				const double epsilon,
				const unsigned int block_size ) ;



}; // end class GPUSolver
 


//---------------------------------------------------------------------------//
// External CUDA Kernel Functions, forward declarations						 //
//---------------------------------------------------------------------------//

extern "C" void cnc_cuda_set_dim_vec ( 
				unsigned int dim_grid_x, unsigned int dim_grid_y,
				unsigned int dim_block_x, unsigned int dim_block_y ) ;

extern "C" void cnc_cuda_set_dim_vec2 (
				unsigned int dim_grid_x, unsigned int dim_grid_y,
				unsigned int dim_block_x, unsigned int dim_block_y ) ;

extern "C" void cnc_cuda_set_dim_vec4 (
				unsigned int dim_grid_x, unsigned int dim_grid_y,
				unsigned int dim_block_x, unsigned int dim_block_y ) ;

extern "C" void cnc_cuda_vecvecmult ( unsigned int size, float * x,
								  float * y, float * r ) ;

 //---------------------------------------------------------------------------//

inline bool CNCGPUSolver::solve_cg (
		const CNCSparseMatrix &A,
		const floatArray &b,
		floatArray &x,
		const unsigned int nb_iter_max,
		const double epsilon,
		const unsigned int block_size ) {


	//check cuda device support
	CNC_ASSERT ( CNCCheckDevice (), "No CUDA device found, please check the compatibility of your hardware with CUDA on NVIDIA's website" ) ;
	CNC_ASSERT ( A.n() == A.m(), "Non-square sparse matrix unsupported" ) ;
	CNC_ASSERT ( (block_size==1||block_size==2||block_size==4), "Wrong Block size" ) ;
	CNC_ASSERT ( nb_iter_max>=1, "Wrong number of iterations" ) ;
	CNC_ASSERT ( epsilon>0.0, "Wrong convergence threshold" ) ;

//	printf ( "############################################################\n" ) ;
//	printf ( "Start Init CG Solver: data conversion, allocation and upload\n" ) ;
//	printf ( "------------------------------------------------------------\n" ) ;
//	printf ( "max iter: %d\ntolerance: %10.10f\nblock_size: %dx%d\n",
//			 nb_iter_max, epsilon, block_size, block_size ) ;
//	printf ( "size vector: %d\n", x.size() ) ;

	if ( block_size == 1 ) {
		//SmCRS smcrs ;
		SmCRS smcrs ;
		convert_matrix<float> ( A, smcrs, false ) ;
		return CNCGPUSolver::solve_cg_internal<CNCSparseMatrixCRS<float> > (
										smcrs, b, x, nb_iter_max,
										epsilon, block_size ) ; 
	}
	else if ( block_size == 2 ) {
		SmBCRS2x2 smbcrs2x2 ;
		convert_matrix<float, 2, 2> ( A, smbcrs2x2 ) ;
		return CNCGPUSolver::solve_cg_internal<CNCSparseMatrixBCRS<float,2,2> > (
										smbcrs2x2, b, x, nb_iter_max,
										epsilon, block_size ) ; 
	}
	else if ( block_size == 4 ) {
		SmBCRS4x4 smbcrs4x4 ;
		convert_matrix<float, 4, 4> ( A, smbcrs4x4 ) ;
		return CNCGPUSolver::solve_cg_internal<CNCSparseMatrixBCRS<float,4,4> > (
										smbcrs4x4, b, x, nb_iter_max,
										epsilon, block_size ) ; 
	}
	else {
		printf ( "Wrong Block size\n" ) ; // you should never reach this point...
		return false ;
	}
}

//---------------------------------------------------------------------------//

template<class T> inline bool CNCGPUSolver::solve_cg_internal (
		T &A,
		const floatArray &b,
		floatArray &x,
		const unsigned int nb_iter_max,
		const double epsilon,
		const unsigned int block_size ) {


	CNCTimer time ;
	double full_time = time.GetTime() ;
	double elapsed_time = 0 ;

	long N = x.size() ;

	// init and check cublas
	cublasStatus st = cublasInit () ;
	CNCgetError ( st ) ;


	// vars to be defined specifically for each storage format (CRS/BCRS...)
	const int size_matrix = A.a.size() ;
	const float * diag_matrix = A.diag.data() ;

//	printf ( "size matrix: %d\n", size_matrix ) ;

	cnc_cuda_set_dim_vec ( (unsigned int)(sqrt((float)N)/THREAD_BLOCK_SIZE+1),
					   (unsigned int)(sqrt((float)N)/THREAD_BLOCK_SIZE+1),
					   THREAD_BLOCK_SIZE, THREAD_BLOCK_SIZE ) ;

	cnc_cuda_set_dim_vec2( (unsigned int)(sqrt((float)N/2.0f)/THREAD_BLOCK_SIZE+1),
					   (unsigned int)(sqrt((float)N/2.0f)/THREAD_BLOCK_SIZE+1),
					   THREAD_BLOCK_SIZE, THREAD_BLOCK_SIZE ) ;

	cnc_cuda_set_dim_vec4( (unsigned int)(sqrt((float)N/4.0f)/THREAD_BLOCK_SIZE+1),
					   (unsigned int)(sqrt((float)N/4.0f)/THREAD_BLOCK_SIZE+1),
					   THREAD_BLOCK_SIZE, THREAD_BLOCK_SIZE ) ;

	void *gpu_r = NULL ;
	void *gpu_d = NULL ;
	void *gpu_h = NULL ;
	void *gpu_Ad= NULL ;
	void *gpu_diag_inv = NULL ;
	void *gpu_b = NULL ;
	void *gpu_x = NULL ;
	void *gpu_temp = NULL ;
	void *gpu_temp0 = NULL ;
	void *gpu_temp1 = NULL ;
	void *gpu_res0 = NULL ;

	// matrix allocation and upload
	A.gpu_allocate_and_upload() ;

	cublasAlloc ( N+16, 4, &gpu_r  ) ;
	cublasAlloc ( N+16, 4, &gpu_d  ) ;
	cublasAlloc ( N+16, 4, &gpu_h  ) ;
	cublasAlloc ( N+16, 4, &gpu_Ad ) ;
	cublasAlloc ( N+16, 4, &gpu_diag_inv ) ;
	cublasAlloc ( N+16, 4, &gpu_x ) ;
	cublasAlloc ( N+16, 4, &gpu_b ) ;
	cublasAlloc ( N+16, 4, &gpu_temp  ) ;
	cublasAlloc ( N+16, 4, &gpu_temp0 ) ;
	cublasAlloc ( N+16, 4, &gpu_temp1 ) ;
	cublasAlloc ( N+16, 4, &gpu_res0  ) ;

	// building the Jacobi preconditionner
	floatArray cpu_diag_inv ( N+16 ) ;
	for(long i=0; i<N; i++) {
		cpu_diag_inv[i] = (float)((i >= N || diag_matrix[i] == 0.0) ? 1.0 : 1.0 / diag_matrix[i]) ;
	}

	cublasSetVector ( N, 4, cpu_diag_inv.data(), 1, gpu_diag_inv, 1 ) ;
	cublasSetVector ( N, 4, x.data()       , 1, gpu_x,        1 ) ;
	cublasSetVector ( N, 4, b.data()       , 1, gpu_b,        1 ) ;

	unsigned int its=0;
	float alpha, beta;

//	printf ( "------------------------------------------------------------\n" ) ;
//	printf ( "End init of CG solver\n" ) ;
//	printf ( "------------------------------------------------------------\n" ) ;
//	printf ( "Start of CG solver\n" ) ;
//	printf ( "------------------------------------------------------------\n" ) ;
	
	// r = A*x
	A.gpu_mult ( gpu_x, gpu_r, N ) ;

	// r = b - A*x
	cublasSaxpy ( N, -1.0f, (float*)gpu_b, 1, (float*)gpu_r, 1 ) ;
	cublasSscal ( N, -1.0f, (float*)gpu_r, 1 ) ;

	// d = M-1 * r
	cnc_cuda_vecvecmult ( N, (float*)gpu_diag_inv, (float*)gpu_r, (float*)gpu_d ) ;

	// cur_err = rT*d
	float cur_err = cublasSdot ( N, (float*)gpu_r, 1, (float*)gpu_d, 1 ) ;

	// err = cur_err
	float err = (float)(cur_err * epsilon * epsilon) ;

 	while ( cur_err > err && (int)its < nb_iter_max) {
//		if(!(its & 31)) {
//			printf ( "%d : %10.10f -- %10.10f\n", its, cur_err, err ) ;
//		}

		// Ad = A*d
		A.gpu_mult ( gpu_d, gpu_Ad, N ) ;

		// alpha = cur_err / (dT*Ad)
		alpha = cur_err / cublasSdot ( N, (float*)gpu_d, 1, (float*)gpu_Ad, 1 ) ;

		// x = x + alpha * d
		cublasSaxpy ( N, alpha, (float*)gpu_d, 1, (float*)gpu_x, 1 ) ;

		// r = r - alpha * Ad
		cublasSaxpy ( N, -alpha, (float*)gpu_Ad, 1, (float*)gpu_r, 1 ) ;

		// h = M-1r
		cnc_cuda_vecvecmult ( N, (float*)gpu_diag_inv, (float*)gpu_r, (float*)gpu_h ) ;

		float old_err = cur_err ;

		// cur_err = rT * h
		cur_err = cublasSdot ( N, (float*)gpu_r, 1, (float*)gpu_h, 1 ) ;

		beta = cur_err / old_err ;

		// d = h + beta * d
		cublasSscal ( N, beta, (float*)gpu_d, 1 ) ;
		cublasSaxpy ( N, 1.0f, (float*)gpu_h, 1, (float*)gpu_d, 1 ) ;
		++its;
	}

//	printf ( "------------------------------------------------------------\n" ) ;
//	if ( its==nb_iter_max ) {
//		printf ( "Maximum #itr reached: SOLVER DID NOT CONVERGE !!!\n" ) ; 
//		printf ( "------------------------------------------------------------\n" ) ;
//	}
//	printf ( "CG Used %d iterations\n", its ) ;
//	double seconds = time.GetElapsedTime(full_time) ;
//	long long int flop =
//		8*N+size_matrix*2+its*(size_matrix*2+11*N+(long long int)(2.*N) ) ;
//	printf ( "GPU whole CG GFlops: %lg GFlops\n        Total execution time: %lg\n",
//			  1e-9 * flop / seconds, seconds ) ;
//	printf ( "############################################################\n\n\n" ) ;
	
	cublasGetVector ( N, 4, (float*)gpu_x, 1, x.data(), 1 ) ;

	cublasFree ( gpu_r ) ;
	cublasFree ( gpu_d ) ;
	cublasFree ( gpu_h  ) ;
	cublasFree ( gpu_Ad ) ;
	cublasFree ( gpu_diag_inv ) ;
	cublasFree ( gpu_x ) ;
	cublasFree ( gpu_b ) ;
	cublasFree ( gpu_temp ) ;
	cublasFree ( gpu_temp0 ) ;
	cublasFree ( gpu_temp1 ) ;
	cublasFree ( gpu_res0 ) ;

	A.gpu_deallocate () ;

	st = cublasShutdown() ;
	CNCgetError ( st ) ;

	return (its<nb_iter_max) ;
}

//---------------------------------------------------------------------------//

void CNCGPUSolver::compare_vec ( float * vec0, int Type0,
							  float * vec1, int Type1,
							  unsigned int vec_size,
							  float tolerance ) { // Type : 0: CPU memory
												  //        1: NVIDIA GPU memory

	float * values0, * values1 ;
	if ( Type0 ) {
		values0 = (float*)malloc(sizeof(float)*vec_size) ;
		cublasGetVector ( vec_size, sizeof(float),  vec0, 1, values0, 1 ) ;
	}
	else {
		values0 = vec0 ;
	}
	if ( Type1 ) {
		values1 = (float*)malloc(sizeof(float)*vec_size) ;
		cublasGetVector ( vec_size, sizeof(float),  vec1, 1, values1, 1 ) ;
	}
	else {
		values1 = vec1 ;
	}

	{for ( unsigned int i=0; i<vec_size; i++) {
		if ( abs(values0[i]-values1[i]) > tolerance ) {
			printf ( "err[%d]: %8.14f, val0[%d]: %8.14f, val1[%d]: %8.14f\n",
				     i, values0[i]-values1[i], i, values0[i], i, values1[i] ) ;
		}
	}}

	if ( Type0 ) {
		free ( values0 ) ;
	}
	if ( Type1 ) {
		free ( values1 ) ;
	}
}

//---------------------------------------------------------------------------//

#endif
