cmake_minimum_required(VERSION 2.8)

project(CGoGNRelease)

SET(CMAKE_BUILD_TYPE Release)

IF(WIN32)
	SET(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/lib)
ELSE(WIN32)
	SET(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/lib/Release)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOTOPOWARNING")
ENDIF(WIN32)

include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
	${CGoGN_ROOT_DIR}/include
	${CGoGN_EXT_INCLUDES}
)

file(
	GLOB_RECURSE
	files_topology
	${CGoGN_ROOT_DIR}/src/Topology/*.cpp
	${CGoGN_ROOT_DIR}/include/Topology/*.hpp
	${CGoGN_ROOT_DIR}/include/Topology/*.h
)
add_library(
	topology
	${files_topology}
)

file(
	GLOB_RECURSE
	files_container
	${CGoGN_ROOT_DIR}/src/Container/*.cpp
	${CGoGN_ROOT_DIR}/include/Container/*.hpp
	${CGoGN_ROOT_DIR}/include/Container/*.h
)
add_library(
	container
	${files_container}
)

file(
	GLOB_RECURSE
	files_algo
	${CGoGN_ROOT_DIR}/src/Algo/*.cpp
	${CGoGN_ROOT_DIR}/src/Algo/*.c
	${CGoGN_ROOT_DIR}/include/Algo/*.hpp
	${CGoGN_ROOT_DIR}/include/Algo/*.h
)
add_library(
	algo
	${files_algo}
)

#file(
#	GLOB_RECURSE
#	files_geometry
#	${CGoGN_ROOT_DIR}/src/Geometry/*.cpp
#	${CGoGN_ROOT_DIR}/include/Geometry/*.hpp
#	${CGoGN_ROOT_DIR}/include/Geometry/*.h
#)
#add_library(
#	geometry
#	${files_geometry}
#)


file(
	GLOB_RECURSE
	shaders_src
	${CGoGN_ROOT_DIR}/include/Utils/*.frag
	${CGoGN_ROOT_DIR}/include/Utils/*.vert
	${CGoGN_ROOT_DIR}/include/Utils/*.geom	
	${CGoGN_ROOT_DIR}/include/Utils/Shaders*.frag
	${CGoGN_ROOT_DIR}/include/Utils/Shaders*.vert
	${CGoGN_ROOT_DIR}/include/Utils/Shaders*.geom
)


IF(WIN32)
add_custom_target(shader_target ${CGoGN_ROOT_DIR}/bin/shader_to_h ${shaders_src}
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	SOURCES ${shaders_src} 
)
ELSE(WIN32)
add_custom_target(shader_target ${CGoGN_ROOT_DIR}/bin/shader_to_h ${shaders_src}
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	SOURCES ${shaders_src} 
)
ENDIF(WIN32)




file(
	GLOB		# WARNING NO MORE RECURSE TO AVOID TAKING QT FILES
	files_utils
	${CGoGN_ROOT_DIR}/src/Utils/*.cpp
	${CGoGN_ROOT_DIR}/include/Utils/*.hpp
	${CGoGN_ROOT_DIR}/include/Utils/*.h

	${CGoGN_ROOT_DIR}/src/Utils/Shaders/*.cpp
	${CGoGN_ROOT_DIR}/include/Utils/Shaders/*.hpp
	${CGoGN_ROOT_DIR}/include/Utils/Shaders/*.h
)

IF(WITH_QT)
	file(
		GLOB
		files_utils_qt
		${CGoGN_ROOT_DIR}/src/Utils/Qt/*.cpp
		${CGoGN_ROOT_DIR}/include/Utils/Qt/*.hpp
		${CGoGN_ROOT_DIR}/include/Utils/Qt/*.h )

	file(
		GLOB_RECURSE
		utils_qt_headers
		${CGoGN_ROOT_DIR}/include/Utils/Qt/qt*.h )
		
	QT4_WRAP_CPP(UTILS_QT_HEADERS_MOC ${utils_qt_headers})
	SET (files_utils_withQt ${files_utils} ${files_utils_qt} ${UTILS_QT_HEADERS_MOC})
	add_library( utils ${files_utils_withQt} )
ELSE(WITH_QT)
	add_library( utils ${files_utils} )
ENDIF (WITH_QT)



add_dependencies(utils shader_target )



