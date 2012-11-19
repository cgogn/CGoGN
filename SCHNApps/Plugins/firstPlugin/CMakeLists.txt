cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(
	${SCHNApps_ROOT_DIR}/include
	${SCHNApps_ROOT_DIR}/Plugins/firstPlugin/include
	${CMAKE_CURRENT_BINARY_DIR}
)

SET( FIRSTPLUGIN_PLUGIN_SRC
	${SCHNApps_ROOT_DIR}/src/plugins/visualPlugin.cpp
    ${SCHNApps_ROOT_DIR}/Plugins/firstPlugin/src/firstPlugin.cpp
)

SET( FIRSTPLUGIN_PLUGIN_H
    ${SCHNApps_ROOT_DIR}/include/plugins/visualPlugin.h
    ${SCHNApps_ROOT_DIR}/Plugins/firstPlugin/include/firstPlugin.h
)

IF( ${CMAKE_BUILD_TYPE} STREQUAL Debug )

	IF(WIN32)
		link_directories( ${CGoGN_ROOT_DIR}/lib/Release )
	ELSE (WIN32)
		link_directories( ${CGoGN_ROOT_DIR}/lib/Debug ${CGoGN_ROOT_DIR}/lib/Release )
	ENDIF (WIN32)

	QT4_WRAP_CPP( FIRSTPLUGIN_MOC ${FIRSTPLUGIN_PLUGIN_H} )

	ADD_LIBRARY( FirstPluginD SHARED
		${FIRSTPLUGIN_PLUGIN_SRC}
		${FIRSTPLUGIN_MOC}
	)

	TARGET_LINK_LIBRARIES( FirstPluginD
		${CGoGN_LIBS_D}
		${COMMON_LIBS}
		${QGLVIEWER_LIBRARIES}
	)

	ADD_DEPENDENCIES( FirstPluginD SCHNAppsD )

	SET_TARGET_PROPERTIES( FirstPluginD PROPERTIES COMPILE_DEFINITIONS "DEBUG" )
		
ELSE ( ${CMAKE_BUILD_TYPE} STREQUAL Debug )
	
	IF(WIN32)
	link_directories( ${CGoGN_ROOT_DIR}/lib/Release )
	ELSE (WIN32)
		link_directories( ${CGoGN_ROOT_DIR}/lib/Release )
	ENDIF (WIN32)

	QT4_WRAP_CPP( FIRSTPLUGIN_MOC ${FIRSTPLUGIN_PLUGIN_H} )

	ADD_LIBRARY( FirstPlugin SHARED
		${FIRSTPLUGIN_PLUGIN_SRC}
		${FIRSTPLUGIN_MOC}
	)

	TARGET_LINK_LIBRARIES( FirstPlugin
		${CGoGN_LIBS_R}
		${COMMON_LIBS}
		${QGLVIEWER_LIBRARIES}
	)

	ADD_DEPENDENCIES( FirstPlugin SCHNApps )
	
ENDIF ( ${CMAKE_BUILD_TYPE} STREQUAL Debug )
