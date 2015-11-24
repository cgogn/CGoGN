## CMake file to locate SuiteSparse and its useful composite projects
## The first developpement of this file was made fro Windows users who
## use the http://code.google.com/p/suitesparse-metis-for-windows/
## downloaded here :
## svn checkout http://suitesparse-metis-for-windows.googlecode.com/svn/trunk/ suitesparse-metis-for-windows
## Anyway, it chould be work also on linux (tested on fedora 17 when you installed suitesparse from yum)
##
##
## Inputs variables this file can process (variable must be given before find_package(SUITESPARES ...) command) :
##   * SUITESPARSE_VERBOSE			Default to OFF
##   * SUITESPARSE_USE_LAPACK_BLAS	Default to OFF. If ON append to SUITESPARSE_LIBRARIES the blas and lapack library
##   Note: SuiteSparse lib usually requires linking to a blas and lapack library.
##
##
## Help variables this file handle internaly :
##   * SUITESPARSE_SEARCH_LIB_POSTFIX		Is set in cache (as advanced) to look into the right lib/lib64 dir for libraries (user can change)
##
##
## Variables this file provide : 
##   * SUITESPARSE_FOUND         			True if SuiteSparse given COMPONENTS include and libraries were found
##   * SUITESPARSE_INCLUDE_DIRS  			Paths containing SuiteSparse needed headers (depend on which COMPONENTS you gave)
##   * SUITESPARSE_LIBRARIES     			Absolute paths of SuiteSparse libs found (depend on which COMPONENTS you gave)
##   If SUITESPARSE_USE_LAPACK_BLAS is set to ON : 
##   	* SUITESPARSE_LAPACK_BLAS_LIBRARIES 	Which contain the libblas and liblapack libraries
##   	On windows:
##   		* SUITESPARSE_LAPACK_BLAS_DLL		Which contain all requiered binaries for use libblas and liblapack
##
##
## Detailed variables this file provide :
##   * SUITESPARSE_<UPPPER_CASE_COMPONENT>_FOUND		True if the given component to look for is found (INCLUDE DIR and LIBRARY)
##   * SUITESPARSE_<UPPPER_CASE_COMPONENT>_INCLUDE_DIR	The path directory where we can found all compenent header files
##   * SUITESPARSE_<UPPPER_CASE_COMPONENT>_LIBRARY		The file path to the component library
##   Note: If a component is not found, a SUITESPARSE_<UPPPER_CASE_COMPONENT>_DIR cache variable is set to allow user set the search directory.
##
##
## Possible componnents to find are (maybe some others can be available):
##   * AMD
##   * CAMD
##   * COLAMD
##   * CCOLAMD
##   * CHOLMOD	: this lib need all previous one. According to how it was build (a single static lib or a full dynamic one), you should looking for its dependencies.
##   * metis (opt): may not be found (depend if suitesparse was build with metis or not) => required by CHOLMOD (optional)
##
##
## How to use this file : 
##   (opt) set(SUITESPARSE_VERBOSE ON)
##   (opt) set(SUITESPARSE_USE_LAPACK_BLAS ON)
##   ( 1 ) find_package(SuiteSparse) ## metis is not search by default because it's not a part of suitesparse (suitesparse can be built without metis)
##   ( 2 ) find_package(SuiteSparse COMPONENTS metis CHOLMOD) 		## be careful, components are case sensitive
##   ( 3 ) find_package(SuiteSparse COMPONENTS metis suitesparse)	## valid on windows (linux have no suitesparse library)
##   ( 4 ) find_package(SuiteSparse COMPONENTS suitesparse)
## 
##    if(SUITESPARSE_FOUND)
##       include_directories(${SUITESPARSE_INCLUDE_DIRS})
##		 target_link_library(<myProject> ${SUITESPARSE_LIBRARIES})
##	  endif()
##
## Created by jesnault (jerome.esnault@inria.fr) 2014-01-15
## Updated by jesnault (jerome.esnault@inria.fr) 2014-01-21


## check if global root SuiteSparse folder is set or not and cache it in order to let user fill it
if(NOT SUITESPARSE_DIR)
    set(SUITESPARSE_DIR "$ENV{SUITESPARSE_DIR}" CACHE PATH "SuiteSparse root directory")
endif()
if(SUITESPARSE_DIR)
	file(TO_CMAKE_PATH ${SUITESPARSE_DIR} SUITESPARSE_DIR)
endif()

## set default verbosity
## Process the CMake automatically-generated var: SUITESPARSE_FIND_QUIETLY: supersedes *_VERBOSE.
if(NOT SUITESPARSE_VERBOSE OR SUITESPARSE_FIND_QUIETLY)
	set(SUITESPARSE_VERBOSE OFF)
endif()

MESSAGE (STATUS "Looking for SuiteSparse ..")

## set the LIB POSTFIX to find in a right directory according to what kind of compiler we use (32/64bits)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)  # Size in bytes!
	set(SUITESPARSE_SEARCH_LIB_POSTFIX "64" CACHE STRING "suffix for 32/64 dir placement")
else()  # Size in bytes!
	set(SUITESPARSE_SEARCH_LIB_POSTFIX "" CACHE STRING "suffix for 32/64 dir placement")
endif()
if(SUITESPARSE_SEARCH_LIB_POSTFIX)
	mark_as_advanced(SUITESPARSE_SEARCH_LIB_POSTFIX)
	if(SUITESPARSE_VERBOSE)
		message(STATUS "   find_library will search inside lib${SUITESPARSE_SEARCH_LIB_POSTFIX} directory (can be changed with SUITESPARSE_SEARCH_LIB_POSTFIX)")
	endif()
endif()


## This utility macro is used to find all suitesparse projects by giving its name
## Since the name structure is the same for lib name and include dir name,
## we can use a generic way to find all of these with simple cmake lines of code
macro(SUITESPARSE_FIND_COMPONENTS )

	## On windows : we absolutly need SUITESPARSE_config.h every time for all projects
	if(WIN32)
		list(FIND SUITESPARSE_FIND_COMPONENTS "suitesparseconfig" SS_config_index)
		if(${SS_config_index} MATCHES "-1")
			list(APPEND SUITESPARSE_FIND_COMPONENTS suitesparseconfig)
			if(SUITESPARSE_VERBOSE)
				message(STATUS "   On windows, we absolutly need SUITESPARSE_config.h every time for all projects : add suitesparseconfig component to look for")
			endif()
		endif()
	endif()

	## special check for suitesparse component (allow to find on windows but not on linux because doesn't exist)
	list(FIND SUITESPARSE_FIND_COMPONENTS "suitesparse" ss_index)
	if(${ss_index} MATCHES "-1")
		## do nothing, the user didn't provide the suisparse componnent
	else()
		if(WIN32)
			## do nothing, the user provide the suisparse componnent we will try to find
		else()
			list(REMOVE_AT SUITESPARSE_FIND_COMPONENTS ${ss_index})
			if(SUITESPARSE_VERBOSE)
				message(STATUS "   On this plateform : suitesparse lib doesn't exist (only on windows), so skip this component")
			endif()
		endif()
	endif()
		
	## Look for each component the same way :
	##  * For include dir the reference file is the <component>.h
	##	* for library fileName the reference is the <component> itself (cmake will prepend/append necessary prefix/suffix according to the plateform)
	foreach(suitesparseComp ${SUITESPARSE_FIND_COMPONENTS})

		## used to construct specific cmake variables (in upper case) according to the component, but also used for find_*()
		string(TOUPPER ${suitesparseComp} suitesparseCompUC)
		string(TOLOWER ${suitesparseComp} suitesparseCompLC)

		## Special case: CXSparse library is named "libcxsparse.*" but headers are "cs.h":
		SET(suitesparseComp_ALT "${suitesparseComp}") # Alternative names
		if("${suitesparseComp}" STREQUAL "CXSPARSE")
			SET(suitesparseComp_ALT "cs") # Alternative name of CXSparse
		endif()

		## try to find include dir (looking for very important header file)
		find_path(SUITESPARSE_${suitesparseCompUC}_INCLUDE_DIR	
			NAMES 			${suitesparseComp}.h ${suitesparseCompLC}.h ${suitesparseCompUC}.h ${suitesparseComp_ALT}.h
						${suitesparseComp}.hpp ${suitesparseCompLC}.hpp ${suitesparseCompUC}.hpp
			PATHS			/opt/local/include
						/usr/include
						/usr/local/include
						/usr/include/suitesparse
						/usr/local/include/suitesparse
						/usr/include/${suitesparseComp}
						/usr/local/include/${suitesparseComp}
						${SUITESPARSE_DIR}/include
						${SUITESPARSE_DIR}/include/suitesparse
						${SUITESPARSE_DIR}/suitesparse/include
						${SUITESPARSE_DIR}/include/${suitesparseComp}
						${SUITESPARSE_DIR}/${suitesparseComp}/include
						${${suitesparseCompUC}_DIR}/include
						${${suitesparseCompUC}_DIR}/${suitesparseComp}/include
						${${suitesparseCompUC}_DIR}
		)
		## check if found
		if(NOT SUITESPARSE_${suitesparseCompUC}_INCLUDE_DIR)
			if (SUITESPARSE_VERBOSE)
				message(WARNING "   Failed to find ${suitesparseComp} :\nSUITESPARSE_${suitesparseCompUC}_INCLUDE_DIR not found.\nCheck you write correctly the component name (case sensitive),\nor set the SUITESPARSE_${suitesparseCompUC}_DIR to look inside")
			endif()
		else()
			list(APPEND SUITESPARSE_INCLUDE_DIRS	${SUITESPARSE_${suitesparseCompUC}_INCLUDE_DIR})
		endif()

		## try to find filepath lib name (looking for very important lib file)
		find_library(SUITESPARSE_${suitesparseCompUC}_LIBRARY_RELEASE 
			NAMES 			lib${suitesparseComp} 	lib${suitesparseCompLC} lib${suitesparseCompUC}
							${suitesparseComp} 		${suitesparseCompLC} 	${suitesparseCompUC}
			PATHS 			/opt/local/lib${SUITESPARSE_SEARCH_LIB_POSTFIX} 		
							/usr/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}
							/usr/local/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}
							${SUITESPARSE_DIR}/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}
							${${suitesparseCompUC}_DIR}/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}
							${${suitesparseCompUC}_DIR}
			PATH_SUFFIXES	Release
		)
		
		## check and append the and SUITESPARSE_LIBRARIES list, and warn if not found (release and debug) otherwise
		if(NOT SUITESPARSE_${suitesparseCompUC}_LIBRARY_RELEASE )
			if (SUITESPARSE_VERBOSE)
			message(WARNING "   Failed to find ${suitesparseComp} :
			Check you write correctly the component name (case sensitive),
			or set the SUITESPARSE_${suitesparseCompUC}_DIR to look inside,
			or set directly SUITESPARSE_${suitesparseCompUC}_LIBRARY_RELEASE
			")
			endif ()
		else()
			list(APPEND SUITESPARSE_LIBRARIES	"${SUITESPARSE_${suitesparseCompUC}_LIBRARY_RELEASE}" )
		endif()
		
		## here we allow to find at least the include OR the lib dir and just warn if one of both missing
		if(NOT SUITESPARSE_${suitesparseCompUC}_INCLUDE_DIR AND NOT SUITESPARSE_${suitesparseCompUC}_LIBRARY_RELEASE)
			set(SUITESPARSE_${suitesparseCompUC}_FOUND OFF)
		else()
			set(SUITESPARSE_${suitesparseCompUC}_FOUND ON)
		endif()
		
		## if one of both (include dir or filepath lib), then we provide a new cmake cache variable for the search. Otherwise we don't need anymore to expose all intermediates variables
		if(NOT SUITESPARSE_${suitesparseCompUC}_FOUND)
			set(SUITESPARSE_${suitesparseCompUC}_DIR "$ENV{SUITESPARSE_${suitesparseCompUC}_DIR}" CACHE PATH "${suitesparseComp} root directory")
		else()
			mark_as_advanced(SUITESPARSE_${suitesparseCompUC}_INCLUDE_DIR)
			mark_as_advanced(SUITESPARSE_${suitesparseCompUC}_LIBRARY_RELEASE)
			if(DEFINED SUITESPARSE_${suitesparseCompUC}_DIR)
				mark_as_advanced(SUITESPARSE_${suitesparseCompUC}_DIR)
			endif()
		endif()

		if(SUITESPARSE_VERBOSE)
			message(STATUS "   SUITESPARSE_${suitesparseCompUC}_FOUND = ${SUITESPARSE_${suitesparseCompUC}_FOUND} : ")
			message(STATUS "      * SUITESPARSE_${suitesparseCompUC}_INCLUDE_DIR = ${SUITESPARSE_${suitesparseCompUC}_INCLUDE_DIR}")
			message(STATUS "      * SUITESPARSE_${suitesparseCompUC}_LIBRARY_RELEASE = ${SUITESPARSE_${suitesparseCompUC}_LIBRARY_RELEASE}")
		endif()
		
		list(APPEND SUITESPARSE_FOUND_LIST SUITESPARSE_${suitesparseCompUC}_FOUND)
		
		## special definition needed for metis
		if(${suitesparseComp} MATCHES "metis")
			set(SUITESPARSE_${suitesparseCompUC}_DEFINITIONS "-DNPARTITION")
			add_definitions(${SUITESPARSE_${suitesparseCompUC}_DEFINITIONS})
			if(SUITESPARSE_VERBOSE)
				message(STATUS "      * SUITESPARSE_${suitesparseCompUC}_DEFINITIONS = ${SUITESPARSE_${suitesparseCompUC}_DEFINITIONS}")
			endif()
		endif()
		
	endforeach()
	
	
	## set the final SUITESPARSE_FOUND based on all previous components found (status)
	foreach(componentToCheck ${SUITESPARSE_FOUND_LIST})
		set(SUITESPARSE_FOUND ON)
		if(SUITESPARSE_VERBOSE)
		MESSAGE(STATUS "final check: ${componentToCheck}")
		endif()
		if(NOT ${componentToCheck})
			set(SUITESPARSE_FOUND OFF)
			break() ## one component not found is enought to failed
		endif()
	endforeach()
endmacro()

## Default behavior if user don't use the COMPONENTS flag in find_package(SuiteSparse ...) command
if(NOT SUITESPARSE_FIND_COMPONENTS)
	list(APPEND SUITESPARSE_FIND_COMPONENTS AMD CAMD CCOLAMD COLAMD CHOLMOD SPQR LDL BTF KLU CXSPARSE UMFPACK)  ## suitesparse and metis are not searched by default (special case)
endif()

SUITESPARSE_FIND_COMPONENTS()

## check if we have to find also blas and lapack lib for SuiteSparse
if(SUITESPARSE_USE_LAPACK_BLAS)

	## set additional search dirs
	set(ADDITIONAL_SEARCH_DIRS 
		${SUITESPARSE_DIR}/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}
		${SUITESPARSE_DIR}/lapack_windows/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}
		${SUITESPARSE_DIR}/lapack_windows/x${SUITESPARSE_SEARCH_LIB_POSTFIX}
		${SUITESPARSE_DIR}/blas_windows/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}
		${SUITESPARSE_DIR}/blas_windows/x${SUITESPARSE_SEARCH_LIB_POSTFIX}
		${SUITESPARSE_DIR}/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}/lapack_windows
		${SUITESPARSE_DIR}/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}/blas_windows
		${SUITESPARSE_DIR}/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}/lapack_blas_windows
		${SUITESPARSE_DIR}/lapack_blas_windows
		${SUITESPARSE_DIR}/lapack_blas_windows/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}
	)

	## try to find blas lib
	find_library(SUITESPARSE_BLAS_LIBRARY 
		NAMES 			blas cblas libblas
		PATHS 			/opt/local/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}		
						/usr/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}
						/usr/local/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}
						${SUITESPARSE_BLAS_DIR}/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}
						${SUITESPARSE_BLAS_DIR}
						${ADDITIONAL_SEARCH_DIRS}
		PATH_SUFFIXES	Release Debug
	)
	if(NOT SUITESPARSE_BLAS_LIBRARY)
		if (SUITESPARSE_VERBOSE)
			# Send all msgs as "STATUS": We'll send an error at the bottom, only if "REQUIRED" is set.
			message(STATUS "   Failed to find SUITESPARSE_BLAS_LIBRARY.Set it manually or set the SUITESPARSE_BLAS_DIR to looking for it inside.")
		endif()
			set(SUITESPARSE_BLAS_DIR "$ENV{SUITESPARSE_BLAS_DIR}" CACHE PATH "blas root directory")
	else()
		if(DEFINED SUITESPARSE_BLAS_DIR)
			mark_as_advanced(SUITESPARSE_BLAS_DIR)
		endif()
		list(APPEND SUITESPARSE_LAPACK_BLAS_LIBRARIES ${SUITESPARSE_BLAS_LIBRARY})
	endif()
	
	## try to find lapack lib
	find_library(SUITESPARSE_LAPACK_LIBRARY 
		NAMES 			lapack liblapack
		PATHS 			/opt/local/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}		
						/usr/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}
						/usr/local/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}
						${SUITESPARSE_LAPACK_DIR}/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}
						${SUITESPARSE_LAPACK_DIR}
						${ADDITIONAL_SEARCH_DIRS}
		PATH_SUFFIXES	Release Debug
	)
	if(NOT SUITESPARSE_LAPACK_LIBRARY)
		if (SUITESPARSE_VERBOSE)
			# Send all msgs as "STATUS": We'll send an error at the bottom, only if "REQUIRED" is set.
			message(STATUS "   Failed to find SUITESPARSE_LAPACK_LIBRARY.Set it manually or set the SUITESPARSE_LAPACK_DIR to looking for it inside.")
		endif()
		set(SUITESPARSE_LAPACK_DIR "$ENV{SUITESPARSE_LAPACK_DIR}" CACHE PATH "lapack root directory")
	else()
		if(DEFINED SUITESPARSE_LAPACK_DIR)
			mark_as_advanced(SUITESPARSE_LAPACK_DIR)
		endif()
		list(APPEND SUITESPARSE_LAPACK_BLAS_LIBRARIES ${SUITESPARSE_LAPACK_LIBRARY})
	endif()
		
	find_library(F2C_LIBRARY 
		NAMES 			f2c libf2c
		PATHS 			/opt/local/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}		
						/usr/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}
						/usr/local/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}
						${SUITESPARSE_DIR}
		PATH_SUFFIXES	Release Debug
	)	
	
	if (F2C_LIBRARY)
		list(APPEND SUITESPARSE_LAPACK_BLAS_LIBRARIES ${F2C_LIBRARY})
	endif()
	
	## well, now append to the SUITESPARSE_LIBRARIES and print infos if VERBOSE
	if(SUITESPARSE_LAPACK_BLAS_LIBRARIES)
		list(APPEND SUITESPARSE_LIBRARIES	${SUITESPARSE_LAPACK_BLAS_LIBRARIES})
		if(SUITESPARSE_VERBOSE)
			message(STATUS "   SUITESPARSE_USE_LAPACK_BLAS = ${SUITESPARSE_USE_LAPACK_BLAS} : ")
			message(STATUS "      * SUITESPARSE_LAPACK_BLAS_LIBRARIES : ")
			foreach(lib ${SUITESPARSE_LAPACK_BLAS_LIBRARIES})
				message(STATUS "         ${lib}")
			endforeach()
		endif()
	endif()
	
	## Now looking for *.dll => note that this is not a safe way to get it...
	if(WIN32)
		if(${SUITESPARSE_SEARCH_LIB_POSTFIX} MATCHES "64")
			set(SUITESPARSE_SEARCH_BIN_POSTFIX_1 "x64")
			set(SUITESPARSE_SEARCH_BIN_POSTFIX_2 "Win64")
		else()
			set(SUITESPARSE_SEARCH_BIN_POSTFIX_1 "x86")
			set(SUITESPARSE_SEARCH_BIN_POSTFIX_2 "Win32")
		endif()
		
		set(SUITESPARSE_DLL_SEARCH_DIRS
			${SUITESPARSE_LAPACK_DIR}/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}
			${SUITESPARSE_LAPACK_DIR}
			${SUITESPARSE_LAPACK_DIR}/bin
			${SUITESPARSE_LAPACK_DIR}/bin/${SUITESPARSE_SEARCH_BIN_POSTFIX_1}
			${SUITESPARSE_LAPACK_DIR}/bin/${SUITESPARSE_SEARCH_BIN_POSTFIX_2}
			${SUITESPARSE_LAPACK_DIR}/bin/Release/${SUITESPARSE_SEARCH_BIN_POSTFIX_1}
			${SUITESPARSE_LAPACK_DIR}/bin/Debug/${SUITESPARSE_SEARCH_BIN_POSTFIX_2}
			${SUITESPARSE_LAPACK_DIR}/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}
			${SUITESPARSE_BLAS_DIR}
			${SUITESPARSE_BLAS_DIR}/bin
			${SUITESPARSE_BLAS_DIR}/bin/${SUITESPARSE_SEARCH_BIN_POSTFIX_1}
			${SUITESPARSE_BLAS_DIR}/bin/${SUITESPARSE_SEARCH_BIN_POSTFIX_2}
			${SUITESPARSE_BLAS_DIR}/bin/Release/${SUITESPARSE_SEARCH_BIN_POSTFIX_1}
			${SUITESPARSE_BLAS_DIR}/bin/Debug/${SUITESPARSE_SEARCH_BIN_POSTFIX_2}
			${ADDITIONAL_SEARCH_DIRS}
			"$ENV{Path}"
		)
		set(dllPatternFileList "libblas" "liblapack" "libgcc_s_" "libgfortran" "libquadmath")
		foreach(dllPattern ${dllPatternFileList})
			string(TOUPPER ${dllPattern} dllPatternUC)
			foreach(searchDir ${SUITESPARSE_DLL_SEARCH_DIRS})
				file(GLOB SUITESPARSE_DLL_${dllPatternUC} "${searchDir}/${dllPattern}*.dll") ## append the *.dll
				list(LENGTH SUITESPARSE_DLL_${dllPatternUC} resultCount)
				if(${resultCount} GREATER "0" )
					list(APPEND SUITESPARSE_LAPACK_BLAS_DLL ${SUITESPARSE_DLL_${dllPatternUC}})
					break()
				endif()
			endforeach()
		endforeach()
		
		if(SUITESPARSE_VERBOSE)
			message(STATUS "      * SUITESPARSE_LAPACK_BLAS_DLL : ")
			foreach(dll ${SUITESPARSE_LAPACK_BLAS_DLL})
				message(STATUS "         ${dll}")
			endforeach()
		endif()
		
	endif()
	
endif()

find_library(SSPARSE_CONFIG_LIB 
		NAMES 			libsuitesparseconfig suitesparseconfig
		PATHS 			/opt/local/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}
						/usr/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}
						/usr/local/lib${SUITESPARSE_SEARCH_LIB_POSTFIX}
						${SUITESPARSE_DIR}
		PATH_SUFFIXES	Release Debug)	

if (SSPARSE_CONFIG_LIB)
	list(APPEND SUITESPARSE_LIBRARIES ${SSPARSE_CONFIG_LIB})
endif()

if(SUITESPARSE_INCLUDE_DIRS)
	list(REMOVE_DUPLICATES SUITESPARSE_INCLUDE_DIRS)
endif()
if(SUITESPARSE_LIBRARIES)
	list(REMOVE_DUPLICATES SUITESPARSE_LIBRARIES)
endif()

if(SUITESPARSE_LAPACK_BLAS_LIBRARIES)
	list(REMOVE_DUPLICATES SUITESPARSE_LAPACK_BLAS_LIBRARIES)
endif()

if(SUITESPARSE_LAPACK_BLAS_DLL)
	list(REMOVE_DUPLICATES SUITESPARSE_LAPACK_BLAS_DLL)
endif()

if(SUITESPARSE_VERBOSE)
	message(STATUS "Finish to FindSuiteSparse.cmake => SUITESPARSE_FOUND=${SUITESPARSE_FOUND}")
endif()

## Show error if not found and _REQUIRED
IF(NOT SUITESPARSE_FOUND)
  # make FIND_PACKAGE friendly
  IF(NOT SUITESPARSE_FIND_QUIETLY)
    IF(SUITESPARSE_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR
        "SuiteSparse required but some headers or libs not found.")
    ELSE()
      MESSAGE(STATUS "ERROR: SuiteSparse was not found.")
    ENDIF()
  ENDIF()
ELSE()
	message (STATUS ".. SuiteSparse Found")
ENDIF()
