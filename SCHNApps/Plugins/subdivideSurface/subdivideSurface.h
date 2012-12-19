#ifndef _SUBDIVIDESURFACE_PLUGIN_H_
#define _SUBDIVIDESURFACE_PLUGIN_H_

#include "plugin.h"
#include "ui_subdivideSurface.h"

#include "Topology/generic/functor.h"
#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"


using namespace CGoGN;
using namespace SCHNApps;


class SubdivideSurfaceDockTab : public QWidget, public Ui::SubdivideSurfaceWidget
{
public:
	SubdivideSurfaceDockTab() { setupUi(this); }
};


class SubdivideSurfacePlugin : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

public:
	SubdivideSurfacePlugin()
	{
		setProvidesRendering(false);
	}

	~SubdivideSurfacePlugin()
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

protected:
	SubdivideSurfaceDockTab* m_dockTab;

public slots:
	void cb_selectedMapChanged();
	void cb_addMapToList(MapHandlerGen* m);
	void cb_removeMapFromList(MapHandlerGen* m);
};

#endif
