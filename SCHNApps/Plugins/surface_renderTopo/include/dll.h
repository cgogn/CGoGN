#ifdef WIN32
#if defined SURFACE_RENDER_TOPO_DLL_EXPORT
#define SURFACE_RENDER_TOPO_API __declspec(dllexport)
#else
#define SURFACE_RENDER_TOPO_API __declspec(dllimport)
#endif
#else
#define SURFACE_RENDER_TOPO_API
#endif
