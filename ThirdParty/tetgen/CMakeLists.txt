# Set  the minimum  required version  of cmake  for a  project.
cmake_minimum_required(VERSION 2.6)

project(tetgen)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#Add a library to the project using the specified source files. 
add_library(tetgen ${CMAKE_CURRENT_SOURCE_DIR}/tetgen.cxx ${CMAKE_CURRENT_SOURCE_DIR}/predicates.cxx)

#Set properties on a target. 
#We use this here to set -DTETLIBRARY for when compiling the library
#set_target_properties(tet PROPERTIES "COMPILE_DEFINITIONS" TETLIBRARY)
