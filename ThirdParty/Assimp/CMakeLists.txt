cmake_minimum_required( VERSION 2.6 )
PROJECT( Assimp )
SET ( PROJECT_VERSION "2.0" )
INCLUDE_DIRECTORIES( include )

SET( Boost_DETAILED_FAILURE_MSG ON )
FIND_PACKAGE( Boost 1.35 )

IF ( NOT Boost_FOUND )
	MESSAGE( FATAL_ERROR
		"Boost libraries (http://www.boost.org/) not found. "
		"You can build a non-boost version of Assimp with slightly reduced "
		"functionality by specifying -DENABLE_BOOST_WORKAROUND=ON."
	)
ENDIF ( NOT Boost_FOUND )

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} )

SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )
SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC" )
ADD_SUBDIRECTORY( code/ )

