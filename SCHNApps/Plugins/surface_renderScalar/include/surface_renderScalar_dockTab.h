#ifndef _SURFACE_RENDERSCALAR_DOCK_TAB_H_
#define _SURFACE_RENDERSCALAR_DOCK_TAB_H_

#include "ui_surface_renderScalar.h"
#include "Utils/vbo.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNApps;
class Surface_RenderScalar_Plugin;
struct MapParameters;

class Surface_RenderScalar_DockTab : public QWidget, public Ui::Surface_RenderScalar_TabWidget
{
	Q_OBJECT

	friend class Surface_RenderScalar_Plugin;

public:
	Surface_RenderScalar_DockTab(SCHNApps* s, Surface_RenderScalar_Plugin* p);

private:
	SCHNApps* m_schnapps;
	Surface_RenderScalar_Plugin* m_plugin;
	MapParameters* m_currentParams;
	bool b_updatingUI;

private slots:
	void positionVBOChanged(int index);
	void selectedScalarVBOChanged();
	void colorMapChanged(int index);
	void expansionChanged(int i);

private:
	void addPositionVBO(QString name);
	void removePositionVBO(QString name);
	void addScalarVBO(QString name);
	void removeScalarVBO(QString name);

	void updateMapParameters();
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
