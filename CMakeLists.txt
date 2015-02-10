cmake_minimum_required(VERSION 2.8)

project(CGoGN)
#cmake_policy(SET CMP0042 NEW) # rpath new policy

#SET ( CMAKE_VERBOSE_MAKEFILE 1 )

SET ( CGoGN_ROOT_DIR ${CMAKE_SOURCE_DIR} )

SET ( CGoGN_SRC_DIR ${CMAKE_SOURCE_DIR}/CGoGN)

SET ( CMAKE_MODULE_PATH ${CGoGN_ROOT_DIR}/cmake_modules/ )


# ====== Check GCC VERSION FOR C++11 ======
if(CMAKE_COMPILER_IS_GNUCXX)
	execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
	if (GCC_VERSION VERSION_LESS 4.8)
		message(FATAL_ERROR "Full support of C++11 needed. Therefore a gcc/g++ compiler with a version at least 4.8 is needed.")
	endif()
endif()

# ====== Check CLANG VERSION FOR C++11 ======
if (APPLE)
	execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE CLANG_VERSION)
	if (CLANG_VERSION VERSION_LESS 3.1)
		message(FATAL_ERROR "Full support of C++11 needed. Therefore a clang compiler with a version at least 3.1 is needed !!")
	endif()
endif()

# ====== Check MSVC VERSION FOR C++11 ======
if(WIN32)
	if(MSVC_VERSION LESS 1800)
		message(FATAL_ERROR "Full support of C++11 needed. Therefore a microsoft compiler with a version at least 12 is needed.")
	endif()
endif()

# ======== Check  Build Type =============
IF (NOT (${CMAKE_BUILD_TYPE} MATCHES "Debug|Release"))
	IF (${CMAKE_CURRENT_BINARY_DIR} MATCHES "(.*)Debug|(.*)debug|(.*)DEBUG")
		SET(CMAKE_BUILD_TYPE "Debug")
	ELSE()
		SET(CMAKE_BUILD_TYPE "Release")
	ENDIF()
ENDIF()


# for shared or not shared (default except on windows)

if(WIN32)
	SET ( BUILD_SHARED_LIBS OFF CACHE BOOL "if used all library are build as shared type (.so/.dylib/.dll)" )
else()
	SET ( BUILD_SHARED_LIBS ON CACHE BOOL "if used all library are build as shared type (.so/.dylib/.dll)" )
endif()



#IF(EXISTS "${CGoGN_ROOT_DIR}/ThirdParty/include/Assimp")
IF(EXISTS "${CGoGN_ROOT_DIR}/lib/${CMAKE_BUILD_TYPE}/*assimp*")
	SET ( WITH_ASSIMP ON CACHE BOOL "build CGoGN with Assimp" )
ELSE()
	SET ( WITH_ASSIMP OFF CACHE BOOL "build CGoGN with Assimp" )
ENDIF()

IF(EXISTS "${CGoGN_ROOT_DIR}/ThirdParty/include/Zinri")
	SET ( WITH_ZINRI ON CACHE BOOL "build CGoGN with Zinri lib" )
ELSE()
	SET ( WITH_ZINRI OFF CACHE BOOL "build CGoGN with Zinri lib" )
ENDIF()

SET ( WITH_QT ON CACHE BOOL "build CGoGN with Qt lib" )
SET ( DESIRED_QT_VERSION "4" CACHE STRING "4: QT4/ 5 not yet implemented" )

SET ( COMPILE_EXAMPLES ON CACHE BOOL "compile examples" )
SET ( COMPILE_TUTOS ON CACHE BOOL "compile tutorials" )
SET ( COMPILE_TESTS ON CACHE BOOL "compile tests" )
SET ( COMPILE_BENCHES ON CACHE BOOL "compile benches" )
SET ( COMPILE_SANDBOX ON CACHE BOOL "compile all in sandbox" )


#create one big lib 
SET ( ONELIB OFF CACHE BOOL "build CGoGN in one lib" )


SET ( WITH_GLEWMX OFF CACHE BOOL "use multi-contex GLEW" )
SET( USE_OGL_CORE_PROFILE OFF CACHE BOOL "use OpenGL 3.3 core profile (do not work on mac)" )


IF(WIN32)
	SET ( CMAKE_PREFIX_PATH ${CGoGN_ROOT_DIR}/windows_dependencies CACHE STRING "path to dependencies" )
ENDIF(WIN32)

find_package(OpenGL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(GLEW REQUIRED)

# MESSAGE HERE FOR MORE EASY USER READING
MESSAGE("Building type = " ${CMAKE_BUILD_TYPE} " (for change with cmake use -DCMAKE_BUILD_TYPE=Release|Debug)" )


IF (DEFINED ASSERTON)
	add_definitions(-DCGOGN_ASSERT_BOOL=${ASSERTON})
ELSE (DEFINED ASSERTON)
	add_definitions(-DCGOGN_ASSERT_BOOL=false)
ENDIF (DEFINED ASSERTON)

IF (ONELIB)
	file(WRITE ${CGoGN_SRC_DIR}/include/cgogn_onelib.h "1" )
ELSE (ONELIB)
	file(WRITE ${CGoGN_SRC_DIR}/include/cgogn_onelib.h "0" )
ENDIF (ONELIB)

IF (WITH_GLEWMX)
	file(WRITE ${CGoGN_SRC_DIR}/include/cgogn_gmx.h "1" )
ELSE (WITH_GLEWMX)
	file(WRITE ${CGoGN_SRC_DIR}/include/cgogn_gmx.h "0" )
ENDIF (WITH_GLEWMX)


IF(USE_OGL_CORE_PROFILE)
	add_definitions(-DUSE_OGL_CORE_PROFILE=1)
ENDIF()

add_definitions(-DGLM_FORCE_RADIANS)

add_definitions(-DSHADERPATH="${CGoGN_ROOT_DIR}/lib/Shaders/")

# define includes of external libs 
SET (CGoGN_EXT_INCLUDES
	${CGoGN_ROOT_DIR}/ThirdParty/include
	${OPENGL_INCLUDE_DIR}
	${GLEW_INCLUDE_DIRS}
	${ZLIB_INCLUDE_DIRS}
)

# define libs for external libs
SET (CGoGN_EXT_LIBS
    	nl
	${OPENGL_LIBRARY}
	${GLEW_LIBRARIES}
	${ZLIB_LIBRARIES}
	tinyxml2
)

IF(WIN32)
	SET (CGoGN_EXT_INCLUDES ${CGoGN_EXT_INCLUDES} ${CGoGN_ROOT_DIR}/windows_dependencies/include/suitesparse)
	SET (CGoGN_EXT_LIBS ${CGoGN_EXT_LIBS} 
		${CGoGN_ROOT_DIR}/windows_dependencies/lib/suitesparse.lib
		${CGoGN_ROOT_DIR}/windows_dependencies/lib/metis.lib
		${CGoGN_ROOT_DIR}/windows_dependencies/lib/liblapack.lib
		${CGoGN_ROOT_DIR}/windows_dependencies/lib/libblas.lib		)
ELSE(WIN32)
	find_package(SuiteSparse REQUIRED)
	SET (CGoGN_EXT_INCLUDES ${CGoGN_EXT_INCLUDES} ${SUITESPARSE_INCLUDE_DIRS})
	SET (CGoGN_EXT_LIBS ${CGoGN_EXT_LIBS} ${SUITESPARSE_LIBRARIES} pthread)
ENDIF(WIN32)


#optionnal libs
IF (WITH_ASSIMP)
	add_definitions(-DWITH_ASSIMP)	
	SET (CGoGN_EXT_LIBS ${CGoGN_EXT_LIBS} assimp)
#	INSTALL (DIRECTORY ThirdParty/Assimp/include  DESTINATION .)
ENDIF (WITH_ASSIMP)

IF (WITH_ZINRI)
	add_definitions(-DWITH_ZINRI)
	SET (CGoGN_EXT_LIBS ${CGoGN_EXT_LIBS} Zinri)
#	INSTALL (DIRECTORY ThirdParty/Zinri/ DESTINATION include FILES_MATCHING PATTERN "*.h")
ENDIF (WITH_ZINRI)

IF (WITH_QT)
	add_definitions(-DWITH_QT)
	find_package(Qt REQUIRED)
	SET(QT_USE_QTCORE TRUE)
	SET(QT_USE_QTOPENGL TRUE)
	SET(QT_USE_QTSVG TRUE)
	SET(QT_USE_QTXML TRUE )
	INCLUDE(${QT_USE_FILE})
	ADD_DEFINITIONS(${QT_DEFINITIONS})
	find_package(QGLViewer REQUIRED)
	SET (CGoGN_EXT_INCLUDES ${CGoGN_EXT_INCLUDES} ${QT_INCLUDE_DIR} ${QGLVIEWER_INCLUDE_DIR} )
	SET (CGoGN_EXT_LIBS ${QGLVIEWER_LIBRARIES} ${QT_LIBRARIES} ${CGoGN_EXT_LIBS})
ENDIF (WITH_QT)

IF (WITH_GLEWMX)
	ADD_DEFINITIONS(-DGLEW_MX)
ENDIF (WITH_GLEWMX)

# qq definition specifiques pour mac
#IF(APPLE)
#	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMAC_OSX")	
#	SET(CMAKE_OSX_ARCHITECTURES x86_64)
#	SET (CGoGN_EXT_LIBS ${CGoGN_EXT_LIBS} "-framework Foundation -framework cocoa" )
#ENDIF(APPLE)

IF(WIN32)
	set(CMAKE_CONFIGURATION_TYPES Release Debug)
	set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Only Release or Debug" FORCE)
ELSE()
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fPIC -std=c++11")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Woverride-init -fPIC")
	# remove the 60000+ "no unused local typedefs" warnings with GCC4.8+
	if(CMAKE_COMPILER_IS_GNUCXX)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-local-typedefs")
	endif()
ENDIF()

if(APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wno-overloaded-virtual")
endif()


IF(WIN32)
	SET(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/lib)
ELSE()
	SET(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/lib/${CMAKE_BUILD_TYPE})
ENDIF()

include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
	${CGoGN_SRC_DIR}/include
	${CGoGN_EXT_INCLUDES}
)

add_subdirectory(${CGoGN_SRC_DIR} CGoGN)



add_subdirectory(${CGoGN_ROOT_DIR}/Apps Apps)

