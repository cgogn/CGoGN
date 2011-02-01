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

add_executable( first ../first.cpp)
target_link_libraries( first
	container topology utils algo  ${COMMON_LIBS} )

add_executable( Viewer ../Viewer.cpp)
target_link_libraries( Viewer
	container topology utils algo  ${COMMON_LIBS} )

add_executable( FilterExamples ../FilterExamples.cpp)
target_link_libraries( FilterExamples
	container topology utils algo numerical lapack blas f2c ${COMMON_LIBS} )

#add_executable( SimpleEMap3 ../simpleEMap3.cpp)
#target_link_libraries( SimpleEMap3
#	container topology utils algo ${COMMON_LIBS} )
 
#add_executable( movingObjects ../movingObjects_Unconstrained-Unoptimized.cpp)
# target_link_libraries( movingObjects
# 	container topology utils algo ${COMMON_LIBS} )

# add_executable( MeshVBO ../MeshVBO.cpp)
# target_link_libraries( MeshVBO
# 	topology utils algo ${COMMON_LIBS} )

# add_executable( MeshViewer ../MeshViewer.cpp)
# target_link_libraries( MeshViewer
# 	topology utils algo gzstream ${COMMON_LIBS} )

# add_executable( primView ../primView.cpp)
# target_link_libraries( primView
# 	topology utils algo ${COMMON_LIBS} )

# add_executable( test_geo3d ../test_geo3d.cpp)
# target_link_libraries( test_geo3d
# 	topology utils algo ${COMMON_LIBS} gmp)

# add_executable( vox2mesh ../vox2mesh.cpp)
# target_link_libraries( vox2mesh
# 	topology utils algo ${COMMON_LIBS} Zinri )

# add_executable( topoSelect ../topoSelect.cpp)
# target_link_libraries( topoSelect
# 	topology utils algo ${COMMON_LIBS} )

# add_executable( MC_Examples ../MC_Examples.cpp)
# target_link_libraries( MC_Examples
# 	topology utils algo ${COMMON_LIBS} Zinri )

# add_executable( volumeExample ../volumeExample.cpp)
# target_link_libraries( volumeExample
# 	topology utils algo ${COMMON_LIBS} )

add_executable( pmView ../pmView.cpp)
target_link_libraries( pmView
	container topology utils algo numerical lapack blas f2c ${COMMON_LIBS} )

# add_executable( modeler ../modeler.cpp)
# target_link_libraries( modeler
# 	topology utils algo ${COMMON_LIBS} )

#add_executable( volumeViewer ../volumeViewer.cpp)
#target_link_libraries( volumeViewer
#	topology utils algo ${COMMON_LIBS} )

add_executable( linearTest ../linearTest.cpp)
target_link_libraries( linearTest
	container topology utils algo numerical lapack blas f2c ${COMMON_LIBS} )

add_executable( simpleGMap2 ../simpleGMap2.cpp)
target_link_libraries( simpleGMap2
	container topology utils algo ${COMMON_LIBS} Zinri)

# add_executable( simpleXMap ../simpleXMap.cpp)
# target_link_libraries( simpleXMap
# 	topology utils algo ${COMMON_LIBS} Zinri)

# add_executable( objTexViewer ../objTexViewer.cpp)
# target_link_libraries( objTexViewer
# 	topology utils algo gzstream ${COMMON_LIBS} )

# add_executable( Viz3Map ../Viz3Map.cpp)
# target_link_libraries( Viz3Map
#     topology utils algo ${COMMON_LIBS} Zinri)

# add_executable( volumeTopoDraw ../volumeTopoDraw.cpp)
# target_link_libraries( volumeTopoDraw
# 	topology utils algo ${COMMON_LIBS})

# add_executable( hm_convexhull ../hm_convexhull.cpp)
# target_link_libraries( hm_convexhull
# 	topology utils algo ${COMMON_LIBS})

add_executable( squelette3carte ../squelette3carte.cpp)
target_link_libraries( squelette3carte
	container topology utils algo  numerical lapack blas f2c ${COMMON_LIBS} )
	
add_executable( decimationVolumique ../decimationVolumique.cpp)
target_link_libraries( decimationVolumique
	container topology utils algo  numerical lapack blas f2c ${COMMON_LIBS} )
	
add_executable( ihmView ../ihmView.cpp)
target_link_libraries( ihmView
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
