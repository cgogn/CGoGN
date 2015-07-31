#ifndef _SURFACE_RADIANCE_DOCK_TAB_H_
#define _SURFACE_RADIANCE_DOCK_TAB_H_

#include "ui_surface_radiance.h"
#include "Utils/vbo.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNApps;
class Surface_Radiance_Plugin;
struct MapParameters;

class Surface_Radiance_DockTab : public QWidget, public Ui::Surface_Radiance_TabWidget
{
	Q_OBJECT

	friend class Surface_Radiance_Plugin;

public:
	Surface_Radiance_DockTab(SCHNApps* s, Surface_Radiance_Plugin* p);

private:
	SCHNApps* m_schnapps;
	Surface_Radiance_Plugin* m_plugin;
	MapParameters* m_currentParams;
	bool b_updatingUI;

private slots:
	void positionVBOChanged(int index);
	void normalVBOChanged(int index);
	void tangentVBOChanged(int index);
	void binormalVBOChanged(int index);
	void fragmentInterpolationChanged(int state);
	void decimateClicked();

private:
	void addPositionVBO(QString name);
	void removePositionVBO(QString name);
	void addNormalVBO(QString name);
	void removeNormalVBO(QString name);
	void addTangentVBO(QString name);
	void removeTangentVBO(QString name);
	void addBiNormalVBO(QString name);
	void removeBiNormalVBO(QString name);

	void updateMapParameters();
};

} // namespace SCHNApps

} // namespace CGoGN

#endif // _SURFACE_RADIANCE_DOCK_TAB_H_
