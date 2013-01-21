cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(
	${SCHNApps_ROOT_DIR}/include
	${SCHNApps_ROOT_DIR}/Plugins/differentialProperties/include
	${CMAKE_CURRENT_BINARY_DIR}
)

SET( PLUGIN_SRC
    ${SCHNApps_ROOT_DIR}/Plugins/differentialProperties/src/differentialProperties.cpp
    ${SCHNApps_ROOT_DIR}/Plugins/differentialProperties/src/computeNormalDialog.cpp
    ${SCHNApps_ROOT_DIR}/Plugins/differentialProperties/src/computeCurvatureDialog.cpp
)

SET( PLUGIN_H
    ${SCHNApps_ROOT_DIR}/Plugins/differentialProperties/include/differentialProperties.h
    ${SCHNApps_ROOT_DIR}/Plugins/differentialProperties/include/computeNormalDialog.h
    ${SCHNApps_ROOT_DIR}/Plugins/differentialProperties/include/computeCurvatureDialog.h
)

SET( PLUGIN_FORM
	${SCHNApps_ROOT_DIR}/Plugins/differentialProperties/forms/computeNormalDialog.ui
	${SCHNApps_ROOT_DIR}/Plugins/differentialProperties/forms/computeCurvatureDialog.ui
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
	
	ADD_LIBRARY( DifferentialPropertiesPluginD SHARED
		${PLUGIN_SRC}
		${PLUGIN_UI}
		${PLUGIN_MOC}
	)

	TARGET_LINK_LIBRARIES( DifferentialPropertiesPluginD
		${CGoGN_LIBS_D}
		${COMMON_LIBS}
		${QGLVIEWER_LIBRARIES}
	)

	ADD_DEPENDENCIES( DifferentialPropertiesPluginD SCHNAppsD )

	SET_TARGET_PROPERTIES( DifferentialPropertiesPluginD PROPERTIES COMPILE_DEFINITIONS "DEBUG" )
		
ELSE ( ${CMAKE_BUILD_TYPE} STREQUAL Debug )
	
	IF(WIN32)
	link_directories( ${CGoGN_ROOT_DIR}/lib/Release )
	ELSE (WIN32)
		link_directories( ${CGoGN_ROOT_DIR}/lib/Release )
	ENDIF (WIN32)

	QT4_WRAP_UI( PLUGIN_UI ${PLUGIN_FORM} )
	QT4_WRAP_CPP( PLUGIN_MOC ${PLUGIN_H} )

	ADD_LIBRARY( DifferentialPropertiesPlugin SHARED
		${PLUGIN_SRC}
		${PLUGIN_UI}
		${PLUGIN_MOC}
	)

	TARGET_LINK_LIBRARIES( DifferentialPropertiesPlugin
		${CGoGN_LIBS_R}
		${COMMON_LIBS}
		${QGLVIEWER_LIBRARIES}
	)

	ADD_DEPENDENCIES( DifferentialPropertiesPlugin SCHNApps )
	
ENDIF ( ${CMAKE_BUILD_TYPE} STREQUAL Debug )
