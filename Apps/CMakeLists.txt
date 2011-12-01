SET(EXECUTABLE_OUTPUT_PATH ${CGoGN_ROOT_DIR}/bin)

# FOR Qt4
SET(QT_USE_QTOPENGL TRUE)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

SET(CGoGN_LIBS_R topology algo container utils)

IF(WIN32)
	SET(CGoGN_LIBS_D topology algo container utils) # libs have same name but in different place in Visual
	add_subdirectory(Examples/Release)
	add_subdirectory(Examples/Tests)
	add_subdirectory(Tuto)
ELSE(WIN32)
	SET(CGoGN_LIBS_D topologyD algoD containerD utilsD)
	add_subdirectory(Examples/Release)
	add_subdirectory(Examples/Debug)
	add_subdirectory(Examples/Tests)
	add_subdirectory(Tuto)
ENDIF(WIN32)

