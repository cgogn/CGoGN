
IF (ONELIB)
	SET(CGoGN_LIBS cgogn)
ELSE (ONELIB)
	SET(CGoGN_LIBS topology algo container utils)
ENDIF (ONELIB)


IF (WIN32)
	link_directories( ${CGoGN_ROOT_DIR}/lib/${ConfigurationName}  ${Boost_LIBRARY_DIRS})
	SET(EXECUTABLE_OUTPUT_PATH ${CGoGN_ROOT_DIR}/bin/) # Release/Debug added automatically by visual
ELSE()
	link_directories( ${CGoGN_ROOT_DIR}/lib/${CMAKE_BUILD_TYPE} )
	SET(EXECUTABLE_OUTPUT_PATH ${CGoGN_ROOT_DIR}/bin/${CMAKE_BUILD_TYPE} )
ENDIF()

include_directories(
	${CGoGN_ROOT_DIR}/include
	${CGoGN_EXT_INCLUDES})

IF (COMPILE_SANDBOX)
	add_subdirectory(SandBox)
ENDIF()

IF (COMPILE_EXAMPLES AND WITH_QT)
		add_subdirectory(Examples)
ENDIF()

IF (COMPILE_TUTOS)
	add_subdirectory(Tuto)
ENDIF()

IF (COMPILE_BENCHES)
	add_subdirectory(Benches)
ENDIF()

IF (COMPILE_TESTS)
	add_subdirectory(Tests)
ENDIF()


#IF (WITH_QT)
#	add_subdirectory(Tuto)
#	add_subdirectory(Examples)
#	IF (NOT DONOT_COMPILE_SANDBOX)
#		add_subdirectory(SandBox)
#	ENDIF (NOT DONOT_COMPILE_SANDBOX)
#ENDIF (WITH_QT)

#add_subdirectory(Tests)
#add_subdirectory(Benches)

#add_subdirectory(Tuto/Traversals)
#add_subdirectory(Tuto/Attributes)
#add_subdirectory(Tuto/Markers)
#add_subdirectory(Tuto/Boundary)
#add_subdirectory(Tuto/Modelling)
