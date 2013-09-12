#ifndef _CONTROL_DOCK_MAP_TAB_H_
#define _CONTROL_DOCK_MAP_TAB_H_

#include "ui_controlDock_MapTabWidget.h"

namespace CGoGN
{

namespace Utils
{
class VBO;
}

namespace SCHNApps
{

class SCHNApps;
class MapHandlerGen;
class View;

class ControlDock_MapTab : public QWidget, public Ui::ControlDock_MapTabWidget
{
	Q_OBJECT

public:
	ControlDock_MapTab(SCHNApps* s);
	QString title() { return QString("Maps"); }

	MapHandlerGen* getSelectedMap() { return m_selectedMap; }

private slots:
	// slots called from UI actions
	void selectedMapChanged();
	void mapCheckStateChanged(QListWidgetItem* item);
	void vertexAttributeCheckStateChanged(QListWidgetItem* item);
	void addDartSelector();
	void removeDartSelector();
	void addVertexSelector();
	void removeVertexSelector();
	void addEdgeSelector();
	void removeEdgeSelector();
	void addFaceSelector();
	void removeFaceSelector();
	void addVolumeSelector();
	void removeVolumeSelector();

	// slots called from SCHNApps signals
	void mapAdded(MapHandlerGen* m);
	void mapRemoved(MapHandlerGen* m);
	void selectedViewChanged(View* prev, View* cur);

	// slots called from selected View signals
	void selectedViewMapLinked(MapHandlerGen* plugin);
	void selectedViewMapUnlinked(MapHandlerGen* plugin);

	// slots called from selected MapHandler signals
	void selectedMapAttributeAdded(unsigned int orbit, const QString& name);
	void selectedMapVBOAdded(Utils::VBO* vbo);
	void selectedMapVBORemoved(Utils::VBO* vbo);
	void selectedMapCellSelectorAdded(unsigned int orbit, const QString& name);
	void selectedMapCellSelectorRemoved(unsigned int orbit, const QString& name);

private:
	void updateSelectedMapInfo();

	SCHNApps* m_schnapps;
	MapHandlerGen* m_selectedMap;
	bool b_updatingUI;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
