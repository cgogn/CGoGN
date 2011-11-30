cmake_minimum_required(VERSION 2.6)

project(gzstream)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(
	GLOB_RECURSE
	source_files
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library( gzstream ${SHARED_OR_STATIC} ${source_files} )
