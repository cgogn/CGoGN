cmake_minimum_required(VERSION 2.6)

project(testing_algo_booleanOperator)
	
add_executable( test_algo_booleanOperator
algo_booleanOperator.cpp
mergeVertices.cpp
)	

target_link_libraries( test_algo_booleanOperator 
	${CGoGN_LIBS} ${CGoGN_EXT_LIBS})


