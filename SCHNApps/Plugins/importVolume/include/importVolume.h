#ifndef _IMPORTVOLUME_PLUGIN_H_
#define _IMPORTVOLUME_PLUGIN_H_

#include "plugin.h"


using namespace CGoGN;
using namespace SCHNApps;


class ImportVolumePlugin : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

public:
	ImportVolumePlugin()
	{
		setProvidesRendering(false);
	}

	~ImportVolumePlugin()
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
	MapHandlerGen* importFromFile(const QString& fileName);
	void importFromFileDialog();

private:
	QAction* importAction;
};

#endif
