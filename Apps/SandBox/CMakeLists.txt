cmake_minimum_required(VERSION 2.8)

project(SandBox)


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

add_executable(multi_att multi_att.cpp)
target_link_libraries(multi_att ${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS})

QT4_WRAP_CPP(tilings_moc tilings.h)
add_executable(tilings tilings.cpp  ${tilings_moc})
target_link_libraries(tilings ${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS})

# Example with Qt
#
#QT4_WRAP_CPP(tuto1_moc tuto1.h)
#add_executable( tuto1 tuto1.cpp ${tuto1_moc})
#target_link_libraries( tuto1 ${CGoGN_LIBS_D}  ${CGoGN_EXT_LIBS} )


#QT4_WRAP_CPP(tuto_mt_moc tuto_mt.h)
#add_executable( tuto_mt tuto_mt.cpp  ${tuto_mt_moc})
#target_link_libraries( tuto_mt
#	${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS} ${Boost_THREAD_LIBRARY})

