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
)

SET(SRC_FILES
	${SRC_DIR}/main.cpp
	${SRC_DIR}/interface/window.cpp
	${SRC_DIR}/interface/system.cpp
	${SRC_DIR}/interface/stateHandler.cpp
	${SRC_DIR}/visualization/view.cpp
	${SRC_DIR}/visualization/scene.cpp
	${SRC_DIR}/visualization/camera.cpp
	${SRC_DIR}/interface/splitArea.cpp
	${SRC_DIR}/dialogs/newSceneDialog.cpp
	${SRC_DIR}/dialogs/sceneSelector.cpp
	${SRC_DIR}/interface/viewButtonArea.cpp
	${SRC_DIR}/dialogs/cameraViewDialog.cpp
	${SRC_DIR}/dialogs/cameraOptionDialog.cpp
	${SRC_DIR}/dialogs/cameraSceneDialog.cpp
	${SRC_DIR}/dialogs/globalCameraDialog.cpp
	${SRC_DIR}/dialogs/pluginDialog.cpp
	${SRC_DIR}/dialogs/linkViewDialog.cpp
	${SRC_DIR}/visualization/vboHandler.cpp
	${SRC_DIR}/visualization/mapHandler.cpp
	${SRC_DIR}/visualization/context.cpp
	${SRC_DIR}/dialogs/mapPluginDialog.cpp
)

SET(H_OBJECT_FILES
	${INC_DIR}/interface/window.h
	${INC_DIR}/interface/system.h
	${INC_DIR}/visualization/view.h
	${INC_DIR}/visualization/scene.h
	${INC_DIR}/interface/splitArea.h
	${INC_DIR}/dialogs/newSceneDialog.h
	${INC_DIR}/dialogs/sceneSelector.h
	${INC_DIR}/interface/viewButtonArea.h
	${INC_DIR}/dialogs/cameraViewDialog.h
	${INC_DIR}/dialogs/cameraOptionDialog.h
	${INC_DIR}/dialogs/cameraSceneDialog.h
	${INC_DIR}/dialogs/globalCameraDialog.h
	${INC_DIR}/dialogs/pluginDialog.h
	${INC_DIR}/dialogs/linkViewDialog.h
	${INC_DIR}/dialogs/mapPluginDialog.h
)

SET(FORMS_FILES
	${FORMS_DIR}/window.ui
	${FORMS_DIR}/questionSessionDialog.ui
	${FORMS_DIR}/infoDialog.ui
	${FORMS_DIR}/newSceneDialog.ui
	${FORMS_DIR}/cameraViewDialog.ui
	${FORMS_DIR}/cameraOptionDialog.ui
	${FORMS_DIR}/cameraSceneDialog.ui
	${FORMS_DIR}/globalCameraDialog.ui
	${FORMS_DIR}/pluginDialog.ui
	${FORMS_DIR}/linkViewDialog.ui
	${FORMS_DIR}/mapPluginDialog.ui
)

QT4_WRAP_UI(UI_H_FILES
	${FORMS_FILES}
)
QT4_WRAP_CPP(MOC_FILES
	${H_OBJECT_FILES}
)
QT4_ADD_RESOURCES(RCC_FILES
	${RESOURCES_DIR}/resources.qrc
)

INCLUDE_DIRECTORIES(
	${FORMS}
	${INC_DIR}
	${INC_DIR}/interface
	${INC_DIR}/visualization
	${INC_DIR}/dialogs
	${INC_DIR}/plugins
)

ADD_EXECUTABLE(CGoGN_plug
	${SRC_FILES}
	${UI_H_FILES}
	${MOC_FILES}
	${RCC_FILES}
)

TARGET_LINK_LIBRARIES(CGoGN_plug
	${CGoGN_LIBS_R}
	${COMMON_LIBS}
	${QGLVIEWER_LIBRARIES}
)

#ADD_SUBDIRECTORY(${PLUGIN_DIR} plugins)

