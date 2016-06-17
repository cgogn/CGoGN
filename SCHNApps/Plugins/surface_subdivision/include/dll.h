#ifdef WIN32
#if defined SURFACE_SUBDIVISION_DLL_EXPORT
#define SURFACE_SUBDIVISION_API __declspec(dllexport)
#else
#define SURFACE_SUBDIVISION_API __declspec(dllimport)
#endif
#else
#define SURFACE_SUBDIVISION_API
#endif
