#ifndef _DIFFERENTIALPROPERTIES_PLUGIN_H_
#define _DIFFERENTIALPROPERTIES_PLUGIN_H_

#include "plugin.h"
#include "ui_computeNormalsDialog.h"

#include "Topology/generic/functor.h"
#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Topology/generic/attributeHandler.h"


using namespace CGoGN;
using namespace SCHNApps;


struct PFP: public PFP_STANDARD
{
	typedef EmbeddedMap2 MAP;
};

typedef PFP::MAP MAP;
typedef PFP::VEC3 VEC3;


class ComputeNormalsDialog : public QDialog, public Ui::ComputeNormalsDialog
{
public:
	ComputeNormalsDialog() { setupUi(this);	}
};


class DifferentialPropertiesPlugin : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

public:
	DifferentialPropertiesPlugin()
	{
		setProvidesRendering(false);
	}

	~DifferentialPropertiesPlugin()
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
	void cb_computeNormals();

private:
	ComputeNormalsDialog* m_computeNormalsDialog;

	QAction* computeNormalsAction;
};

#endif
