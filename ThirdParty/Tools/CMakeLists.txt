cmake_minimum_required(VERSION 2.6)

project(Tools)

SET(EXECUTABLE_OUTPUT_PATH ${CGoGN_ROOT_DIR}/bin)

add_executable( shader_to_h ${CMAKE_CURRENT_SOURCE_DIR}/shader_to_h.cpp )


