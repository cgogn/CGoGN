#ifdef WIN32
#if defined SCHNAPPSLIB_DLL_EXPORT
#define SCHNAPPS_API __declspec(dllexport)
#else
#define SCHNAPPS_API __declspec(dllimport)
#endif
#else
#define SCHNAPPS_API
#endif
