cmake_minimum_required(VERSION 2.6)

project(geometryAssertD)

SET(CMAKE_BUILD_TYPE Debug)

#SET (COMMON_LIBS ${GLUT_LIBRARY} ${OPENGL_LIBRARY} ${GLEW_LIBRARY} ${DEVIL_LIBRARIES} ${ZLIB_LIBRARIES} ${LIBXML2_LIBRARIES} gzstream AntTweakBar openctm)

# define includes path
include_directories(
	${CGoGN_ROOT_DIR}/include
	/usr/include/libxml2/
	${CGoGN_ROOT_DIR}/include
	${CGoGN_ROOT_DIR}/ThirdParty/Numerical
	${CGoGN_ROOT_DIR}/ThirdParty/Numerical/UFconfig
	${CGoGN_ROOT_DIR}/ThirdParty/gzstream
	${CGoGN_ROOT_DIR}/ThirdParty/Zinri
	${CGoGN_ROOT_DIR}/ThirdParty/AntTweakBar/include
	${CGoGN_ROOT_DIR}/ThirdParty/OpenCTM
	${CGoGN_ROOT_DIR}/ThirdParty/Assimp/include
)

# define libs path
link_directories(
	${CGoGN_ROOT_DIR}/lib/Debug
	${CGoGN_ROOT_DIR}/lib/Release
)

#define exec to compile

add_executable( Geom_orientationD ./Geom_orientation.cpp)
target_link_libraries( Geom_orientationD
	${COMMON_LIBS} )

add_executable( Geom_inclusionD ./Geom_inclusion.cpp)
target_link_libraries( Geom_inclusionD
	${COMMON_LIBS} )

add_executable( Algo_Geom_inclusionD ./Algo_Geom_inclusion.cpp)
target_link_libraries( Algo_Geom_inclusionD
	containerD topologyD utilsD algoD ${COMMON_LIBS} )
