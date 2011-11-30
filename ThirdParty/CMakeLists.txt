cmake_minimum_required(VERSION 2.6)

project(CGoGN_ThirdParty)


SET(CGoGN_ROOT_DIR ${CMAKE_SOURCE_DIR}/..)
SET(CMAKE_BUILD_TYPE Release)

SET ( BUILD_SHARED_LIBRARIES OFF CACHE BOOL
	"if used all library are build as shared type (.so /dylib/dll)."
)

IF ( BUILD_SHARED_LIBRARIES )
	SET (SHARED_OR_STATIC SHARED)
ENDIF ( BUILD_SHARED_LIBRARIES )


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
add_subdirectory(Numerical Numerical/build)
add_subdirectory(Tools Tools/build)

add_subdirectory(Assimp Assimp/build)


