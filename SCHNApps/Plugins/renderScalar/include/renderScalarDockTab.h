#ifndef _RENDERSCALAR_DOCK_TAB_H_
#define _RENDERSCALAR_DOCK_TAB_H_

#include "ui_renderScalar.h"
#include "Utils/vbo.h"

namespace CGoGN
{

namespace SCHNApps
{

class Window;
class RenderScalarPlugin;
struct ParameterSet;

class RenderScalarDockTab : public QWidget, public Ui::RenderScalarDockWidget
{
	Q_OBJECT

public:
	RenderScalarDockTab(Window* w, RenderScalarPlugin* p);

private:
	Window* m_window;
	RenderScalarPlugin* m_plugin;
	ParameterSet* m_currentParams;
	bool b_refreshingUI;

public slots:
	void refreshUI(ParameterSet* params);

	void selectedMapChanged();

	void positionVBOChanged(int index);
	void selectedScalarVBOChanged();
	void expansionChanged(int i);

	void addVBOToList(Utils::VBO* vbo);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
