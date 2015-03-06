cmake_minimum_required(VERSION 2.8)

project(Tuto_Attributes)


add_executable( simple_attribs simple_attribs.cpp)
target_link_libraries( simple_attribs ${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )


add_executable( multi_attribs multi_attribs.cpp)
target_link_libraries( multi_attribs ${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )


IF(CGoGN_WITH_QT)

	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	qt_wrap_ui(vbo_attribs_ui vbo_attribs.ui)
	qt_wrap_cpp(vbo_attribs_moc vbo_attribs.h)
	add_executable( vbo_attribs vbo_attribs.cpp ${vbo_attribs_ui} ${vbo_attribs_moc})
	target_link_libraries( vbo_attribs ${CGoGN_LIBS}  ${CGoGN_EXT_LIBS} )

ENDIF()
