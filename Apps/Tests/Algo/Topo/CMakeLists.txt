cmake_minimum_required(VERSION 2.6)

project(testing_algo_topo)
	
add_executable( test_algo_topo 
algo_topo.cpp 
basic.cpp
embedding.cpp
simplex.cpp
Map2/uniformOrientation.cpp
)	

target_link_libraries( test_algo_topo 
	${CGoGN_LIBS} ${CGoGN_EXT_LIBS})

