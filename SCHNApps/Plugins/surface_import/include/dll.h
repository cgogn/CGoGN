#ifdef WIN32
#if defined SURFACE_IMPORT_DLL_EXPORT
#define SURFACE_IMPORT_API __declspec(dllexport)
#else
#define SURFACE_IMPORT_API __declspec(dllimport)
#endif
#else
#define SURFACE_IMPORT_API
#endif
