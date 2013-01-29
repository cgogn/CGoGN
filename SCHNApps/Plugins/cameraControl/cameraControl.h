#ifndef _CAMERACONTROL_PLUGIN_H_
#define _CAMERACONTROL_PLUGIN_H_

#include "plugin.h"


using namespace CGoGN;
using namespace SCHNApps;


class CameraControlPlugin : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

public:
	CameraControlPlugin()
	{
		setProvidesRendering(false);
	}

	~CameraControlPlugin()
	{}

	virtual bool enable();
	virtual void disable() {}

	virtual void redraw(View *view) {}

	virtual void keyPress(View* view, int key) {}
	virtual void keyRelease(View* view, int key) {}
	virtual void mousePress(View* view, int button, int x, int y) {}
	virtual void mouseRelease(View* view, int button, int x, int y) {}
	virtual void mouseMove(View* view, int buttons, int x, int y) {}
	virtual void wheelEvent(View* view, int delta, int x, int y) {}

public slots:

private:

};

#endif
