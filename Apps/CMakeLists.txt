
IF (CGoGN_ONELIB)
	SET(CGoGN_LIBS cgogn)
ELSE ()
	SET(CGoGN_LIBS algo) # topology container utils include by dependencies
ENDIF ()


IF (WIN32)
	link_directories( ${CGoGN_ROOT_DIR}/lib/${ConfigurationName})
	SET(EXECUTABLE_OUTPUT_PATH ${CGoGN_ROOT_DIR}/bin/) # Release/Debug added automatically by visual
ELSE()
	link_directories( ${CGoGN_ROOT_DIR}/lib/${CMAKE_BUILD_TYPE} )
	SET(EXECUTABLE_OUTPUT_PATH ${CGoGN_ROOT_DIR}/bin/${CMAKE_BUILD_TYPE} )
ENDIF()

#include_directories(
#	${CGoGN_ROOT_DIR}/include
#	${CGoGN_EXT_INCLUDES})

IF (CGoGN_COMPILE_SANDBOX)
	add_subdirectory(SandBox)
ENDIF()

IF (CGoGN_COMPILE_EXAMPLES AND CGoGN_WITH_QT)
	add_subdirectory(Examples)
ENDIF()

IF (CGoGN_COMPILE_TUTOS)
	add_subdirectory(Tuto)
ENDIF()

IF (CGoGN_COMPILE_BENCHES)
	add_subdirectory(Benches)
ENDIF()

IF (CGoGN_COMPILE_TESTS)
	add_subdirectory(Tests)
ENDIF()

