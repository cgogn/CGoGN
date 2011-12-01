cmake_minimum_required(VERSION 2.8)

project(CGoGN)

SET(CGoGN_ROOT_DIR ${CMAKE_SOURCE_DIR}/..)

# for GLEW
SET(CMAKE_MODULE_PATH ${CGoGN_ROOT_DIR}/cmake_modules/)

# for shared or not shared
SET (BUILD_SHARED_LIBS OFF CACHE BOOL 
	"if used all library are build as shared type (.so /dylib/dll).")


find_package(OpenGL REQUIRED)
find_package(Qt4 REQUIRED)
find_package(Boost COMPONENTS regex thread REQUIRED)
find_package(ZLIB REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(GLEW REQUIRED)


IF (DEFINED ASSERTON)
	add_definitions(-DCGOGN_ASSERT_BOOL=${ASSERTON})
ELSE (DEFINED ASSERTON)
	add_definitions(-DCGOGN_ASSERT_BOOL=false)
ENDIF (DEFINED ASSERTON)

SET ( WITH_ASSIMP ON CACHE BOOL "build CGoGN with Assimp ")
IF (WITH_ASSIMP)
	add_definitions(-DWITH_ASSIMP)
ENDIF (WITH_ASSIMP)

# FOR Qt4
SET(QT_USE_QTOPENGL TRUE)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})


add_definitions(-DSHADERPATH="${CGoGN_ROOT_DIR}/lib/Shaders/")

# define includes of external libs 
SET(CGoGN_EXT_INCLUDES 
	${CGoGN_ROOT_DIR}/ThirdParty/Numerical
	${CGoGN_ROOT_DIR}/ThirdParty/Numerical/UFconfig
	${CGoGN_ROOT_DIR}/ThirdParty/gzstream
	${CGoGN_ROOT_DIR}/ThirdParty/Zinri
	${CGoGN_ROOT_DIR}/ThirdParty/glm
	${CGoGN_ROOT_DIR}/ThirdParty/Assimp/include
	${OPENGL_INCLUDE_DIR}
	${GLEW_INCLUDE_DIRS}
	${ZLIB_INCLUDE_DIRS}
	${LIBXML2_INCLUDE_DIR}
	${Boost_INCLUDE_DIRS} 
	${QT_INCLUDE_DIR} )

IF (WITH_ASSIMP)
	SET (CGoGN_EXT_INCLUDES ${CGoGN_EXT_INCLUDES} ThirdParty/Assimp/include)
ENDIF (WITH_ASSIMP)

# define libs for external libs
SET (CGoGN_EXT_LIBS 
	${OPENGL_LIBRARY}
	${GLEW_LIBRARIES}
	${ZLIB_LIBRARIES}
	${LIBXML2_LIBRARIES}
	${Boost_REGEX_LIBRARY}
	${Boost_THREAD_LIBRARY}
	${QT_LIBRARIES}
	gzstream)
	
IF (WITH_ASSIMP)
	SET (CGoGN_EXT_LIBS ${CGoGN_EXT_LIBS} assimp)
ENDIF (WITH_ASSIMP)


# qq definition specifiques pour mac
IF(APPLE)
# attention a changer pour chercher la bonne version automatiquement
	SET(CMAKE_OSX_SYSROOT "/Developer/SDKs/MacOSX10.6.sdk" CACHE STRING)
	SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-undefined -Wl,dynamic_lookup")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMAC_OSX")
	SET(CMAKE_OSX_ARCHITECTURES x86_64)
ENDIF(APPLE)

IF(WIN32)
	INCLUDE_DIRECTORIES(${CGoGN_ROOT_DIR}/windows_dependencies/include/)
	LINK_DIRECTORIES(${CGoGN_ROOT_DIR}/windows_dependencies/lib/ ${boost_path}/lib)
	add_subdirectory(Release)
	add_subdirectory(${CGoGN_ROOT_DIR}/Apps Apps)
	if(CMAKE_CONFIGURATION_TYPES)
		set(CMAKE_CONFIGURATION_TYPES Release Debug)
		set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Only Release or Debug" FORCE)
	endif()
ELSE(WIN32)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
	add_subdirectory(Release)
	add_subdirectory(Debug)
	add_subdirectory(${CGoGN_ROOT_DIR}/Apps Apps)
	if (EXISTS "${CGoGN_ROOT_DIR}/Perso")
                add_subdirectory(${CGoGN_ROOT_DIR}/Perso Perso)
	endif (EXISTS "${CGoGN_ROOT_DIR}/Perso")
ENDIF(WIN32)
