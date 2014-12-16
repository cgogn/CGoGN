cmake_minimum_required(VERSION 2.8)

project(CGoGN)

#SET ( CMAKE_VERBOSE_MAKEFILE 1 )

SET ( CGoGN_ROOT_DIR ${CMAKE_SOURCE_DIR} )

SET ( CMAKE_MODULE_PATH ${CGoGN_ROOT_DIR}/cmake_modules/ )



IF (NOT (${CMAKE_BUILD_TYPE} MATCHES "Debug|Release"))
	IF (${CMAKE_CURRENT_BINARY_DIR} MATCHES "(.*)Debug|(.*)debug|(.*)DEBUG")
		SET(CMAKE_BUILD_TYPE "Debug")
	ELSE()
		SET(CMAKE_BUILD_TYPE "Release")
	ENDIF()
ENDIF()

# for shared or not shared
SET ( BUILD_SHARED_LIBS OFF CACHE BOOL "if used all library are build as shared type (.so/.dylib/.dll)" )

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
find_package(Boost COMPONENTS regex REQUIRED)
find_package(ZLIB REQUIRED)
#find_package(LibXml2 REQUIRED)
find_package(TinyXml2 REQUIRED)
find_package(GLEW REQUIRED)

# MESSAGE HERE FOR MORE EASY USER READING
MESSAGE("Building type = " ${CMAKE_BUILD_TYPE} " (for change with cmake use -DCMAKE_BUILD_TYPE=Release|Debug)" )


IF (DEFINED ASSERTON)
	add_definitions(-DCGOGN_ASSERT_BOOL=${ASSERTON})
ELSE (DEFINED ASSERTON)
	add_definitions(-DCGOGN_ASSERT_BOOL=false)
ENDIF (DEFINED ASSERTON)

IF (ONELIB)
	file(WRITE ${CGoGN_ROOT_DIR}/include/cgogn_onelib.h "1" )
ELSE (ONELIB)
	file(WRITE ${CGoGN_ROOT_DIR}/include/cgogn_onelib.h "0" )
ENDIF (ONELIB)

IF (WITH_GLEWMX)
	file(WRITE ${CGoGN_ROOT_DIR}/include/cgogn_gmx.h "1" )
ELSE (WITH_GLEWMX)
	file(WRITE ${CGoGN_ROOT_DIR}/include/cgogn_gmx.h "0" )
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
	${TINYXML2_INCLUDE_DIR}
	${Boost_INCLUDE_DIRS}
)

# define libs for external libs
SET (CGoGN_EXT_LIBS
    nl
	${OPENGL_LIBRARY}
	${GLEW_LIBRARIES}
	${ZLIB_LIBRARIES}
	${TINYXML2_LIBRARY}
	${Boost_REGEX_LIBRARY}
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
		execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
		if (GCC_VERSION VERSION_GREATER 4.8 OR GCC_VERSION VERSION_EQUAL 4.8)
			SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
			SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-local-typedefs")
		endif(GCC_VERSION VERSION_GREATER 4.8 OR GCC_VERSION VERSION_EQUAL 4.8)
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
	${CGoGN_ROOT_DIR}/include
	${CGoGN_EXT_INCLUDES}
)

file(
	GLOB_RECURSE
	files_topology
	${CGoGN_ROOT_DIR}/src/Topology/*.cpp
	${CGoGN_ROOT_DIR}/include/Topology/*.hpp
	${CGoGN_ROOT_DIR}/include/Topology/*.h
)

file(
	GLOB_RECURSE
	files_container
	${CGoGN_ROOT_DIR}/src/Container/*.cpp
	${CGoGN_ROOT_DIR}/include/Container/*.hpp
	${CGoGN_ROOT_DIR}/include/Container/*.h
)

file(
	GLOB_RECURSE
	files_algo
	${CGoGN_ROOT_DIR}/src/Algo/*.cpp
	${CGoGN_ROOT_DIR}/src/Algo/*.c
	${CGoGN_ROOT_DIR}/include/Algo/*.hpp
	${CGoGN_ROOT_DIR}/include/Algo/*.h
)

file(
	GLOB
	shaders_src
	${CGoGN_ROOT_DIR}/include/Utils/*.frag
	${CGoGN_ROOT_DIR}/include/Utils/*.vert
	${CGoGN_ROOT_DIR}/include/Utils/*.geom	
	${CGoGN_ROOT_DIR}/include/Utils/Shaders/*.frag
	${CGoGN_ROOT_DIR}/include/Utils/Shaders/*.vert
	${CGoGN_ROOT_DIR}/include/Utils/Shaders/*.geom
)


IF(WIN32)
	add_custom_target(shader_target ${CGoGN_ROOT_DIR}/ThirdParty/bin/Release/shader_to_h ${shaders_src}
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	SOURCES ${shaders_src} )
ELSE()
	add_custom_target(shader_target ${CGoGN_ROOT_DIR}/ThirdParty/bin/shader_to_h ${shaders_src}
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	SOURCES ${shaders_src} )
ENDIF()


file(
	GLOB		# WARNING NO MORE RECURSE TO AVOID TAKING QT FILES
	files_utils
	${CGoGN_ROOT_DIR}/src/Utils/*.cpp
	${CGoGN_ROOT_DIR}/include/Utils/*.hpp
	${CGoGN_ROOT_DIR}/include/Utils/*.h

	${CGoGN_ROOT_DIR}/src/Utils/Shaders/*.cpp
	${CGoGN_ROOT_DIR}/include/Utils/Shaders/*.hpp
	${CGoGN_ROOT_DIR}/include/Utils/Shaders/*.h
)

IF(WITH_QT)
	file(
		GLOB
		files_utils_qt
		${CGoGN_ROOT_DIR}/src/Utils/Qt/*.cpp
		${CGoGN_ROOT_DIR}/include/Utils/Qt/*.hpp
		${CGoGN_ROOT_DIR}/include/Utils/Qt/*.h
	)
	file(
		GLOB
		utils_qt_headers
		${CGoGN_ROOT_DIR}/include/Utils/Qt/qtgl.h
		${CGoGN_ROOT_DIR}/include/Utils/Qt/qtSimple.h
		${CGoGN_ROOT_DIR}/include/Utils/Qt/qtQGLV_glw.h
		${CGoGN_ROOT_DIR}/include/Utils/Qt/qtQGLV.h
		${CGoGN_ROOT_DIR}/include/Utils/Qt/qtpopup.h
		${CGoGN_ROOT_DIR}/include/Utils/Qt/qthistodraw.h 
		${CGoGN_ROOT_DIR}/include/Utils/Qt/qtcolorschooser.h
	)	
		
	QT4_WRAP_CPP(UTILS_QT_HEADERS_MOC ${utils_qt_headers})
	SET (files_utils_withQt ${files_utils} ${files_utils_qt} ${UTILS_QT_HEADERS_MOC})
ENDIF()

file(	GLOB_RECURSE
		files_geometry
		${CGoGN_ROOT_DIR}/include/Geometry/*.h
		${CGoGN_ROOT_DIR}/include/Geometry/*.hpp
)

file(	GLOB_RECURSE
		files_thirdParty
		${CGoGN_ROOT_DIR}/ThirdParty/include/*.h
		${CGoGN_ROOT_DIR}/ThirdParty/include/*.hpp
)

link_directories( ${CGoGN_ROOT_DIR}/lib/${CMAKE_BUILD_TYPE} )

IF (ONELIB)
	IF(WITH_QT)
		add_library( cgogn ${files_topology} ${files_container} ${files_algo} ${files_utils_withQt}) 
	ELSE()
		add_library( cgogn ${files_topology} ${files_container} ${files_algo} ${files_utils} )
	ENDIF()
	add_dependencies(cgogn shader_target)
	target_link_libraries(cgogn ${CGoGN_EXT_LIBS})

ELSE()
	IF (WITH_QT)
		add_library( utils ${files_utils_withQt} )
	ELSE()
		add_library( utils ${files_utils} )
	ENDIF()
	target_link_libraries(utils ${CGoGN_EXT_LIBS})

	add_library( container	${files_container})
	target_link_libraries(container utils)

	add_library( topology	${files_topology})
	target_link_libraries(topology container)

	add_library( algo		${files_algo})
	target_link_libraries(algo utils topology)


	add_custom_target( Geometry 	SOURCES ${files_geometry} )
	add_custom_target( ThirdParty 	SOURCES ${files_thirdParty} )
	add_dependencies( utils shader_target )
ENDIF()


add_subdirectory(${CGoGN_ROOT_DIR}/Apps Apps)

