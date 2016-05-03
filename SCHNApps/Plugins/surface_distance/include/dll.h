#ifdef WIN32
#if defined SURFACE_DISTANCE_DLL_EXPORT
#define SURFACE_DISTANCE_API __declspec(dllexport)
#else
#define SURFACE_DISTANCE_API __declspec(dllimport)
#endif
#else
#define SURFACE_DISTANCE_API
#endif
