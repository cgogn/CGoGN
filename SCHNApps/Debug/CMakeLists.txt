cmake_minimum_required(VERSION 2.8)

SET(CMAKE_BUILD_TYPE Debug)

IF(WIN32)
	link_directories( ${CGoGN_ROOT_DIR}/lib/Release )
ELSE (WIN32)
	link_directories( ${CGoGN_ROOT_DIR}/lib/Debug ${CGoGN_ROOT_DIR}/lib/Release )
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
	${SCHNApps_ROOT_DIR}/src/camera.cpp
	${SCHNApps_ROOT_DIR}/src/context.cpp
	${SCHNApps_ROOT_DIR}/src/main.cpp
	${SCHNApps_ROOT_DIR}/src/mapHandler.cpp
	${SCHNApps_ROOT_DIR}/src/plugin.cpp
	${SCHNApps_ROOT_DIR}/src/scene.cpp
	${SCHNApps_ROOT_DIR}/src/splitArea.cpp
	${SCHNApps_ROOT_DIR}/src/system.cpp
	${SCHNApps_ROOT_DIR}/src/view.cpp
	${SCHNApps_ROOT_DIR}/src/window.cpp
	${SCHNApps_ROOT_DIR}/src/viewSelector.cpp
	${SCHNApps_ROOT_DIR}/src/cameraViewDialog.cpp
	${SCHNApps_ROOT_DIR}/src/pluginDialog.cpp
)

SET(SCHNApps_H_OBJECT_FILES
	${SCHNApps_ROOT_DIR}/include/window.h
	${SCHNApps_ROOT_DIR}/include/scene.h
	${SCHNApps_ROOT_DIR}/include/view.h
	${SCHNApps_ROOT_DIR}/include/splitArea.h
	${SCHNApps_ROOT_DIR}/include/viewSelector.h
	${SCHNApps_ROOT_DIR}/include/cameraViewDialog.h
	${SCHNApps_ROOT_DIR}/include/pluginDialog.h
	
#	${SCHNApps_ROOT_DIR}/include/dialogs/newSceneDialog.h
#	${SCHNApps_ROOT_DIR}/include/dialogs/sceneSelector.h
#	${SCHNApps_ROOT_DIR}/include/dialogs/cameraOptionDialog.h
#	${SCHNApps_ROOT_DIR}/include/dialogs/cameraSceneDialog.h
#	${SCHNApps_ROOT_DIR}/include/dialogs/globalCameraDialog.h
#	${SCHNApps_ROOT_DIR}/include/dialogs/pluginDialog.h
#	${SCHNApps_ROOT_DIR}/include/dialogs/linkViewDialog.h
#	${SCHNApps_ROOT_DIR}/include/dialogs/mapPluginDialog.h
)

file(
	GLOB
	SCHNApps_FORM_FILES
	${SCHNApps_ROOT_DIR}/forms/window.ui
	${SCHNApps_ROOT_DIR}/forms/cameraViewDialog.ui
	${SCHNApps_ROOT_DIR}/forms/pluginDialog.ui
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

ADD_EXECUTABLE( SCHNAppsD
	${SCHNApps_SRC_FILES}
	${UI_H_FILES}
	${MOC_FILES}
	${RCC_FILES}
)

TARGET_LINK_LIBRARIES( SCHNAppsD
	${CGoGN_LIBS_D}
	${COMMON_LIBS}
	${QGLVIEWER_LIBRARIES}
)


#ADD_SUBDIRECTORY(${SCHNApps_ROOT_DIR}/Plugins PluginsD)
