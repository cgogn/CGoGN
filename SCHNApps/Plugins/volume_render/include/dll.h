#ifdef WIN32
#if defined VOLUME_RENDER_DLL_EXPORT
#define VOLUME_RENDER_API __declspec(dllexport)
#else
#define VOLUME_RENDER_API __declspec(dllimport)
#endif
#else
#define VOLUME_RENDER_API
#endif
