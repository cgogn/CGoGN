cmake_minimum_required(VERSION 2.6)

project(testing_utils)

add_executable( test_utils 
	test_utils.cpp
	colorMaps.cpp
	colourConverter.cpp
	qem.cpp
	quadricRGBfunctions.cpp
	quantization.cpp
#	shared_mem.cpp
	sphericalHarmonics.cpp
	textures.cpp )	
	
target_link_libraries( test_utils 
	${CGoGN_LIBS} ${CGoGN_EXT_LIBS})

qt_use_cgogn_modules(test_utils)

