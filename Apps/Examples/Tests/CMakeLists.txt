cmake_minimum_required(VERSION 2.6)

project(geometryAssertD)

SET(CMAKE_BUILD_TYPE Debug)

#SET (COMMON_LIBS ${GLUT_LIBRARY} ${OPENGL_LIBRARY} ${GLEW_LIBRARY} ${DEVIL_LIBRARIES} ${ZLIB_LIBRARIES} ${LIBXML2_LIBRARIES} gzstream AntTweakBar openctm)

# FOR Qt4
FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QTOPENGL TRUE)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})


# define includes path
include_directories(
	${CGoGN_ROOT_DIR}/include
	/usr/include/libxml2/
	${CGoGN_ROOT_DIR}/include
	${CGoGN_ROOT_DIR}/ThirdParty/Numerical
	${CGoGN_ROOT_DIR}/ThirdParty/Numerical/UFconfig
	${CGoGN_ROOT_DIR}/ThirdParty/gzstream
	${CGoGN_ROOT_DIR}/ThirdParty/Zinri
	${CGoGN_ROOT_DIR}/ThirdParty/Assimp/include
	${CGoGN_ROOT_DIR}/ThirdParty/glm
)

# define libs path
link_directories(
	${CGoGN_ROOT_DIR}/lib/Debug
	${CGoGN_ROOT_DIR}/lib/Release
)

#define exec to compile

add_executable( Geom_orientationD ./Geom_orientation.cpp)
target_link_libraries( Geom_orientationD
	${CGoGN_LIBS_D} ${COMMON_LIBS} )

add_executable( Geom_inclusionD ./Geom_inclusion.cpp)
target_link_libraries( Geom_inclusionD
	${CGoGN_LIBS_D} ${COMMON_LIBS} )

add_executable( Geom_intersectionD ./Geom_intersection.cpp)
target_link_libraries( Geom_intersectionD
	${CGoGN_LIBS_D} ${COMMON_LIBS} )
	
	
QT4_WRAP_CPP(concave_rendering_moc concave_rendering.h)
add_executable( concave_renderingD concave_rendering.cpp ${concave_rendering_moc})
target_link_libraries( concave_renderingD
	${CGoGN_LIBS_D}  ${COMMON_LIBS} ${QT_LIBRARIES} )
