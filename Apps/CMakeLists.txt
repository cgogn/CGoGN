SET(EXECUTABLE_OUTPUT_PATH ${CGoGN_ROOT_DIR}/bin)

# define  libs
SET (COMMON_LIBS ${OPENGL_LIBRARY} ${GLEW_LIBRARY} ${DEVIL_LIBRARIES} ${ZLIB_LIBRARIES} ${LIBXML2_LIBRARIES} ${Boost_REGEX_LIBRARY} ${Boost_THREAD_LIBRARY} gzstream assimp)

# define includes of external libs 
SET(CGoGN_EXT_INCLUDES 
	${LIBXML2_INCLUDE_DIR}
	${CGoGN_ROOT_DIR}/ThirdParty/Numerical
	${CGoGN_ROOT_DIR}/ThirdParty/Numerical/UFconfig
	${CGoGN_ROOT_DIR}/ThirdParty/gzstream
	${CGoGN_ROOT_DIR}/ThirdParty/Zinri
	${CGoGN_ROOT_DIR}/ThirdParty/glm
	${CGoGN_ROOT_DIR}/ThirdParty/Assimp/include )



# FOR Qt4
SET(QT_USE_QTOPENGL TRUE)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})



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

