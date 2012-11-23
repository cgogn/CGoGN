cmake_minimum_required(VERSION 2.8)

SET(CMAKE_BUILD_TYPE Release)

ADD_DEFINITIONS(-fPIC)

IF(WIN32)
	link_directories( ${CGoGN_ROOT_DIR}/lib/Release )
ELSE (WIN32)
	link_directories( ${CGoGN_ROOT_DIR}/lib/Release )
ENDIF (WIN32)

INCLUDE_DIRECTORIES(
	${CGoGN_ROOT_DIR}/include
	${COMMON_INCLUDES}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${QGLVIEWER_INCLUDE_DIR}
	${SCHNApps_ROOT_DIR}/include
)

file(
	GLOB
	SCHNApps_SRC_FILES
	${SCHNApps_ROOT_DIR}/src/main.cpp
	${SCHNApps_ROOT_DIR}/src/interface/*.cpp
	${SCHNApps_ROOT_DIR}/src/visualization/*.cpp
	${SCHNApps_ROOT_DIR}/src/dialogs/*.cpp
)

SET(SCHNApps_H_OBJECT_FILES
	${SCHNApps_ROOT_DIR}/include/interface/window.h
	${SCHNApps_ROOT_DIR}/include/interface/system.h
	${SCHNApps_ROOT_DIR}/include/interface/viewButtonArea.h
	${SCHNApps_ROOT_DIR}/include/interface/splitArea.h
	${SCHNApps_ROOT_DIR}/include/visualization/view.h
	${SCHNApps_ROOT_DIR}/include/visualization/scene.h
	${SCHNApps_ROOT_DIR}/include/dialogs/newSceneDialog.h
	${SCHNApps_ROOT_DIR}/include/dialogs/sceneSelector.h
	${SCHNApps_ROOT_DIR}/include/dialogs/cameraViewDialog.h
	${SCHNApps_ROOT_DIR}/include/dialogs/cameraOptionDialog.h
	${SCHNApps_ROOT_DIR}/include/dialogs/cameraSceneDialog.h
	${SCHNApps_ROOT_DIR}/include/dialogs/globalCameraDialog.h
	${SCHNApps_ROOT_DIR}/include/dialogs/pluginDialog.h
	${SCHNApps_ROOT_DIR}/include/dialogs/linkViewDialog.h
	${SCHNApps_ROOT_DIR}/include/dialogs/mapPluginDialog.h
)

file(
	GLOB
	SCHNApps_FORM_FILES
	${SCHNApps_ROOT_DIR}/forms/*.ui
)

QT4_WRAP_UI( UI_H_FILES
	${SCHNApps_FORM_FILES}
)
QT4_WRAP_CPP( MOC_FILES
	${SCHNApps_H_OBJECT_FILES}
)
QT4_ADD_RESOURCES( RCC_FILES
	${SCHNApps_ROOT_DIR}/resources/resources.qrc
)

ADD_EXECUTABLE( SCHNApps
	${SCHNApps_SRC_FILES}
	${UI_H_FILES}
	${MOC_FILES}
	${RCC_FILES}
)

TARGET_LINK_LIBRARIES( SCHNApps
	${CGoGN_LIBS_R}
	${COMMON_LIBS}
	${QGLVIEWER_LIBRARIES}
)


ADD_SUBDIRECTORY(${SCHNApps_ROOT_DIR}/Plugins Plugins)
