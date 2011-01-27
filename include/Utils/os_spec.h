
// SPECIFIC FOR WINDOWS

#ifdef WIN32
	#define NOMINMAX 
	#define _CRT_SECURE_NO_WARNINGS
	#include <windows.h>
	#include <stdio.h>
	#include <limits>
	#include <cmath>
	#ifndef PI_DEFINED
	#define PI_DEFINED
		const double M_PI = 3.14159265359;
	#endif
  #ifdef EXPORTING
 //   #define APIEXPORT __declspec(dllexport)
   #define APIEXPORT 
#else
//    #define APIEXPORT __declspec(dllimport)
    #define APIEXPORT 
  #endif
#else
  #define APIEXPORT
#endif

// SPECIFIC FOR MAC 

// SPECIFIC UBUNTU ???
// for libDevil 
#define ILUT_USE_OPENGL
