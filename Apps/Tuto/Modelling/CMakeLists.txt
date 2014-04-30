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

add_executable( basics basics.cpp)
target_link_libraries( basics ${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS} )

QT4_WRAP_UI( tuto_oper2_ui tuto_oper2.ui )
QT4_WRAP_CPP(tuto_oper2_moc tuto_oper2.h)
add_executable( tuto_oper2 tuto_oper2.cpp ${tuto_oper2_ui} ${tuto_oper2_moc})
target_link_libraries( tuto_oper2
	${CGoGN_LIBS_D}  ${CGoGN_EXT_LIBS}  )


QT4_WRAP_UI( tuto_oper3_ui tuto_oper3.ui )
QT4_WRAP_CPP(tuto_oper3_moc tuto_oper3.h)
add_executable( tuto_oper3 tuto_oper3.cpp ${tuto_oper3_ui} ${tuto_oper3_moc})
target_link_libraries( tuto_oper3
	${CGoGN_LIBS_D}  ${CGoGN_EXT_LIBS}  )
