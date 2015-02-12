cmake_minimum_required(VERSION 2.8)

# Thanks to Etienne Schmitt for CMakeLists modifications

project(CGoGN)

SET ( CGoGN_ROOT_DIR ${CMAKE_SOURCE_DIR} )
SET ( CGoGN_SRC_DIR ${CMAKE_SOURCE_DIR}/CGoGN)
SET ( CMAKE_MODULE_PATH ${CGoGN_ROOT_DIR}/cmake_modules/ )

INCLUDE(${CGoGN_ROOT_DIR}/cmake_modules/functions.cmake)



#
# Available options
#
SET ( CGoGN_WITH_ASSIMP OFF CACHE BOOL "build CGoGN with Assimp" )
SET ( CGoGN_WITH_ZINRI OFF CACHE BOOL "build CGoGN with Zinri lib" )
SET ( CGoGN_WITH_QT ON CACHE BOOL "build CGoGN with Qt lib" )
SET ( CGoGN_DESIRED_QT_VERSION "4" CACHE STRING "4: QT4/ 5 not yet implemented" )
SET ( CGoGN_WITH_PYTHONQT ON CACHE BOOL "build PythonQt (for Schnapps)")
SET ( CGoGN_WITH_GLEWMX OFF CACHE BOOL "use multi-contex GLEW" )
SET ( CGoGN_USE_OGL_CORE_PROFILE OFF CACHE BOOL "use OpenGL 3.3 core profile (do not work on mac)" )

SET ( CGoGN_COMPILE_EXAMPLES OFF CACHE BOOL "compile examples" )
SET ( CGoGN_COMPILE_TUTOS OFF CACHE BOOL "compile tutorials" )
SET ( CGoGN_COMPILE_TESTS OFF CACHE BOOL "compile tests" )
SET ( CGoGN_COMPILE_BENCHES OFF CACHE BOOL "compile benches" )
SET ( CGoGN_COMPILE_SANDBOX OFF CACHE BOOL "compile all in sandbox" )
SET ( CGoGN_ASSERT_ACTIVED OFF CACHE BOOL "assertion activated")
SET ( CGoGN_ONELIB OFF CACHE BOOL "build CGoGN in one lib" )
IF (WIN32)
	SET ( BUILD_SHARED_LIBS OFF CACHE BOOL "if used all library are build as shared type (.so/.dylib/.dll)" )
	SET ( CMAKE_CONFIGURATION_TYPES Release Debug)
	SET ( CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Only Release or Debug" FORCE)
ELSE (WIN32)
	SET ( BUILD_SHARED_LIBS ON CACHE BOOL "if used all library are build as shared type (.so/.dylib/.dll)" )
ENDIF ()


checkCpp11Support()
setBuildType()


SET(EXECUTABLE_OUTPUT_PATH ${CGoGN_ROOT_DIR}/bin/)
IF(WIN32)
	SET(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/lib)
ELSE()
	SET(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/lib/${CMAKE_BUILD_TYPE})
ENDIF()

#
# External Libraries (not included a third party libraries)
#
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(GLEW REQUIRED)
IF (CGoGN_WITH_QT)
	SET(QT_USE_QTCORE TRUE)
	SET(QT_USE_QTOPENGL TRUE)
	SET(QT_USE_QTSVG TRUE)
	SET(QT_USE_QTXML TRUE )
	FIND_PACKAGE(Qt REQUIRED)
ELSE ()
	FIND_PACKAGE(DevIL REQUIRED)
ENDIF()
IF (NOT WIN32)
	FIND_PACKAGE(SuiteSparse REQUIRED)
ENDIF()

#
# COMPILER FLAGS
#
IF(NOT WIN32)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fPIC -std=c++11")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Woverride-init -fPIC")
	# remove the 60000+ "no unused local typedefs" warnings with GCC4.8+
	IF(CMAKE_COMPILER_IS_GNUCXX)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-local-typedefs")
	ENDIF()
ENDIF()

if(APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wno-overloaded-virtual")
endif()


#
# COMPILE DEFINITIONS
#
add_definitions("-DCGOGN_ASSERT_BOOL=${ASSERT_ACTIVED}")

IF(CGoGN_USE_OGL_CORE_PROFILE)
        add_definitions("-DUSE_OGL_CORE_PROFILE=1")
ENDIF()

add_definitions("-DGLM_FORCE_RADIANS")

add_definitions("-DSHADERPATH=\"${CGoGN_ROOT_DIR}/lib/Shaders/\"")

IF (CGoGN_WITH_ASSIMP)
	add_definitions(-DWITH_ASSIMP)	
ENDIF ()

IF (CGoGN_WITH_ZINRI)
	add_definitions(-DWITH_ZINRI)
ENDIF ()

IF (CGoGN_WITH_GLEWMX)
	ADD_DEFINITIONS(-DGLEW_MX)
ENDIF ()


#
#	ThirdParty
#
#add_subdirectory(ThirdParty)

INCLUDE_DIRECTORIES(${CGoGN_ROOT_DIR}/ThirdParty/include)

ADD_SUBDIRECTORY(ThirdParty/Tools)

IF (CGoGN_WITH_PYTHONQT)
	ADD_SUBDIRECTORY(ThirdParty/PythonQt)
ENDIF ()

ADD_SUBDIRECTORY(ThirdParty/OpenNL)
INCLUDE_DIRECTORIES(${CGoGN_ROOT_DIR}/ThirdParty/OpenNL/src)

ADD_SUBDIRECTORY(ThirdParty/tetgen)
INCLUDE_DIRECTORIES(${CGoGN_ROOT_DIR}/ThirdParty/tetgen)

IF (CGoGN_WITH_ZINRI)
	ADD_SUBDIRECTORY(ThirdParty/Zinri)
	INCLUDE_DIRECTORIES(${CGoGN_ROOT_DIR}/ThirdParty/Zinri)
ENDIF()

IF (CGoGN_WITH_ASSIMP)
	ADD_SUBDIRECTORY(ThirdParty/Assimp)
	INCLUDE_DIRECTORIES(${CGoGN_ROOT_DIR}/ThirdParty/Assimp/include/)
ENDIF ()

ADD_SUBDIRECTORY(ThirdParty/TinyXml2)
INCLUDE_DIRECTORIES(${CGoGN_ROOT_DIR}/ThirdParty/TinyXml2)

IF (CGoGN_WITH_QT)
	ADD_SUBDIRECTORY(ThirdParty/libQGLViewer/QGLViewer)
	INCLUDE_DIRECTORIES(${CGoGN_ROOT_DIR}/ThirdParty/libQGLViewer/)
ENDIF()

IF (NOT WIN32)
	ADD_SUBDIRECTORY(ThirdParty/libuuid)
	INCLUDE_DIRECTORIES(${CGoGN_ROOT_DIR}/ThirdParty/libuuid)
ENDIF()



IF (CGoGN_ONELIB)
	file(WRITE ${CGoGN_SRC_DIR}/include/cgogn_onelib.h "1" )
ELSE ()
	file(WRITE ${CGoGN_SRC_DIR}/include/cgogn_onelib.h "0" )
ENDIF ()

IF (CGoGN_WITH_GLEWMX)
	file(WRITE ${CGoGN_SRC_DIR}/include/cgogn_gmx.h "1" )
ELSE ()
	file(WRITE ${CGoGN_SRC_DIR}/include/cgogn_gmx.h "0" )
ENDIF ()


#
# INCLUDES AND LIBS
#
SET (CGoGN_EXT_INCLUDES)
SET (CGoGN_EXT_LIBS)
IF (CGoGN_WITH_QT)
		INCLUDE(${QT_USE_FILE})
        ADD_DEFINITIONS(-DWITH_QT)
        ADD_DEFINITIONS(${QT_DEFINITIONS})
        LIST(APPEND CGoGN_EXT_INCLUDES ${QT_INCLUDE_DIR})
        LIST(APPEND CGoGN_EXT_LIBS QGLViewer ${QT_LIBRARIES})
ELSE ()
	 LIST(APPEND CGoGN_EXT_INCLUDES ${IL_INCLUDE_DIR})
	 LIST(APPEND CGoGN_EXT_LIBS ${IL_LIBRARIES})
ENDIF ()

# define includes of external libs 
LIST(INSERT CGoGN_EXT_INCLUDES 0
    ${CGoGN_ROOT_DIR}/ThirdParty/include
	${OPENGL_INCLUDE_DIR}
	${GLEW_INCLUDE_DIRS}
	${ZLIB_INCLUDE_DIRS}
)

# define libs for external libs

IF (CGoGN_WITH_ASSIMP)
	LIST(APPEND CGoGN_EXT_LIBS assimp)
ENDIF ()

IF (CGoGN_WITH_ZINRI)
	LIST(APPEND CGoGN_EXT_LIBS Zinri)
ENDIF ()

LIST(APPEND CGoGN_EXT_LIBS
    nl
    tinyxml2
	${OPENGL_LIBRARY}
	${GLEW_LIBRARIES}
	${ZLIB_LIBRARIES}
)

# W32 specific libraries and includes
IF(WIN32)
	INCLUDE_DIRECTORIES(${CGoGN_ROOT_DIR}/windows_dependencies/include/)
	INCLUDE_DIRECTORIES(${CGoGN_ROOT_DIR}/windows_dependencies/include/suitesparse)
	LIST(APPEND CGoGN_EXT_INCLUDES  ${CGoGN_ROOT_DIR}/windows_dependencies/include/suitesparse)
	LIST(APPEND CGoGN_EXT_LIBS 
		${CGoGN_ROOT_DIR}/windows_dependencies/lib/suitesparse.lib
		${CGoGN_ROOT_DIR}/windows_dependencies/lib/metis.lib
		${CGoGN_ROOT_DIR}/windows_dependencies/lib/liblapack.lib
		${CGoGN_ROOT_DIR}/windows_dependencies/lib/libblas.lib		)
ELSE(WIN32)
	LIST(APPEND CGoGN_EXT_INCLUDES  ${SUITESPARSE_INCLUDE_DIRS})
	LIST(APPEND CGoGN_EXT_LIBS ${SUITESPARSE_LIBRARIES} pthread)
ENDIF(WIN32)


#LIST(REMOVE_DUPLICATES CGoGN_EXT_INCLUDES)

include_directories( 
	${CGoGN_SRC_DIR}/include
	${CGoGN_EXT_INCLUDES} )


add_subdirectory(${CGoGN_SRC_DIR} CGoGN)

add_subdirectory(${CGoGN_ROOT_DIR}/Apps Apps)


# 
# Add custom clean targets for cleaning without cleaning dependencies
#
if (WIN32)
	add_custom_target(clean_cgogn 
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	add_custom_command(TARGET clean_cgogn PRE_BUILD 
			COMMAND del /q CCoGN
			COMMAND cmake ..
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMENT "cleaning CGoGN libs")

	add_custom_target(clean_apps 
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	add_custom_command(TARGET clean_apps PRE_BUILD 
			COMMAND del /q Apps
			COMMAND cmake ..
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMENT "cleaning Apps")

	add_custom_target(clean_cgogn_apps 
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	add_custom_command(TARGET clean_cgogn_apps PRE_BUILD 
			COMMAND del /q CGoGN
			COMMAND del /q Apps
			COMMAND cmake ..
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMENT "cleaning Apps")

ELSE()

	add_custom_target(clean_cgogn 
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	add_custom_command(TARGET clean_cgogn PRE_BUILD 
			COMMAND rm -rf CGoGN
			COMMAND cmake ..
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMENT "cleaning CGoGN libs")

	add_custom_target(clean_apps 
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	add_custom_command(TARGET clean_apps PRE_BUILD 
			COMMAND rm -rf Apps
			COMMAND cmake ..
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMENT "cleaning Apps")

	add_custom_target(clean_cgogn_apps 
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	add_custom_command(TARGET clean_cgogn_apps PRE_BUILD 
			COMMAND rm -rf CGoGN
			COMMAND rm -rf Apps
			COMMAND cmake ..
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMENT "cleaning Apps")

ENDIF()