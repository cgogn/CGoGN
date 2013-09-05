#ifndef _SURFACE_RENDERVECTOR_DOCK_TAB_H_
#define _SURFACE_RENDERVECTOR_DOCK_TAB_H_

#include "ui_surface_renderVector.h"
#include "Utils/vbo.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNApps;
class Surface_RenderVector_Plugin;
struct MapParameters;

class Surface_RenderVector_DockTab : public QWidget, public Ui::Surface_RenderVector_TabWidget
{
	Q_OBJECT

	friend class Surface_RenderVector_Plugin;

public:
	Surface_RenderVector_DockTab(SCHNApps* s, Surface_RenderVector_Plugin* p);

private:
	SCHNApps* m_schnapps;
	Surface_RenderVector_Plugin* m_plugin;
	bool b_updatingUI;

private slots:
	void positionVBOChanged(int index);
	void selectedVectorsVBOChanged();
	void vectorsScaleFactorChanged(int i);

private:
	void addPositionVBO(QString name);
	void removePositionVBO(QString name);
	void addVectorVBO(QString name);
	void removeVectorVBO(QString name);

	void updateMapParameters();
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
