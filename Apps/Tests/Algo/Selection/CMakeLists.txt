cmake_minimum_required(VERSION 2.6)

project(testing_algo_selection)
	
add_executable( test_algo_selection 
algo_selection.cpp 
collector.cpp
raySelector.cpp
)	

target_link_libraries( test_algo_selection 
	${CGoGN_LIBS} ${CGoGN_EXT_LIBS})

