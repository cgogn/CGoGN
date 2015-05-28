cmake_minimum_required(VERSION 2.6)

project(testing_algo_filtering)
	
add_executable( test_algo_filtering 
algo_filtering.cpp
average.cpp
average_normals.cpp
bilateral.cpp
taubin.cpp
tools.cpp
)	

target_link_libraries( test_algo_filtering 
	${CGoGN_LIBS} ${CGoGN_EXT_LIBS})


