#-----------------------------------------------------------------------------
# NOTE: The CMake files have been contributed to PythonQt and have not been tested with the current
# PythonQt version. They have not yet been updated to support Qt 5 and/or Python 3.
#
# If you are not a CMake expert, you should better use the provided qmake profiles.
#-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8.11)
#-----------------------------------------------------------------------------
project(PythonQt)
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Python libraries

find_package(PythonLibs 2.7 REQUIRED)
include_directories("${PYTHON_INCLUDE_DIR}")
add_definitions("-DPYTHONQT_USE_RELEASE_PYTHON_FALLBACK")

#-----------------------------------------------------------------------------
# Build options

option(PythonQt_Wrap_QtAll "Make all Qt components available in python" OFF)

set(qtlibs core gui network opengl sql svg uitools webkit xml xmlpatterns)
foreach(qtlib ${qtlibs})
  OPTION(PythonQt_Wrap_Qt${qtlib} "Make all of Qt${qtlib} available in python" OFF)
endforeach()

# Force option if it applies
if(PythonQt_Wrap_QtAll)
  list(REMOVE_ITEM qtlibs xmlpatterns) # xmlpatterns wrapper does *NOT* build at all :(  
  foreach(qtlib ${qtlibs})
    if(NOT ${PythonQt_Wrap_Qt${qtlib}})
      set(PythonQt_Wrap_Qt${qtlib} ON CACHE BOOL "Make all of Qt${qtlib} available in python" FORCE)
      message(STATUS "Enabling [PythonQt_Wrap_Qt${qtlib}] because of [PythonQt_Wrap_QtAll] evaluates to True")
    endif()
  endforeach()
endif()

option(PythonQt_DEBUG "Enable/Disable PythonQt debug output" OFF)
if(PythonQt_DEBUG)
  add_definitions(-DPYTHONQT_DEBUG)
else()
  remove_definitions(-DPYTHONQT_DEBUG)
endif()

if(WIN32)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif() 

#-----------------------------------------------------------------------------
# Setup Qt

if("${QT_VERSION_MAJOR}" STREQUAL "5")
    # NEW
    if ("${QT_VERSION_MINOR}" STREQUAL "4")
        set(generated_cpp_suffix "_54")
    else()
        if ("${QT_VERSION_MINOR}" STREQUAL "3")
            set(generated_cpp_suffix "_53")
        else()
            set(generated_cpp_suffix "_50")
        endif()
    endif()
else()
    # OLD
    set(generated_cpp_suffix "_${QT_VERSION_MAJOR}${QT_VERSION_MINOR}")
    if("${generated_cpp_suffix}" STREQUAL "_46")
            set(generated_cpp_suffix "_47") # Also use 4.7 wrappers for 4.6.x version
    endif()
endif()

set(sources
    src/PythonQtBoolResult.cpp
    src/PythonQtClassInfo.cpp
    src/PythonQtClassWrapper.cpp
    src/PythonQtConversion.cpp
    src/PythonQt.cpp
    src/PythonQtImporter.cpp
    src/PythonQtInstanceWrapper.cpp
    src/PythonQtMethodInfo.cpp
    src/PythonQtMisc.cpp
    src/PythonQtObjectPtr.cpp
    src/PythonQtQFileImporter.cpp
    src/PythonQtSignalReceiver.cpp
    src/PythonQtSlot.cpp
    src/PythonQtSignal.cpp
    src/PythonQtStdDecorators.cpp
    src/PythonQtStdIn.cpp
    src/PythonQtStdOut.cpp
    src/gui/PythonQtScriptingConsole.cpp

    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_core_builtin/com_trolltech_qt_core_builtin0.cpp
    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_core_builtin/com_trolltech_qt_core_builtin_init.cpp
    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_gui_builtin/com_trolltech_qt_gui_builtin0.cpp
    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_gui_builtin/com_trolltech_qt_gui_builtin_init.cpp
)

#-----------------------------------------------------------------------------
# List headers.  This is list is used for the install command.

set(headers
    src/PythonQtBoolResult.h
    src/PythonQtClassInfo.h
    src/PythonQtClassWrapper.h
    src/PythonQtConversion.h
    src/PythonQtCppWrapperFactory.h
    src/PythonQtDoc.h
    src/PythonQt.h
    src/PythonQtImporter.h
    src/PythonQtImportFileInterface.h
    src/PythonQtInstanceWrapper.h
    src/PythonQtMethodInfo.h
    src/PythonQtMisc.h
    src/PythonQtObjectPtr.h
    src/PythonQtPythonInclude.h
    src/PythonQtQFileImporter.h
    src/PythonQtSignal.h
    src/PythonQtSignalReceiver.h
    src/PythonQtSlot.h
    src/PythonQtStdDecorators.h
    src/PythonQtStdIn.h
    src/PythonQtStdOut.h
    src/PythonQtSystem.h
    src/PythonQtUtils.h
    src/PythonQtVariants.h
)

#-----------------------------------------------------------------------------
# Headers that should run through moc

set(moc_sources
    src/PythonQt.h
    src/PythonQtSignalReceiver.h
    src/PythonQtStdDecorators.h
    src/gui/PythonQtScriptingConsole.h

    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_core_builtin/com_trolltech_qt_core_builtin0.h
    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_gui_builtin/com_trolltech_qt_gui_builtin0.h
)

#-----------------------------------------------------------------------------
# Add extra sources
#foreach(qtlib core gui network opengl sql svg uitools webkit xml xmlpatterns)
  
#  if (${PythonQt_Wrap_Qt${qtlib}})
    
#    ADD_DEFINITIONS(-DPYTHONQT_WRAP_Qt${qtlib})
    
#    set(file_prefix generated_cpp${generated_cpp_suffix}/com_trolltech_qt_${qtlib}/com_trolltech_qt_${qtlib})
    
#    foreach(index RANGE 0 11)
    
#      # Source files
#      if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${file_prefix}${index}.cpp)
#        list(APPEND sources ${file_prefix}${index}.cpp)
#      endif()
      
#      # Headers that should run through moc
#      if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${file_prefix}${index}.h)
#        list(APPEND moc_sources ${file_prefix}${index}.h)
#      endif()
      
#    endforeach()
    
#    list(APPEND sources ${file_prefix}_init.cpp)

#  endif()
#endforeach()

#-----------------------------------------------------------------------------
# UI files
set(ui_sources )

#-----------------------------------------------------------------------------
# Resources
set(qrc_sources )

#-----------------------------------------------------------------------------
# Do wrapping

#qt_wrap_ui(gen_ui_sources ${ui_sources})
#qt_add_resources(gen_qrc_sources ${qrc_sources})

#-----------------------------------------------------------------------------
# Build the library
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

qt_wrap_cpp(gen_moc_sources ${moc_sources})

add_library(${PROJECT_NAME} SHARED
            ${sources}
            ${gen_moc_sources}
  )

target_compile_definitions(${PROJECT_NAME} PRIVATE PYTHONQT_EXPORTS)

    qt_use_modules(${PROJECT_NAME} Core Gui)

target_link_libraries(${PROJECT_NAME}
              ${PYTHON_LIBRARIES}
  )
set_target_properties(${PROJECT_NAME} PROPERTIES DEFINE_SYMBOL PYTHONQT_EXPORTS)

#
# That should solve linkage error on Mac when the project is used in a superbuild setup
# See http://blog.onesadcookie.com/2008/01/installname-magic.html
#
set_target_properties(${PROJECT_NAME}  PROPERTIES
  INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
  )



#-----------------------------------------------------------------------------
# Install library (on windows, put the dll in 'bin' and the archive in 'lib')

install(TARGETS PythonQt
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install(FILES ${headers} DESTINATION include/PythonQt)
