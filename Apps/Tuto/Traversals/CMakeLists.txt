cmake_minimum_required(VERSION 2.8)

project(Tuto_Traversals)


SET (CMAKE_BUILD_TYPE Debug)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOTOPOWARNING")



include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
	${CGoGN_ROOT_DIR}/include
	${CGoGN_EXT_INCLUDES}
)

# define libs path
IF(WIN32)
	link_directories(${CGoGN_ROOT_DIR}/lib/$(ConfigurationName)
	${Boost_LIBRARY_DIRS})
ELSE(WIN32)
	link_directories(${CGoGN_ROOT_DIR}/lib/Release ${CGoGN_ROOT_DIR}/lib/Debug)
ENDIF(WIN32)


#define exec to compile

#QT4_WRAP_CPP(tuto1_moc tuto1.h)
#add_executable( tuto1 tuto1.cpp ${tuto1_moc})
#target_link_libraries( tuto1
#	${CGoGN_LIBS_D}  ${CGoGN_EXT_LIBS} )

#QT4_WRAP_UI( tuto2_ui tuto2.ui )
#QT4_WRAP_CPP(tuto2_moc tuto2.h)
#add_executable( tuto2 tuto2.cpp tuto2.h ${tuto2_ui}  ${tuto2_moc})
#target_link_libraries( tuto2
#	${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS} )

add_executable( traverse_cells traverse_cells.cpp)
target_link_libraries( traverse_cells ${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS} )

add_executable( traverse_neighbours traverse_neighbours.cpp)
target_link_libraries( traverse_neighbours ${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS} )

QT4_WRAP_UI( show_traversors_ui2 show_traversors2.ui )
QT4_WRAP_CPP(show_traversors_moc2 show_traversors2.h)
add_executable( show_traversors2 show_traversors2.cpp ${show_traversors_ui2} ${show_traversors_moc2})
target_link_libraries( show_traversors2 ${CGoGN_LIBS_D}  ${CGoGN_EXT_LIBS}  )

QT4_WRAP_UI( show_traversors_ui3 show_traversors3.ui )
QT4_WRAP_CPP(show_traversors_moc3 show_traversors3.h)
add_executable( show_traversors3 show_traversors3.cpp ${show_traversors_ui3} ${show_traversors_moc3})
target_link_libraries( show_traversors3 ${CGoGN_LIBS_D}  ${CGoGN_EXT_LIBS}  )

