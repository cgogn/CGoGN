#ifdef WIN32
#if defined SURFACE_DEFORMATION_DLL_EXPORT
#define SURFACE_DEFORMATION_API __declspec(dllexport)
#else
#define SURFACE_DEFORMATION_API __declspec(dllimport)
#endif
#else
#define SURFACE_DEFORMATION_API
#endif
