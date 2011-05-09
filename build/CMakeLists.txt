cmake_minimum_required(VERSION 2.6)

project(CGoGN)

# a decommenter si on veut voir ce qui se passe a la compilation
#set( CMAKE_VERBOSE_MAKEFILE 1 )

SET(CGoGN_ROOT_DIR ${CMAKE_SOURCE_DIR}/..)

SET(CMAKE_MODULE_PATH " ${CMAKE_MODULE_PATH} ${CGoGN_ROOT_DIR}/cmake_modules/")

find_package(OpenGL REQUIRED)
find_package(Qt4 REQUIRED)
find_package(GLUT)

# qq definition specifiques pour mac
IF(APPLE)
# attention a changer pour chercher la bonne version automatiquement
	SET(CMAKE_OSX_SYSROOT "/Developer/SDKs/MacOSX10.6.sdk")
	SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-undefined -Wl,dynamic_lookup")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMAC_OSX")
	SET(CMAKE_OSX_ARCHITECTURES x86_64)
ENDIF(APPLE)

IF(WIN32)
	SET(GLEW_LIBRARY glew32)
	SET(DEVIL_LIBRARIES DevIL ILU ILUT)
	SET(ZLIB_LIBRARIES zlib)
	SET(LIBXML2_LIBRARIES xml2)
	
	MACRO(BOOST_LIBS lib_lists names)
	SET (${lib_lists} "")
	FOREACH(name ${names})
		STRING(REGEX REPLACE "-mt" $ENV{boost_extension} newname ${name})
		SET (${lib_lists} ${${lib_lists}} ${newname} )
	ENDFOREACH(name)
	ENDMACRO(BOOST_LIBS)

	set (BOOST_INCLUDE_PATH $ENV{boost_path})
	
	INCLUDE_DIRECTORIES(${CGoGN_ROOT_DIR}/windows_dependencies/include/)
	LINK_DIRECTORIES(${CGoGN_ROOT_DIR}/windows_dependencies/lib/ $ENV{boost_path}/lib)
	add_subdirectory(Release)
	add_subdirectory(${CGoGN_ROOT_DIR}/Apps Apps)
	if (EXISTS "${CGoGN_ROOT_DIR}/Perso")
                add_subdirectory(${CGoGN_ROOT_DIR}/Perso Perso)
	endif (EXISTS "${CGoGN_ROOT_DIR}/Perso")

	if(CMAKE_CONFIGURATION_TYPES)
		set(CMAKE_CONFIGURATION_TYPES Release Debug)
		set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Only Release or Debug" FORCE)
	endif()
	
ELSE(WIN32)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
	find_package(DevIL)
	find_package(ZLIB)
	find_package(LibXml2)
	SET(GLEW_LIBRARY GLEW)
	IF (IL_LIBRARIES)
		SET(DEVIL_LIBRARIES ${IL_LIBRARIES} ${ILU_LIBRARIES} ${ILUT_LIBRARIES})
	ELSE(IL_LIBRARIES)
		SET(DEVIL_LIBRARIES ${IL_LIBRARY} ${ILU_LIBRARY} ${ILUT_LIBRARY})
	ENDIF(IL_LIBRARIES)
	
	MACRO(BOOST_LIBS lib_lists names)
	SET (${lib_lists})
	FOREACH(name ${names})
		SET (${lib_lists} ${${lib_lists}} ${name} )
	ENDFOREACH(name)
	ENDMACRO(BOOST_LIBS)

	add_subdirectory(Release)
	add_subdirectory(Debug)
	add_subdirectory(${CGoGN_ROOT_DIR}/Apps Apps)
	if (EXISTS "${CGoGN_ROOT_DIR}/Perso")
                add_subdirectory(${CGoGN_ROOT_DIR}/Perso Perso)
	endif (EXISTS "${CGoGN_ROOT_DIR}/Perso")
ENDIF(WIN32)
