cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(
	${SCHNApps_ROOT_DIR}/include
	${SCHNApps_ROOT_DIR}/Plugins/subdivideSurface
	${CMAKE_CURRENT_BINARY_DIR}
)

SET( PLUGIN_SRC
    ${SCHNApps_ROOT_DIR}/Plugins/subdivideSurface/subdivideSurface.cpp
)

SET( PLUGIN_H
    ${SCHNApps_ROOT_DIR}/Plugins/subdivideSurface/subdivideSurface.h
)

SET( PLUGIN_FORM
	${SCHNApps_ROOT_DIR}/Plugins/subdivideSurface/subdivideSurface.ui
)

IF( ${CMAKE_BUILD_TYPE} STREQUAL Debug )

	ADD_DEFINITIONS(-DDEBUG)

	IF(WIN32)
		link_directories( ${CGoGN_ROOT_DIR}/lib/Release )
	ELSE (WIN32)
		link_directories( ${CGoGN_ROOT_DIR}/lib/Debug ${CGoGN_ROOT_DIR}/lib/Release )
	ENDIF (WIN32)

	QT4_WRAP_UI( PLUGIN_UI ${PLUGIN_FORM} )
	QT4_WRAP_CPP( PLUGIN_MOC ${PLUGIN_H} )
	
	ADD_LIBRARY( SubdivideSurfacePluginD SHARED
		${PLUGIN_SRC}
		${PLUGIN_UI}
		${PLUGIN_MOC}
	)

	TARGET_LINK_LIBRARIES( SubdivideSurfacePluginD
		${LIBRARY_OUTPUT_PATH}/libSCHNApps.so
		${CGoGN_LIBS_D}
		${COMMON_LIBS}
		${QGLVIEWER_LIBRARIES}
	)

	ADD_DEPENDENCIES( SubdivideSurfacePluginD SCHNAppsD )

	SET_TARGET_PROPERTIES( SubdivideSurfacePluginD PROPERTIES COMPILE_DEFINITIONS "DEBUG" )
		
ELSE ( ${CMAKE_BUILD_TYPE} STREQUAL Debug )
	
	IF(WIN32)
	link_directories( ${CGoGN_ROOT_DIR}/lib/Release )
	ELSE (WIN32)
		link_directories( ${CGoGN_ROOT_DIR}/lib/Release )
	ENDIF (WIN32)

	QT4_WRAP_UI( PLUGIN_UI ${PLUGIN_FORM} )
	QT4_WRAP_CPP( PLUGIN_MOC ${PLUGIN_H} )

	ADD_LIBRARY( SubdivideSurfacePlugin SHARED
		${PLUGIN_SRC}
		${PLUGIN_UI}
		${PLUGIN_MOC}
	)

	TARGET_LINK_LIBRARIES( SubdivideSurfacePlugin
		${LIBRARY_OUTPUT_PATH}/libSCHNApps.so
		${CGoGN_LIBS_R}
		${COMMON_LIBS}
		${QGLVIEWER_LIBRARIES}
	)

	ADD_DEPENDENCIES( SubdivideSurfacePlugin SCHNApps )
	
ENDIF ( ${CMAKE_BUILD_TYPE} STREQUAL Debug )
