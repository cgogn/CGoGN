cmake_minimum_required(VERSION 2.8)

project(benches)

#SET(CMAKE_BUILD_TYPE Release)
#SET(CMAKE_BUILD_TYPE Debug)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOTOPOWARNING")


# define includes path
include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
	${CGoGN_ROOT_DIR}/include
	${CGoGN_EXT_INCLUDES}
)

# define libs path
#IF(WIN32)
#	link_directories( ${CGoGN_ROOT_DIR}/lib/$(ConfigurationName) ${Boost_LIBRARY_DIRS} )
#ELSE(WIN32)
#	link_directories( ${CGoGN_ROOT_DIR}/lib/Release )
#ENDIF(WIN32)


add_executable(bench_trav bench_trav.cpp )
target_link_libraries( bench_trav ${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )

add_executable(bench_remesh bench_remesh.cpp )
target_link_libraries( bench_remesh ${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )

add_executable(bench_compact bench_compact.cpp )
target_link_libraries( bench_compact ${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )
