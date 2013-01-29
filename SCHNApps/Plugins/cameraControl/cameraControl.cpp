#include "cameraControl.h"

#include "mapHandler.h"

#include "Algo/Import/import.h"

#include <QFileDialog>
#include <QFileInfo>

bool CameraControlPlugin::enable()
{
	return true;
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(CameraControlPlugin, CameraControlPlugin)
#else
Q_EXPORT_PLUGIN2(CameraControlPluginD, CameraControlPlugin)
#endif
