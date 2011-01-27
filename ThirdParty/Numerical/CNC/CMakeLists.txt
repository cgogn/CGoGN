cmake_minimum_required(VERSION 2.6)

project(CNC_CUDA)

set(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/lib/Release)

IF(WIN32)
	ADD_DEFINITIONS(-DOS_WIN)
ELSE(WIN32)
	ADD_DEFINITIONS(-DOS_LINUX)
ENDIF(WIN32)

CUDA_ADD_LIBRARY(cnccuda
	cnc_cuda_blas.cu
	cnc_sparse_matrix.cpp)

