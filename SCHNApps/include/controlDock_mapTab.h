#ifndef _CONTROL_DOCK_MAP_TAB_H_
#define _CONTROL_DOCK_MAP_TAB_H_

#include "ui_controlDock_MapTabWidget.h"

#include "mapHandler.h"


#include "dll.h"

namespace CGoGN
{

namespace Utils
{
class VBO;
}

namespace SCHNApps
{

class SCHNApps;
class View;

class SCHNAPPS_API ControlDock_MapTab : public QWidget, public Ui::ControlDock_MapTabWidget
{
	Q_OBJECT

public:
	ControlDock_MapTab(SCHNApps* s);
	QString title() { return QString("Maps"); }

	MapHandlerGen* getSelectedMap() { return m_selectedMap; }
	unsigned int getCurrentOrbit();
	CellSelectorGen* getSelectedSelector(unsigned int orbit) { return m_selectedSelector[orbit]; }

	void setSelectedMap(const QString& mapName);

private slots:
	// slots called from UI actions
	void selectedMapChanged();

	void duplicateCurrentMap();
	void removeCurrentMap();

	void showBBChanged(bool b);
	void bbVertexAttributeChanged(int index);
	void vertexAttributeCheckStateChanged(QListWidgetItem* item);

	void selectedSelectorChanged();
	void selectorCheckStateChanged(QListWidgetItem* item);
	void addSelector();
	void removeSelector();

	// slots called from SCHNApps signals
	void mapAdded(MapHandlerGen* m);
	void mapRemoved(MapHandlerGen* m);

	// slots called from selected MapHandler signals
	void selectedMapAttributeAdded(unsigned int orbit, const QString& name);
	void selectedMapVBOAdded(Utils::VBO* vbo);
	void selectedMapVBORemoved(Utils::VBO* vbo);
	void selectedMapCellSelectorAdded(unsigned int orbit, const QString& name);
	void selectedMapCellSelectorRemoved(unsigned int orbit, const QString& name);


	void scaleRenderMap();

private:
	void updateSelectedMapInfo();

	SCHNApps* m_schnapps;
	MapHandlerGen* m_selectedMap;
	CellSelectorGen* m_selectedSelector[NB_ORBITS];
	bool b_updatingUI;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
