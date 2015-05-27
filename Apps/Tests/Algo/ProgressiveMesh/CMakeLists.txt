cmake_minimum_required(VERSION 2.6)

project(testing_algo_progessiveMesh)
	
add_executable( test_algo_progessiveMesh
algo_progressiveMesh.cpp
pmesh.cpp
vsplit.cpp
)	

target_link_libraries( test_algo_progessiveMesh 
	${CGoGN_LIBS} ${CGoGN_EXT_LIBS})

