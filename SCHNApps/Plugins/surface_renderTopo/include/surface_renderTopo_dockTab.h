#ifndef _SURFACE_RENDERTOPO_DOCK_TAB_H_
#define _SURFACE_RENDERTOPO_DOCK_TAB_H_

#include "ui_surface_renderTopo.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNApps;
class Surface_RenderTopo_Plugin;
struct MapParameters;

class Surface_RenderTopo_DockTab : public QWidget, public Ui::Surface_RenderTopo_TabWidget
{
	Q_OBJECT

	friend class Surface_RenderTopo_Plugin;

public:
	Surface_RenderTopo_DockTab(SCHNApps* s, Surface_RenderTopo_Plugin* p);

private:
	SCHNApps* m_schnapps;
	Surface_RenderTopo_Plugin* m_plugin;
	bool b_updatingUI;

private slots:
	void positionAttributeChanged(int index);
	void drawDartsChanged(bool b);
	void dartsColorChanged(int);
	void drawPhi1Changed(bool b);
	void phi1ColorChanged(int);
	void drawPhi2Changed(bool b);
	void phi2ColorChanged(int);
	void edgesScaleFactorChanged(int i);
	void facesScaleFactorChanged(int i);

private:
	void addAttributeToList(unsigned int orbit, const QString& nameAttr);

	void updateMapParameters();
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
