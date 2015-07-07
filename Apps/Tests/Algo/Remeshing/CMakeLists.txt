cmake_minimum_required(VERSION 2.6)

project(testing_algo_remeshing)
	
add_executable( test_algo_remeshing 
algo_remeshing.cpp 
pliant.cpp
)	

target_link_libraries( test_algo_remeshing 
	${CGoGN_LIBS} ${CGoGN_EXT_LIBS})

