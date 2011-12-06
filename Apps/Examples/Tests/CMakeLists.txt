cmake_minimum_required(VERSION 2.6)

project(testing_debug)

SET(CMAKE_BUILD_TYPE Debug)

include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
	${CGoGN_ROOT_DIR}/include
	${CGoGN_EXT_INCLUDES}
)


IF(WIN32)
	link_directories( 
		${CGoGN_ROOT_DIR}/lib/$(ConfigurationName)
		${Boost_LIBRARY_DIRS} )
ELSE(WIN32)
	link_directories(
		${CGoGN_ROOT_DIR}/lib/Debug
		${CGoGN_ROOT_DIR}/lib/Release )
ENDIF(WIN32)

#define exec to compile

add_executable( Geom_orientationD ./Geom_orientation.cpp)
target_link_libraries( Geom_orientationD
	${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS})

add_executable( Geom_inclusionD ./Geom_inclusion.cpp)
target_link_libraries( Geom_inclusionD
	${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS})

add_executable( Geom_intersectionD ./Geom_intersection.cpp)
target_link_libraries( Geom_intersectionD
	${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS})
	
	
IF (WITH_QT)
	QT4_WRAP_CPP(concave_rendering_moc concave_rendering.h)
	add_executable( concave_renderingD concave_rendering.cpp ${concave_rendering_moc})
	target_link_libraries( concave_renderingD
		${CGoGN_LIBS_D}  ${CGoGN_EXT_LIBS} )
ENDIF (WITH_QT)
