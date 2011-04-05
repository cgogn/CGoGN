cmake_minimum_required(VERSION 2.6)

project(CGoGNDebug)

set(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/lib/Debug)

#SET(BUILD_SHARED_LIBS,FALSE)

SET(CMAKE_BUILD_TYPE Debug)


# FOR Qt4
FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QTOPENGL TRUE)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})


include_directories(
	/usr/include/libxml2
	${CGoGN_ROOT_DIR}/include
	${CGoGN_ROOT_DIR}/ThirdParty/Numerical
	${CGoGN_ROOT_DIR}/ThirdParty/AntTweakBar/include
	${CGoGN_ROOT_DIR}/ThirdParty/gzstream
	${CGoGN_ROOT_DIR}/ThirdParty/Assimp/include
	${CGoGN_ROOT_DIR}/ThirdParty/glm

)

file(
	GLOB_RECURSE
	files_topology
	${CGoGN_ROOT_DIR}/src/Topology/*.cpp
	${CGoGN_ROOT_DIR}/include/Topology/*.hpp
	${CGoGN_ROOT_DIR}/include/Topology/*.h
)
add_library(
	topologyD
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
	containerD
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
	algoD
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
#	geometryD
#	${files_geometry}
#)

add_definitions(-DSHADERPATH="${CGoGN_ROOT_DIR}/lib/Shaders/")

file(
	GLOB_RECURSE
	files_utils
	${CGoGN_ROOT_DIR}/src/Utils/*.cpp
	${CGoGN_ROOT_DIR}/include/Utils/*.hpp
	${CGoGN_ROOT_DIR}/include/Utils/*.h
)

file(
	GLOB_RECURSE
	utils_qt_headers
	${CGoGN_ROOT_DIR}/include/Utils/qt*.h
)
	
QT4_WRAP_CPP(UTILS_QT_HEADERS_MOC ${utils_qt_headers})

SET (files_utils_withQt ${files_utils} ${UTILS_QT_HEADERS_MOC})

add_library(
	utilsD
	${files_utils_withQt}
)


