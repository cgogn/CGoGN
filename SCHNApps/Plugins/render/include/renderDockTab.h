#ifndef _RENDER_DOCK_TAB_H_
#define _RENDER_DOCK_TAB_H_

#include "ui_render.h"

namespace CGoGN
{

namespace SCHNApps
{

class Window;
class RenderPlugin;
struct ParameterSet;

class RenderDockTab : public QWidget, public Ui::RenderDockWidget
{
	Q_OBJECT

public:
	RenderDockTab(Window* w, RenderPlugin* p);

private:
	Window* m_window;
	RenderPlugin* m_plugin;
	ParameterSet* m_currentParams;
	bool b_refreshingUI;

public slots:
	void refreshUI(ParameterSet* params);

	void selectedMapChanged();
	void addVBOToList(QString name);
	void removeVBOFromList(QString name);
	void positionVBOChanged(int index);
	void normalVBOChanged(int index);
	void renderVerticesChanged(bool b);
	void verticesScaleFactorChanged(int i);
	void renderEdgesChanged(bool b);
	void renderFacesChanged(bool b);
	void faceStyleChanged(QAbstractButton* b);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
