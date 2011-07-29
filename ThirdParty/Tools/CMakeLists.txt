cmake_minimum_required(VERSION 2.6)

project(Tools)

SET (CMAKE_BUILD_TYPE Release)

add_executable( shader_to_h shader_to_h.cpp )
target_link_libraries( shader_to_h )

