cmake_minimum_required(VERSION 2.6)

project(AntTweakBar)

set(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/lib/Release)

# specific mac
IF(APPLE)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -fno-strict-aliasing -D_MACOSX -ObjC++ -D__PLACEMENT_NEW_INLINE")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall -fno-strict-aliasing -D_MACOSX")
	set(PATHSDK "/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks") 
	include_directories(include/
		${PATHSDK}/OpenGL.framework/Headers/
		${PATHSDK}/GLUT.framework/Headers/
		${PATHSDK}/AppKit.framework/Headers/ )
ELSE(APPLE)
	IF(UNIX)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -fno-strict-aliasing -D_UNIX -D__PLACEMENT_NEW_INLINE ")
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall -fno-strict-aliasing -D_UNIX")
		include_directories(include/)
	ENDIF(UNIX)
ENDIF(APPLE)

file(
	GLOB_RECURSE
	source_files
	TwColors.cpp TwFonts.cpp TwOpenGL.cpp TwBar.cpp TwMgr.cpp TwPrecomp.cpp LoadOGL.cpp
	TwEventGLFW.c TwEventGLUT.c TwEventSDL.c)

add_library(
	AntTweakBar
	${source_files}
)
