# Install script for directory: /home/thery/Downloads/ASS/code

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FOREACH(file
      "$ENV{DESTDIR}/usr/local/lib/libassimp.so.1.0.0"
      "$ENV{DESTDIR}/usr/local/lib/libassimp.so.1"
      "$ENV{DESTDIR}/usr/local/lib/libassimp.so"
      )
    IF(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      FILE(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    ENDIF()
  ENDFOREACH()
  FILE(INSTALL DESTINATION "/usr/local/lib" TYPE SHARED_LIBRARY FILES
    "/home/thery/Downloads/ASS/lib/libassimp.so.1.0.0"
    "/home/thery/Downloads/ASS/lib/libassimp.so.1"
    "/home/thery/Downloads/ASS/lib/libassimp.so"
    )
  FOREACH(file
      "$ENV{DESTDIR}/usr/local/lib/libassimp.so.1.0.0"
      "$ENV{DESTDIR}/usr/local/lib/libassimp.so.1"
      "$ENV{DESTDIR}/usr/local/lib/libassimp.so"
      )
    IF(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      IF(CMAKE_INSTALL_DO_STRIP)
        EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "${file}")
      ENDIF(CMAKE_INSTALL_DO_STRIP)
    ENDIF()
  ENDFOREACH()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "/usr/local/include/assimp" TYPE FILE FILES
    "/home/thery/Downloads/ASS/code/../include/aiAnim.h"
    "/home/thery/Downloads/ASS/code/../include/aiAssert.h"
    "/home/thery/Downloads/ASS/code/../include/aiCamera.h"
    "/home/thery/Downloads/ASS/code/../include/aiColor4D.h"
    "/home/thery/Downloads/ASS/code/../include/aiColor4D.inl"
    "/home/thery/Downloads/ASS/code/../include/aiConfig.h"
    "/home/thery/Downloads/ASS/code/../include/aiDefines.h"
    "/home/thery/Downloads/ASS/code/../include/aiFileIO.h"
    "/home/thery/Downloads/ASS/code/../include/aiLight.h"
    "/home/thery/Downloads/ASS/code/../include/aiMaterial.h"
    "/home/thery/Downloads/ASS/code/../include/aiMaterial.inl"
    "/home/thery/Downloads/ASS/code/../include/aiMatrix3x3.h"
    "/home/thery/Downloads/ASS/code/../include/aiMatrix3x3.inl"
    "/home/thery/Downloads/ASS/code/../include/aiMatrix4x4.h"
    "/home/thery/Downloads/ASS/code/../include/aiMatrix4x4.inl"
    "/home/thery/Downloads/ASS/code/../include/aiMesh.h"
    "/home/thery/Downloads/ASS/code/../include/aiPostProcess.h"
    "/home/thery/Downloads/ASS/code/../include/aiQuaternion.h"
    "/home/thery/Downloads/ASS/code/../include/aiScene.h"
    "/home/thery/Downloads/ASS/code/../include/aiTexture.h"
    "/home/thery/Downloads/ASS/code/../include/aiTypes.h"
    "/home/thery/Downloads/ASS/code/../include/aiVector2D.h"
    "/home/thery/Downloads/ASS/code/../include/aiVector3D.h"
    "/home/thery/Downloads/ASS/code/../include/aiVector3D.inl"
    "/home/thery/Downloads/ASS/code/../include/aiVersion.h"
    "/home/thery/Downloads/ASS/code/../include/assimp.h"
    "/home/thery/Downloads/ASS/code/../include/assimp.hpp"
    "/home/thery/Downloads/ASS/code/../include/DefaultLogger.h"
    "/home/thery/Downloads/ASS/code/../include/IOStream.h"
    "/home/thery/Downloads/ASS/code/../include/IOSystem.h"
    "/home/thery/Downloads/ASS/code/../include/Logger.h"
    "/home/thery/Downloads/ASS/code/../include/LogStream.h"
    "/home/thery/Downloads/ASS/code/../include/NullLogger.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "/usr/local/include/assimp/Compiler" TYPE FILE FILES
    "/home/thery/Downloads/ASS/code/../include/Compiler/pushpack1.h"
    "/home/thery/Downloads/ASS/code/../include/Compiler/poppack1.h"
    "/home/thery/Downloads/ASS/code/pstdint.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

