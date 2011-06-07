cmake_minimum_required(VERSION 2.6)

project(Tutos)


SET (CMAKE_BUILD_TYPE Debug)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOTOPOWARNING")

# FOR Qt4
FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QTOPENGL TRUE)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})


# define includes path

MESSAGE (STATUS ${BOOST_INCLUDE_PATH})
include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
	/usr/include/libxml2/
	${CGoGN_ROOT_DIR}/ThirdParty/gzstream
	${CGoGN_ROOT_DIR}/ThirdParty/OpenCTM
	${CGoGN_ROOT_DIR}/ThirdParty/Assimp/include
	${CGoGN_ROOT_DIR}/ThirdParty/glm
	${CGoGN_ROOT_DIR}/include
	${BOOST_INCLUDE_PATH}
)

# define libs path
IF(WIN32)
	link_directories(${CGoGN_ROOT_DIR}/lib/$(ConfigurationName))
ELSE(WIN32)
	link_directories(${CGoGN_ROOT_DIR}/lib/Release ${CGoGN_ROOT_DIR}/lib/Debug)
ENDIF(WIN32)


#define exec to compile

QT4_WRAP_CPP(tuto1_moc tuto1.h)
add_executable( tuto1 tuto1.cpp ${tuto1_moc})
target_link_libraries( tuto1
	${CGoGN_LIBS_D}  ${COMMON_LIBS} ${QT_LIBRARIES} )

QT4_WRAP_UI( tuto2_ui tuto2.ui )
QT4_WRAP_CPP(tuto2_moc tuto2.h)
add_executable( tuto2 tuto2.cpp tuto2.h ${tuto2_ui}  ${tuto2_moc})
target_link_libraries( tuto2
	${CGoGN_LIBS_D} ${COMMON_LIBS} ${QT_LIBRARIES} )

QT4_WRAP_CPP(tuto3_moc tuto3.h)
add_executable( tuto3 tuto3.cpp ${tuto3_moc})
target_link_libraries( tuto3
	${CGoGN_LIBS_D} ${COMMON_LIBS}  ${QT_LIBRARIES})

QT4_WRAP_UI( tuto4_ui tuto4.ui )
QT4_WRAP_CPP(tuto4_moc tuto4.h)
add_executable( tuto4 tuto4.cpp ${tuto4_ui} ${tuto4_moc})
target_link_libraries( tuto4
	${CGoGN_LIBS_D}  ${COMMON_LIBS}  ${QT_LIBRARIES} )

QT4_WRAP_UI( tuto5_ui tuto5.ui )
QT4_WRAP_CPP(tuto5_moc tuto5.h)
add_executable( tuto5 tuto5.cpp ${tuto5_ui} ${tuto5_moc})
target_link_libraries( tuto5
	${CGoGN_LIBS_D}  ${COMMON_LIBS}  ${QT_LIBRARIES} )


QT4_WRAP_UI( stage_shader_ui stage_shader.ui )
QT4_WRAP_CPP(stage_shader_moc stage_shader.h)
add_executable( stage_shader stage_shader.cpp ${stage_shader_ui} ${stage_shader_moc})
target_link_libraries( stage_shader
	${CGoGN_LIBS_D}  ${COMMON_LIBS}  ${QT_LIBRARIES} )


#add_executable( tuto_subdivision tuto_subdivision.cpp)
#target_link_libraries( tuto_subdivision
#	${CGoGN_LIBS_D}  ${COMMON_LIBS} )

QT4_WRAP_CPP(tp_master_moc tp_master.h)
add_executable( tp_master tp_master.cpp ${tp_master_moc})
target_link_libraries( tp_master
	${CGoGN_LIBS_D}  ${COMMON_LIBS}  ${QT_LIBRARIES} )

#BOOST_LIBS macro: allow using compiled boost in windows (see readme)
# -first argument variable that will contain the libs
# -second argument list of boost libs separated by ; in " "
# exemple: BOOST_LIBS(boost_lib_lists "boost_thread-mt;boost_iostreams-mt")
BOOST_LIBS(boost_lib_lists "boost_thread-mt")

QT4_WRAP_CPP(tuto_mt_moc tuto_mt.h)
add_executable( tuto_mt tuto_mt.cpp  ${tuto_mt_moc})
target_link_libraries( tuto_mt
	${CGoGN_LIBS_D} ${COMMON_LIBS} ${QT_LIBRARIES} ${boost_lib_lists})

QT4_WRAP_CPP(tuto_ogl3_moc tuto_ogl3.h)
add_executable( tuto_ogl3 tuto_ogl3.cpp ${tuto_ogl3_moc})
target_link_libraries( tuto_ogl3
	${CGoGN_LIBS_D}  ${COMMON_LIBS} ${QT_LIBRARIES} )



#add_executable( tuto_ogl2 tuto_ogl2.cpp)
#target_link_libraries( tuto_ogl2
#	${CGoGN_LIBS_D}  ${COMMON_LIBS} )
