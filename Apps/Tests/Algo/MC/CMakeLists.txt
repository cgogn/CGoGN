cmake_minimum_required(VERSION 2.6)

project(testing_algo_mc)
	
add_executable( test_algo_mc 
algo_mc.cpp
image.cpp
marchingcube.cpp
marchingcubeGen.cpp
windowing.cpp
)	

target_link_libraries( test_algo_mc
	${CGoGN_LIBS} ${CGoGN_EXT_LIBS})

