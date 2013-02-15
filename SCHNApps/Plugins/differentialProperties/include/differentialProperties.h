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
	void mapAdded(MapHandlerGen* map);
	void mapRemoved(MapHandlerGen* map);
	void attributeModified(unsigned int orbit, QString nameAttr);

	void openComputeNormalDialog();
	void openComputeCurvatureDialog();

	void computeNormalFromDialog();
	void computeCurvatureFromDialog();

	void computeNormal(const QString& mapName,
		const QString& positionAttributeName = "position",
		const QString& normalAttributeName = "normal",
		bool autoUpdate = true);

	void computeCurvature(
		const QString& mapName,
		const QString& positionAttributeName = "position",
		const QString& normalAttributeName = "normal",
		const QString& KmaxAttributeName = "Kmax",
		const QString& kmaxAttributeName = "kmax",
		const QString& KminAttributeName = "Kmin",
		const QString& kminAttributeName = "kmin",
		const QString& KnormalAttributeName = "Knormal",
		bool autoUpdate = true
	);

private:
	ComputeNormalDialog* m_computeNormalDialog;
	ComputeCurvatureDialog* m_computeCurvatureDialog;

	QAction* m_computeNormalAction;
	QAction* m_computeCurvatureAction;

	struct ComputeNormalParameters
	{
		ComputeNormalParameters() {}
		ComputeNormalParameters(const QString& p, const QString& n, bool update) :
			positionName(p), normalName(n), autoUpdate(update)
		{}
		QString positionName;
		QString normalName;
		bool autoUpdate;
	};
	QHash<QString, ComputeNormalParameters> computeNormalLastParameters;

	struct ComputeCurvatureParameters
	{
		ComputeCurvatureParameters() {}
		ComputeCurvatureParameters(
			const QString& p, const QString& n,
			const QString& Kmax, const QString& kmax, const QString& Kmin, const QString& kmin, const QString& Knormal,
			bool update) :
			positionName(p), normalName(n),
			KmaxName(Kmax), kmaxName(kmax), KminName(Kmin), kminName(kmin), KnormalName(Knormal),
			autoUpdate(update)
		{}
		QString positionName;
		QString normalName;
		QString KmaxName;
		QString kmaxName;
		QString KminName;
		QString kminName;
		QString KnormalName;
		bool autoUpdate;
	};
	QHash<QString, ComputeCurvatureParameters> computeCurvatureLastParameters;
};

#endif
