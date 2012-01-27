cmake_minimum_required(VERSION 2.8)

project(examples)

SET(CMAKE_BUILD_TYPE Release)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOTOPOWARNING")


# define includes path
include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
	${CGoGN_ROOT_DIR}/include
	${CGoGN_EXT_INCLUDES}
)

# define libs path
IF(WIN32)
	link_directories( ${CGoGN_ROOT_DIR}/lib/$(ConfigurationName)  ${Boost_LIBRARY_DIRS})
ELSE(WIN32)
	link_directories( ${CGoGN_ROOT_DIR}/lib/Release )
ENDIF(WIN32)


#define exec to compile

QT4_WRAP_CPP(polyhedronsView_moc ../polyhedronsView.h)	
add_executable( polyhedronsView ../polyhedronsView.cpp ${polyhedronsView_moc})
target_link_libraries( polyhedronsView
		${CGoGN_LIBS_R} ${CGoGN_EXT_LIBS}  )

QT4_WRAP_CPP(frame_manip_moc ../frame_manip.h)	
add_executable( frame_manip ../frame_manip.cpp ${frame_manip_moc})
target_link_libraries( frame_manip
		${CGoGN_LIBS_R} ${CGoGN_EXT_LIBS}  )
		
QT4_WRAP_CPP(texturesExample_moc ../texturesExample.h)  
add_executable( texturesExample ../texturesExample.cpp ${texturesExample_moc} )
target_link_libraries( texturesExample
		${CGoGN_LIBS_R} ${CGoGN_EXT_LIBS} )

QT4_WRAP_CPP(simpleGMap2_moc ../simpleGMap2.h)  
add_executable( simpleGMap2 ../simpleGMap2.cpp ${simpleGMap2_moc} )
target_link_libraries( simpleGMap2
                ${CGoGN_LIBS_R} ${CGoGN_EXT_LIBS} )

QT4_WRAP_CPP(simpleGMap3_moc ../simpleGMap3.h)  
add_executable( simpleGMap3 ../simpleGMap3.cpp ${simpleGMap3_moc} )
target_link_libraries( simpleGMap3
                ${CGoGN_LIBS_R} ${CGoGN_EXT_LIBS} )
                
QT4_WRAP_CPP(extrusionView_moc ../extrusionView.h)	
add_executable( extrusionView ../extrusionView.cpp ${extrusionView_moc})
target_link_libraries( extrusionView
		${CGoGN_LIBS_R} ${CGoGN_EXT_LIBS}  )

QT4_WRAP_UI( viewer_ui ../viewer.ui )
QT4_WRAP_CPP( viewer_moc ../viewer.h )
add_executable( viewer ../viewer.cpp ${viewer_moc} ${viewer_ui})
target_link_libraries( viewer
	${CGoGN_LIBS_R} ${NUMERICAL_LIBS} ${CGoGN_EXT_LIBS} )

QT4_WRAP_UI( clipping_ui ../clipping.ui )
QT4_WRAP_CPP( clipping_moc ../clipping.h )
add_executable( clipping ../clipping.cpp ${clipping_ui} ${clipping_moc})
target_link_libraries( clipping
	${CGoGN_LIBS_R}  ${CGoGN_EXT_LIBS}  )

QT4_WRAP_UI( volumeExplorer_ui ../volumeExplorer.ui )
QT4_WRAP_CPP( volumeExplorer_moc ../volumeExplorer.h )
add_executable( volumeExplorer ../volumeExplorer.cpp ${volumeExplorer_ui} ${volumeExplorer_moc})
target_link_libraries( volumeExplorer
	${CGoGN_LIBS_R}  ${CGoGN_EXT_LIBS}  )	
	