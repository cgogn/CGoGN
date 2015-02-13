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
SET (CGoGN_SRC_DIR ${CGoGN_ROOT_DIR}/CGoGN)

SET( CGoGN_SLOT_DEBUG_MODE OFF CACHE BOOL "build with slot debugging mode" )
SET( CGoGN_ASSERT_ACTIVED OFF CACHE BOOL "assertion activated")


find_package(OpenGL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(GLEW REQUIRED)
find_package(Qt4 REQUIRED)
#find_package(QGLViewer REQUIRED)
find_package(PythonLibs 2.7 REQUIRED)
find_package(SuiteSparse REQUIRED)
#find_package(SuperLU REQUIRED)


add_definitions(-DWITH_QT)

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

file(STRINGS ${CGoGN_SRC_DIR}/include/cgogn_onelib.h ONELIB_STR)
IF (ONELIB_STR EQUAL 1)
	SET(CGoGN_LIBS cgogn)
ELSE()
	SET(CGoGN_LIBS topology algo container utils)
ENDIF()

file(STRINGS ${CGoGN_SRC_DIR}/include/cgogn_coreprofile.h COREPROFILE_STR)
IF (COREPROFILE_STR EQUAL 1)
	add_definitions(-DCGoGN_USE_OGL_CORE_PROFILE=1)
ENDIF()


add_definitions(-DCGOGN_ASSERT_BOOL=${CGoGN_ASSERT_ACTIVED})

IF(CGoGN_SLOT_DEBUG_MODE)
	add_definitions(-DCGoGN_SLOT_DEBUG_MODE=1)
ENDIF()


# define includes of external libs 
SET (EXT_INCLUDES 
	${CGoGN_ROOT_DIR}/ThirdParty/include
	${CGoGN_ROOT_DIR}/ThirdParty/OpenNL/src
	${OPENGL_INCLUDE_DIR}
	${GLEW_INCLUDE_DIRS}
	${ZLIB_INCLUDE_DIRS}
	${QT_INCLUDE_DIR}
	${CGoGN_ROOT_DIR}/ThirdParty/libQGLViewer/
	${PYTHON_INCLUDE_DIRS}
	${SUITESPARSE_INCLUDE_DIRS}
	${CGoGN_ROOT_DIR}/ThirdParty/TinyXml2
)

# define libs for external libs
SET (EXT_LIBS 
	PythonQt
	nl
	QGLViewer
	${OPENGL_LIBRARY}
	${GLEW_LIBRARIES}
	${ZLIB_LIBRARIES}
	${QT_LIBRARIES}
	${PYTHON_LIBRARIES}
	${SUITESPARSE_LIBRARIES}
	tinyxml2
)


if(APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wno-overloaded-virtual")	
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-undefined -Wl,dynamic_lookup")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-local-typedefs")
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
	${CGoGN_SRC_DIR}/include
	${CGoGN_ROOT_DIR}/ThirdParty/PythonQt/src/
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
