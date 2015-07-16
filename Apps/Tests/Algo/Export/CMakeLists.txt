cmake_minimum_required(VERSION 2.6)

project(testing_algo_export)
	
add_executable( test_algo_export 
algo_export.cpp
export.cpp
exportPov.cpp
exportVTU.cpp
exportVol.cpp
)	

target_link_libraries( test_algo_export
	${CGoGN_LIBS} ${CGoGN_EXT_LIBS})