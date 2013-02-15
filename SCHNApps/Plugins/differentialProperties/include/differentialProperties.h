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

	virtual void keyPress(View* view, QKeyEvent* event) {}
	virtual void keyRelease(View* view, QKeyEvent* event) {}
	virtual void mousePress(View* view, QMouseEvent* event) {}
	virtual void mouseRelease(View* view, QMouseEvent* event) {}
	virtual void mouseMove(View* view, QMouseEvent* event) {}
	virtual void wheelEvent(View* view, QWheelEvent* event) {}

public slots:
	void openComputeNormalDialog();
	void openComputeCurvatureDialog();

	void computeNormalFromDialog();
	void computeCurvatureFromDialog();

	void computeNormal(
		const QString& mapName,
		const QString& positionAttributeName = "position",
		const QString& normalAttributeName = "normal",
		bool createNormalVBO = true
	);
	void computeCurvature(
		const QString& mapName,
		const QString& positionAttributeName = "position",
		const QString& normalAttributeName = "normal",
		const QString& KmaxAttributeName = "Kmax",
		const QString& kmaxAttributeName = "kmax",
		const QString& KminAttributeName = "Kmin",
		const QString& kminAttributeName = "kmin",
		const QString& KnormalAttributeName = "Knormal",
		bool createKmaxVBO = true,
		bool createkmaxVBO = true,
		bool createKminVBO = true,
		bool createkminVBO = true,
		bool createKnormalVBO = true
	);

private:
	ComputeNormalDialog* m_computeNormalDialog;
	ComputeCurvatureDialog* m_computeCurvatureDialog;

	QAction* m_computeNormalAction;
	QAction* m_computeCurvatureAction;

	struct ComputeNormalParameters
	{
		ComputeNormalParameters() {}
		ComputeNormalParameters(QString p, QString n) : positionName(p), normalName(n) {}
		QString positionName;
		QString normalName;
	};
	QHash<QString, ComputeNormalParameters> computeNormalLastParameters;
};

#endif
