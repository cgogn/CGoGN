cmake_minimum_required(VERSION 2.8)

project(SCHNApps)

SET( QT_USE_QTXML TRUE )
SET (QT_USE_QTDESIGNER TRUE)
SET (QT_USE_QTGUI TRUE)
SET(QT_USE_QTPLUGIN TRUE)

#========CGoGN========#
SET(CGoGN_ROOT_DIR ${CMAKE_SOURCE_DIR}/.. CACHE STRING "CGoGN root dir")
INCLUDE(${CGoGN_ROOT_DIR}/apps_cmake.txt)


#=======QGLViewer=========#
find_package(QGLViewer REQUIRED)


#======Directories=======#
SET(INC_DIR ${CMAKE_SOURCE_DIR}/include)
SET(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
SET(FORMS_DIR ${CMAKE_SOURCE_DIR}/forms)
SET(RESOURCES_DIR ${CMAKE_SOURCE_DIR}/resources)

SET(PLUGIN_DIR ${CMAKE_SOURCE_DIR}/Plugins)
SET(PLUGIN_COMPILE_DIR ${EXECUTABLE_OUTPUT_PATH}/plugins)

ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/Release Release)
IF (NOT WIN32)
	ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/Debug Debug)
ENDIF (NOT WIN32)

configure_file(${CMAKE_SOURCE_DIR}/settings/state_save.xml ${EXECUTABLE_OUTPUT_PATH}/state_save.xml @ONLY)


#======Documentation=======#
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc
	${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Doc
	COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)
