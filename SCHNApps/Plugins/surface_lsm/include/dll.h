#ifdef WIN32
#if defined SURFACE_LSM_DLL_EXPORT
#define SURFACE_LSM_API __declspec(dllexport)
#else
#define SURFACE_LSM_API __declspec(dllimport)
#endif
#else
#define SURFACE_LSM_API
#endif
