cmake_minimum_required(VERSION 2.6)

project(testing_algo_linearSolving)
	
add_executable( test_algo_linearSolving
algo_linearSolving.cpp 
basic.cpp
)	

target_link_libraries( test_algo_linearSolving 
	${CGoGN_LIBS} ${CGoGN_EXT_LIBS})

