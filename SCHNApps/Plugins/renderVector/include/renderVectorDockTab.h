#ifndef _RENDERVECTOR_DOCK_TAB_H_
#define _RENDERVECTOR_DOCK_TAB_H_

#include "ui_renderVector.h"

namespace CGoGN
{

namespace SCHNApps
{

class Window;
class RenderVectorPlugin;
struct ParameterSet;

class RenderVectorDockTab : public QWidget, public Ui::RenderVectorDockWidget
{
	Q_OBJECT

public:
	RenderVectorDockTab(Window* w, RenderVectorPlugin* p);

private:
	Window* m_window;
	RenderVectorPlugin* m_plugin;
	ParameterSet* m_currentParams;
	bool b_refreshingUI;

public slots:
	void refreshUI(ParameterSet* params);

	void selectedMapChanged();
	void addVBOToList(QString name);
	void removeVBOFromList(QString name);
	void positionVBOChanged(int index);
	void selectedVectorsVBOChanged();
	void vectorsScaleFactorChanged(int i);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
