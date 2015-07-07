cmake_minimum_required(VERSION 2.6)

project(testing_algo_tiling)
	
add_executable( test_algo_tiling 
algo_tiling.cpp 
Surface/hexagonal.cpp
Surface/square.cpp
Surface/triangular.cpp
Volume/cubic.cpp
)	

target_link_libraries( test_algo_tiling 
	${CGoGN_LIBS} ${CGoGN_EXT_LIBS})

