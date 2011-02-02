cmake_minimum_required(VERSION 2.6)

project(examples)

SET(CMAKE_BUILD_TYPE Release)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOTOPOWARNING")

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

)

# define libs path
link_directories(
	${CGoGN_ROOT_DIR}/lib/Release
)

#define exec to compile

add_executable( miniTest ../miniTest.cpp)
target_link_libraries( miniTest
	container topology utils algo  numerical lapack blas f2c ${COMMON_LIBS} )

add_executable( Viewer ../Viewer.cpp)
target_link_libraries( Viewer
	container topology utils algo  ${COMMON_LIBS} )

add_executable( simpleGMap2 ../simpleGMap2.cpp)
target_link_libraries( simpleGMap2
	container topology utils algo ${COMMON_LIBS} Zinri)

# add_executable( hm_convexhull ../hm_convexhull.cpp)
# target_link_libraries( hm_convexhull
# 	topology utils algo ${COMMON_LIBS})

add_executable( squelette3carte ../squelette3carte.cpp)
target_link_libraries( squelette3carte
	container topology utils algo  numerical lapack blas f2c ${COMMON_LIBS} )
	
add_executable( decimationVolumique ../decimationVolumique.cpp)
target_link_libraries( decimationVolumique
	container topology utils algo  numerical lapack blas f2c ${COMMON_LIBS} )
	
add_executable( triangulation ../triangulation.cpp)
target_link_libraries( triangulation
	container topology utils algo ${COMMON_LIBS} )

add_executable( ter_meshes ../ter_meshes.cpp)
target_link_libraries( ter_meshes
	container topology utils algo ${COMMON_LIBS} )
	
add_executable( polyhedronsView ../polyhedronsView.cpp)
target_link_libraries( polyhedronsView
        container topology utils algo ${COMMON_LIBS} )

add_executable( extrusionView ../extrusionView.cpp)
target_link_libraries( extrusionView
        container topology utils algo ${COMMON_LIBS} )
        
add_executable( scene ../scene.cpp)
target_link_libraries( scene
        container topology utils algo ${COMMON_LIBS} )
