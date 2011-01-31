cmake_minimum_required(VERSION 2.6)

#include(ExternalProject)

project(CGoGN_ThirdParty)

SET(CGoGN_ROOT_DIR ${CMAKE_SOURCE_DIR}/..)

find_package(OpenGL)
find_package(GLUT)

#On Windows we use AntTweakBar/OpenCTM of dependencies, not compiled
IF(WIN32)
	add_subdirectory(Zinri/Build)
	add_subdirectory(gzstream/Build)
	add_subdirectory(Numerical)
	add_subdirectory(Assimp)
ELSE(WIN32)
	add_custom_target( FakeTargetOpenCTM ALL "make" 
		WORKING_DIRECTORY ${CGoGN_ROOT_DIR}/ThirdParty/OpenCTM
		COMMENT "Custom compile of OpenCTM" )
	add_subdirectory(Zinri/Build)
	add_subdirectory(gzstream/Build)
	add_subdirectory(AntTweakBar)
	add_subdirectory(Numerical)
	add_subdirectory(Assimp)
ENDIF(WIN32)

