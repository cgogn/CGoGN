cmake_minimum_required(VERSION 2.6)

project(testing_algo_simulation)
	
add_executable( test_algo_simulation 
algo_simulation.cpp 
ShapeMatching/shapeMatchingLinear.cpp
ShapeMatching/shapeMatchingQuadratic.cpp
)	

target_link_libraries( test_algo_simulation 
	${CGoGN_LIBS} ${CGoGN_EXT_LIBS})

