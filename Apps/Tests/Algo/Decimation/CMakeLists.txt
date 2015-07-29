cmake_minimum_required(VERSION 2.6)

project(testing_algo_decimation)
	
add_executable( test_algo_decimation
algo_decimation.cpp
colorPerVertexApproximator.cpp
decimation.cpp
edgeSelector.cpp
geometryApproximator.cpp
geometryPredictor.cpp
halfEdgeSelector.cpp
predictor.cpp
)	

target_link_libraries( test_algo_decimation
	${CGoGN_LIBS} ${CGoGN_EXT_LIBS})

