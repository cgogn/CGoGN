cmake_minimum_required(VERSION 2.6)

project(testing_geometry)
	
add_executable( test_geometry 
	test_geometry.cpp
	basic.cpp
	bounding_box.cpp
	distances.cpp
	frame.cpp
	inclusion.cpp
	intersection.cpp
	matrix.cpp
	orientation.cpp
	plane_3d.cpp
	tensor.cpp
	transfo.cpp
	vector_gen.cpp )	
	
target_link_libraries( test_geometry 
	${CGoGN_LIBS} ${CGoGN_EXT_LIBS})
