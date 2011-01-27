#include(ExternalProject)

project(CGoGN_ThirdParty)

#On Windows we use AntTweakBar/OpenCTM of dependencies, not compiled
IF(WIN32)
	SUBDIRS (Zinri/Build gzstream/Build Numerical)
ELSE(WIN32)

	add_custom_target( FakeTargetOpenCTM ALL "make" 
		WORKING_DIRECTORY ${CGoGN_ROOT_DIR}/ThirdParty/OpenCTM
		COMMENT "Custom compile of OpenCTM" )

	SUBDIRS (Zinri/Build gzstream/Build AntTweakBar Numerical Assimp/code )

ENDIF(WIN32)

