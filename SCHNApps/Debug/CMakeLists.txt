cmake_minimum_required(VERSION 2.8)

SET(CMAKE_BUILD_TYPE Debug)

IF(WIN32)
	link_directories( ${CGoGN_ROOT_DIR}/lib/Release )
ELSE (WIN32)
	link_directories( ${CGoGN_ROOT_DIR}/lib/Debug ${CGoGN_ROOT_DIR}/lib/Release )
ENDIF (WIN32)

INCLUDE_DIRECTORIES(
	${CGoGN_ROOT_DIR}/include
	${SCHNApps_ROOT_DIR}/include
	${EXT_INCLUDES}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

file(
	GLOB
	SCHNApps_SRC_FILES
	${SCHNApps_ROOT_DIR}/src/main.cpp
	${SCHNApps_ROOT_DIR}/src/camera.cpp
	${SCHNApps_ROOT_DIR}/src/mapHandler.cpp
	${SCHNApps_ROOT_DIR}/src/plugin.cpp
	${SCHNApps_ROOT_DIR}/src/view.cpp
	${SCHNApps_ROOT_DIR}/src/window.cpp
	${SCHNApps_ROOT_DIR}/src/viewButtonArea.cpp
	${SCHNApps_ROOT_DIR}/src/dialogs/camerasDialog.cpp
	${SCHNApps_ROOT_DIR}/src/dialogs/pluginsDialog.cpp
	${SCHNApps_ROOT_DIR}/src/dialogs/mapsDialog.cpp
	${SCHNApps_ROOT_DIR}/src/dialogs/cameraViewDialog.cpp
	${SCHNApps_ROOT_DIR}/src/dialogs/pluginsViewDialog.cpp
	${SCHNApps_ROOT_DIR}/src/dialogs/mapsViewDialog.cpp
)

SET(SCHNApps_H_OBJECT_FILES
	${SCHNApps_ROOT_DIR}/include/view.h
	${SCHNApps_ROOT_DIR}/include/window.h
	${SCHNApps_ROOT_DIR}/include/mapHandler.h
	${SCHNApps_ROOT_DIR}/include/plugin.h
	${SCHNApps_ROOT_DIR}/include/viewButtonArea.h
	${SCHNApps_ROOT_DIR}/include/dialogs/camerasDialog.h
	${SCHNApps_ROOT_DIR}/include/dialogs/pluginsDialog.h
	${SCHNApps_ROOT_DIR}/include/dialogs/mapsDialog.h
	${SCHNApps_ROOT_DIR}/include/dialogs/cameraViewDialog.h
	${SCHNApps_ROOT_DIR}/include/dialogs/pluginsViewDialog.h
	${SCHNApps_ROOT_DIR}/include/dialogs/mapsViewDialog.h
)

file(
	GLOB
	SCHNApps_FORM_FILES
	${SCHNApps_ROOT_DIR}/forms/window.ui
	${SCHNApps_ROOT_DIR}/forms/camerasDialog.ui
	${SCHNApps_ROOT_DIR}/forms/pluginsDialog.ui
	${SCHNApps_ROOT_DIR}/forms/mapsDialog.ui
	${SCHNApps_ROOT_DIR}/forms/cameraViewDialog.ui
	${SCHNApps_ROOT_DIR}/forms/pluginsViewDialog.ui
	${SCHNApps_ROOT_DIR}/forms/mapsViewDialog.ui
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
	${EXT_LIBS}
)


ADD_SUBDIRECTORY(${SCHNApps_ROOT_DIR}/Plugins PluginsD)
