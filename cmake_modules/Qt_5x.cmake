
set(DESIRED_QT_VERSION = CGoGN_DESIRED_QT_VERSION)
find_package(Qt5Core REQUIRED)
#Qt5Widgets required for qt5_wrap_ui
find_package(Qt5Widgets REQUIRED)

# aliases (module Widgets included by default)
macro(qt_use_modules)
  qt5_use_modules(${ARGN} Widgets)
endmacro()

# macro that include module need by cgogn
macro(qt_use_cgogn_modules)
  qt5_use_modules(${ARGN} Gui OpenGL Xml Svg Widgets)
endmacro()

macro(qt_wrap_cpp)
  qt5_wrap_cpp(${ARGN})
endmacro()

macro(qt_add_resources)
  qt5_add_resources(${ARGN})
endmacro()

macro(qt_wrap_ui)
  qt5_wrap_ui(${ARGN})
endmacro()

# version
set(QT_VERSION_MAJOR ${Qt5Core_VERSION_MAJOR})
set(QT_VERSION_MINOR ${Qt5Core_VERSION_MINOR})
set(QT_VERSION_PATCH ${Qt5Core_VERSION_PATCH})
