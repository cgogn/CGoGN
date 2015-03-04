#ifndef _SURFACE_DIFFERENTIALPROPERTIES_PLUGIN_H_
#define _SURFACE_DIFFERENTIALPROPERTIES_PLUGIN_H_

#include "plugin_processing.h"

#include "dialog_computeNormal.h"
#include "dialog_computeCurvature.h"

namespace CGoGN
{

namespace SCHNApps
{

class Surface_DifferentialProperties_Plugin : public PluginProcessing
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)
#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#endif
public:
	Surface_DifferentialProperties_Plugin()
	{}

	~Surface_DifferentialProperties_Plugin()
	{}

	virtual bool enable();
	virtual void disable();

private slots:
	void mapAdded(MapHandlerGen* map);
	void mapRemoved(MapHandlerGen* map);
	void attributeModified(unsigned int orbit, QString nameAttr);

	void openComputeNormalDialog();
	void openComputeCurvatureDialog();

	void computeNormalFromDialog();
	void computeCurvatureFromDialog();

	void appsFinished();

public slots:
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
		bool compute_kmean = true,
		bool compute_kgaussian = true,
		bool autoUpdate = true
	);

private:
	Dialog_ComputeNormal* m_computeNormalDialog;
	Dialog_ComputeCurvature* m_computeCurvatureDialog;

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
			bool kmean, bool kgaussian, bool update) :
			positionName(p), normalName(n),
			KmaxName(Kmax), kmaxName(kmax), KminName(Kmin), kminName(kmin), KnormalName(Knormal),
			compute_kmean(kmean), compute_kgaussian(kgaussian), autoUpdate(update)
		{}
		QString positionName;
		QString normalName;
		QString KmaxName;
		QString kmaxName;
		QString KminName;
		QString kminName;
		QString KnormalName;
		bool compute_kmean;
		bool compute_kgaussian;
		bool autoUpdate;
	};
	QHash<QString, ComputeCurvatureParameters> computeCurvatureLastParameters;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
