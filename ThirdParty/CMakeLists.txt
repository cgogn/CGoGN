cmake_minimum_required(VERSION 2.8)

project(CGoGN_ThirdParty)

SET(CGoGN_ROOT_DIR ${CMAKE_SOURCE_DIR}/..)

IF(WIN32)
	
ENDIF(WIN32)

IF (${CMAKE_CURRENT_BINARY_DIR} MATCHES "(.*)Debug")
	set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Debug/Release" FORCE)
ELSE()
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Debug/Release" FORCE)	
ENDIF()

IF(WIN32)
	set(CMAKE_PREFIX_PATH ${CGoGN_ROOT_DIR}/windows_dependencies CACHE STRING "path to dependencies")
	INCLUDE_DIRECTORIES(${CGoGN_ROOT_DIR}/windows_dependencies/include/)
	INCLUDE_DIRECTORIES(${CGoGN_ROOT_DIR}/windows_dependencies/include/suitesparse)
	SET(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/lib)
	set(CMAKE_CONFIGURATION_TYPES Release Debug)
#	SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE})
#	SET(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "" FORCE)
#	MARK_AS_ADVANCED(CMAKE_CONFIGURATION_TYPES)
ELSE()
	SET(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/lib/${CMAKE_BUILD_TYPE})
ENDIF()
IF(WIN32)
	
ENDIF(WIN32)



# for shared or not shared
SET (BUILD_SHARED_LIBS OFF CACHE BOOL 
	"if used all library are build as shared type (.so/.dylib/.dll)")

SET (WITH_ASSIMP OFF CACHE BOOL "build with Assimp")
SET (WITH_ZINRI ON CACHE BOOL "build with Zinri libs")
SET (WITH_PYTHONQT ON CACHE BOOL "build PythonQt (for Schnapps)")

IF (APPLE)
	SET(CMAKE_OSX_ARCHITECTURES x86_64)
ENDIF(APPLE)

add_subdirectory(Tools Tools/build)

IF (WITH_PYTHONQT)
	add_subdirectory(PythonQt PythonQt/build)
	INSTALL (DIRECTORY PythonQt/src/ DESTINATION ${CGoGN_ROOT_DIR}/ThirdParty/include/PythonQt FILES_MATCHING PATTERN "*.h")
ENDIF (WITH_PYTHONQT)

add_subdirectory(OpenNL OpenNL/build)
INSTALL (DIRECTORY OpenNL/src/NL/ DESTINATION ${CGoGN_ROOT_DIR}/ThirdParty/include/NL FILES_MATCHING PATTERN "*.h")

add_subdirectory(tetgen tetgen/build)
INSTALL (DIRECTORY tetgen/ DESTINATION ${CGoGN_ROOT_DIR}/ThirdParty/include/tetgen FILES_MATCHING PATTERN "*.h")

IF (WITH_ZINRI)
	add_subdirectory(Zinri Zinri/build)
	INSTALL (DIRECTORY Zinri/ DESTINATION ${CGoGN_ROOT_DIR}/ThirdParty/include/Zinri FILES_MATCHING PATTERN "*.h")
ENDIF(WITH_ZINRI)

IF (WITH_ASSIMP)
	add_subdirectory(Assimp Assimp/build)
	INSTALL (DIRECTORY Assimp/include/ DESTINATION ${CGoGN_ROOT_DIR}/ThirdParty/include/Assimp)
ENDIF (WITH_ASSIMP)



