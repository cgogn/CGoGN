cmake_minimum_required(VERSION 2.8)

project(SCHNApps)

SET( CGoGN_SLOT_DEBUG_MODE OFF CACHE BOOL "build with slot debugging mode" )
SET( CGoGN_SCHNApps_USE_DOUBLE OFF CACHE BOOL "build with doubled instead of floats" )
IF(WIN32)
	SET ( CGoGN_WITH_QTCREATOR OFF CACHE BOOL "use QtCreator to compile" )
ENDIF(WIN32)

SET(CGoGN_ROOT_DIR ${CMAKE_SOURCE_DIR}/.. CACHE STRING "CGoGN root dir")
IF(WIN32)
	SET ( CMAKE_PREFIX_PATH ${CGoGN_ROOT_DIR}/windows_dependencies CACHE STRING "path to dependencies" )
ENDIF(WIN32)

SET(CMAKE_MODULE_PATH ${CGoGN_ROOT_DIR}/cmake_modules/)
SET (CGoGN_SRC_DIR ${CGoGN_ROOT_DIR}/CGoGN)

INCLUDE(${CGoGN_ROOT_DIR}/cmake_modules/functions.cmake)

setBuildType()

SET(SCHNApps_ROOT_DIR ${CGoGN_ROOT_DIR}/SCHNApps)
IF(WIN32)
	IF (CGoGN_WITH_QTCREATOR)
		SET(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/bin/${CMAKE_BUILD_TYPE})
		SET(EXECUTABLE_OUTPUT_PATH ${CGoGN_ROOT_DIR}/bin/${CMAKE_BUILD_TYPE})
		link_directories( ${CGoGN_ROOT_DIR}/bin/${CMAKE_BUILD_TYPE} )
	ELSE()
		SET(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/bin)
		SET(EXECUTABLE_OUTPUT_PATH ${CGoGN_ROOT_DIR}/bin)
		link_directories( ${CGoGN_ROOT_DIR}/bin/${ConfigurationName} )
	ENDIF()
ELSE()
	SET(EXECUTABLE_OUTPUT_PATH ${SCHNApps_ROOT_DIR}/bin/${CMAKE_BUILD_TYPE})
#	SET(LIBRARY_OUTPUT_PATH ${SCHNApps_ROOT_DIR}/lib/${CMAKE_BUILD_TYPE})
	link_directories( ${CGoGN_ROOT_DIR}/lib/${CMAKE_BUILD_TYPE} )
ENDIF()


# choose same Qt version than in CGoGN
file(READ ${CGoGN_SRC_DIR}/include/cgogn_qt.h CGoGN_DESIRED_QT_VERSION)
ADD_DEFINITIONS("-DCGOGN_QT_DESIRED_VERSION=${CGoGN_DESIRED_QT_VERSION}")
IF(CGoGN_DESIRED_QT_VERSION STREQUAL 5)
	include(${CGoGN_ROOT_DIR}/cmake_modules/Qt_5x.cmake)
ELSE()
	include(${CGoGN_ROOT_DIR}/cmake_modules/Qt_4x.cmake)
ENDIF()

# PythonQt third-party dependency
ADD_SUBDIRECTORY(${CGoGN_ROOT_DIR}/ThirdParty/PythonQt ThirdParty_PythonQt)


find_package(OpenGL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(GLEW REQUIRED)
find_package(PythonLibs 2.7 REQUIRED)



# CGoGN compiled in one or several libs
file(READ ${CGoGN_SRC_DIR}/include/cgogn_onelib.h ONELIB_STR)
IF (ONELIB_STR EQUAL 1)
	SET(CGoGN_LIBS cgogn)
ELSE()
	SET(CGoGN_LIBS topology algo container utils)
ENDIF()

# definitions from CGoGN
FILE(STRINGS ${CGoGN_SRC_DIR}/include/cgogn_defs.h CGoGN_DEFS)

#and schnapps options
IF(CGoGN_SLOT_DEBUG_MODE)
	LIST (APPEND CGoGN_DEFS -DCGoGN_SLOT_DEBUG_MODE=1)
ENDIF()
IF (${CGoGN_SCHNApps_USE_DOUBLE})
	LIST (APPEND CGoGN_DEFS -DSCHNAPPS_USE_DOUBLE=1)
ENDIF()
ADD_DEFINITIONS(${CGoGN_DEFS})

# includes from CGoGN + python
FILE(STRINGS ${CGoGN_SRC_DIR}/include/cgogn_ext_includes.h EXT_INCLUDES)
LIST(APPEND EXT_INCLUDES ${PYTHON_INCLUDE_DIRS})

# libs from CGoGN + python + PythonQt
FILE(STRINGS ${CGoGN_SRC_DIR}/include/cgogn_ext_libs.h EXT_LIBS)
LIST(APPEND EXT_LIBS PythonQt ${PYTHON_LIBRARIES})




if(CMAKE_COMPILER_IS_GNUCXX)
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
endif()

if(WIN32)
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
else()
	if(APPLE)
		SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -std=c++11 -Wno-deprecated -Wno-overloaded-virtual")
	else()
		SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -std=c++11")
	endif()
endif()



INCLUDE_DIRECTORIES(
	${CGoGN_SRC_DIR}/include
	${CGoGN_ROOT_DIR}/ThirdParty/PythonQt/src/
	${SCHNApps_ROOT_DIR}/include
	${EXT_INCLUDES}
	${CMAKE_CURRENT_BINARY_DIR}/SCHNAppsLib
)

ADD_SUBDIRECTORY(${SCHNApps_ROOT_DIR}/Plugins/schnapps/ SCHNAppsLib)

ADD_SUBDIRECTORY(${SCHNApps_ROOT_DIR}/Plugins Plugins)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
)


# SCHNApps main application
file(
	GLOB_RECURSE
	SCHNApps_FILES
	${SCHNApps_ROOT_DIR}/src/main.cpp
)

ADD_EXECUTABLE( SCHNApps
	${SCHNApps_FILES}
)

TARGET_LINK_LIBRARIES( SCHNApps
	SCHNAppsLib
	${CGoGN_LIBS}
	${EXT_LIBS}
)

qt_use_modules(SCHNApps Core Gui OpenGL Xml Svg)

IF(IS_DIRECTORY ${SCHNApps_ROOT_DIR}/../../Plugins)
	ADD_SUBDIRECTORY(${SCHNApps_ROOT_DIR}/../../Plugins ExtPlugins)
ENDIF(IS_DIRECTORY ${SCHNApps_ROOT_DIR}/../../Plugins)
