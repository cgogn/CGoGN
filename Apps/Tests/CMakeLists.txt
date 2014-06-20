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

add_executable( loadSave ./loadSave.cpp)
target_link_libraries( loadSave
	${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS})

add_executable( copyfrom ./copyfrom.cpp)
target_link_libraries( copyfrom
	${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS})

add_executable( movefrom ./movefrom.cpp)
target_link_libraries( movefrom
	${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS})

add_executable( compact ./compact.cpp)
target_link_libraries( compact
	${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS})
