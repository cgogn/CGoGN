cmake_minimum_required(VERSION 2.6)

project(testing_container)

add_executable( test_container 
	test_container.cpp
	attributeContainer.cpp
	attributeMultiVector.cpp
	containerBrowser.cpp )	
	
target_link_libraries( test_container 
	${CGoGN_LIBS} ${CGoGN_EXT_LIBS})
