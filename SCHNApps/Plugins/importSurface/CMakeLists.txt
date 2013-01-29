cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(
	${SCHNApps_ROOT_DIR}/include
	${SCHNApps_ROOT_DIR}/Plugins/importSurface
	${CMAKE_CURRENT_BINARY_DIR}
)

SET( PLUGIN_SRC
    ${SCHNApps_ROOT_DIR}/Plugins/importSurface/importSurface.cpp
)

SET( PLUGIN_H
    ${SCHNApps_ROOT_DIR}/Plugins/importSurface/importSurface.h
)

IF( ${CMAKE_BUILD_TYPE} STREQUAL Debug )

	ADD_DEFINITIONS(-DDEBUG)

	IF(WIN32)
		link_directories( ${CGoGN_ROOT_DIR}/lib/Release )
	ELSE (WIN32)
		link_directories( ${CGoGN_ROOT_DIR}/lib/Debug ${CGoGN_ROOT_DIR}/lib/Release )
	ENDIF (WIN32)

#	QT4_WRAP_UI( PLUGIN_UI ${PLUGIN_FORM} )
	QT4_WRAP_CPP( PLUGIN_MOC ${PLUGIN_H} )
	
	ADD_LIBRARY( ImportSurfaceD SHARED
		${PLUGIN_SRC}
		${PLUGIN_UI}
		${PLUGIN_MOC}
	)

	TARGET_LINK_LIBRARIES( ImportSurfaceD
		${CGoGN_LIBS_D}
		${COMMON_LIBS}
	)

	ADD_DEPENDENCIES( ImportSurfaceD SCHNAppsD )

	SET_TARGET_PROPERTIES( ImportSurfaceD PROPERTIES COMPILE_DEFINITIONS "DEBUG" )
		
ELSE ( ${CMAKE_BUILD_TYPE} STREQUAL Debug )
	
	IF(WIN32)
	link_directories( ${CGoGN_ROOT_DIR}/lib/Release )
	ELSE (WIN32)
		link_directories( ${CGoGN_ROOT_DIR}/lib/Release )
	ENDIF (WIN32)

#	QT4_WRAP_UI( PLUGIN_UI ${PLUGIN_FORM} )
	QT4_WRAP_CPP( PLUGIN_MOC ${PLUGIN_H} )

	ADD_LIBRARY( ImportSurface SHARED
		${PLUGIN_SRC}
		${PLUGIN_UI}
		${PLUGIN_MOC}
	)

	TARGET_LINK_LIBRARIES( ImportSurface
		${CGoGN_LIBS_R}
		${COMMON_LIBS}
	)

	ADD_DEPENDENCIES( ImportSurface SCHNApps )
	
ENDIF ( ${CMAKE_BUILD_TYPE} STREQUAL Debug )
