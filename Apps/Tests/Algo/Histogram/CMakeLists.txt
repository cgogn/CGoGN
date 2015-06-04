cmake_minimum_required(VERSION 2.6)

project(testing_algo_histogram)

add_executable( test_algo_histogram
algo_histogram.cpp
histogram.cpp
)	

target_link_libraries( test_algo_histogram 
	${CGoGN_LIBS} ${CGoGN_EXT_LIBS})


