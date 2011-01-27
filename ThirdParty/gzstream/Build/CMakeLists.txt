cmake_minimum_required(VERSION 2.6)

project(gzstream)

set(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/lib/Release)

include_directories(..)

file(
	GLOB_RECURSE
	source_files
	../*.cpp
)

add_library(
	gzstream
	${source_files}
)
