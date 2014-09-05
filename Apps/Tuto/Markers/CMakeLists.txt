cmake_minimum_required(VERSION 2.8)

project(Tuto_Markers)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable( dartmarkers dartmarkers.cpp)
target_link_libraries( dartmarkers ${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )

add_executable( cellmarkers cellmarkers.cpp)
target_link_libraries( cellmarkers ${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )

