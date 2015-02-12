cmake_minimum_required(VERSION 2.8)

project(Tuto_Modelling)


add_executable( basics basics.cpp)
target_link_libraries( basics ${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )

IF (CGoGN_WITH_QT)

	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	QT4_WRAP_UI( tuto_oper2_ui tuto_oper2.ui )
	QT4_WRAP_CPP(tuto_oper2_moc tuto_oper2.h)
	add_executable( tuto_oper2 tuto_oper2.cpp ${tuto_oper2_ui} ${tuto_oper2_moc})
	target_link_libraries( tuto_oper2
		${CGoGN_LIBS}  ${CGoGN_EXT_LIBS}  )

	QT4_WRAP_UI( tuto_oper3_ui tuto_oper3.ui )
	QT4_WRAP_CPP(tuto_oper3_moc tuto_oper3.h)
	add_executable( tuto_oper3 tuto_oper3.cpp ${tuto_oper3_ui} ${tuto_oper3_moc})
	target_link_libraries( tuto_oper3
		${CGoGN_LIBS}  ${CGoGN_EXT_LIBS}  )
ENDIF()