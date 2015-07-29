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
SET ( CGoGN_DESIRED_QT_VERSION "4" CACHE STRING "4: QT4/5" )
SET ( CGoGN_WITH_GLEWMX OFF CACHE BOOL "use multi-contex GLEW (for VRJuggler)" )
SET ( CGoGN_USE_OGL_CORE_PROFILE OFF CACHE BOOL "use OpenGL 3.3 core profile (do not work on mac)" )

SET ( CGoGN_COMPILE_EXAMPLES OFF CACHE BOOL "compile examples" )
SET ( CGoGN_COMPILE_TUTOS OFF CACHE BOOL "compile tutorials" )
SET ( CGoGN_COMPILE_TESTS OFF CACHE BOOL "compile tests" )
SET ( CGoGN_COMPILE_BENCHES OFF CACHE BOOL "compile benches" )
SET ( CGoGN_COMPILE_SANDBOX OFF CACHE BOOL "compile all in sandbox" )
SET ( CGoGN_ASSERT_ACTIVED OFF CACHE BOOL "assertion activated")
SET ( CGoGN_ONELIB OFF CACHE BOOL "build CGoGN in one lib" )
IF (WIN32)
	SET ( CMAKE_CONFIGURATION_TYPES Release Debug)
	SET ( CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Only Release or Debug" FORCE)
	SET ( CGoGN_WITH_QTCREATOR OFF CACHE BOOL "use QtCreator to compile (Experimental" )
ENDIF()


checkCpp11Support()
checkCpp11Feature("regex" HAS_CPP11_REGEX "False")
setBuildType()

SET(EXECUTABLE_OUTPUT_PATH ${CGoGN_ROOT_DIR}/bin/)
IF(WIN32)
	IF (CGoGN_WITH_QTCREATOR)
		SET(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/bin/${CMAKE_BUILD_TYPE})
	ELSE()
		SET(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/bin/)
	ENDIF()
ELSE()
	SET(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/lib/${CMAKE_BUILD_TYPE})
ENDIF()

#
# External Libraries (not included a third party libraries)
#
IF(WIN32)
	SET ( CMAKE_PREFIX_PATH ${CGoGN_ROOT_DIR}/windows_dependencies CACHE STRING "path to dependencies" )
ENDIF(WIN32)

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(GLEW REQUIRED)
IF (CGoGN_WITH_QT)
	IF(${CGoGN_DESIRED_QT_VERSION} STREQUAL "5")
		include(cmake_modules/Qt_5x.cmake)
	ELSE()
		include(cmake_modules/Qt_4x.cmake)
	ENDIF()
ELSE ()
	FIND_PACKAGE(DevIL REQUIRED)
ENDIF()

IF (WIN32)
	set(SUITESPARSE_SUITESPARSECONFIG_INCLUDE_DIR  ${CGoGN_ROOT_DIR}/windows_dependencies/include)
	set(SUITESPARSE_USE_LAPACK_BLAS ON)
ENDIF()
FIND_PACKAGE(SuiteSparse REQUIRED)

#
#	ThirdParty
#

INCLUDE_DIRECTORIES(${CGoGN_ROOT_DIR}/ThirdParty/include)

ADD_SUBDIRECTORY(ThirdParty/Tools)

ADD_SUBDIRECTORY(ThirdParty/OpenNL)

ADD_SUBDIRECTORY(ThirdParty/tetgen)

IF (CGoGN_WITH_ZINRI)
	ADD_SUBDIRECTORY(ThirdParty/Zinri)
ENDIF()

IF (CGoGN_WITH_ASSIMP)
	ADD_SUBDIRECTORY(ThirdParty/Assimp)
ENDIF ()

ADD_SUBDIRECTORY(ThirdParty/TinyXml2)

#IF (CGoGN_WITH_QT AND (NOT WIN32))
IF (CGoGN_WITH_QT)
	ADD_SUBDIRECTORY(ThirdParty/libQGLViewer/QGLViewer)
ENDIF()

IF (NOT WIN32)
	ADD_SUBDIRECTORY(ThirdParty/libuuid)
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
	IF(APPLE)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wno-overloaded-virtual")
	ENDIF()
ELSE()
	ADD_DEFINITIONS("/wd4251") #dll export warnings
ENDIF()




#
# COMPILE DEFINITIONS
#
LIST(APPEND CGoGN_DEFS -DGLM_FORCE_RADIANS)

LIST(APPEND CGoGN_DEFS -DCGOGN_ASSERT_BOOL=${CGoGN_ASSERT_ACTIVED})

IF(CGoGN_USE_OGL_CORE_PROFILE)
	LIST(APPEND CGoGN_DEFS -DCGOGN_USE_OGL_CORE_PROFILE=1)
ENDIF()

LIST(APPEND CGoGN_DEFS -DSHADERPATH=\"${CGoGN_ROOT_DIR}/lib/Shaders/\")

IF (CGoGN_WITH_ASSIMP)
	LIST(APPEND CGoGN_DEFS -DCGOGN_WITH_ASSIMP)
ENDIF ()

IF (CGoGN_WITH_ZINRI)
	LIST(APPEND CGoGN_DEFS -DCGOGN_WITH_ZINRI)
ENDIF ()

IF (CGoGN_WITH_GLEWMX)
	LIST(APPEND CGoGN_DEFS -DCGOGN_GLEW_MX)
ENDIF ()

IF (CGoGN_WITH_QT)
	LIST(APPEND CGoGN_DEFS -DCGOGN_WITH_QT)
#	LIST(APPEND CGoGN_DEFS("-DCGOGN_QT_DESIRED_VERSION=${CGoGN_DESIRED_QT_VERSION}"))
ENDIF()

IF(WIN32)
	LIST(APPEND CGoGN_DEFS -D_USE_MATH_DEFINES)
ENDIF ()


IF (APPLE)
	LIST(APPEND CGoGN_DEFS -DCGOGN_APPLE)
ENDIF ()

IF (HAS_CPP11_REGEX)
	LIST(APPEND CGoGN_DEFS -DHAS_CPP11_REGEX)
ENDIF ()


ADD_DEFINITIONS(${CGoGN_DEFS})



#
# INCLUDES AND LIBS
#
SET (CGoGN_EXT_INCLUDES)
SET (CGoGN_EXT_LIBS)
IF (CGoGN_WITH_QT)
        LIST(APPEND CGoGN_EXT_INCLUDES ${QT_INCLUDE_DIR})
        LIST(APPEND CGoGN_EXT_LIBS QGLViewer ${QT_LIBRARIES})
		LIST(APPEND CGoGN_EXT_INCLUDES ${CGoGN_ROOT_DIR}/ThirdParty/libQGLViewer/)
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
	${SUITESPARSE_INCLUDE_DIRS}
)

LIST(APPEND CGoGN_EXT_INCLUDES ${CGoGN_ROOT_DIR}/ThirdParty/OpenNL/src)
LIST(APPEND CGoGN_EXT_INCLUDES ${CGoGN_ROOT_DIR}/ThirdParty/tetgen)
LIST(APPEND CGoGN_EXT_INCLUDES ${CGoGN_ROOT_DIR}/ThirdParty/Zinri)
LIST(APPEND CGoGN_EXT_INCLUDES ${CGoGN_ROOT_DIR}/ThirdParty/Assimp/include/)
LIST(APPEND CGoGN_EXT_INCLUDES ${CGoGN_ROOT_DIR}/ThirdParty/TinyXml2)

IF (NOT WIN32)
	LIST(APPEND CGoGN_EXT_INCLUDES ${CGoGN_ROOT_DIR}/ThirdParty/libuuid)
ENDIF()

list(REMOVE_DUPLICATES CGoGN_EXT_INCLUDES)


# define libs for external libs
IF (CGoGN_WITH_ASSIMP)
	LIST(APPEND CGoGN_EXT_LIBS assimp)
	file(WRITE ${CGoGN_SRC_DIR}/include/cgogn_assimp.h "1" )
ELSE ()
	file(WRITE ${CGoGN_SRC_DIR}/include/cgogn_assimp.h "0" )
ENDIF ()

IF (CGoGN_WITH_ZINRI)
	LIST(APPEND CGoGN_EXT_LIBS Zinri)
	file(WRITE ${CGoGN_SRC_DIR}/include/cgogn_zinri.h "1" )
ELSE ()
	file(WRITE ${CGoGN_SRC_DIR}/include/cgogn_zinri.h "0" )
ENDIF ()

LIST(APPEND CGoGN_EXT_LIBS
    nl
    tinyxml2
	${OPENGL_LIBRARY}
	${GLEW_LIBRARIES}
	${ZLIB_LIBRARIES}
	${SUITESPARSE_LIBRARIES}
)

# added for debug linking ??
IF (APPLE)
	FIND_LIBRARY(ACCELERATE_LIBRARY Accelerate)
	LIST(APPEND CGoGN_EXT_LIBS ${ACCELERATE_LIBRARY})
ENDIF()

IF(NOT WIN32)
	LIST(APPEND CGoGN_EXT_LIBS pthread) #for c++11 thread
ENDIF()

IF (NOT HAS_CPP11_REGEX)
       find_package(Boost COMPONENTS regex REQUIRED)
       include_directories( ${Boost_INCLUDE_DIR} )
       list(APPEND CGoGN_EXT_LIBS ${Boost_LIBRARIES})
ENDIF ()


include_directories( 
	${CGoGN_SRC_DIR}/include
	${CGoGN_EXT_INCLUDES} )

#
# save some infos for schnapps and apps
#
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

IF (CGoGN_WITH_QT)
    file(WRITE ${CGoGN_ROOT_DIR}/CGoGN/include/cgogn_qt.h ${CGoGN_DESIRED_QT_VERSION})
ENDIF ()


file(WRITE ${CGoGN_SRC_DIR}/include/cgogn_ext_includes.h "" )
foreach(VAR_INC ${CGoGN_EXT_INCLUDES})
	file(APPEND ${CGoGN_SRC_DIR}/include/cgogn_ext_includes.h ${VAR_INC} \n )
endforeach()

file(WRITE ${CGoGN_SRC_DIR}/include/cgogn_ext_libs.h "" )
foreach(VAR_LIB ${CGoGN_EXT_LIBS})
	file(APPEND ${CGoGN_SRC_DIR}/include/cgogn_ext_libs.h ${VAR_LIB} \n )
endforeach()

file(WRITE ${CGoGN_SRC_DIR}/include/cgogn_defs.h "" )
foreach(VAR_DEF ${CGoGN_DEFS})
	file(APPEND ${CGoGN_SRC_DIR}/include/cgogn_defs.h ${VAR_DEF} \n )
endforeach()


add_subdirectory(${CGoGN_SRC_DIR} CGoGN)

add_subdirectory(${CGoGN_ROOT_DIR}/Apps Apps)


# 
# Add custom clean targets for cleaning without cleaning dependencies
#
if (NOT WIN32)
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
