cmake_minimum_required(VERSION 2.6)

#include(ExternalProject)

project(CGoGN_ThirdParty)

SET(CGoGN_ROOT_DIR ${CMAKE_SOURCE_DIR}/..)

find_package(OpenGL)
find_package(GLUT)

#On Windows we use AntTweakBar/OpenCTM of dependencies, not compiled
IF(WIN32)
	SET(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/lib)#release added by visual
	INCLUDE_DIRECTORIES(${CGoGN_ROOT_DIR}/windows_dependencies/include/)
	add_subdirectory(Zinri/Build)
	add_subdirectory(gzstream/Build)
	add_subdirectory(Numerical)
	add_subdirectory(Assimp/code)
	if(CMAKE_CONFIGURATION_TYPES)
		set(CMAKE_CONFIGURATION_TYPES Release)
		set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Use only Release configuration for ThirdParty" FORCE)
	endif()
ELSE(WIN32)
	SET(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/lib/Release)
	add_custom_target( FakeTargetOpenCTM ALL "make" 
		WORKING_DIRECTORY ${CGoGN_ROOT_DIR}/ThirdParty/OpenCTM
		COMMENT "Custom compile of OpenCTM" )
	add_subdirectory(Zinri/Build)
	add_subdirectory(gzstream/Build)
	add_subdirectory(AntTweakBar)
	add_subdirectory(Numerical)
	add_subdirectory(Assimp)
ENDIF(WIN32)

