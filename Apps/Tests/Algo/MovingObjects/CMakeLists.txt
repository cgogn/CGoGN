cmake_minimum_required(VERSION 2.6)

project(testing_algo_movingObjects)
	
add_executable( test_algo_movingObjects 
algo_movingObjects.cpp
particle_cell_2D.cpp
particle_cell_2D_memo.cpp
particle_cell_2D_secured.cpp
particle_cell_2DandHalf.cpp
particle_cell_2DandHalf_memo.cpp
particle_cell_3D.cpp
)	

target_link_libraries( test_algo_movingObjects 
	${CGoGN_LIBS} ${CGoGN_EXT_LIBS})
