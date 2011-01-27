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

#ifndef CNC_GPU_UTILS_H
#define CNC_GPU_UTILS_H

#include <cublas.h>
#include <cuda.h>
#include <cuda_runtime.h>
//#include <cutil.h>
#include <stdio.h>
#include <string.h>


//---------------------------------------------------------------------------//
// thread block size (number of threads per block = BLOCK_SIZE*BLOCK_SIZE)	 //
//---------------------------------------------------------------------------//

#define THREAD_BLOCK_SIZE 16

//---------------------------------------------------------------------------//

static void CNCgetError ( cublasStatus st ) {
	switch (st) {
		case CUBLAS_STATUS_SUCCESS:	break ;  //printf ( "cublas : no error\n" ) ; break ;
		case CUBLAS_STATUS_NOT_INITIALIZED:	printf ("CUBLAS_STATUS_NOT_INITIALIZED\n") ;break ;
		case CUBLAS_STATUS_ALLOC_FAILED:	printf ("CUBLAS_STATUS_ALLOC_FAILED\n") ;	break ;
		case CUBLAS_STATUS_INVALID_VALUE:	printf ("CUBLAS_STATUS_INVALID_VALUE\n") ;	break ;
		case CUBLAS_STATUS_MAPPING_ERROR:	printf ("CUBLAS_STATUS_MAPPING_ERROR\n") ;	break ;
		case CUBLAS_STATUS_EXECUTION_FAILED:printf ("CUBLAS_STATUS_EXECUTION_FAILED\n");break ;
		case CUBLAS_STATUS_INTERNAL_ERROR:	printf ("CUBLAS_STATUS_INTERNAL_ERROR\n") ;	break ;
	default: printf ("unkown error message\n"); break ;
	}
}

//---------------------------------------------------------------------------//

static void CNCgetError () {
	cublasStatus st = cublasGetError() ;
	CNCgetError ( st ) ;
}

//---------------------------------------------------------------------------//

// Check if there is a device supporting CUDA
static bool CNCCheckDevice()
{
#if __DEVICE_EMULATION__
    return true;
#else
    int deviceCount;
    cudaGetDeviceCount(&deviceCount);
    if (deviceCount == 0)
        return false;
    int dev;
    for (dev = 0; dev < deviceCount; ++dev) {
        cudaDeviceProp deviceProp;
        cudaGetDeviceProperties(&deviceProp, dev);
        if (strncmp(deviceProp.name, "Device Emulation", 16))
            break;
    }
    if (dev == deviceCount)
        return false;
    else {
        cudaSetDevice(dev);
        return true;
    }
#endif
}

//---------------------------------------------------------------------------//


#endif //CNC_UTILS_H
