cmake_minimum_required(VERSION 2.6)

project(Zinri)

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
