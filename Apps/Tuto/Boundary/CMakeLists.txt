cmake_minimum_required(VERSION 2.8)

project(Tuto_Boundaries)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable( boundaries boundaries.cpp)
target_link_libraries( boundaries ${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )


