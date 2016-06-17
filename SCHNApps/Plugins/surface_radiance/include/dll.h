#ifdef WIN32
#if defined SURFACE_RADIANCE_DLL_EXPORT
#define SURFACE_RADIANCE_API __declspec(dllexport)
#else
#define SURFACE_RADIANCE_API __declspec(dllimport)
#endif
#else
#define SURFACE_RADIANCE_API
#endif
