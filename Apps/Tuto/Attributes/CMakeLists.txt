cmake_minimum_required(VERSION 2.8)

project(Tuto_Traversals)


SET (CMAKE_BUILD_TYPE Debug)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOTOPOWARNING")



include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
	${CGoGN_ROOT_DIR}/include
	${CGoGN_EXT_INCLUDES}
)

# define libs path
IF(WIN32)
	link_directories(${CGoGN_ROOT_DIR}/lib/$(ConfigurationName)
	${Boost_LIBRARY_DIRS})
ELSE(WIN32)
	link_directories(${CGoGN_ROOT_DIR}/lib/Release ${CGoGN_ROOT_DIR}/lib/Debug)
ENDIF(WIN32)


#define exec to compile

add_executable( simple_attribs simple_attribs.cpp)
target_link_libraries( simple_attribs ${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS} )

add_executable( multi_attribs multi_attribs.cpp)
target_link_libraries( multi_attribs ${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS} )
