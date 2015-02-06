cmake_minimum_required(VERSION 2.6)

project(Zinri)

find_package(ZLIB REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${ZLIB_INCLUDE_DIRS})

file(
	GLOB_RECURSE
	source_files
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
)

add_library( Zinri ${source_files} )
target_link_libraries(Zinri ${ZLIB_LIBRARIES})
