cmake_minimum_required(VERSION 2.8)

project(SandBox)

add_executable(multi_att multi_att.cpp)
target_link_libraries(multi_att ${CGoGN_LIBS} ${CGoGN_EXT_LIBS})

add_executable(trav3_test trav3_test.cpp)
target_link_libraries(trav3_test ${CGoGN_LIBS} ${CGoGN_EXT_LIBS})

IF (CGoGN_WITH_QT)

	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	qt_wrap_cpp(tilings_moc tilings.h)
	add_executable(tilings tilings.cpp  ${tilings_moc})
	target_link_libraries(tilings ${CGoGN_LIBS} ${CGoGN_EXT_LIBS})
	qt_use_cgogn_modules(tilings)

	qt_wrap_ui( test_traversor2_ui test_traversor2.ui )
	qt_wrap_cpp(test_traversor2_moc test_traversor2.h)
	add_executable(test_traversor2 test_traversor2.cpp  ${test_traversor2_ui} ${test_traversor2_moc})
	target_link_libraries(test_traversor2 ${CGoGN_LIBS} ${CGoGN_EXT_LIBS})


	qt_wrap_ui( testPlaneCutting_ui testPlaneCutting.ui )
	qt_wrap_cpp(testPlaneCutting_moc testPlaneCutting.h)
	add_executable(testPlaneCutting testPlaneCutting.cpp  ${testPlaneCutting_ui} ${testPlaneCutting_moc})
	target_link_libraries(testPlaneCutting ${CGoGN_LIBS} ${CGoGN_EXT_LIBS})

	qt_wrap_cpp(wallPaper_moc wallPaper.h)
	add_executable(wallPaper wallPaper.cpp  ${wallPaper_moc})
	target_link_libraries(wallPaper ${CGoGN_LIBS} ${CGoGN_EXT_LIBS})

ENDIF()

# Example with Qt
#
#qt_wrap_cpp(tuto1_moc tuto1.h)
#add_executable( tuto1 tuto1.cpp ${tuto1_moc})
#target_link_libraries( tuto1 ${CGoGN_LIBS}  ${CGoGN_EXT_LIBS} )

