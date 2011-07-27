cmake_minimum_required(VERSION 2.6)

project(examplesD)

SET(CMAKE_BUILD_TYPE Debug)

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
link_directories(
	${CGoGN_ROOT_DIR}/lib/Debug
	${CGoGN_ROOT_DIR}/lib/Release
)

#define exec to compile

QT4_WRAP_CPP(polyhedronsView_moc ../polyhedronsView.h)	
add_executable( polyhedronsViewD ../polyhedronsView.cpp ${polyhedronsView_moc})
target_link_libraries( polyhedronsViewD
		${CGoGN_LIBS_D} ${COMMON_LIBS} ${QT_LIBRARIES} )

QT4_WRAP_CPP(frame_manip_moc ../frame_manip.h)  
add_executable( frame_manipD ../frame_manip.cpp ${frame_manip_moc})
target_link_libraries( frame_manipD
                ${CGoGN_LIBS_D} ${COMMON_LIBS}  ${QT_LIBRARIES} )

QT4_WRAP_CPP(texturesExample_moc ../texturesExample.h)  
add_executable( texturesExampleD ../texturesExample.cpp ${texturesExample_moc} )
target_link_libraries( texturesExampleD
                ${CGoGN_LIBS_D} ${COMMON_LIBS} ${QT_LIBRARIES} )

QT4_WRAP_CPP(extrusionView_moc ../extrusionView.h)	
add_executable( extrusionViewD ../extrusionView.cpp ${extrusionView_moc})
target_link_libraries( extrusionViewD
		${CGoGN_LIBS_D} ${COMMON_LIBS} ${QT_LIBRARIES} )

QT4_WRAP_UI( viewer_ui ../viewer.ui )
QT4_WRAP_CPP( viewer_moc ../viewer.h )
add_executable( viewerD ../viewer.cpp ${viewer_moc} ${viewer_ui})
target_link_libraries( viewerD 
	${CGoGN_LIBS_D} ${NUMERICAL_LIBS} ${COMMON_LIBS} ${QT_LIBRARIES} )

