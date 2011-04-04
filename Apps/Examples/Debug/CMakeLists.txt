cmake_minimum_required(VERSION 2.6)

project(examplesD)

SET(CMAKE_BUILD_TYPE Debug)

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

add_executable( miniTestD ../miniTest.cpp)
target_link_libraries( miniTestD
	containerD topologyD utilsD algoD  numerical lapack blas f2c ${COMMON_LIBS} )

add_executable( ViewerD ../Viewer.cpp)
target_link_libraries( ViewerD
	containerD topologyD utilsD algoD  ${COMMON_LIBS} )

add_executable( simpleGMap2D ../simpleGMap2.cpp)
target_link_libraries( simpleGMap2D
	containerD topologyD utilsD algoD ${COMMON_LIBS} Zinri)

#add_executable( SimpleEMap3D ../simpleEMap3.cpp)
#target_link_libraries( SimpleEMap3D
#	containerD topologyD utilsD algoD ${COMMON_LIBS} )

add_executable( triangulationD ../triangulation.cpp)
target_link_libraries( triangulationD
        containerD topologyD utilsD algoD ${COMMON_LIBS} )

add_executable( decimationVolumiqueD ../decimationVolumique.cpp)
target_link_libraries( decimationVolumiqueD
	containerD topologyD utilsD algoD numerical lapack blas f2c  ${COMMON_LIBS} )

add_executable( ter_meshesD ../ter_meshes.cpp)
target_link_libraries( ter_meshesD
        containerD topologyD utilsD algoD ${COMMON_LIBS} )

add_executable( polyhedronsViewD ../polyhedronsView.cpp)
target_link_libraries( polyhedronsViewD
        containerD topologyD utilsD algoD ${COMMON_LIBS} )

add_executable( extrusionViewD ../extrusionView.cpp)
target_link_libraries( extrusionViewD
        containerD topologyD utilsD algoD ${COMMON_LIBS} )
        
add_executable( sceneD ../scene.cpp)
target_link_libraries( sceneD
        containerD topologyD utilsD algoD ${COMMON_LIBS} )
        
                
#add_executable( VizVolumeD ../VizVolume.cpp)
#target_link_libraries( VizVolumeD
#        containerD topologyD utilsD algoD ${COMMON_LIBS} Zinri)
        
