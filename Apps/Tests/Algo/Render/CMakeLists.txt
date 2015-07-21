cmake_minimum_required(VERSION 2.6)

project(testing_algo_render)
	
add_executable( test_algo_render
algo_render.cpp
GL2/colorPerEdgeRender.cpp
GL2/colorPerFaceRender.cpp
GL2/dataPerFaceRender.cpp
GL2/drawerCells.cpp
GL2/explodeVolumeRender.cpp
GL2/mapRender.cpp
GL2/topo3PrimalRender.cpp
GL2/topo3Render.cpp
GL2/topoPrimalRender.cpp
GL2/topoRender.cpp
SVG/mapSVGRender.cpp
)	

target_link_libraries( test_algo_render 
	${CGoGN_LIBS} ${CGoGN_EXT_LIBS})


