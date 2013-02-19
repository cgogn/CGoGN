#ifndef _SURFACE_DEFORMATION_DOCK_TAB_H_
#define _SURFACE_DEFORMATION_DOCK_TAB_H_

#include "ui_surfaceDeformation.h"

namespace CGoGN
{

namespace SCHNApps
{

class Window;
class SurfaceDeformationPlugin;
class ParameterSet;

class SurfaceDeformationDockTab : public QWidget, public Ui::SurfaceDeformationWidget
{
	Q_OBJECT

public:
	SurfaceDeformationDockTab(Window* w, SurfaceDeformationPlugin* p);

private:
	Window* m_window;
	SurfaceDeformationPlugin* m_plugin;
	ParameterSet* m_currentParams;
	bool b_refreshingUI;

public slots:
	void refreshUI(ParameterSet* params);

	void selectedMapChanged();
	void positionAttributeChanged(int index);
	void selectLockedVertices(bool b);
	void selectHandleVertices(bool b);
	void addAttributeToList(unsigned int orbit, const QString& nameAttr);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
