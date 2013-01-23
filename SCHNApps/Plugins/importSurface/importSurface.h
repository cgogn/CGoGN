#ifndef _IMPORTSURFACE_PLUGIN_H_
#define _IMPORTSURFACE_PLUGIN_H_

#include "plugin.h"


using namespace CGoGN;
using namespace SCHNApps;


class ImportSurfacePlugin : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

public:
	ImportSurfacePlugin()
	{
		setProvidesRendering(false);
	}

	~ImportSurfacePlugin()
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

	virtual void viewLinked(View* view) {}
	virtual void viewUnlinked(View* view) {}
	virtual void currentViewChanged(View* view) {}

	virtual void mapLinked(View* view, MapHandlerGen* m) {}
	virtual void mapUnlinked(View* view, MapHandlerGen* m) {}

public slots:
	void importFromFile(const QString& fileName);
	void importFromFileDialog();

private:
	QAction* importAction;
};

#endif
