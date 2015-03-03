function(checkCpp11Support)
	# ====== Check GCC VERSION FOR C++11 ======
	if(CMAKE_COMPILER_IS_GNUCXX)
		execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
		if (GCC_VERSION VERSION_LESS 4.8)
			message(FATAL_ERROR "Full support of C++11 needed. Therefore a gcc/g++ compiler with a version at least 4.8 is needed.")
		endif()
	endif()

	# ====== Check CLANG VERSION FOR C++11 ======
	if (APPLE)
		execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE CLANG_VERSION)
		if (CLANG_VERSION VERSION_LESS 3.1)
			message(FATAL_ERROR "Full support of C++11 needed. Therefore a clang compiler with a version at least 3.1 is needed !!")
		endif()
	endif()

	# ====== Check MSVC VERSION FOR C++11 ======
	if(WIN32)
		if(MSVC_VERSION LESS 1800)
			message(FATAL_ERROR "Full support of C++11 needed. Therefore a microsoft compiler with a version at least 12 is needed.")
		endif()
	endif()
endfunction(checkCpp11Support)


function(setBuildType)
IF (NOT (${CMAKE_BUILD_TYPE} MATCHES "Debug|Release"))
	IF (${CMAKE_CURRENT_BINARY_DIR} MATCHES "(.*)Debug|(.*)debug|(.*)DEBUG")
		SET(CMAKE_BUILD_TYPE "Debug" PARENT_SCOPE)
	ELSE()
		SET(CMAKE_BUILD_TYPE "Release" PARENT_SCOPE)
	ENDIF()
ENDIF()
endfunction(setBuildType)

macro(ToRelativePath outFiles fromDirectory inFiles)
	unset(tmpFiles)
	foreach(inFile ${inFiles})
		file(RELATIVE_PATH outFile "${fromDirectory}" "${inFile}")
		list(APPEND tmpFiles "${outFile}")
	endforeach()

	set(${outFiles} ${tmpFiles})
endmacro()
