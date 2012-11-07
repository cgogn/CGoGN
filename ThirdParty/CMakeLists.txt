cmake_minimum_required(VERSION 2.6)

project(CGoGN_ThirdParty)

SET(CGoGN_ROOT_DIR ${CMAKE_SOURCE_DIR}/..)

IF(WIN32)
	set(CMAKE_PREFIX_PATH ${CGoGN_ROOT_DIR}/windows_dependencies CACHE STRING "path to dependencies")
ENDIF(WIN32)

SET(CMAKE_BUILD_TYPE Release)

IF (${CMAKE_CURRENT_BINARY_DIR} MATCHES "(.*)Debug")
	SET(CMAKE_BUILD_TYPE Debug)
ENDIF (${CMAKE_CURRENT_BINARY_DIR} MATCHES "(.*)Debug")

IF(WIN32)
	SET(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/lib)#release added by visual
	INCLUDE_DIRECTORIES(${CGoGN_ROOT_DIR}/windows_dependencies/include/)
ELSE(WIN32)
	SET(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/lib/${CMAKE_BUILD_TYPE})
ENDIF(WIN32)


# for shared or not shared
SET (BUILD_SHARED_LIBS OFF CACHE BOOL 
	"if used all library are build as shared type (.so /dylib/dll).")

SET (WITH_ASSIMP ON CACHE BOOL "build with Assimp")
SET (WITH_NUMERICAL ON CACHE BOOL "build with numerical libs")
SET (WITH_ZINRI ON CACHE BOOL "build with Zinri libs")

IF (APPLE)
	SET(CMAKE_OSX_ARCHITECTURES x86_64)
ENDIF(APPLE)



add_subdirectory(Tools Tools/build)
add_subdirectory(QGLViewer QGLViewer/build)

IF (WITH_ZINRI)
	add_subdirectory(Zinri Zinri/build)
ENDIF(WITH_ZINRI)

IF (WITH_ASSIMP)
	add_subdirectory(Assimp Assimp/build)
ENDIF (WITH_ASSIMP)

IF (WITH_NUMERICAL)
	add_subdirectory(Numerical Numerical/build)
ENDIF (WITH_NUMERICAL)


IF(WIN32)
	SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE})
	SET(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "" FORCE)
	MARK_AS_ADVANCED(CMAKE_CONFIGURATION_TYPES)
ENDIF(WIN32)
