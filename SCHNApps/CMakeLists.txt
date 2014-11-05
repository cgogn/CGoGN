cmake_minimum_required(VERSION 2.8)

project(SCHNApps)

IF (NOT (${CMAKE_BUILD_TYPE} MATCHES "Debug|Release"))
	IF (${CMAKE_CURRENT_BINARY_DIR} MATCHES "(.*)Debug|(.*)debug|(.*)DEBUG")
		SET(CMAKE_BUILD_TYPE "Debug")
	ELSE()
		SET(CMAKE_BUILD_TYPE "Release")
	ENDIF()
ENDIF()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -std=c++11")

SET(CGoGN_ROOT_DIR ${CMAKE_SOURCE_DIR}/.. CACHE STRING "CGoGN root dir")
SET(CMAKE_MODULE_PATH ${CGoGN_ROOT_DIR}/cmake_modules/)

SET( SLOT_DEBUG_MODE OFF CACHE BOOL "build with slot debugging mode" )


find_package(OpenGL REQUIRED)
find_package(Boost COMPONENTS regex system REQUIRED)
find_package(ZLIB REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(Qt4 REQUIRED)
find_package(QGLViewer REQUIRED)
find_package(PythonLibs 2.7 REQUIRED)
find_package(SuiteSparse REQUIRED)
#find_package(SuperLU REQUIRED)


SET( QT_USE_QTOPENGL TRUE )
SET( QT_USE_QTXML TRUE )
SET( QT_USE_QTDESIGNER TRUE )
SET( QT_USE_QTGUI TRUE )
SET( QT_USE_QTPLUGIN TRUE )
SET( QT_USE_QTSVG TRUE )

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})
add_definitions(-DGLM_FORCE_RADIANS)

#MESSAGE HERE FOR MORE EASY READING (MESSAGE AT THE END)
MESSAGE("Build type = " ${CMAKE_BUILD_TYPE} " (for change -DCMAKE_BUILD_TYPE=Release|Debug)" )

file(STRINGS ${CGoGN_ROOT_DIR}/include/cgogn_onelib.h ONELIB_STR)
IF (ONELIB_STR EQUAL 1)
	SET(CGoGN_LIBS cgogn)
ELSE()
	SET(CGoGN_LIBS topology algo container utils)
ENDIF()

IF(DEFINED ASSERTON)
	add_definitions(-DCGOGN_ASSERT_BOOL=${ASSERTON})
ELSE(DEFINED ASSERTON)
	add_definitions(-DCGOGN_ASSERT_BOOL=false)
ENDIF (DEFINED ASSERTON)

IF(SLOT_DEBUG_MODE)
	add_definitions(-DSLOT_DEBUG_MODE=1)
ENDIF()

# define includes of external libs 
SET (EXT_INCLUDES 
	${CGoGN_ROOT_DIR}/ThirdParty/include
	${OPENGL_INCLUDE_DIR}
	${GLEW_INCLUDE_DIRS}
	${ZLIB_INCLUDE_DIRS}
	${LIBXML2_INCLUDE_DIR}
	${Boost_INCLUDE_DIRS}
	${QT_INCLUDE_DIR}
	${QGLVIEWER_INCLUDE_DIR}
	${PYTHON_INCLUDE_DIRS}
	${SUITESPARSE_INCLUDE_DIRS}
#	${SUPERLU_INCLUDE_DIRS}
)

# define libs for external libs
SET (EXT_LIBS 
	PythonQt
	nl
	${OPENGL_LIBRARY}
	${GLEW_LIBRARIES}
	${ZLIB_LIBRARIES}
	${LIBXML2_LIBRARIES}
	${Boost_SYSTEM_LIBRARY}
	${Boost_REGEX_LIBRARY}
	${QT_LIBRARIES}
	${QGLVIEWER_LIBRARIES}
	${PYTHON_LIBRARIES}
	${SUITESPARSE_LIBRARIES}
#	${SUPERLU_LIBRARIES}
)




# qq definition specifiques pour mac
#IF(APPLE)
# attention a changer pour chercher la bonne version automatiquement
#	SET(CMAKE_OSX_SYSROOT "/Developer/SDKs/MacOSX10.6.sdk" CACHE STRING "developer SDK")
#	SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-undefined -Wl,dynamic_lookup")
#	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMAC_OSX")
#	SET(CMAKE_OSX_ARCHITECTURES x86_64)
#ENDIF(APPLE)

if(APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")	
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
	execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
	if (GCC_VERSION VERSION_GREATER 4.8 OR GCC_VERSION VERSION_EQUAL 4.8)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-local-typedefs")
	endif(GCC_VERSION VERSION_GREATER 4.8 OR GCC_VERSION VERSION_EQUAL 4.8)
endif(CMAKE_COMPILER_IS_GNUCXX)



SET(SCHNApps_ROOT_DIR ${CGoGN_ROOT_DIR}/SCHNApps)
SET(EXECUTABLE_OUTPUT_PATH ${SCHNApps_ROOT_DIR}/bin/${CMAKE_BUILD_TYPE})
SET(LIBRARY_OUTPUT_PATH ${SCHNApps_ROOT_DIR}/lib/${CMAKE_BUILD_TYPE})

# for application auto selection of plugin directory
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/plugins_default_directory.h 
\#define\ PLUGINS_DEFAULT_DIRECTORY\ \"${SCHNApps_ROOT_DIR}/lib/${CMAKE_BUILD_TYPE}\" )

file(
	GLOB_RECURSE
	SCHNApps_FILES
	${SCHNApps_ROOT_DIR}/src/*.cpp
	${SCHNApps_ROOT_DIR}/include/*.h
	${SCHNApps_ROOT_DIR}/include/*.hpp
)

file(
	GLOB_RECURSE
	SCHNApps_UI_FILES
	${SCHNApps_ROOT_DIR}/forms/*.ui
)

SET(SCHNApps_QOBJECT_FILES
	${SCHNApps_ROOT_DIR}/include/camera.h
	${SCHNApps_ROOT_DIR}/include/mapHandler.h
	${SCHNApps_ROOT_DIR}/include/cellSelector.h
	${SCHNApps_ROOT_DIR}/include/plugin.h
	${SCHNApps_ROOT_DIR}/include/plugin_interaction.h
	${SCHNApps_ROOT_DIR}/include/plugin_processing.h
	${SCHNApps_ROOT_DIR}/include/view.h
	${SCHNApps_ROOT_DIR}/include/schnapps.h
	${SCHNApps_ROOT_DIR}/include/viewButtonArea.h
	${SCHNApps_ROOT_DIR}/include/controlDock_cameraTab.h
	${SCHNApps_ROOT_DIR}/include/controlDock_pluginTab.h
	${SCHNApps_ROOT_DIR}/include/controlDock_mapTab.h
	${SCHNApps_ROOT_DIR}/include/colorComboBox.h
	${SCHNApps_ROOT_DIR}/include/dialogList.h
)



link_directories( ${CGoGN_ROOT_DIR}/lib/${CMAKE_BUILD_TYPE} )


INCLUDE_DIRECTORIES(
	${CGoGN_ROOT_DIR}/include
	${SCHNApps_ROOT_DIR}/include
	${EXT_INCLUDES}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

QT4_WRAP_UI( UIC_FILES
	${SCHNApps_UI_FILES}
)

QT4_WRAP_CPP( MOC_FILES
	${SCHNApps_QOBJECT_FILES}
)

QT4_ADD_RESOURCES( RCC_FILES
	${SCHNApps_ROOT_DIR}/resources/resources.qrc
)

ADD_EXECUTABLE( SCHNApps
	${SCHNApps_FILES}
	${UIC_FILES}
	${MOC_FILES}
	${RCC_FILES}
)

TARGET_LINK_LIBRARIES( SCHNApps
	${CGoGN_LIBS}
	${EXT_LIBS}
	pthread
)


ADD_SUBDIRECTORY(${SCHNApps_ROOT_DIR}/Plugins Plugins)

IF(IS_DIRECTORY ${SCHNApps_ROOT_DIR}/../../Plugins)
	ADD_SUBDIRECTORY(${SCHNApps_ROOT_DIR}/../../Plugins ExtPlugins)
ENDIF(IS_DIRECTORY ${SCHNApps_ROOT_DIR}/../../Plugins)
