#ifndef _RENDER_TOPO_SURFACE_DOCK_TAB_H_
#define _RENDER_TOPO_SURFACE_DOCK_TAB_H_

#include "ui_renderTopoSurface.h"

namespace CGoGN
{

namespace SCHNApps
{

class Window;
class RenderTopoSurfacePlugin;
struct ParameterSet;

class RenderTopoSurfaceDockTab : public QWidget, public Ui::RenderTopoSurfaceDockWidget
{
	Q_OBJECT

public:
	RenderTopoSurfaceDockTab(Window* w, RenderTopoSurfacePlugin* p);

private:
	Window* m_window;
	RenderTopoSurfacePlugin* m_plugin;
	ParameterSet* m_currentParams;
	bool b_refreshingUI;

public slots:
	void refreshUI(ParameterSet* params);

	void selectedMapChanged();
	void positionAttributeChanged(int index);
	void drawDartsChanged(bool b);
	void dartsColorChanged(int);
	void drawPhi1Changed(bool b);
	void phi1ColorChanged(int);
	void drawPhi2Changed(bool b);
	void phi2ColorChanged(int);
	void edgesScaleFactorChanged(int i);
	void facesScaleFactorChanged(int i);
	void addAttributeToList(unsigned int orbit, const QString& nameAttr);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
