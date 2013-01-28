#ifndef _DIFFERENTIALPROPERTIES_PLUGIN_H_
#define _DIFFERENTIALPROPERTIES_PLUGIN_H_

#include "plugin.h"

#include "computeNormalDialog.h"
#include "computeCurvatureDialog.h"


using namespace CGoGN;
using namespace SCHNApps;


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

public slots:
	void openComputeNormalDialog();
	void openComputeCurvatureDialog();

	void computeNormal();
	void computeCurvature();

private:
	ComputeNormalDialog* m_computeNormalDialog;
	ComputeCurvatureDialog* m_computeCurvatureDialog;

	QAction* m_computeNormalAction;
	QAction* m_computeCurvatureAction;
};

#endif
