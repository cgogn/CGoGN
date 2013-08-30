#ifndef _IMPORTVOLUME_PLUGIN_H_
#define _IMPORTVOLUME_PLUGIN_H_

#include "plugin.h"


namespace CGoGN
{

namespace SCHNApps
{

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

	virtual void keyPress(View* view, QKeyEvent* event) {}
	virtual void keyRelease(View* view, QKeyEvent* event) {}
	virtual void mousePress(View* view, QMouseEvent* event) {}
	virtual void mouseRelease(View* view, QMouseEvent* event) {}
	virtual void mouseMove(View* view, QMouseEvent* event) {}
	virtual void wheelEvent(View* view, QWheelEvent* event) {}

public slots:
	MapHandlerGen* importFromFile(const QString& fileName);
	void importFromFileDialog();

private:
	QAction* importAction;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
