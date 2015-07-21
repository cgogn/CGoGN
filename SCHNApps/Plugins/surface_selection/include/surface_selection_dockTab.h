#ifndef _SURFACE_SELECTION_DOCK_TAB_H_
#define _SURFACE_SELECTION_DOCK_TAB_H_

#include "ui_surface_selection.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNApps;
class Surface_Selection_Plugin;
struct MapParameters;

class Surface_Selection_DockTab : public QWidget, public Ui::Surface_Selection_TabWidget
{
	Q_OBJECT

	friend class Surface_Selection_Plugin;

public:
	Surface_Selection_DockTab(SCHNApps* s, Surface_Selection_Plugin* p);

private:
	SCHNApps* m_schnapps;
	Surface_Selection_Plugin* m_plugin;
	bool b_updatingUI;

private slots:
	void positionAttributeChanged(int index);
	void normalAttributeChanged(int index);
	void selectionMethodChanged(int index);
	void verticesScaleFactorChanged(int i);
	void verticesScaleFactorPressed();
	void colorChanged(int i);
	void clearPressed();
private:
	void addVertexAttribute(const QString& name);

	void updateMapParameters();
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
