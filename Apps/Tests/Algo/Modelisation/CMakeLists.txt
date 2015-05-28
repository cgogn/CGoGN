cmake_minimum_required(VERSION 2.6)

project(testing_algo_modelisation)
	
add_executable( test_algo_modelisation 
algo_modelisation.cpp
boundEmb.cpp
extrusion.cpp
planeCutting.cpp
polyhedron.cpp
subdivision.cpp
subdivision3.cpp
tetrahedralization.cpp
triangulation.cpp
voxellisation.cpp
)	

target_link_libraries( test_algo_modelisation 
	${CGoGN_LIBS} ${CGoGN_EXT_LIBS})
