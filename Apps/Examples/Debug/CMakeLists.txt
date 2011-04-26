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

#add_executable( miniTestD ../miniTest.cpp)
#target_link_libraries( miniTestD
#	containerD topologyD utilsD algoD  numerical lapack blas f2c ${COMMON_LIBS} )

#add_executable( ViewerD ../Viewer.cpp)
#target_link_libraries( ViewerD
#	containerD topologyD utilsD algoD  ${COMMON_LIBS} )

#add_executable( simpleGMap2D ../simpleGMap2.cpp)
#target_link_libraries( simpleGMap2D
#	containerD topologyD utilsD algoD ${COMMON_LIBS} Zinri)

#add_executable( SimpleEMap3D ../simpleEMap3.cpp)
#target_link_libraries( SimpleEMap3D
#	containerD topologyD utilsD algoD ${COMMON_LIBS} )

#add_executable( triangulationD ../triangulation.cpp)
#target_link_libraries( triangulationD
#        containerD topologyD utilsD algoD ${COMMON_LIBS} )

#add_executable( decimationVolumiqueD ../decimationVolumique.cpp)
#target_link_libraries( decimationVolumiqueD
#	containerD topologyD utilsD algoD numerical lapack blas f2c  ${COMMON_LIBS} )

#add_executable( ter_meshesD ../ter_meshes.cpp)
#target_link_libraries( ter_meshesD
#        containerD topologyD utilsD algoD ${COMMON_LIBS} )

QT4_WRAP_CPP(polyhedronsView_moc ../polyhedronsView.h)	
add_executable( polyhedronsViewD ../polyhedronsView.cpp ${polyhedronsView_moc})
target_link_libraries( polyhedronsViewD
		${CGoGN_LIBS_D} ${COMMON_LIBS}  ${QT_LIBRARIES} )

QT4_WRAP_CPP(extrusionView_moc ../extrusionView.h)	
add_executable( extrusionViewD ../extrusionView.cpp ${extrusionView_moc})
target_link_libraries( extrusionViewD
		${CGoGN_LIBS_D} ${COMMON_LIBS}  ${QT_LIBRARIES} )
        
        
#add_executable( sceneD ../scene.cpp)
#target_link_libraries( sceneD
#       containerD topologyD utilsD algoD ${COMMON_LIBS} )
        
                
#add_executable( VizVolumeD ../VizVolume.cpp)
#target_link_libraries( VizVolumeD
#        containerD topologyD utilsD algoD ${COMMON_LIBS} Zinri)
        
