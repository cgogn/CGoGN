#ifdef WIN32
#if defined SURFACE_MODELISATION_DLL_EXPORT
#define SURFACE_MODELISATION_API __declspec(dllexport)
#else
#define SURFACE_MODELISATION_API __declspec(dllimport)
#endif
#else
#define SURFACE_MODELISATION_API
#endif
