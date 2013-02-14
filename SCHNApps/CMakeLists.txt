cmake_minimum_required(VERSION 2.8)

project(SCHNApps)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC")


SET(CGoGN_ROOT_DIR ${CMAKE_SOURCE_DIR}/.. CACHE STRING "CGoGN root dir")
SET(CMAKE_MODULE_PATH ${CGoGN_ROOT_DIR}/cmake_modules/)


find_package(OpenGL REQUIRED)
find_package(Boost COMPONENTS regex thread system REQUIRED)
find_package(ZLIB REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(Qt4 REQUIRED)
find_package(QGLViewer REQUIRED)
find_package(PythonLibs 2.7 REQUIRED)
find_package(SuiteSparse REQUIRED)

SET( QT_USE_QTOPENGL TRUE )
SET( QT_USE_QTXML TRUE )
SET( QT_USE_QTDESIGNER TRUE )
SET( QT_USE_QTGUI TRUE )
SET( QT_USE_QTPLUGIN TRUE )

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})




file(STRINGS ${CGoGN_ROOT_DIR}/include/cgogn_mr.h FORCE_MR)
IF (FORCE_MR EQUAL 1)
	add_definitions(-DCGoGN_FORCE_MR=1)
ENDIF (FORCE_MR EQUAL 1)

file(STRINGS ${CGoGN_ROOT_DIR}/include/cgogn_onelib.h ONELIB_STR)
IF (ONELIB_STR EQUAL 1)
	SET(CGoGN_LIBS_D cgognD)
	SET(CGoGN_LIBS_R cgogn)
ELSE (ONELIB_STR EQUAL 1)
	SET(CGoGN_LIBS_D topologyD algoD containerD utilsD)
	SET(CGoGN_LIBS_R topology algo container utils)
ENDIF (ONELIB_STR EQUAL 1)

IF (DEFINED ASSERTON)
	add_definitions(-DCGOGN_ASSERT_BOOL=${ASSERTON})
ELSE (DEFINED ASSERTON)
	add_definitions(-DCGOGN_ASSERT_BOOL=false)
ENDIF (DEFINED ASSERTON)

add_definitions(-DSHADERPATH="${CGoGN_ROOT_DIR}/lib/Shaders/")




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
	${Boost_THREAD_LIBRARY}
	${QT_LIBRARIES}
	${QGLVIEWER_LIBRARIES}
	${PYTHON_LIBRARIES}
	${SUITESPARSE_LIBRARIES}
)




# qq definition specifiques pour mac
IF(APPLE)
# attention a changer pour chercher la bonne version automatiquement
	SET(CMAKE_OSX_SYSROOT "/Developer/SDKs/MacOSX10.6.sdk" CACHE STRING "developer SDK")
	SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-undefined -Wl,dynamic_lookup")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMAC_OSX")
	SET(CMAKE_OSX_ARCHITECTURES x86_64)
ENDIF(APPLE)




SET(SCHNApps_ROOT_DIR ${CGoGN_ROOT_DIR}/SCHNApps)
SET(EXECUTABLE_OUTPUT_PATH ${SCHNApps_ROOT_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${SCHNApps_ROOT_DIR}/lib)


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
	${SCHNApps_ROOT_DIR}/include/plugin.h
	${SCHNApps_ROOT_DIR}/include/view.h
	${SCHNApps_ROOT_DIR}/include/window.h
	${SCHNApps_ROOT_DIR}/include/viewButtonArea.h
	${SCHNApps_ROOT_DIR}/include/dialogs/camerasDialog.h
	${SCHNApps_ROOT_DIR}/include/dialogs/pluginsDialog.h
	${SCHNApps_ROOT_DIR}/include/dialogs/mapsDialog.h
	${SCHNApps_ROOT_DIR}/include/dialogs/cameraViewDialog.h
	${SCHNApps_ROOT_DIR}/include/dialogs/pluginsViewDialog.h
	${SCHNApps_ROOT_DIR}/include/dialogs/mapsViewDialog.h
)



ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/Release Release)
IF (NOT WIN32)
	ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/Debug Debug)
ENDIF (NOT WIN32)
