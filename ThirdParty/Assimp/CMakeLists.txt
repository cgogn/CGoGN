cmake_minimum_required( VERSION 2.6 )
PROJECT( Assimp )

INCLUDE_DIRECTORIES( include )

# If this is an in-source build (CMAKE_SOURCE_DIR == CMAKE_BINARY_DIR),
# write the library/executable files to the respective directories in the
# source tree. During an out-of-source build, however, do not litter this
# directory, since that is probably what the user wanted to avoid.
#IF ( CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR )
#	SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib )
#	SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin )
#ENDIF ( CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR )

set(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/lib/Release)

# Cache these to allow the user to override them manually.
#SET( LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE PATH
#	"Path the built library files are installed to." )

# Libs
ADD_SUBDIRECTORY( code/ )

