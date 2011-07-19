cmake_minimum_required(VERSION 2.6)

project(examples)

SET(CMAKE_BUILD_TYPE Release)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOTOPOWARNING")

# FOR Qt4
FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QTOPENGL TRUE)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})


# define includes path
include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
	/usr/include/libxml2/
	${CGoGN_ROOT_DIR}/include
	${CGoGN_ROOT_DIR}/ThirdParty/Numerical
	${CGoGN_ROOT_DIR}/ThirdParty/Numerical/UFconfig
	${CGoGN_ROOT_DIR}/ThirdParty/gzstream
	${CGoGN_ROOT_DIR}/ThirdParty/Zinri
	${CGoGN_ROOT_DIR}/ThirdParty/AntTweakBar/include
	${CGoGN_ROOT_DIR}/ThirdParty/OpenCTM
	${CGoGN_ROOT_DIR}/ThirdParty/Assimp/include
	${CGoGN_ROOT_DIR}/ThirdParty/glm
)

# define libs path
IF(WIN32)
	link_directories( ${CGoGN_ROOT_DIR}/lib/$(ConfigurationName) )
ELSE(WIN32)
	link_directories( ${CGoGN_ROOT_DIR}/lib/Release )
ENDIF(WIN32)


#define exec to compile

QT4_WRAP_CPP(polyhedronsView_moc ../polyhedronsView.h)	
add_executable( polyhedronsView ../polyhedronsView.cpp ${polyhedronsView_moc})
target_link_libraries( polyhedronsView
		${CGoGN_LIBS_R} ${COMMON_LIBS}  ${QT_LIBRARIES} )

QT4_WRAP_CPP(frame_manip_moc ../frame_manip.h)	
add_executable( frame_manip ../frame_manip.cpp ${frame_manip_moc})
target_link_libraries( frame_manip
		${CGoGN_LIBS_R} ${COMMON_LIBS}  ${QT_LIBRARIES} )

QT4_WRAP_CPP(extrusionView_moc ../extrusionView.h)	
add_executable( extrusionView ../extrusionView.cpp ${extrusionView_moc})
target_link_libraries( extrusionView
		${CGoGN_LIBS_R} ${COMMON_LIBS}  ${QT_LIBRARIES} )

QT4_WRAP_UI( viewer_ui ../viewer.ui )
QT4_WRAP_CPP( viewer_moc ../viewer.h )
add_executable( viewer ../viewer.cpp ${viewer_moc} ${viewer_ui})
target_link_libraries( viewer
	${CGoGN_LIBS_R} ${NUMERICAL_LIBS} ${COMMON_LIBS} ${QT_LIBRARIES} )

