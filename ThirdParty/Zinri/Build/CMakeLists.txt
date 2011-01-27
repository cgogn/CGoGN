cmake_minimum_required(VERSION 2.6)

project(Zinri)

set(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/lib/Release)

include_directories(..)

file(
	GLOB_RECURSE
	source_files
	../*.c
)

add_library(
	Zinri
	${source_files}
)
