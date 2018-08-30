PROJECT(QGLViewer)
FIND_PACKAGE(OpenGL REQUIRED)

# in order to be able to include generated files (ui and moc)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(HEADER_FILES

	qglviewer.h 
	camera.h 
	manipulatedFrame.h 
	manipulatedCameraFrame.h 
	frame.h 
	keyFrameInterpolator.h 
	)
	
set(SOURCE_FILES

	qglviewer.cpp 
	camera.cpp 
	manipulatedFrame.cpp 
	manipulatedCameraFrame.cpp 
	frame.cpp 
	saveSnapshot.cpp 
	constraint.cpp 
	keyFrameInterpolator.cpp 
	mouseGrabber.cpp 
	quaternion.cpp 
	vec.cpp
	
	VRender/BackFaceCullingOptimizer.cpp
	VRender/EPSExporter.cpp
	VRender/FIGExporter.cpp
	VRender/NVector3.cpp
	VRender/Primitive.cpp
	VRender/TopologicalSortMethod.cpp
	VRender/Vector3.cpp
	VRender/VRender.cpp
	VRender/BSPSortMethod.cpp
	VRender/Exporter.cpp
	VRender/gpc.cpp
	VRender/ParserGL.cpp
	VRender/PrimitivePositioning.cpp
	VRender/Vector2.cpp
	VRender/VisibilityOptimizer.cpp
	)

set(UI_FILES
	ImageInterface.ui
	VRenderInterface.ui
	)	
	
qt_wrap_cpp(MOC_FILES ${HEADER_FILES})
qt_wrap_ui(FORM_FILES ${UI_FILES})


# make mocced header / form files path from absolute to relative
ToRelativePath(MOC_FILES "${CMAKE_CURRENT_SOURCE_DIR}" "${MOC_FILES}")
ToRelativePath(MOC_UI_FILES "${CMAKE_CURRENT_SOURCE_DIR}" "${MOC_UI_FILES}")
ToRelativePath(FORM_FILES "${CMAKE_CURRENT_SOURCE_DIR}" "${FORM_FILES}")

# these header files do not need MOCcing
list(APPEND HEADER_FILES
	config.h
	constraint.h 
	domUtils.h 
	mouseGrabber.h 
	quaternion.h 
	vec.h 
	VRender/AxisAlignedBox.h
	VRender/Exporter.h
	VRender/gpc.h
	VRender/NVector3.h
	VRender/Optimizer.h
	VRender/ParserGL.h
	VRender/Primitive.h
	VRender/PrimitivePositioning.h
	VRender/SortMethod.h
	VRender/Types.h
	VRender/Vector2.h
	VRender/Vector3.h
	VRender/VRender.h
	)

set(MOC_FILES_GROUP "Generated")
set(MOC_UI_FILES_GROUP "Generated")
set(FORM_FILES_GROUP "Generated")

find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5Widgets)
find_package(Qt5OpenGL)
find_package(Qt5Xml)
find_package(Qt5Svg)
include_directories(".." ${Qt5OpenGL_INCLUDE_DIRS} ${Qt5Xml_INCLUDE_DIRS})


if(WIN32)
    ADD_DEFINITIONS(-DCREATE_QGLVIEWER_DLL -DNOMINMAX)
else()
	ADD_DEFINITIONS(-fPIC)
endif() 


add_library(QGLViewer SHARED ${HEADER_FILES} ${MOC_FILES} ${MOC_UI_FILES} ${FORM_FILES} ${SOURCE_FILES})
target_link_libraries(QGLViewer  ${Qt5OpenGL_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Svg_LIBRARIES} ${OPENGL_LIBRARY}  )
qt_use_modules(QGLViewer Core OpenGL Xml Svg Gui)
IF(${CGoGN_DESIRED_QT_VERSION} STREQUAL "5")
	qt_use_modules(QGLViewer Widgets)
ENDIF()




