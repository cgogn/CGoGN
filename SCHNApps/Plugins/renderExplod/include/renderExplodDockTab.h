#ifndef _RENDEREXPLOD_DOCK_TAB_H_
#define _RENDEREXPLOD_DOCK_TAB_H_

#include "ui_renderExplod.h"

namespace CGoGN
{

namespace SCHNApps
{

class Window;
class RenderExplodPlugin;
class ParameterSet;

class RenderExplodDockTab : public QWidget, public Ui::RenderExplodDockWidget
{
	Q_OBJECT

public:
	RenderExplodDockTab(Window* w, RenderExplodPlugin* p);

private:
	Window* m_window;
	RenderExplodPlugin* m_plugin;
	ParameterSet* m_currentParams;
	bool b_refreshingUI;

public slots:
	void refreshUI(ParameterSet* params);

	void selectedMapChanged();
	void positionVBOChanged(int index);
	void colorVBOChanged(int index);
	void renderEdgesChanged(bool b);
	void renderFacesChanged(bool b);
	void facesScaleFactorChanged(int i);
	void volumesScaleFactorChanged(int i);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
