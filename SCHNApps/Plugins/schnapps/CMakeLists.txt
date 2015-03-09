cmake_minimum_required(VERSION 2.8)

project(SCHNAppsLib)

IF(WIN32 AND (NOT (CGoGN_WITH_QTCREATOR)))
	file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/plugins_default_directory.h 
	\#define\ PLUGINS_DEFAULT_DIRECTORY\ \"${SCHNApps_ROOT_DIR}/lib/plugins/\" )
ELSE()
	file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/plugins_default_directory.h 
	\#define\ PLUGINS_DEFAULT_DIRECTORY\ \"${SCHNApps_ROOT_DIR}/lib/plugins/${CMAKE_BUILD_TYPE}\" )
ENDIF()

IF(WIN32)
	IF (CGoGN_WITH_QTCREATOR)
		SET(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/bin/${CMAKE_BUILD_TYPE})
		link_directories( ${CGoGN_ROOT_DIR}/bin/${CMAKE_BUILD_TYPE} )
	ELSE()
		SET(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/bin)
		link_directories( ${CGoGN_ROOT_DIR}/bin/${ConfigurationName} )
	ENDIF()
ELSE()
	SET(LIBRARY_OUTPUT_PATH ${SCHNApps_ROOT_DIR}/lib/${CMAKE_BUILD_TYPE})
	link_directories( ${CGoGN_ROOT_DIR}/lib/${CMAKE_BUILD_TYPE} )
ENDIF()

# SCHNApps files to compile in a lib for dll access from plugins
file(
	GLOB_RECURSE
	SCHNAppsLib_FILES	
	${SCHNApps_ROOT_DIR}/src/camera.cpp
	${SCHNApps_ROOT_DIR}/src/cellSelector.cpp
	${SCHNApps_ROOT_DIR}/src/colorComboBox.cpp
	${SCHNApps_ROOT_DIR}/src/controlDock_cameraTab.cpp
	${SCHNApps_ROOT_DIR}/src/controlDock_mapTab.cpp
	${SCHNApps_ROOT_DIR}/src/controlDock_pluginTab.cpp
	${SCHNApps_ROOT_DIR}/src/dialogList.cpp
	${SCHNApps_ROOT_DIR}/src/mapHandler.cpp
	${SCHNApps_ROOT_DIR}/src/plugin.cpp
	${SCHNApps_ROOT_DIR}/src/plugin_interaction.cpp
	${SCHNApps_ROOT_DIR}/src/plugin_processing.cpp
	${SCHNApps_ROOT_DIR}/src/schnapps.cpp
	${SCHNApps_ROOT_DIR}/src/slot_debug.cpp
	${SCHNApps_ROOT_DIR}/src/view.cpp
	${SCHNApps_ROOT_DIR}/src/viewButtonArea.cpp
	${SCHNApps_ROOT_DIR}/include/*.h
	${SCHNApps_ROOT_DIR}/include/*.hpp
)

file(
	GLOB_RECURSE
	SCHNAppsLib_UI_FILES
	${SCHNApps_ROOT_DIR}/forms/*.ui
)

SET(SCHNAppsLib_QOBJECT_FILES
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

qt_wrap_ui( UIC_FILES
	${SCHNAppsLib_UI_FILES}
)

qt_wrap_cpp( MOC_FILES
	${SCHNAppsLib_QOBJECT_FILES}
)

qt_add_resources( RCC_FILES
	${SCHNApps_ROOT_DIR}/resources/resources.qrc
)

ADD_DEFINITIONS(-DSCHNAPPSLIB_DLL_EXPORT)

ADD_LIBRARY( SCHNAppsLib SHARED
	${SCHNAppsLib_FILES}
	${UIC_FILES}
	${MOC_FILES}
	${RCC_FILES}
)
TARGET_LINK_LIBRARIES( SCHNAppsLib
	${CGoGN_LIBS}
	${EXT_LIBS}
)

qt_use_modules(SCHNAppsLib Core Gui OpenGL Xml Svg)
