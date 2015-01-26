#########################
# UPDATE VARIABLES:
# - TINYXML2_FOUND
# - TINYXML2_INCLUDE_DIR
# - TINYXML2_LIBRARY
#########################

find_path(TINYXML2_INCLUDE_DIR tinyxml2.h
	HINTS $ENV{TINYXML2DIR}
	PATH_SUFFIXES include
	PATHS ~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr)

find_library(TINYXML2_LIBRARY 	tinyxml2
	HINTS $ENV{TINYXML2DIR}
	PATH_SUFFIXES lib64 lib
	PATH_SUFFIXESS ~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr)

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(TINYXML2 DEFAULT_MSG TINYXML2_LIBRARY)
mark_as_advanced(TINYXML2_LIBRARY TINYXML2_INCLUDE_DIR)
