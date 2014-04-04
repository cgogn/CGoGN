cmake_minimum_required(VERSION 2.8)

project(examplesD)

SET(CMAKE_BUILD_TYPE Debug)

# define includes path
include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
	${CGoGN_ROOT_DIR}/include
	${CGoGN_EXT_INCLUDES}
)

# define libs path
link_directories(
	${CGoGN_ROOT_DIR}/lib/Debug
	${CGoGN_ROOT_DIR}/lib/Release
	${Boost_LIBRARY_DIRS}
)
IF(WIN32)
	link_directories( ${CGoGN_ROOT_DIR}/lib/$(ConfigurationName)
	${Boost_LIBRARY_DIRS})
ELSE(WIN32)
	link_directories( ${CGoGN_ROOT_DIR}/lib/Debug  ${CGoGN_ROOT_DIR}/lib/Release )
ENDIF(WIN32)

#define exec to compile

QT4_WRAP_CPP(frame_manip_moc ../frame_manip.h)  
add_executable( frame_manipD ../frame_manip.cpp ${frame_manip_moc})
target_link_libraries( frame_manipD ${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS}  )

QT4_WRAP_CPP(texturesExample_moc ../texturesExample.h)  
add_executable( texturesExampleD ../texturesExample.cpp ${texturesExample_moc} )
target_link_libraries( texturesExampleD ${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS} )

QT4_WRAP_CPP(simpleMap3_moc ../simpleMap3.h)  
add_executable( simpleMap3D ../simpleMap3.cpp ${simpleMap3_moc} )
target_link_libraries( simpleMap3D ${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS} )
                
QT4_WRAP_CPP(simpleGMap2_moc ../simpleGMap2.h)  
add_executable( simpleGMap2D ../simpleGMap2.cpp ${simpleGMap2_moc} )
target_link_libraries( simpleGMap2D ${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS} )

QT4_WRAP_CPP(simpleGMap3_moc ../simpleGMap3.h)  
add_executable( simpleGMap3D ../simpleGMap3.cpp ${simpleGMap3_moc} )
target_link_libraries( simpleGMap3D ${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS} )

QT4_WRAP_CPP(extrusionView_moc ../extrusionView.h)	
add_executable( extrusionViewD ../extrusionView.cpp ${extrusionView_moc})
target_link_libraries( extrusionViewD ${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS} )

QT4_WRAP_UI( viewer_ui ../viewer.ui )
QT4_WRAP_CPP( viewer_moc ../viewer.h )
add_executable( viewerD ../viewer.cpp ${viewer_moc} ${viewer_ui})
target_link_libraries( viewerD ${CGoGN_LIBS_D} ${NUMERICAL_LIBS} ${CGoGN_EXT_LIBS} )

QT4_WRAP_UI( clipping_ui ../clipping.ui )
QT4_WRAP_CPP( clipping_moc ../clipping.h )
add_executable( clippingD ../clipping.cpp ${clipping_ui} ${clipping_moc})
target_link_libraries( clippingD ${CGoGN_LIBS_D}  ${CGoGN_EXT_LIBS}  )

QT4_WRAP_UI( volumeExplorer_ui ../volumeExplorer.ui )
QT4_WRAP_CPP( volumeExplorer_moc ../volumeExplorer.h )
add_executable( volumeExplorerD ../volumeExplorer.cpp ${volumeExplorer_ui} ${volumeExplorer_moc})
target_link_libraries( volumeExplorerD ${CGoGN_LIBS_D}  ${CGoGN_EXT_LIBS} )
	
QT4_WRAP_UI( mcmesh_ui ../mcmesh.ui )
QT4_WRAP_CPP( mcmesh_moc ../mcmesh.h )
add_executable( mcmeshD ../mcmesh.cpp ${mcmesh_moc}	${mcmesh_ui} )
target_link_libraries( mcmeshD ${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS} )

QT4_WRAP_CPP( concave_rendering_moc ../concave_rendering.h )
add_executable( concave_renderingD ../concave_rendering.cpp ${concave_rendering_moc}	${concave_rendering_ui} )
target_link_libraries( concave_renderingD ${CGoGN_LIBS_D} ${CGoGN_EXT_LIBS} )

QT4_WRAP_CPP( viewerOBJ_moc ../viewerOBJ.h )
add_executable( viewerOBJD ../viewerOBJ.cpp ${viewerOBJ_moc} )
target_link_libraries( viewerOBJD ${CGoGN_LIBS_R} ${CGoGN_EXT_LIBS} )
