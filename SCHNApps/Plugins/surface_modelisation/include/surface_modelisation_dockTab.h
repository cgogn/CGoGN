#ifndef _SURFACE_MODELISATION_DOCK_TAB_H_
#define _SURFACE_MODELISATION_DOCK_TAB_H_

#include "ui_surface_modelisation.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNApps;
class Surface_Modelisation_Plugin;
struct MapParameters;

class Surface_Modelisation_DockTab : public QWidget, public Ui::Surface_Modelisation_TabWidget
{
	Q_OBJECT

	friend class Surface_Modelisation_Plugin;

public:
	Surface_Modelisation_DockTab(SCHNApps* s, Surface_Modelisation_Plugin* p);

private:
	SCHNApps* m_schnapps;
	Surface_Modelisation_Plugin* m_plugin;
	bool b_updatingUI;

private slots:
	void positionAttributeChanged(int index);
	void vertexSelectorChanged(int index);
	void edgeSelectorChanged(int index);
	void faceSelectorChanged(int index);

	void createEmptyMapButtonClicked();
	void addCubeButtonClicked();
	void flipEdgeButtonClicked();

private:
	void addVertexAttribute(const QString& name);
	void addVertexSelector(const QString& name);
	void removeVertexSelector(const QString& name);
	void addEdgeSelector(const QString& name);
	void removeEdgeSelector(const QString& name);
	void addFaceSelector(const QString& name);
	void removeFaceSelector(const QString& name);

	void updateMapParameters();
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
