#ifndef _IMPORT_PLUGIN_H_
#define _IMPORT_PLUGIN_H_

#include "plugin.h"

#include "Topology/generic/functor.h"
#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"


struct PFP: public CGoGN::PFP_STANDARD
{
	// definition of the map
	typedef CGoGN::EmbeddedMap2 MAP;
};

typedef PFP::MAP MAP;
typedef PFP::VEC3 VEC3;


class ImportPlugin : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(Plugin)

public:
	ImportPlugin()
	{
		setProvidesRendering(false);
	}

	~ImportPlugin()
	{}

	virtual bool enable();
	virtual void disable();

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

public slots:
	void cb_import();

private:
	QAction* importAction;
};

#endif
