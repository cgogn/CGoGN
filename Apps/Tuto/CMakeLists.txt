cmake_minimum_required(VERSION 2.6)

project(Tutos)

#SET (COMMON_LIBS ${GLUT_LIBRARY} ${OPENGL_LIBRARY} ${GLEW_LIBRARY} ${DEVIL_LIBRARIES} ${ZLIB_LIBRARIES} ${LIBXML2_LIBRARIES} gzstream AntTweakBar openctm)


#SET (CMAKE_BUILD_TYPE Debug)
SET(CMAKE_BUILD_TYPE Release)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOTOPOWARNING")

# define includes path
include_directories(
	/usr/include/libxml2/
	${CGoGN_ROOT_DIR}/ThirdParty/gzstream
	${CGoGN_ROOT_DIR}/ThirdParty/OpenCTM
	${CGoGN_ROOT_DIR}/ThirdParty/Assimp/include
	${CGoGN_ROOT_DIR}/include
)

# define libs path
link_directories(${CGoGN_ROOT_DIR}/lib/Release)

#define exec to compile

add_executable( tuto1 tuto1.cpp)
target_link_libraries( tuto1
	container topology utils algo  ${COMMON_LIBS} )

add_executable( tuto2 tuto2.cpp)
target_link_libraries( tuto2
	container topology utils algo  ${COMMON_LIBS} )

add_executable( tuto3 tuto3.cpp)
target_link_libraries( tuto3
	container topology utils algo  ${COMMON_LIBS} )

add_executable( tuto4 tuto4.cpp)
target_link_libraries( tuto4
	container topology utils algo  ${COMMON_LIBS} )

add_executable( tuto5 tuto5.cpp)
target_link_libraries( tuto5
	container topology utils algo  ${COMMON_LIBS} )

add_executable( tuto_subdivision tuto_subdivision.cpp)
target_link_libraries( tuto_subdivision
	container topology utils algo  ${COMMON_LIBS} )

add_executable( tp_master tp_master.cpp)
target_link_libraries( tp_master
	container topology utils algo  ${COMMON_LIBS} )

add_executable( tuto_mt tuto_mt.cpp)
target_link_libraries( tuto_mt
#	containerD topologyD utilsD algoD  ${COMMON_LIBS} boost_thread)
	container topology utils algo ${COMMON_LIBS} boost_thread)
