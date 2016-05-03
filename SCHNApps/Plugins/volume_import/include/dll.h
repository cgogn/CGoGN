#ifdef WIN32
#if defined VOLUME_IMPORT_DLL_EXPORT
#define VOLUME_IMPORT_API __declspec(dllexport)
#else
#define VOLUME_IMPORT_API __declspec(dllimport)
#endif
#else
#define VOLUME_IMPORT_API
#endif
