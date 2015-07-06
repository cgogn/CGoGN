#ifndef _SURFACE_DIFFERENTIALPROPERTIES_PLUGIN_H_
#define _SURFACE_DIFFERENTIALPROPERTIES_PLUGIN_H_

#include "plugin_processing.h"

#include "dialog_computeNormal.h"
#include "dialog_computeCurvature.h"

namespace CGoGN
{

namespace SCHNApps
{
/**
 * @brief Plugin that manage the computation of differential properties
 * - Normals
 * - Curvatures
 */
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

	void schnappsClosing();

public slots:
	/**
	 * @brief [PYTHON] compute the normals of a mesh
	 * @param mapName name of the 2d map (mesh)
	 * @param positionAttributeName name of position attribute used for computation
	 * @param normalAttributeName name of result attribute
	 * @param autoUpdate automatically update the normal attribute when position attribute change.
	 */
	void computeNormal(const QString& mapName,
		const QString& positionAttributeName = "position",
		const QString& normalAttributeName = "normal",
		bool autoUpdate = true);

	/**
	 * @brief [PYTHON] compute curvatures of a mesh
	 * @param mapName name of 2d map
	 * @param positionAttributeName name of input position attribute
	 * @param normalAttributeName name of input normal attributes
	 * @param KmaxAttributeName ?? result attribute name
	 * @param kmaxAttributeName ?? result attribute name
	 * @param KminAttributeName ?? result attribute name
	 * @param kminAttributeName ?? result attribute name
	 * @param KnormalAttributeName ?? result attribute aname
	 * @param compute_kmean compute the mean curvature
	 * @param compute_kgaussian compute the gaussian curvature
	 * @param autoUpdate automatically update the output attributes when input attribute change.
	 */
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
