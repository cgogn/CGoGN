cmake_minimum_required(VERSION 2.8)

project(examples)


# define includes path
include_directories(${CMAKE_CURRENT_BINARY_DIR})


#define exec to compile

qt_wrap_cpp( frame_manip_moc frame_manip.h )
add_executable( frame_manip frame_manip.cpp ${frame_manip_moc} )
target_link_libraries( frame_manip ${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )

qt_use_modules(frame_manip Gui OpenGL Xml Svg)
#IF(${CGoGN_DESIRED_QT_VERSION} STREQUAL "5")
#	qt_use_modules(frame_manip Widgets)
#ENDIF()
		
qt_wrap_cpp( texturesExample_moc texturesExample.h )
add_executable( texturesExample texturesExample.cpp ${texturesExample_moc} )
target_link_libraries( texturesExample ${CGoGN_LIBS} ${CGoGN_EXT_LIBS}  )

qt_wrap_cpp(simpleMap3_moc simpleMap3.h)
add_executable( simpleMap3 simpleMap3.cpp ${simpleMap3_moc} )
target_link_libraries( simpleMap3 ${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )

qt_wrap_cpp( simpleGMap2_moc simpleGMap2.h )
add_executable( simpleGMap2 simpleGMap2.cpp ${simpleGMap2_moc} )
target_link_libraries( simpleGMap2 ${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )

qt_wrap_cpp( simpleGMap3_moc simpleGMap3.h )
add_executable( simpleGMap3 simpleGMap3.cpp ${simpleGMap3_moc} )
target_link_libraries( simpleGMap3 ${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )
                
qt_wrap_cpp( extrusionView_moc extrusionView.h )
add_executable( extrusionView extrusionView.cpp ${extrusionView_moc} )
target_link_libraries( extrusionView ${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )

qt_wrap_ui( viewer_ui viewer.ui )
qt_wrap_cpp( viewer_moc viewer.h )
add_executable( viewer viewer.cpp ${viewer_moc} ${viewer_ui} )
target_link_libraries( viewer ${CGoGN_LIBS} ${NUMERICAL_LIBS} ${CGoGN_EXT_LIBS} )

qt_wrap_ui( clipping_ui clipping.ui )
qt_wrap_cpp( clipping_moc clipping.h )
add_executable( clipping clipping.cpp ${clipping_ui} ${clipping_moc} )
target_link_libraries( clipping	${CGoGN_LIBS}  ${CGoGN_EXT_LIBS} )

qt_wrap_ui( volumeExplorer_ui volumeExplorer.ui )
qt_wrap_cpp( volumeExplorer_moc volumeExplorer.h )
add_executable( volumeExplorer volumeExplorer.cpp ${volumeExplorer_ui} ${volumeExplorer_moc} )
target_link_libraries( volumeExplorer ${CGoGN_LIBS}  ${CGoGN_EXT_LIBS} )

qt_wrap_ui( mcmesh_ui mcmesh.ui )
qt_wrap_cpp( mcmesh_moc mcmesh.h )
add_executable( mcmesh mcmesh.cpp ${mcmesh_moc} ${mcmesh_ui} )
target_link_libraries( mcmesh ${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )

qt_wrap_cpp( concave_rendering_moc concave_rendering.h )
add_executable( concave_rendering concave_rendering.cpp ${concave_rendering_moc} ${concave_rendering_ui} )
target_link_libraries( concave_rendering ${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )

qt_wrap_cpp( viewerOBJ_moc viewerOBJ.h )
add_executable( viewerOBJ viewerOBJ.cpp ${viewerOBJ_moc} )
target_link_libraries( viewerOBJ ${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )
