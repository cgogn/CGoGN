cmake_minimum_required(VERSION 2.8)

project(Tutos)

#define exec to compile
IF (CGoGN_WITH_QT)

	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	qt_wrap_cpp(tuto1_moc tuto1.h)
	add_executable( tuto1 tuto1.cpp ${tuto1_moc})
	target_link_libraries( tuto1
		${CGoGN_LIBS}  ${CGoGN_EXT_LIBS} )
	qt_use_cgogn_modules(tuto1)

	qt_wrap_ui( tuto2_ui tuto2.ui )
	qt_wrap_cpp(tuto2_moc tuto2.h)
	add_executable( tuto2 tuto2.cpp tuto2.h ${tuto2_ui}  ${tuto2_moc})
	target_link_libraries( tuto2
		${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )

	qt_wrap_cpp(tuto3_moc tuto3.h)
	add_executable( tuto3 tuto3.cpp ${tuto3_moc})
	target_link_libraries( tuto3
		${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )

	qt_wrap_ui( tuto4_ui tuto4.ui )
	qt_wrap_cpp(tuto4_moc tuto4.h)
	add_executable( tuto4 tuto4.cpp ${tuto4_ui} ${tuto4_moc})
	target_link_libraries( tuto4
		${CGoGN_LIBS}  ${CGoGN_EXT_LIBS}  )

	qt_wrap_ui( tuto5_ui tuto5.ui )
	qt_wrap_cpp(tuto5_moc tuto5.h)
	add_executable( tuto5 tuto5.cpp ${tuto5_ui} ${tuto5_moc})
	target_link_libraries( tuto5
		${CGoGN_LIBS}  ${CGoGN_EXT_LIBS}  )
	
	qt_wrap_ui( tuto_orbits_ui tuto_orbits.ui )
	qt_wrap_cpp(tuto_orbits_moc tuto_orbits.h)
	add_executable( tuto_orbits tuto_orbits.cpp ${tuto_orbits_ui} ${tuto_orbits_moc})
	target_link_libraries( tuto_orbits
		${CGoGN_LIBS}  ${CGoGN_EXT_LIBS}  )

	qt_wrap_cpp(tuto_histo_moc tuto_histo.h)
	add_executable( tuto_histo tuto_histo.cpp tuto_histo.h ${tuto_histo_ui}  ${tuto_histo_moc})
	target_link_libraries( tuto_histo
		${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )


#	qt_wrap_cpp(tp_master_moc tp_master.h)
#	add_executable( tp_master tp_master.cpp ${tp_master_moc})
#	target_link_libraries( tp_master ${CGoGN_LIBS}  ${CGoGN_EXT_LIBS}  )


ENDIF()

add_executable( tuto_subdivision tuto_subdivision.cpp)
target_link_libraries( tuto_subdivision
	${CGoGN_LIBS}  ${CGoGN_EXT_LIBS} )
	
add_executable( tuto_dual2 tuto_dual2.cpp)
target_link_libraries( tuto_dual2
	${CGoGN_LIBS}  ${CGoGN_EXT_LIBS} )
	
add_executable( tuto_dual3 tuto_dual3.cpp)
target_link_libraries( tuto_dual3
	${CGoGN_LIBS}  ${CGoGN_EXT_LIBS} )

add_subdirectory(Boundary)
add_subdirectory(Modelling)
add_subdirectory(Markers)
add_subdirectory(Traversals)
add_subdirectory(Attributes)