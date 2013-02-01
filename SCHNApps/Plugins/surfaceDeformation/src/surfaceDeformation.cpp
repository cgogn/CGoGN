#include "surfaceDeformation.h"

bool SurfaceDeformationPlugin::enable()
{
	return true;
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(SurfaceDeformationPlugin, SurfaceDeformationPlugin)
#else
Q_EXPORT_PLUGIN2(SurfaceDeformationPluginD, SurfaceDeformationPlugin)
#endif
