cmake_minimum_required(VERSION 2.8)

project(Tutos)


#SET (CMAKE_BUILD_TYPE Debug)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOTOPOWARNING")



include_directories(${CMAKE_CURRENT_BINARY_DIR})
#	${CGoGN_ROOT_DIR}/include
#	${CGoGN_EXT_INCLUDES})

# define libs path
#IF(WIN32)
#	link_directories(${CGoGN_ROOT_DIR}/lib/$(ConfigurationName)
#	${Boost_LIBRARY_DIRS})
#ELSE(WIN32)
#	link_directories(${CGoGN_ROOT_DIR}/lib/Release ${CGoGN_ROOT_DIR}/lib/Debug)
#ENDIF(WIN32)

add_subdirectory(Boundary)
add_subdirectory(Modelling)
add_subdirectory(Markers)
add_subdirectory(Traversals)
add_subdirectory(Attributes)

#define exec to compile
IF (WITH_QT)

	QT4_WRAP_CPP(tuto1_moc tuto1.h)
	add_executable( tuto1 tuto1.cpp ${tuto1_moc})
	target_link_libraries( tuto1
		${CGoGN_LIBS}  ${CGoGN_EXT_LIBS} )

	QT4_WRAP_UI( tuto2_ui tuto2.ui )
	QT4_WRAP_CPP(tuto2_moc tuto2.h)
	add_executable( tuto2 tuto2.cpp tuto2.h ${tuto2_ui}  ${tuto2_moc})
	target_link_libraries( tuto2
		${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )

	QT4_WRAP_CPP(tuto3_moc tuto3.h)
	add_executable( tuto3 tuto3.cpp ${tuto3_moc})
	target_link_libraries( tuto3
		${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )

	QT4_WRAP_UI( tuto4_ui tuto4.ui )
	QT4_WRAP_CPP(tuto4_moc tuto4.h)
	add_executable( tuto4 tuto4.cpp ${tuto4_ui} ${tuto4_moc})
	target_link_libraries( tuto4
		${CGoGN_LIBS}  ${CGoGN_EXT_LIBS}  )

	QT4_WRAP_UI( tuto5_ui tuto5.ui )
	QT4_WRAP_CPP(tuto5_moc tuto5.h)
	add_executable( tuto5 tuto5.cpp ${tuto5_ui} ${tuto5_moc})
	target_link_libraries( tuto5
		${CGoGN_LIBS}  ${CGoGN_EXT_LIBS}  )
	
	QT4_WRAP_UI( tuto_orbits_ui tuto_orbits.ui )
	QT4_WRAP_CPP(tuto_orbits_moc tuto_orbits.h)
	add_executable( tuto_orbits tuto_orbits.cpp ${tuto_orbits_ui} ${tuto_orbits_moc})
	target_link_libraries( tuto_orbits
		${CGoGN_LIBS}  ${CGoGN_EXT_LIBS}  )

	QT4_WRAP_CPP(tuto_histo_moc tuto_histo.h)
	add_executable( tuto_histo tuto_histo.cpp tuto_histo.h ${tuto_histo_ui}  ${tuto_histo_moc})
	target_link_libraries( tuto_histo
		${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )

ENDIF()

#QT4_WRAP_UI( show_traversors_ui show_traversors.ui )
#QT4_WRAP_CPP(show_traversors_moc show_traversors.h)
#add_executable( show_traversors show_traversors.cpp ${show_traversors_ui} ${show_traversors_moc})
#target_link_libraries( show_traversors
#	${CGoGN_LIBS}  ${CGoGN_EXT_LIBS}  )

add_executable( tuto_subdivision tuto_subdivision.cpp)
target_link_libraries( tuto_subdivision
	${CGoGN_LIBS}  ${CGoGN_EXT_LIBS} )
	
add_executable( tuto_dual2 tuto_dual2.cpp)
target_link_libraries( tuto_dual2
	${CGoGN_LIBS}  ${CGoGN_EXT_LIBS} )
	
add_executable( tuto_dual3 tuto_dual3.cpp)
target_link_libraries( tuto_dual3
	${CGoGN_LIBS}  ${CGoGN_EXT_LIBS} )

#QT4_WRAP_CPP(tp_master_moc tp_master.h)
#add_executable( tp_master tp_master.cpp ${tp_master_moc})
#target_link_libraries( tp_master
#	${CGoGN_LIBS}  ${CGoGN_EXT_LIBS}  )




#BOOST_LIBS macro: allow using compiled boost in windows (see readme)
# -first argument variable that will contain the libs
# -second argument list of boost libs separated by ; in " "
# exemple: BOOST_LIBS(boost_lib_lists "boost_thread-mt;boost_iostreams-mt")
#BOOST_LIBS(boost_lib_lists "boost_thread-mt")

#QT4_WRAP_CPP(tuto_mt_moc tuto_mt.h)
#add_executable( tuto_mt tuto_mt.cpp  ${tuto_mt_moc})
#target_link_libraries( tuto_mt
#	${CGoGN_LIBS} ${CGoGN_EXT_LIBS} ${Boost_THREAD_LIBRARY})

