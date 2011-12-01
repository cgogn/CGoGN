cmake_minimum_required(VERSION 2.6)

project(CGoGN_ThirdParty)


SET(CGoGN_ROOT_DIR ${CMAKE_SOURCE_DIR}/..)
SET(CMAKE_BUILD_TYPE Release)

# for shared or not shared
SET (BUILD_SHARED_LIBS OFF CACHE BOOL 
	"if used all library are build as shared type (.so /dylib/dll).")

SET (WITH_ASSIMP ON CACHE BOOL "build CGoGN with Assimp")
SET (WITH_NUMERICAL ON CACHE BOOL "build CGoGN with numerical libs")


IF (APPLE)
	SET(CMAKE_OSX_ARCHITECTURES x86_64)
ENDIF(APPLE)

IF(WIN32)
	SET(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/lib)#release added by visual
	INCLUDE_DIRECTORIES(${CGoGN_ROOT_DIR}/windows_dependencies/include/)
ELSE(WIN32)
	SET(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/lib/Release)
ENDIF(WIN32)

add_subdirectory(Zinri Zinri/build)
add_subdirectory(gzstream gzstream/build)
add_subdirectory(Tools Tools/build)

IF (WITH_ASSIMP)
	add_subdirectory(Numerical Numerical/build)
ENDIF (WITH_ASSIMP)

IF (WITH_NUMERICAL)
	add_subdirectory(Assimp Assimp/build)
ENDIF (WITH_NUMERICAL)

