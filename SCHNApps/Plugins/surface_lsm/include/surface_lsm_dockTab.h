#ifndef _SURFACE_DEFORMATION_DOCK_TAB_H_
#define _SURFACE_DEFORMATION_DOCK_TAB_H_

#include "ui_surface_lsm.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNApps;
class Surface_LSM_Plugin;
struct MapParameters;

class Surface_LSM_DockTab : public QWidget, public Ui::Surface_LSM_TabWidget
{
	Q_OBJECT

	friend class Surface_LSM_Plugin;

public:
	Surface_LSM_DockTab(SCHNApps* s, Surface_LSM_Plugin* p);

private:
	SCHNApps* m_schnapps;
	Surface_LSM_Plugin* m_plugin;
	bool b_updatingUI;

private slots:
	void positionAttributeChanged(int index);
	void handleSelectorChanged(int index);
	void freeSelectorChanged(int index);
	void startStopButtonClicked();

private:
	void addVertexAttribute(const QString& nameAttr);
	void addVertexSelector(const QString& name);
	void removeVertexSelector(const QString& name);

	void mapParametersInitialized(bool b);

	void updateMapParameters();
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
