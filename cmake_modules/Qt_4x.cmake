
set(minimum_required_qt_version "4.6.2")

set(DESIRED_QT_VERSION = CGoGN_DESIRED_QT_VERSION)
find_package(Qt4 REQUIRED)

set(found_qt_version ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH})

if(${found_qt_version} VERSION_LESS ${minimum_required_qt_version})
        message(FATAL_ERROR "error: CGoGN requires Qt >= ${minimum_required_qt_version} -- you cannot use Qt ${found_qt_version}.")
endif()

# qt5_use_modules port
macro(qt_use_modules _target _link_type)
	if(NOT TARGET ${_target})
		message(FATAL_ERROR "The first argument to qt_use_modules must be an existing target.")
	endif()
	if("${_link_type}" STREQUAL "LINK_PUBLIC" OR "${_link_type}" STREQUAL "LINK_PRIVATE" )
		set(_qt4_modules ${ARGN})
		set(_qt4_link_type ${_link_type})
	else()
		set(_qt4_modules ${_link_type} ${ARGN})
	endif()

	if("${_qt4_modules}" STREQUAL "")
		message(FATAL_ERROR "qt_use_modules requires at least one Qt module to use.")
	endif()

	foreach(_module5 ${_qt4_modules})
		string(TOUPPER ${_module5} _module)
		if(NOT QT_QT${_module}_FOUND)
			message(FATAL_ERROR "qt_use_modules(): QT_QT${_module} *not* FOUND.")
		endif()
		set(QT_USE_QT${_module} 1)
	endforeach()

	include(${QT_USE_FILE})
	target_link_libraries(${_target} ${_qt4_link_type} ${QT_LIBRARIES})
	set_property(TARGET ${_target} APPEND PROPERTY INCLUDE_DIRECTORIES ${QT_INCLUDE_DIRS})
	set_property(TARGET ${_target} APPEND PROPERTY COMPILE_DEFINITIONS ${QT_COMPILE_DEFINITIONS})
	set_property(TARGET ${_target} APPEND PROPERTY COMPILE_DEFINITIONS_RELEASE QT_NO_DEBUG)

	if(Qt_POSITION_INDEPENDENT_CODE)
		set_property(TARGET ${_target} PROPERTY POSITION_INDEPENDENT_CODE ${Qt_POSITION_INDEPENDENT_CODE})
	endif()
endmacro()

# macro that include module need by cgogn
macro(qt_use_cgogn_modules _target )
  qt_use_modules(${_target} Gui OpenGL Xml Svg)
endmacro()

# aliases to make it work across Qt4/Qt5
macro(qt_add_resources)
  qt4_add_resources(${ARGN})
endmacro()

macro(qt_wrap_cpp)
  qt4_wrap_cpp(${ARGN})
endmacro()

macro(qt_wrap_ui)
  qt4_wrap_ui(${ARGN})
endmacro()
