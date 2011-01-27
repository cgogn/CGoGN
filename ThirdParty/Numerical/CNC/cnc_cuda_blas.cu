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

// Header files
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Thread block size (THREAD_BLOCK_SIZE² = 768 at max)
#define THREAD_BLOCK_SIZE 16

//---------------------------------------------------------------------------//

dim3 CNCdimGrid_vec  ;
dim3 CNCdimBlock_vec ;
dim3 CNCdimGrid_vec2 ;
dim3 CNCdimBlock_vec2;
dim3 CNCdimGrid_vec4 ;
dim3 CNCdimBlock_vec4;

//---------------------------------------------------------------------------//

extern "C" void cnc_cuda_set_dim_vec ( unsigned int dim_grid_x, unsigned int dim_grid_y,
								   unsigned int dim_block_x, unsigned int dim_block_y ) ;

extern "C" void cnc_cuda_set_dim_vec2( unsigned int dim_grid_x, unsigned int dim_grid_y,
								   unsigned int dim_block_x, unsigned int dim_block_y ) ;

extern "C" void cnc_cuda_set_dim_vec4( unsigned int dim_grid_x, unsigned int dim_grid_y,
								   unsigned int dim_block_x, unsigned int dim_block_y ) ;

extern "C" void cnc_cuda_vecvecmult( unsigned int size, float * x, float * y, float * r ) ;

extern "C" void cnc_cuda_mat1x1vecmult ( float * matrix, unsigned int size_matrix,
									 uint2 * rowptr, unsigned int size_rowptr,
									 unsigned int * colind, unsigned int size_colind,
									 float * x, float * b, unsigned int size_vec ) ;

extern "C" void cnc_cuda_mat2x2vecmult4 ( float4 * matrix, unsigned int size_matrix,
									  uint2 * rowptr, unsigned int size_rowptr,
									  unsigned int * colind, unsigned int size_colind,
									  float2 * x, float2 * b, unsigned int size_vec ) ;

extern "C" void cnc_cuda_mat4x4vecmult4 ( float4 * mat0,  float4 * mat1,
									  float4 * mat2,  float4 * mat3,
									  unsigned int size_matrix,
									  uint2 * rowptr, unsigned int size_rowptr,
									  unsigned int * colind, unsigned int size_colind,
									  float4 * x, float4 * b, unsigned int size_vec ) ;

//---------------------------------------------------------------------------//
// Forward declaration of the blas kernels									 //
//---------------------------------------------------------------------------//

__global__ void CNCVecVecMultKernel ( unsigned int size, float * x, float * y, float * r ) ;

__global__ void CNCMat1x1VecMultKernel ( float * matrix, unsigned int size_matrix,
									  uint2 * rowptr, unsigned int size_rowptr,
									  unsigned int * colind, unsigned int size_colind,
									  float * x, float * b, unsigned int size_vec ) ;

__global__ void CNCMat2x2VecMult4Kernel ( float4 * matrix, unsigned int size_matrix,
									  uint2 * rowptr, unsigned int size_rowptr,
									  unsigned int * colind, unsigned int size_colind,
									  float2 * x, float2 * b, unsigned int size_vec ) ;

__global__ void CNCMat4x4VecMult4Kernel ( float4 * mat0, float4 * mat1,
									   float4 * mat2, float4 * mat3,
									   unsigned int size_matrix,
									   uint2  * rowptr, unsigned int size_rowptr,
									   unsigned int * colind, unsigned int size_colind,
									   float4 * x, float4 * b, unsigned int size_vec ) ;

//---------------------------------------------------------------------------//
// Setup functions and call of the kernels									 //
//---------------------------------------------------------------------------//

void cnc_cuda_set_dim_vec ( unsigned int dim_grid_x, unsigned int dim_grid_y,
					    unsigned int dim_block_x, unsigned int dim_block_y ) {
	CNCdimGrid_vec.x  = dim_grid_x ;
	CNCdimGrid_vec.y  = dim_grid_y ;
	CNCdimBlock_vec.x = dim_block_x ;
	CNCdimBlock_vec.y = dim_block_y ;
}
void cnc_cuda_set_dim_vec2 ( unsigned int dim_grid_x, unsigned int dim_grid_y,
						 unsigned int dim_block_x, unsigned int dim_block_y ) {
	CNCdimGrid_vec2.x  = dim_grid_x ;
	CNCdimGrid_vec2.y  = dim_grid_y ;
	CNCdimBlock_vec2.x = dim_block_x ;
	CNCdimBlock_vec2.y = dim_block_y ;
}
void cnc_cuda_set_dim_vec4 ( unsigned int dim_grid_x, unsigned int dim_grid_y,
						 unsigned int dim_block_x, unsigned int dim_block_y ) {
	CNCdimGrid_vec4.x  = dim_grid_x ;
	CNCdimGrid_vec4.y  = dim_grid_y ;
	CNCdimBlock_vec4.x = dim_block_x ;
	CNCdimBlock_vec4.y = dim_block_y ;
}

//---------------------------------------------------------------------------//

void cnc_cuda_mat1x1vecmult ( float * matrix, unsigned int size_matrix,
						  uint2 * rowptr, unsigned int size_rowptr,
						  unsigned int * colind, unsigned int size_colind,
						  float * x, float * b, unsigned int size_vec ) {

	// Launch the device computation
	CNCMat1x1VecMultKernel<<<CNCdimGrid_vec, CNCdimBlock_vec>>>(
		matrix, size_matrix, rowptr, size_rowptr,
		colind, size_colind, x, b, size_vec);
}

//---------------------------------------------------------------------------//

void cnc_cuda_mat2x2vecmult4 ( float4 * matrix, unsigned int size_matrix,
						   uint2 * rowptr, unsigned int size_rowptr,
						   unsigned int * colind, unsigned int size_colind,
						   float2 * x, float2 * b, unsigned int size_vec ) {

	// Launch the device computation
	CNCMat2x2VecMult4Kernel<<<CNCdimGrid_vec2, CNCdimBlock_vec2>>>(
		matrix, size_matrix,
		rowptr, size_rowptr,
		colind, size_colind,
		x, b, size_vec);
}

//---------------------------------------------------------------------------//

void cnc_cuda_mat4x4vecmult4 ( float4 * mat0,  float4 * mat1,
						   float4 * mat2,  float4 * mat3,
						   unsigned int size_matrix,
						   uint2 * rowptr, unsigned int size_rowptr,
						   unsigned int * colind, unsigned int size_colind,
						   float4 * x, float4 * b, unsigned int size_vec ) {

    // Launch the device computation
	CNCMat4x4VecMult4Kernel<<<CNCdimGrid_vec4, CNCdimBlock_vec4>>>(
												mat0, mat1, mat2, mat3,
											    size_matrix,
											    rowptr, size_rowptr,
											    colind, size_colind,
											    x, b, size_vec);
}

//---------------------------------------------------------------------------//

void cnc_cuda_vecvecmult( unsigned int size, float * x, float * y, float * r ) {

    // Launch the device computation
    CNCVecVecMultKernel<<<CNCdimGrid_vec, CNCdimBlock_vec>>>(size, x, y, r);
}

//---------------------------------------------------------------------------//

__device__ unsigned int compute_thread_index () {

	return ( blockIdx.x*THREAD_BLOCK_SIZE*THREAD_BLOCK_SIZE+
			 blockIdx.y*THREAD_BLOCK_SIZE*THREAD_BLOCK_SIZE*gridDim.x+
			 threadIdx.x+threadIdx.y*THREAD_BLOCK_SIZE) ;
}

//---------------------------------------------------------------------------//

__global__ void CNCVecVecMultKernel ( unsigned int size,
								   float * x,
								   float * y,
								   float * r ) {

	// Thread index
	const unsigned int index = compute_thread_index () ;
		
	if ( index < size )
		r[index] = x[index]*y[index] ;
}

//---------------------------------------------------------------------------//

__global__ void CNCMat1x1VecMultKernel ( float * matrix, unsigned int size_matrix,
									  uint2 * rowptr, unsigned int size_rowptr,
									  unsigned int * colind, unsigned int size_colind,
									  float * x, float * b, unsigned int size_vec ) {


	// Thread index
	const unsigned int index = compute_thread_index () ;

	if ( index < size_vec ) {

		uint2 rowptr_bounds = rowptr[index] ;

		float res = 0.0f ;

		// for each block of the block_row, mult
		for ( unsigned int i=rowptr_bounds.x; i<rowptr_bounds.y; i++ ) { 
			res += matrix[i]*x[colind[i]] ;
		}
		b[index] = res ;
	}
}

//---------------------------------------------------------------------------//

__global__ void CNCMat2x2VecMult4Kernel ( float4 * matrix, unsigned int size_matrix,
									  uint2 * rowptr, unsigned int size_rowptr,
									  unsigned int * colind, unsigned int size_colind,
									  float2 * x, float2 * b, unsigned int size_vec ) {

	// Thread index
	const unsigned int index = compute_thread_index () ;

	if ( index<<1 < size_vec ) {

		uint2 rowptr_bounds = rowptr[index] ;
		float2 res ;
		res.x = res.y = 0.0f ;

		unsigned int ci = 0 ;
		float2 x_vec ;
		float4 mat_vec ;

		// for each block of the block_row, mult
		for ( int i=rowptr_bounds.x; i<rowptr_bounds.y; i++ ) { 

			ci = colind[i] ;
			mat_vec = matrix[i] ;
			x_vec = x[ci] ;

			res.x += mat_vec.x*x_vec.x+mat_vec.y*x_vec.y ;
			res.y += mat_vec.z*x_vec.x+mat_vec.w*x_vec.y ;
		}
		b[index] = res ;
	}
}

//---------------------------------------------------------------------------//

__global__ void CNCMat4x4VecMult4Kernel ( float4 * mat0, float4 * mat1,
									   float4 * mat2, float4 * mat3,
									   unsigned int size_matrix,
									   uint2 * rowptr, unsigned int size_rowptr,
									   unsigned int * colind, unsigned int size_colind,
									   float4 * x, float4 * b, unsigned int size_vec ) {

    // Thread index
	const unsigned int index = compute_thread_index () ;

	if ( index<<2 < size_vec ) {

		uint2 rowptr_bounds = rowptr[index] ;
		float4 res ;
		res.x=res.y=res.z=res.w = 0.0f ;

		unsigned int ci = 0 ;
		float4 x_vec ;
		float4 row0  ;
		float4 row1  ;
		float4 row2  ;
		float4 row3  ;

		// for each block of the block_row, mult
		for ( int i=rowptr_bounds.x; i<rowptr_bounds.y; i++ ) {
			row0 = mat0[i] ;
			row1 = mat1[i] ;
			row2 = mat2[i] ;
			row3 = mat3[i] ;
			ci = colind[i] ;
			x_vec = x[ci]  ;
			res.x += row0.x*x_vec.x+row0.y*x_vec.y+row0.z*x_vec.z+row0.w*x_vec.w ;
			res.y += row1.x*x_vec.x+row1.y*x_vec.y+row1.z*x_vec.z+row1.w*x_vec.w ;
			res.z += row2.x*x_vec.x+row2.y*x_vec.y+row2.z*x_vec.z+row2.w*x_vec.w ;
			res.w += row3.x*x_vec.x+row3.y*x_vec.y+row3.z*x_vec.z+row3.w*x_vec.w ;
		}
		b[index] = res ;
	}
}

//---------------------------------------------------------------------------//
