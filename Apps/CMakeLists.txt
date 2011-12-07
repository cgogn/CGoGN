SET(EXECUTABLE_OUTPUT_PATH ${CGoGN_ROOT_DIR}/bin)

# FOR Qt4
#SET(QT_USE_QTOPENGL TRUE)
#INCLUDE(${QT_USE_FILE})
#ADD_DEFINITIONS(${QT_DEFINITIONS})

SET(CGoGN_LIBS_R topology algo container utils)

IF(WIN32)
	SET(CGoGN_LIBS_D topology algo container utils) # libs have same name but in different place in Visual
ELSE(WIN32)
	SET(CGoGN_LIBS_D topologyD algoD containerD utilsD)
	IF (WITH_QT)
		add_subdirectory(Examples/Debug)
	ENDIF (WITH_QT)
ENDIF(WIN32)

IF (WITH_QT)
	add_subdirectory(Tuto)
	add_subdirectory(Examples/Release)
ENDIF (WITH_QT)
add_subdirectory(Examples/Tests)

