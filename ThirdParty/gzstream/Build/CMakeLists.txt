cmake_minimum_required(VERSION 2.6)

project(gzstream)

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
