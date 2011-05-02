
// SPECIFIC FOR WINDOWS

#ifdef WIN32
	#define NOMINMAX 
	#define _CRT_SECURE_NO_WARNINGS
	#include <windows.h>
	#include <stdio.h>
	#include <limits>
	#include <cmath>
	//#ifndef PI_DEFINED
	//#define PI_DEFINED
	//double M_PI=3.14159265359;
	//#endif
#endif

// SPECIFIC FOR MAC 

// SPECIFIC UBUNTU ???
// for libDevil 
#define ILUT_USE_OPENGL
