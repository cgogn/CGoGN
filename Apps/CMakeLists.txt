SET(EXECUTABLE_OUTPUT_PATH ${CGoGN_ROOT_DIR}/bin)

SET (COMMON_LIBS ${GLUT_LIBRARY} ${OPENGL_LIBRARY} ${GLEW_LIBRARY} ${DEVIL_LIBRARIES} ${ZLIB_LIBRARIES} ${LIBXML2_LIBRARIES} gzstream AntTweakBar openctm assimp)

SET(CGoGN_LIBS_R topology algo container utils)

IF(WIN32)
	SET(CGoGN_LIBS_D topology algo container utils) # libs have same name but in different place in Visual
	add_subdirectory(Examples/Release)
	add_subdirectory(Tuto)
ELSE(WIN32)
	SET(CGoGN_LIBS_D topologyD algoD containerD utilsD)
	add_subdirectory(Examples/Release)
	add_subdirectory(Examples/Debug)
	add_subdirectory(Examples/Tests)
	add_subdirectory(Tuto)
ENDIF(WIN32)

