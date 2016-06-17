#ifdef WIN32
#if defined SURFACE_SELECTION_DLL_EXPORT
#define SURFACE_SELECTION_API __declspec(dllexport)
#else
#define SURFACE_SELECTION_API __declspec(dllimport)
#endif
#else
#define SURFACE_SELECTION_API
#endif
