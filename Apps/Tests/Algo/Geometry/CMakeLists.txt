cmake_minimum_required(VERSION 2.6)

project(testing_algo_geometry)
	
add_executable( test_algo_geometry 
algo_geometry.cpp
area.cpp
basic.cpp
boundingbox.cpp
centroid.cpp
convexity.cpp
curvature.cpp
distances.cpp
feature.cpp
inclusion.cpp
intersection.cpp
laplacian.cpp
localFrame.cpp
normal.cpp
normalization.cpp
orientation.cpp
plane.cpp
stats.cpp
volume.cpp
voronoiDiagrams.cpp 
)	
	
target_link_libraries( test_algo_geometry 
	${CGoGN_LIBS} ${CGoGN_EXT_LIBS})

