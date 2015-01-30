#ifndef _SURFACE_DISTANCE_PLUGIN_H_
#define _SURFACE_DISTANCE_PLUGIN_H_

#include "plugin_processing.h"

#include "dialog_computeDistance.h"

namespace CGoGN
{

namespace SCHNApps
{

class Surface_Distance_Plugin : public PluginProcessing
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

public:
	Surface_Distance_Plugin()
	{}

	~Surface_Distance_Plugin()
	{}

	virtual bool enable();
	virtual void disable();

private slots:
	void openComputeDistanceDialog();
	void computeDistanceFromDialog();

	void appsFinished();

public slots:
	void computeDistance(
		const QString& mapName1,
		const QString& positionAttributeName1,
		const QString& distanceAttributeName1,
		const QString& mapName2,
		const QString& positionAttributeName2,
		const QString& distanceAttributeName2
	);

private:
	Dialog_ComputeDistance* m_computeDistanceDialog;
	QAction* m_computeDistanceAction;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
