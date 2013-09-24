#ifndef _SURFACE_DEFORMATION_DOCK_TAB_H_
#define _SURFACE_DEFORMATION_DOCK_TAB_H_

#include "ui_surface_deformation.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNApps;
class Surface_Deformation_Plugin;
struct MapParameters;

class Surface_Deformation_DockTab : public QWidget, public Ui::Surface_Deformation_TabWidget
{
	Q_OBJECT

	friend class Surface_Deformation_Plugin;

public:
	Surface_Deformation_DockTab(SCHNApps* s, Surface_Deformation_Plugin* p);

private:
	SCHNApps* m_schnapps;
	Surface_Deformation_Plugin* m_plugin;
	bool b_updatingUI;

private slots:
	void positionAttributeChanged(int index);
	void lockedSelectorChanged(int index);
	void handleSelectorChanged(int index);
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
